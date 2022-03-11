#include<linux/init.h>
#include<linux/module.h>
#include<linux/miscdevice.h>
#include<linux/fs.h>
#include<asm/uaccess.h>
#include<linux/sched.h>
#include<linux/slab.h>
#include<linux/kernel.h>
#include <linux/uaccess.h>
MODULE_LICENSE("Dual BSD/GPL");
#include <linux/version.h>
#include <linux/sched/signal.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
static struct class*  proclistClass  = NULL;
static struct device* proclistDevice = NULL;
struct task_struct *parent_task;   //this struct is used for for_each_process
static struct task_struct *task;  //this task struct is used to store the init_task
static int major_status,minor_status;
static int pl_open(struct inode*,struct file *);
static ssize_t pl_read(struct file *, char * , size_t, loff_t *);
static int pl_close(struct inode*, struct file *);


//device struct
static struct miscdevice p_device  = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "process_list",
	.fops = &p_fops
};
/*file_operations struct*/
static struct file_operations p_fops = {
	.open = pl_open,
	.read = pl_read,
	.release = pl_close
};
//character device initialization
static int __init p_initial(void){
/* major number registration for character devices*/
	major_status = register_chrdev(0, "process_list", &p_fops);
	if (major_status<0)
	{
		printk(KERN_ALERT "ERROR: Failed to register a major number for character device\n");
	  return major_status;
  	}
/*minor number registration for character device*/
	minor_status = misc_register(&p_device);
	if (minor_status<0)
	{
		printk(KERN_ALERT "ERROR: Failed to register a minor number for character device\n");
		misc_deregister(&p_device);
		return minor_status;
	}
	printk(KERN_INFO "Process_list registered correctly with major number: %d and minor number: %d\n", major_status,minor_status);

	proclistClass = class_create(THIS_MODULE, "process_class");
	if (IS_ERR(proclistClass))                                             // Check for error and clean up if there is
	{
		unregister_chrdev(major_status, "process_list");
		printk(KERN_ALERT "ERROR: Device Class Registration FAILED!\n");
		return PTR_ERR(proclistClass);
	}
	printk(KERN_ALERT "Device class created successfully\n");
	proclistDevice = device_create(proclistClass, NULL, MKDEV(major_status, 0), NULL, "process_list");
	if (IS_ERR(proclistDevice))                                            // Check for error and clean up if there is
	{
		class_destroy(proclistClass);
		unregister_chrdev(major_status, "process_list");
		printk(KERN_ALERT "Device Creation FAILED!\n");
		return PTR_ERR(proclistDevice);
	}
    	printk(KERN_ALERT "Device driver created successfully\n");
	task=next_task(&init_task);
	return 0;
}
/*exit function to deregister the character device driver major number, minor number, class struct created and device struct created.*/
static void __exit p_quit(void) {
	printk(KERN_INFO "Process_List: Exiting from the Process List KO\n");
	device_destroy(proclistClass, MKDEV(major_status, 0));
  	class_unregister(proclistClass);
  	class_destroy(proclistClass);
  	unregister_chrdev(major_status, "process_list");
	misc_deregister(&p_device);
}
/*get task state from integer value*/
static char* getState(long s)
{
	if(s==0)
		return "TASK_RUNNING";
	else if (s==(1|2|4|8))
		return "TASK_NORMAL , __TASK_STOPPED , __TASK_TRACED";
	else if (s==(0|1|2|4|8|32|16))
		return "TASK_RUNNING , TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE , __TASK_STOPPED , __TASK_TRACED , EXIT_ZOMBIE , EXIT_DEAD";
	else if (s==(1 | 2 ))
		return "TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE";
	else if (s==(2 | 1024))
		return "TASK_UNINTERRUPTIBLE , TASK_NOLOAD";
	else if (s==(128 | 8))
		return "TASK_WAKEKILL , __TASK_TRACED";
	else if (s==(128 | 4))
		return "TASK_WAKEKILL , __TASK_STOPPED";
	else if (s==(128 | 2))
		return "TASK_WAKEKILL , TASK_UNINTERRUPTIBLE";
	else if (s == (16 | 32))
		return "EXIT_ZOMBIE , EXIT_DEAD";
	else if (s==4096)
		return "TASK_STATE_MAX";
	else if (s==2048)
		return "TASK_NEW";
	else if (s==1024)
		return "TASK_NOLOAD";
	else if (s==512)
		return "TASK_PARKED";
	else if (s==256)
		return "TASK_WAKING";
	else if (s==128)
		return "TASK_WAKEKILL";
	else if (s==64)
		return "TASK_DEAD";
	else if (s==32)
		return "EXIT_ZOMBIE";
	else if (s==16)
		return "EXIT_DEAD";
	else if (s==8)
		return "__TASK_TRACED";
	else if(s==4)
		return "__TASK_STOPPED";
	else if(s==2)
		return "TASK_UNINTERRUPTIBLE";
	else if(s==1)
		return "TASK_INTERRUPTIBLE";
	else
		return "Unknown State\n";
}
static ssize_t pl_read(struct file *file, char *output_buffer, size_t size, loff_t * off)
{
	int status;
	char process[256];

	memset(process,0,sizeof(char)*256); //Reset the buffers

	for_each_process(parent_task) {                                         //Iterarte through each task to print process details
		if(task==parent_task){
			memset(process,0,sizeof(char)*256);

			pr_info("PID=%d PPID=%d CPU=%d STATE=%s\n",parent_task->pid,parent_task->parent->pid,task_cpu(parent_task),getState(parent_task->state));
			sprintf(process, "PID=%d PPID=%d CPU=%d STATE=%s\n",parent_task->pid,parent_task->parent->pid,task_cpu(parent_task),getState(parent_task->state));
			
			status = copy_to_user(output_buffer, process, strlen(process));
			if(status)
			{
				printk(KERN_ALERT "ERROR: Unable to copy data to user!", status);
				return -EFAULT;
			}
			task=next_task(parent_task);
			break;
		}
	}
	return strlen(process);
}
//Release function is called whenever the device is closed/released by the user space program

static int pl_close(struct inode *inodep, struct file *filep){
   printk(KERN_ALERT "process_list device has been successfully closed\n");
   task = &init_task;
   return 0;
}


module_init(p_initial);       // called at module insertion
module_exit(p_quit);      // called at module removal
