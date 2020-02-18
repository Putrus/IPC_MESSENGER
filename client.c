#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>


struct message{
	long type;
	char text[1024];
};
struct group{
	int type;
	struct group *next;
};
struct login{
	char name[64];
	char password[64];
	struct group * groups;
	//struct availableGroup  *availableGroups;
};


//taka sama funkcja jak w serwerze tylko tym razem dodajemy do clienta
void lineToStruct(struct login *client, char line[])
{
struct group *groupIterator;
struct group *g = NULL;
int typeStartIndex = 0;
for(int i=0; i<strlen(line);i++)
{
if(line[0] == ',')
{
break;
}
if(line[i] == ',')
{
g = malloc(sizeof(struct group));
g->next = NULL;
char *tempType = malloc(20*sizeof(char));
strncpy(tempType,line + typeStartIndex, i- typeStartIndex);
g->type = strtol(tempType,NULL,10);
if(client->groups == NULL)
{
client->groups = g;
}
else
{
groupIterator = client->groups;
while(groupIterator->next)
{
groupIterator = groupIterator->next;
}
groupIterator->next = g;
}
typeStartIndex = i+1;

}


}



}






//logowanie jedna z wazniejszych funkcji w kliencie
int login(struct login*client){
//usuwamy te procesy ktorych numery sa od 10001 do 10019
//aby bylo miejsce na nasze procesy
while(getpid() >= 10001 && getpid() <= 10019)
{
	if(fork() != 0)
	{
	exit(0);
	}
}
//login i haslo
char name[64];
char password[64];
memset(name,0,strlen(name));
memset(password,0,strlen(password));
printf("Login:");
fgets(name,sizeof(name),stdin);
printf("Password:");
fgets(password,sizeof(name),stdin);
int instructionID = msgget(10001, 0644 | IPC_CREAT);
struct message instructionMessage;
instructionMessage.type = 1;
instructionMessage.text[0] = '1';
msgsnd(instructionID, &instructionMessage, sizeof(instructionMessage)-sizeof(long),0);

int loginID = msgget(10002, 0644 | IPC_CREAT);
struct message loginMessage;
strcpy(loginMessage.text, "\0");
loginMessage.type = 2;
strncat(loginMessage.text, name, strlen(name));
loginMessage.text[strlen(loginMessage.text)-1] = '\0';
strcat(loginMessage.text, ";\0");
strncat(loginMessage.text, password, strlen(password));
loginMessage.text[strlen(loginMessage.text)-1] = '\0';
//to bylo do testow
//printf("%s\n",loginMessage.text);
msgsnd(loginID,&loginMessage,sizeof(loginMessage)-sizeof(long),0);

int verificationID = msgget(10003, 0644 | IPC_CREAT);
struct message verificationMessage;
verificationMessage.type = 3;
msgrcv(verificationID, &verificationMessage, sizeof(verificationMessage) - sizeof(long),3,0);

//zalogowano poprawnie wyswietlamy komunikat i czekamy na dane do zapisania
//w strukturze zalogowanego
//zwracamy 1
if(verificationMessage.text[0] == '1')
{
int groupID = msgget(10004, 0644 | IPC_CREAT);
struct message groupMessage;
groupMessage.type = 4;
printf("Login succesful!\n");
//tutaj robimy pustego stringa i oczekujemy na otrzymanie stringa z grupami uzytkownika
//aby dodac je do struktury
memset(groupMessage.text,0,strlen(groupMessage.text));
msgrcv(groupID, &groupMessage, sizeof(groupMessage) - sizeof(long),4,0);
//dodanie nazwy, hasla i grup do struktury
memset(client->name,0,strlen(client->name));
memset(client->password,0,strlen(client->password));
strncpy(client->name, name,strlen(name));
client->name[strlen(client->name)-1] = '\0';
strncpy(client->password, password,strlen(password));
client->password[strlen(client->password)-1] = '\0';

lineToStruct(client, groupMessage.text);

return 1;
}
//nie zalogowano poprawnie wyswietlamy komunikat i zwracamy wartosc 0
else
{
printf("Wrong login/password or user is already logged in! Try again!\n");
return 0;
}





}
//wylogowanie uzytkownika
void logOut(struct login *client)
{
int instructionID = msgget(10001, 0644 | IPC_CREAT);
struct message instructionMessage;
instructionMessage.type = 1;
instructionMessage.text[0] = '2';
msgsnd(instructionID, &instructionMessage, sizeof(instructionMessage) -sizeof(long),0);
int logoutID = msgget(10005, 0644 | IPC_CREAT);
struct message logoutMessage;
logoutMessage.type = 5;
memset(logoutMessage.text, 0, strlen(logoutMessage.text));
strncpy(logoutMessage.text, client->name, strlen(client->name));
//
msgsnd(logoutID, &logoutMessage, sizeof(logoutMessage) - sizeof(long),0);
}


