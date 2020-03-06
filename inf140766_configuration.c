#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
//plik konfiguracyjny
int main(int argc, char *argv[])
{
	int file = open("inf140766_registeredUsersDatabase.txt", O_WRONLY | O_CREAT, 0644);

	char number = '0';
	char user[64] = "user0";
	char userPass[64] = "pass0";
	char groups[64];
	memset(groups,0,strlen(groups));
	char text[1024];
	memset(text,0,strlen(text));
	for(int i=0;i<9;i++)
	{
	user[4] = number+i;
	userPass[4] = number+i;
	if(i==0 || i==3 || i==6)
	{
	strcpy(groups,";20,;\n\0");
	}
	if(i==1 || i==4|| i ==7)
	{
	strcpy(groups,";20,21,;\n\0");
	}
	if(i==2 || i ==5 || i ==8)
	{
	strcpy(groups,";20,21,22,;\n\0");
	}	
	strncat(text,user,strlen(user));
	strcat(text,";\0");
	strncat(text,userPass,strlen(userPass));
	strncat(text,groups,strlen(groups));
	memset(groups,0,strlen(groups));
	
	}
	write(file,text,strlen(text));

	close(file);
	int file1 = open("20.txt", O_CREAT,0644);
	int file2 = open("21.txt", O_CREAT,0644);
	int file3 = open("22.txt", O_CREAT,0644);
	int fileGroups = open("inf140766_groups.txt", O_WRONLY | O_CREAT,0644);
	char grText[64] = "20,21,22,";
	write(fileGroups,grText,strlen(grText));

	close(file1);
	close(file2);
	close(file3);
	close(fileGroups);
}
