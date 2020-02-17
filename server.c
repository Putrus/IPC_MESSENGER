#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
struct message{

long type;
char text[1024];
};

struct group{
long type;
struct group *next;
};


struct login{
	char name[64];
	char password[64];
	struct group *groups;
	struct login *next;
};



struct registered {
char name[64];
char password[64];
struct group *groups;
struct registered *next;
};




/////////////////////////////
//POKAZANIE LISTY ZAREJESTROWANYCH UZYTKOWNIKOW
void showRegisteredDatabase(struct registered **registeredDatabase)
{
struct registered *registeredIterator;
struct group *groupIterator;
int index = 0;
	printf("REGISTERED USERS\n");
	if(*registeredDatabase == NULL)
	{
		printf("There are no registered users\n");	
		
	}
	else
	{
		registeredIterator = *registeredDatabase;
		while(registeredIterator)
		{
			printf("\nUser %d\n", index);
			printf("Name: %s\n", registeredIterator->name);
			printf("Password: %s\n", registeredIterator->password);
			printf("User's groups:\n");
			if(registeredIterator->groups == NULL)
			{
				printf("The user is not in any group\n");	
			}
			else
			{
				groupIterator = registeredIterator->groups;
				while(groupIterator)
				{
				printf("Group %li\n", groupIterator->type);	
				groupIterator = groupIterator->next;
				}
				
			}
			registeredIterator = registeredIterator->next;
			index++;
		}
		
	}
	
	
	
	
}




/////////////
//LINIA DO STRUKTURY FUNKCJA POMOCNICZA
void lineToStruct(char line[], struct registered **r)
{
	int i=0;
	char name[64];
	char password[64];
	int nameStartIndex = i;
	while(line[i] != ';')
	{
		i++;
	}
	strncpy((*r)->name, line + nameStartIndex, i - nameStartIndex);
	(*r)->name[i-nameStartIndex] = '\0';
	i++;
	int passwordStartIndex = i;
	while(line[i] != ';')
	{
		i++;		
	}
	strncpy((*r)->password, line + passwordStartIndex, i - passwordStartIndex);
	(*r)->password[i-nameStartIndex] = '\0';
	i+=1;
	struct group *g = NULL;	
	struct group *groupIterator;
	int typeStartIndex = i;
	for(; i<strlen(line); i++)
	{
		
		if(line[i] == ',')
		{	
			
			g = malloc(sizeof(struct group));
			g->next = NULL;
			char *tempType  = malloc(20*sizeof(char));
			strncpy(tempType,line + typeStartIndex, i - typeStartIndex);
			g->type = strtol(tempType,NULL,10);
			if((*r)->groups == NULL)
			{
				(*r)->groups = g;
			}
			else{
				groupIterator = (*r)->groups;
				while(groupIterator->next){
				groupIterator = groupIterator->next;	
				}
				groupIterator->next = g;	
			}
			i++;
			typeStartIndex = i;
		
		}	
	}
}







//////////////////////////////
//ZAREJESTROWANI UZYTKOWNICY Z PLIKU DO LISTY STRUKTUR
void getRegisteredDatabase(struct registered **registeredUsers)
{
	int file = open("registeredUsersDatabase.txt", O_RDWR);
	char sign;
	char tempSign[1] = "1";
	char temp[512] = "";
	struct registered *registeredIterator;
	while(read(file,&sign,1)>0)
	{
		if(sign == '\n')
		{
			struct registered *r = malloc(sizeof(struct registered));
			lineToStruct(temp, &r);
			
			strcpy(temp,"\0");
			if(*registeredUsers == NULL){
					*registeredUsers = r;
			}
			else
			{
				registeredIterator = *registeredUsers;
				while(registeredIterator->next)
				{
					registeredIterator = registeredIterator->next;	
				}
				registeredIterator->next = r;
				
			}
			
		}
		else
		{	
			strncat(temp,&sign,1);
		}
		
	}
	
	
}
//funkcja sprawdzajaca czy dane logowania sa ok 0 - nie 1 - tak
int checkLoginData(struct registered ** registeredUsers,struct login **loggedUsers, char *data)
{
	struct registered *registeredIterator;
	char temp[64] = "\0";
	struct login *logIterator = NULL;
		if(*registeredUsers == NULL)
			{
				return 0;
			}
			else
			{
				registeredIterator = *registeredUsers;
				while(registeredIterator)
				{
					strncat(temp,registeredIterator->name,strlen(registeredIterator->name));
					strcat(temp,";");
					strncat(temp,registeredIterator->password,strlen(registeredIterator->password));
					if(strcmp(temp, data)==0)
					{
						struct login *l = malloc(sizeof(struct login));
						strcpy(l->name, registeredIterator->name);
						strcpy(l->password, registeredIterator->password);

						l->groups = registeredIterator->groups;
						if(*loggedUsers == NULL)
						{
							*loggedUsers = l;
						}	
						else
						{
							logIterator = *loggedUsers;
								while(logIterator->next)
								{
								logIterator = logIterator->next;

								}
							logIterator->next = l;

						}

						registeredIterator = *registeredUsers;
						memset(temp,0,strlen(temp));
						return 1;
					}
				memset(temp,0,strlen(temp));
				registeredIterator = registeredIterator->next;
				}
}
return 0;
}