void showGroups(struct login *client)
{
struct group *groupIterator;
if(client->groups == NULL)
{
printf("You are not in any group!\n");
}
else
{
groupIterator = client->groups;
while(groupIterator)
{
printf("Group %li\n",groupIterator->type);
groupIterator = groupIterator->next;
}
}
}

void showClientGroups(struct login *client)
{
printf("\n-----YOUR GROUPS-----\n");
showGroups(client);
printf("\nPress enter to continue\n");
int c;
while((c = getchar())!='\n'){}

}

//pokazuje konwersacje
void showConversation(char strType[], struct login *client)
{
int instructionID = msgget(10001, 0644 | IPC_CREAT);
struct message instructionMessage;
instructionMessage.type = 1;
memset(instructionMessage.text, 0 , strlen(instructionMessage.text));
instructionMessage.text[0] = '3';
msgsnd(instructionID, &instructionMessage, sizeof(instructionMessage)-sizeof(long),0);
int groupID = msgget(10006, 0644 | IPC_CREAT);
struct message groupMessage;
groupMessage.type = 6;
memset(groupMessage.text,0,strlen(groupMessage.text));
strncpy(groupMessage.text, strType, strlen(strType));
msgsnd(groupID, &groupMessage, sizeof(groupMessage)-sizeof(long),0);

int conversationID = msgget(10007,0644 | IPC_CREAT);
struct message conversationMessage;
conversationMessage.type = 7;
memset(conversationMessage.text,0,strlen(conversationMessage.text));
msgrcv(conversationID, &conversationMessage, sizeof(conversationMessage)-sizeof(long),7,0);
printf("\nConversation of group %s:\n%s", strType, conversationMessage.text);
printf("\nPress enter to continue\n");
int c;
while((c = getchar())!='\n'){}


}

//sprawdza czy uzytkownik jest w danej grupie i zwraca 1 jesli tak i 0 jesli nie
int checkUserInGroup(struct login *client, char group[])
{
long groupType;
groupType = strtol(group,NULL,10);
struct group *groupIterator;
if(client->groups == NULL)
{
return 0;
}
else
{
groupIterator = client->groups;
while(groupIterator)
{
if(groupType == groupIterator->type)
{
return 1;
}
groupIterator = groupIterator->next;
}
}
return 0;



}



//pokazuje konwersacje danej grupy
void showGroupConversation(struct login *client)
{
printf("\n-----Show conversation of one of the groups-----\n");
printf("Your groups:\n");
showGroups(client);
printf("Choose group:");
char *groupType = malloc(3*sizeof(char));
fgets(groupType,sizeof(groupType),stdin);
groupType[strlen(groupType)-1] = '\0';
int check;
check = checkUserInGroup(client, groupType);
if(check == 1)
{
showConversation(groupType, client);
memset(groupType,0,strlen(groupType));
}
else
{
printf("You aren't in this group!\n");
printf("\nPress enter to continue\n");
int c;
while((c = getchar())!='\n'){}
}

}


