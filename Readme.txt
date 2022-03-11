Amit Pandit	
Email- apandit1@binghamton.edu


Files:
1.Makefile-Compile the Program
2.process_list.c -Kernel Character Device Driver
3.user.c-User Space program

How to Run:
Step1:Open a terminal in project directory : make (to build both kernel and user module).
Step2:Load the kernel module use : sudo insmod process_list.ko.
Step3:Run user code : sudo ./user.
Step4:Run dmesg to see the processes.
Step5:Unload the kernel module use : sudo rmmod process_list
Step6:Clear Dmesg by executing :sudo dmesg -c

Part A:
I have used my own linux VM on my personal computer also installed Oracle VM
Virtual-box. I have set memory to 2048 MB also created Virtual hard drive for the same
and selected VDI as my hard drive and also set storage on physical hard drive to
dynamically allocated.


Part B:
Downloaded hello.c and hellon.c and compiled kernel module by using make and there
files generated which are hello.ko, hello.mod.c, hello/mod, hellon.ko, hellon.mod and
loaded kernel module by sudo ismod.ko after loading of kernel module and checking in
kernel log by using “dmesg” will able to see "mymodule: Hello World!" and after unload
using sudo rmmod hello and checking in “dmesg” “mymodule:Goodbye, cruel world!!”
displayed. By loading hellon.ko using sudo insmod hellon.ko whom=class howmany=5
and checking in kernel log able to see 5 times "mymodule: Hello" and by unloading
hellon.ko and checking in “dmseg” logs are "mymodule: %d: Goodbye, cruel !!" 5
times.


Part C:
I downloaded source code of sample miscellaneous character device driver which
invoke simple misc_register function and declared structure by declare a device struct,
file operations struct and register the device with kernel
‘misc_register(&my_misc_device);’ and then .read file operation to return “Hello
World!” message to user space program that invokes the read() system call on
miscellaneous device and after unregister the device by
‘misc_deregister(&my_misc_device); ‘when removed the module.


Part D:
First installed cscope on my VM and downloaded stable 5.14.20 tarball source code and
run command cscope -R to build Cscope database and generated cscope.files and I am
able to search for all instance of symbol, or find its original definition, global definition,
functions called by this function, text string, change text string, egrep patterns, file,
assignments to this symbol and by using cscope -d to not regenerate the database and
type ‘?’ to see the help page.



