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
strncpy(client->name, name,strlen(name)-1);
strncpy(client->password, password,strlen(password)-1);
lineToStruct(client, groupMessage.text);

return 1;
}
//nie zalogowano poprawnie wyswietlamy komunikat i zwracamy wartosc 0
else
{
printf("Wrong login or password! Try again!\n");
return 0;
}





}

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



void chooseOption(struct login *client, int option)
{
	switch(option)
	{
		case '0':
			break;
		case '1':
			break;
		case '2':
			logOut(client);
			break;
		default:
			break;

	}



}


void showMenu(struct login *client){
printf("-----USER %s MENU------\n",client->name);
printf("(0) Show your groups\n");
printf("(1) Empty\n");
printf("(2) Log out\n");
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