void messageToGroup(struct login *client, char strType[], char message[])
{
int instructionID = msgget(10001, 0644 | IPC_CREAT);
struct message instructionMessage;
instructionMessage.type = 1;
memset(instructionMessage.text, 0 , strlen(instructionMessage.text));
instructionMessage.text[0] = '4';
msgsnd(instructionID, &instructionMessage, sizeof(instructionMessage)-sizeof(long),0);
int groupID = msgget(10008, 0644 | IPC_CREAT);
struct message groupMessage;
groupMessage.type = 8;
memset(groupMessage.text,0,strlen(groupMessage.text));
strncpy(groupMessage.text, strType, strlen(strType));
groupMessage.text[strlen(groupMessage.text)] = '\0';
msgsnd(groupID, &groupMessage, sizeof(groupMessage)-sizeof(long),0);
int messageID = msgget(10009, 0644 | IPC_CREAT);
struct message messageMessage;
messageMessage.type = 9;
memset(messageMessage.text,0,strlen(messageMessage.text));
strncpy(messageMessage.text,message,strlen(message));
msgsnd(messageID,&messageMessage, sizeof(messageMessage)-sizeof(long),0);
printf("Message send succesfully!\n");
memset(messageMessage.text,0,strlen(messageMessage.text));
memset(groupMessage.text,0,strlen(groupMessage.text));





}

//wyslanie wiadomosci do grupy
void sendMessageToGroup(struct login*client)
{
printf("-----Send message to group's conversation-----\n");
printf("Your groups:\n");
showGroups(client);
printf("Choose group:");
char *groupType = malloc(3*sizeof(char));
memset(groupType,0,strlen(groupType));
fgets(groupType,sizeof(groupType),stdin);
groupType[strlen(groupType)-1] = '\0';
int check;
check = checkUserInGroup(client, groupType);
if(check == 1)
{
printf("Message:");
char message[256];
strcpy(message,"\0");
memset(message,0,strlen(message));
fgets(message,sizeof(message),stdin);
message[strlen(message)] = '\0';
char *messageFull = malloc(256*sizeof(char));
memset(messageFull,0,strlen(messageFull));
strncpy(messageFull,client->name,strlen(client->name));
strcat(messageFull,";\0");
strncat(messageFull,message,strlen(message));;
messageFull[strlen(messageFull)] = '\0';
messageToGroup(client,groupType,messageFull);
memset(messageFull,0,strlen(messageFull));
memset(message,0,strlen(message));
memset(groupType,0,strlen(groupType));
}
else
{
printf("You aren't in this group!\n");
printf("\nPress enter to continue\n");
int c;
while((c = getchar())!='\n'){}
}

}






void showLoggedUsers()
{
int instructionID = msgget(10001, 0644 | IPC_CREAT);
struct message instructionMessage;
instructionMessage.type = 1;
instructionMessage.text[0] = '5';
msgsnd(instructionID, &instructionMessage, sizeof(instructionMessage) - sizeof(long),0);
int loggedUsersID = msgget(10010, 0644 | IPC_CREAT);
struct message loggedMessage;
loggedMessage.type = 10;
memset(loggedMessage.text,0,strlen(loggedMessage.text));
msgrcv(loggedUsersID,&loggedMessage,sizeof(loggedMessage)-sizeof(long),10,0);
printf("\n%s\n", loggedMessage.text);
printf("\nPress enter to continue\n");
int c;
while((c = getchar())!='\n'){}

}





void chooseOption(struct login *client, int option)
{
	switch(option)
	{
		case '0':
			showClientGroups(client);
			break;
		case '1':
			showGroupConversation(client);
			break;
		case '2':
			logOut(client);
			break;
		case '3':
			sendMessageToGroup(client);
			break;
		case '4':
			showLoggedUsers();
			break;
		default:
			break;

	}



}


void showMenu(struct login *client){
printf("-----USER %s MENU------\n",client->name);
printf("(0) Show your groups\n");
printf("(1) Show conversations of one of the group\n");
printf("(2) Log out\n");
printf("(3) Send message to group\n");
printf("(4) Show logged users\n");
}





void menu(struct login *client){
	while(1){
	showMenu(client);
	printf("\nChoose option:");
	int option,c;
	option = getchar();
	while((c = getchar())!='\n'){}
	chooseOption(client, option);	
	if(option == '2')
	{
		printf("Log out succesfully\n");
		break;

	}




	}


}

int main(int argc, char*argv[])
{
	printf("PUTRA MESSENGER 4.20\n");
	struct login client;
	int checkLogin = 0;
	client.groups = NULL;
	checkLogin = login(&client);
	while(checkLogin == 0)
	{
	checkLogin = login(&client);
	}
	menu(&client);
	

return 0;
}