//pokazuje zalogowanych uzytkownikow
void showLoggedUsers(struct login **loggedUsers)
{
struct login *loginIterator;

printf("\nLOGGED USERS:\n");
if(*loggedUsers == NULL)
{
	printf("No logged users\n");
}
else
{
loginIterator = *loggedUsers;
int i=0;
struct group *groupIterator;
while(loginIterator)
{
printf("\nUser %d\n",i);
printf("Name: %s\n",loginIterator->name);
printf("Password: %s\n",loginIterator->password);
printf("User's group: \n");
if(loginIterator->groups == NULL)
			{
				printf("The user is not in any group\n");	
			}
			else
			{
				groupIterator = loginIterator->groups;
				while(groupIterator)
				{
				printf("Group %li\n", groupIterator->type);	
				groupIterator = groupIterator->next;
				}
			
			}
loginIterator = loginIterator->next;
i++;
}

}


}




//funkcja zwracajaca grupy ostatniego uzytkownika do stringa
void lastGroupLoggedString(struct login **loggedUsers, char *str)
{
	struct login *loginIterator;
	struct group *groupIterator;
	char temp[3];
	if(*loggedUsers == NULL)
	{
		printf("ERROR! NO LOGGED USERS!\n");
	}
	else
	{
	loginIterator = *loggedUsers;
	while(loginIterator->next)
	{
		loginIterator = loginIterator->next;
	}
	if(loginIterator->groups == NULL)
	{
		strcpy(str,";\0");
	}
	else
	{
		groupIterator = loginIterator->groups;
		while(groupIterator)
		{
			snprintf(temp,3,"%lu",groupIterator->type);
			strncat(str,temp,3);
			strcat(str,",\0");
			groupIterator = groupIterator->next;
		}
	}

	}

}






//logowanie sie uzytkownika
void loginUser(struct registered **registeredUsers, struct login **loggedUsers){
printf("Logging\n");
int loginID = msgget(10002, 0644 | IPC_CREAT);
struct message loginMessage;
loginMessage.type = 2;
msgrcv(loginID, &loginMessage, sizeof(loginMessage)-sizeof(long),2,0);
printf("%s\n",loginMessage.text);
int check = checkLoginData(registeredUsers, loggedUsers, loginMessage.text);
int verificationID = msgget(10003, 0644 | IPC_CREAT);
struct message verificationMessage;
verificationMessage.type = 3;
if(check == 1)
{
//wiadomosc z grupami dla uzytkownika
int groupID = msgget(10004, 0644 | IPC_CREAT);
struct message groupMessage;
groupMessage.type = 4;

printf("User logged in!\n");
verificationMessage.text[0] = '1';
msgsnd(verificationID, &verificationMessage, sizeof(verificationMessage) - sizeof(long),0);
memset(groupMessage.text,0,strlen(groupMessage.text));
lastGroupLoggedString(loggedUsers, groupMessage.text);
msgsnd(groupID, &groupMessage, sizeof(groupMessage) - sizeof(long),0);


}
else
{
printf("User doesn't exist!\n");
verificationMessage.text[0] = '0';
msgsnd(verificationID, &verificationMessage, sizeof(verificationMessage) - sizeof(long),0);

}

}
void logOut(struct login **loggedUsers, char name[])
{
struct login *loginIterator = *loggedUsers;
struct login *previous;
if(strcmp(loginIterator->name, name) == 0)
{
*loggedUsers = loginIterator->next;
free(loginIterator);
}
else
{
while(loginIterator && (strcmp(loginIterator->name, name)!=0))
{
	previous = loginIterator;
	loginIterator = loginIterator->next;
}
previous->next = loginIterator->next;
free(loginIterator);
}

}


//log out uzytkownika
void logOutUser(struct login **loggedUsers)
{
int logoutID = msgget(10005, 0644 | IPC_CREAT);
struct message logoutMessage;
logoutMessage.type = 5;
memset(logoutMessage.text, 0 , strlen(logoutMessage.text));
msgrcv(logoutID, &logoutMessage,sizeof(logoutMessage)-sizeof(long),5,0);
logOut(loggedUsers, logoutMessage.text);
printf("User %s logged out!\n", logoutMessage.text);

}

