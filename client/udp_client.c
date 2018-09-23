#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <stdbool.h>

#define MAXBUFSIZE 100

/* You will have to modify the program below */
// TODO: remove file on get command

int main (int argc, char * argv[])
{

	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];
	bool flag = false;
	char userInput[MAXBUFSIZE];
	char fileName[MAXBUFSIZE];
	struct sockaddr_in remote;              //"Internet socket address structure"

	char get[] = "get";
	char put[] = "put";
	char delete[] = "delete";
	char ls[] = "ls";
	char exitServer[] = "exit";
	char over[] = "Over";
	int fd;

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet
	  i.e the Server.
	 ******************/
	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf("unable to create socket");
	}

	/******************
	  sendto() sends immediately.
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	struct sockaddr_in server_addr;
	unsigned int addr_length= sizeof(server_addr);
	char* splitInput;

	while(1) {
		printf("\n\nHere is a list of commands: [get, put, ls, delete, exit]\n");
		printf("What would you like to do?\n");
		fgets(userInput, MAXBUFSIZE, stdin);
		splitInput = strtok(userInput, " ");
		printf("EXECUTING: %s\n", splitInput);

		if(!strcmp(splitInput, "get")) {
			nbytes = sendto(sock, &get, sizeof(get), 0, (struct sockaddr *) &remote, sizeof(remote));

			splitInput = strtok(NULL, " ");
			splitInput[strlen(splitInput) - 1] = '\0';
			nbytes = sendto(sock, splitInput, sizeof(splitInput), 0, (struct sockaddr *) &remote, sizeof(remote));

			sprintf(fileName, "%s", splitInput);

			remove(fileName);
			int fd = open(fileName, O_CREAT | O_WRONLY | O_APPEND, 0755);

			if(fd < 0) {
				printf("Error opening file.\n");
				return -1;
			}

			bzero(buffer,sizeof(buffer));
			while(true) {
				nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &server_addr, (socklen_t *) &addr_length);
				if(!strcmp(buffer, "Over")) {
					close(fd);
					break;
				}
				if(!strcmp(buffer, "Error opening file.")) {
					printf("Error opening file on server.\n");
					close(fd);
					break;
				}
				write(fd, buffer, sizeof(buffer));
			}
		}

		else if(!strcmp(splitInput, "put")) {
			nbytes = sendto(sock, &put, sizeof(put), 0, (struct sockaddr *) &remote, sizeof(remote));
			splitInput = strtok(NULL, " ");
			splitInput[strlen(splitInput) - 1] = '\0';
			nbytes = sendto(sock, splitInput, sizeof(splitInput), 0, (struct sockaddr *) &remote, sizeof(remote));

			int fd = open(splitInput, O_RDWR);
			if(fd < 0) {
				printf("Error opening file.\n");
				continue;
			}

			bzero(buffer,sizeof(buffer));
			while(1)
			{
				bzero(buffer,sizeof(buffer));
				if(read(fd, buffer, sizeof(buffer)) <= 0) {
					break;
				}
				nbytes = sendto(sock, &buffer, sizeof(buffer), 0, (struct sockaddr *) &remote, sizeof(remote));
			}
			nbytes = sendto(sock, &over, sizeof(over), 0, (struct sockaddr *) &remote, sizeof(remote));
			close(fd);
		}

		else if(!strcmp(splitInput, "ls\n")) {
			nbytes = sendto(sock, &ls, sizeof(ls), 0, (struct sockaddr *) &remote, sizeof(remote));
			bzero(buffer,sizeof(buffer));
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &server_addr, (socklen_t *) &addr_length);
			printf("%s\n", buffer);
		}

		else if(!strcmp(splitInput, "delete")) {
			nbytes = sendto(sock, &delete, sizeof(delete), 0, (struct sockaddr *) &remote, sizeof(remote));

			splitInput = strtok(NULL, " ");
			splitInput[strlen(splitInput) - 1] = '\0';
			nbytes = sendto(sock, splitInput, sizeof(splitInput), 0, (struct sockaddr *) &remote, sizeof(remote));

			bzero(buffer,sizeof(buffer));
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &server_addr, (socklen_t *) &addr_length);
			printf("\n%s\n", buffer);

		}

		else if(!strcmp(splitInput, "exit\n")) {
			nbytes = sendto(sock, &exitServer, sizeof(exitServer), 0, (struct sockaddr *) &remote, sizeof(remote));
			bzero(buffer,sizeof(buffer));
		}

		else {
			printf("Command not found.\n");
		}
	}
	close(sock);
}
