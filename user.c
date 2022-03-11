#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
int main() {
	int rd,fd;                                                            //rd=read data status,fd = file pointer to opened character device file
	char read_data[256];                                                 // Char Buffer to hold character device read data.*/
	fd = open("/dev/process_list",O_RDONLY);
	if(fd < 0)                          				 // if there is an error while opening process_list character device in read-only made
	{
		perror("ERROR: Unable to open character device Process_List");                     //display error message
		return errno;
	}
	printf("/dev/process_list opened\n");
   	printf("Reading from character device:\n");                            //else assign opened device to a file pointer
	while((rd = read(fd,read_data,strlen(read_data)))  > 0) {                //reading data from process_list until its value is 0 which means file has ended
		printf("%s\n",read_data);                                              //print data read from file
		memset(read_data,0,sizeof(char)*256);                                  //re-initialize for next line
	}
	if(rd < 0) {
		perror("ERROR: Unable to read from character device Process_List!");
		return errno;
	}
	printf("\n");
	rd = close(fd);                                                                  //Close character device
	if (rd < 0) {
     printf("Error occured while closing file\n");
     return errno;
   	}
   	printf("/dev/process_list closed\n");
   	return 0;
}
