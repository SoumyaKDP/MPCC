#ifndef CLIENT_C
#define CLIENT_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LENGTH 2048

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

#define ip "127.0.0.1"

int choice;
int check;
char username[MAX_USERNAME_LENGTH + 1];
char password[MAX_PASSWORD_LENGTH + 1];
char exist_user_name[1024];
char exist_password[1024];
int i;
int sockfd = 0;
char name[32];

char message[LENGTH] = {};
char buffer[LENGTH + 32] = {};

typedef struct User
{
	char username[MAX_USERNAME_LENGTH + 1];
	char password[MAX_PASSWORD_LENGTH + 1];
	struct User* next;
}User;

User* head = NULL;

void save_user(User* user)
{
	FILE* fp = fopen("/home2/trainee67/classwork/sprint3/data/users.txt", "a");
	if (fp == NULL)
	{
		printf("Error opening file!\n");
        	return;
	}
	fprintf(fp, "%s %s\n", user->username, user->password);
	fclose(fp);
}

void load_users()
{
	FILE* fp = fopen("/home2/trainee67/classwork/sprint3/data/users.txt", "r");
	if (fp == NULL)
	{
        	printf("Error opening file!\n");
        	return;
    	}
	while (fscanf(fp, "%s %s\n", username, password) == 2)
	{
        	User* user = (User*) malloc(sizeof(User));
        	strcpy(user->username, username);
        	strcpy(user->password, password);
        	user->next = head;
        	head = user;
	}
	fclose(fp);
}

int allowed(char user_name[])
{
	//char exist_user_name[1024];
	//char exist_password[1024];
	FILE *fp;
	int flag=1;
	fp = fopen("/home2/trainee67/classwork/sprint3/data/users.txt","r");
	if(fp == NULL)
	{
		printf("\nSorry,,please Try again\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		while(fscanf(fp,"%s %s\n",exist_user_name,exist_password)==2)
		{
			if(strcmp(user_name,exist_user_name)==0)
			{
				flag=0;
				break;
			}
		}
	}
	fclose(fp);
	return flag;
}


User* create_user(char* username, char* password)
{
	User* user = (User*) malloc(sizeof(User));
	strcpy(user->username, username);
	strcpy(user->password, password);
	check = allowed(username);
	if(check==0)
	{
 		printf("\nUser already Exist\n");
		sleep(2);
		system("clear");
		load_users();
		return NULL;
	}
	else
	{
    		user->next = head;
    		head = user;
    		save_user(user);
		printf("\nSuccessfully Registered\n");

		sleep(2);
		system("clear");
    		return user;
	}
	return NULL;
}

User* find_user(char* username, char* password)
{
	User* current = head;
	while (current != NULL)
	{
        	if(strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0)
		{
			return current;
		}
		current = current->next;
	}
	return NULL;
}
/*
void print_users()
{
	printf("Users:\n");
	User* current = head;
	while (current != NULL)
	{
        	printf("%s\n", current->username);
        	current = current->next;
	}
}
*/
int reg()
{
	load_users();
	while(1)
	{	    
		system("clear");
		printf("\n-----------------WEL COME----------------------");
		printf("\n_______________________________________________\n");
		printf("********************MPCC***********************");
		printf("\n<><><><><><><><><><><><><><><><<><><><><><><><>\n");
		printf("\n1.Register\n2.Log In\n3.Exit\nEnter Choice: ");
		scanf("%d", &choice);
		if (choice == 1)
		{
			printf("Enter username: ");
			scanf("%s", username);
			printf("Enter password: ");
			scanf("%s", password);
			create_user(username, password);
		} 
		else if (choice == 2)
		{
			printf("Enter username: ");
			scanf("%s", username);
			printf("Enter password: ");
			scanf("%s", password);
			User* user = find_user(username, password);
			if (user != NULL)
			{
				choice=0;
				printf("Successfully Logged In\n\n");

				sleep(2);
				system("clear");
				printf("Welcome, %s!\n", user->username);
				return 0;
			}
			else
			{
				choice=0;
				printf("Invalid username or password.\n");
				sleep(1);
				system("clear");
			}
		}
		else if (choice == 3)
		{
			printf("<<<<<<<<<<<<<<<<<<<<< THANK YOU |||| COME AGAIN  >>>>>>>>>>>>>>>>>>>>>\n");
			sleep(3);
			system("clear");
	    		exit(0);
		} 
		else
		{
			printf("Invalid choice.\n");
		}
    	}
}

//******reg*******//

/* Global variables*/

volatile sig_atomic_t flag = 0;

void str_overwrite_stdout()
{
	printf("%s", "> ");
	fflush(stdout);
}

void str_trim_lf (char* arr, int length) 
{
	for (i = 0; i < length; i++)
	{ 
		if (arr[i] == '\n')
		{
      			arr[i] = '\0';
      			break;
		}
	}
}

void catch_ctrl_c_and_exit(int sig)
{
	flag = 1;
}

void send_msg_handler()
{
	User* current = head;
	while(1)
	{
  		str_overwrite_stdout();
		fgets(message, LENGTH, stdin);
		str_trim_lf(message, LENGTH);

		if (strcmp(message, "exit") == 0)
		{
			break;
		}
		else
		{
			sprintf(buffer, "%s: %s\n", current->username, message);
			send(sockfd, buffer, strlen(buffer), 0);
		}

		bzero(message, LENGTH);
		bzero(buffer, LENGTH + 32);
	}
	catch_ctrl_c_and_exit(2);
}

void recv_msg_handler()
{
	while (1)
	{
		int receive = recv(sockfd, message, LENGTH, 0);
		if (receive > 0)
		{
			printf("%s", message);
			str_overwrite_stdout();
		}
		else if (receive == 0)
		{
			break;
		}
		else
		{
			// -1
		}
		memset(message, 0, sizeof(message));
  	}
}

int main(int argc, char **argv)
{
	reg();
	User* current = head;
	if(argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int port = atoi(argv[1]);

	signal(SIGINT, catch_ctrl_c_and_exit);
	
	str_trim_lf(current->username, strlen(current->username));

	struct sockaddr_in server_addr;

	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);


  	/* Connect to Server */
	int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err == -1)
	{
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	/* Send name */
	send(sockfd, current->username, 32, 0);

	printf("=== WELCOME TO THE CHATROOM ===\n");

	pthread_t send_msg_thread;
	if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0)
	{
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	pthread_t recv_msg_thread;
	if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0)
	{
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	while (1)
	{
		if(flag)
		{
			printf("\n==================THANK YOU !!! COME AGAIN=======================\n");
			break;
    		}
	}

	close(sockfd);

	return EXIT_SUCCESS;
}

#endif