//wczytywanie z pliku txt podanej grupy do stringa ktorego wyslemy do uzytkownika
char *conversationFromFileToString(char type[]){
char *fileStr = malloc(20 *sizeof(char));

strncpy(fileStr,type,strlen(type));
strcat(fileStr, ".txt\0");
int file = open(fileStr, O_RDWR);
char sign;
char *str = malloc(1024 * sizeof(char));
memset(str,0,strlen(str));
while(read(file,&sign,1)>0)
{
strncat(str, &sign,1);

}
return str;
}





void groupConversationToUser()
{
int groupID = msgget(10006, 0644 | IPC_CREAT);
struct message groupMessage;
groupMessage.type = 6;
memset(groupMessage.text,0,strlen(groupMessage.text));
msgrcv(groupID,&groupMessage,sizeof(groupMessage)-sizeof(long),6,0);
printf("Send conversation of group %s to client\n",groupMessage.text);
int conversationID = msgget(10007, 0644 | IPC_CREAT);
struct message conversationMessage;
conversationMessage.type = 7;
memset(conversationMessage.text,0,strlen(conversationMessage.text));
char *temp = conversationFromFileToString(groupMessage.text);
strncpy(conversationMessage.text,temp,strlen(temp));
msgsnd(conversationID,&conversationMessage,sizeof(conversationMessage)-sizeof(long),0);
printf("Conversation:\n%s",conversationMessage.text);

}


void addMessageToGroupFile(char group[], char message[])
{
char *fileStr = malloc(20 *sizeof(char));
strncpy(fileStr,group,strlen(group));
strcat(fileStr, ".txt\0");
int file = open(fileStr, O_RDWR);
lseek(file,0,SEEK_END);
char sign;
int i=0;
while(message[i] != '\n')
{
sign = message[i];
write(file, &sign, 1);
i++;
}
sign = '\n';
write(file,&sign,1);

}



void messageToGroup()
{
int groupID = msgget(10008, 0644 | IPC_CREAT);
struct message groupMessage;
groupMessage.type = 8;
memset(groupMessage.text,0,strlen(groupMessage.text));
msgrcv(groupID, &groupMessage, sizeof(groupMessage)-sizeof(long),8,0);
int messageID = msgget(10009, 0644 | IPC_CREAT);
struct message messageMessage;
messageMessage.type = 9;
memset(messageMessage.text,0,strlen(messageMessage.text));
strcpy(messageMessage.text,"\0");
msgrcv(messageID, &messageMessage, sizeof(messageMessage)-sizeof(long),9,0);
addMessageToGroupFile(groupMessage.text,messageMessage.text);
printf("Send message to group %s succesfully\n",groupMessage.text);
memset(groupMessage.text,0,strlen(groupMessage.text));
memset(messageMessage.text,0,strlen(messageMessage.text));

}

////////////////////////////////
//Najwazniejsza funkcja otrzymywanie instrukcji
void getInstruction(struct registered **registeredUsers, struct login **loggedUsers){
	struct message instructionMessage;
	int instructionID = msgget(10001, 0644 | IPC_CREAT);
	memset(instructionMessage.text,0,strlen(instructionMessage.text));
	msgrcv(instructionID, &instructionMessage, sizeof(instructionMessage) - sizeof(long), 1, 0);
	printf("\nInstruction received!\n");
	switch(instructionMessage.text[0])
	{
		case '1':
			loginUser(registeredUsers, loggedUsers);
		break;
		case '2':
			logOutUser(loggedUsers);
		break;
		case '3':
			groupConversationToUser();
		break;
		case '4':
			messageToGroup();
		break;
		default:
		break;
	}
}



//////////////////////////////////////
//MAIN
///////////////////////////////////
int main(int argc, char *argv[])
{

struct registered *registeredUsers = NULL;
struct login *loggedUsers =  NULL;
getRegisteredDatabase(&registeredUsers);
showRegisteredDatabase(&registeredUsers);
while(getpid() >= 10001 && getpid() <=10019)
	{
		if(fork!=0)
		{
			exit(0);
		}

	}

while(1)
{
		getInstruction(&registeredUsers, &loggedUsers);
		showLoggedUsers(&loggedUsers);
}
return 0;
}

//msggety
//1 - instrukcja
//2 - login message
//3 - weryfikacja
//4 - grupy podczas logowania
//5 - imie podczas wylogowania
//6 - numer grupy jaka konwersacje wyslac
//7 - wyslanie calej konwersacji danej grupy
//8 - numer grupy ponownie ale do wyslania wiadomosci
//9 - wiadomosc wysylana do grupy
