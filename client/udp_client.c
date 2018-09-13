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

int main (int argc, char * argv[])
{

	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];
	bool flag = false;
	char* userInput = (char*)malloc(MAXBUFSIZE);

	struct sockaddr_in remote;              //"Internet socket address structure"

	char get[] = "get";
	char put[] = "put";
	char delete[] = "delete";
	char ls[] = "ls";
	char exitServer[] = "exit";




	FILE* fp;
	fp = fopen("foo1", "r");
	// fseek(fp, 0, SEEK_END);
	// long fileSize = ftell(fp);
	// fseek(fp, 0, SEEK_SET);
	// char* fileBuffer = (char *)malloc(fileSize);
	// fread(fileBuffer, sizeof(char), fileSize, fp);
	// printf("%s\n", fileBuffer);
	// printf("%ld\n", fileSize);

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
	char message[] = "Over";
	int offset = 0;

	while(1) {
		printf("Here is a list of commands: [get, put, ls, exit]\n");
		printf("What would you like to do?\n");
		fgets(userInput, MAXBUFSIZE, stdin);

		char* splitInput;
		splitInput = strtok(userInput, " ");




		if(!strcmp(splitInput, "get")) {
			nbytes = sendto(sock, &get, sizeof(get), 0, (struct sockaddr *) &remote, sizeof(remote));
			splitInput = strtok(NULL, " ");
			nbytes = sendto(sock, splitInput, sizeof(splitInput), 0, (struct sockaddr *) &remote, sizeof(remote));
			return 0;

		}
		else {
			// Blocks till bytes are received
			nbytes = sendto(sock, &splitInput, sizeof(splitInput), 0, (struct sockaddr *) &remote, sizeof(remote));
			struct sockaddr_in from_addr;
			unsigned int addr_length = sizeof(from_addr);
			bzero(buffer,sizeof(buffer));
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &from_addr, (socklen_t *) &addr_length);
			printf("%s\n", buffer);
		}
	}

	while(!feof(fp))
	{
		fread(buffer, sizeof(char), MAXBUFSIZE, fp);
		printf("%s\n", buffer);
		nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &remote, sizeof(remote));
		printf("Sent: %i bytes\n", nbytes);
	}
	nbytes = sendto(sock, &message, sizeof(message), 0, (struct sockaddr *) &remote, sizeof(remote));

	//nbytes = sendto(sock, fileBuffer, sizeof(buffer), 0, (struct sockaddr *) &remote, sizeof(remote));
	// printf("Sent: %i bytes\n", nbytes);


	close(sock);

}
