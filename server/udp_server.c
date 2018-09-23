#include <sys/types.h>
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
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
/* You will have to modify the program below */

// TODO: make files directory

#define MAXBUFSIZE 100

int main (int argc, char * argv[] )
{


	int sock;                           //This will be our socket
	struct sockaddr_in sin, remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	char compareBuffer[MAXBUFSIZE];
	char sendBuffer[MAXBUFSIZE];
	char fileName[MAXBUFSIZE];
	char folderName[MAXBUFSIZE] = "./files/";
	char filePath[MAXBUFSIZE];
	char errOpeningFile[] = "Error opening file.";
	char over[] = "Over";
	char fileDeleted[] = "File deleted.";
	char fileDNE[] = "File does not exit.";
	char errCommand[] = "Did not understand command.\n\n\n";
	int fd;

	DIR* dir;
	struct dirent* in_file;

	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine

	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf("unable to create socket");
	}

	/******************
	  Once we've created a socket, we must bind that socket to the
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}

	remote_length = sizeof(remote);
	bzero(&remote, remote_length);

	while(true)
	{
		bzero(buffer,sizeof(buffer));
		printf("Waiting for bytes...\n");
		nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, (socklen_t *) &remote_length);

		if(!strcmp(buffer, "put")) {
			bzero(buffer,sizeof(buffer));
			bzero(filePath,sizeof(filePath));
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, (socklen_t *) &remote_length);

			sprintf(fileName, "%s", buffer);
			strcat(filePath, folderName);
			strcat(filePath, fileName);

			umask(0);
			fd = open(filePath, O_CREAT | O_WRONLY | O_APPEND, 0755);
			if(fd < 0) {
				printf("Error opening file.\n");
				close(fd);
				continue;
			}

			while(true) {
				bzero(buffer,sizeof(buffer));
				nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, (socklen_t *) &remote_length);

				if(!strcmp(buffer, "Over")) {
					printf("Recieved Over\n");
					close(fd);
					break;
				}
				write(fd, buffer, sizeof(buffer));
			}
		}

		else if(!strcmp(buffer, "get")) {
			bzero(buffer,sizeof(buffer));
			bzero(filePath,sizeof(filePath));
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, (socklen_t *) &remote_length);

			sprintf(fileName, "%s", buffer);
			strcat(filePath, folderName);
			strcat(filePath, fileName);

			fd = open(filePath, O_RDONLY);
			if(fd < 0) {
				printf("Error opening file.\n");
				nbytes = sendto(sock, &errOpeningFile, sizeof(errOpeningFile), 0, (struct sockaddr *) &remote, sizeof(remote));
				close(fd);
				continue;
			}

			bzero(sendBuffer,sizeof(sendBuffer));
			while(read(fd, sendBuffer, sizeof(sendBuffer)) != 0) {
				nbytes = sendto(sock, &sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr *) &remote, sizeof(remote));
			}
			nbytes = sendto(sock, &over, sizeof(over), 0, (struct sockaddr *) &remote, sizeof(remote));
			close(fd);
		}

		else if(!strcmp(buffer, "ls")) {
			dir = opendir("./files");
			if(dir == NULL) {
				printf("Error opening directory\n");
				return -1;
			}

			bzero(buffer,sizeof(buffer));
			while((in_file = readdir(dir))) {
				if(!strcmp(in_file->d_name, ".") || !strcmp(in_file->d_name, ".."))
					continue;
				strcat(buffer, in_file->d_name);
				strcat(buffer, " ");
			}

			buffer[strlen(buffer) - 1] = '\0';
			nbytes = sendto(sock, &buffer, strlen(buffer), 0, (struct sockaddr *) &remote, remote_length);
		}

		else if(!strcmp(buffer, "delete")) {
			bzero(buffer,sizeof(buffer));
			bzero(filePath,sizeof(filePath));
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, (socklen_t *) &remote_length);

			sprintf(fileName, "%s", buffer);
			sprintf(fileName, "%s", buffer);
			strcat(filePath, folderName);
			strcat(filePath, fileName);

			int ret = 1;
			ret = remove(filePath);

		  if(ret == 0) {
				printf("File deleted successfully\n");
				nbytes = sendto(sock, &fileDeleted, strlen(fileDeleted), 0, (struct sockaddr *) &remote, remote_length);
		  }
			else {
				printf("Error: unable to delete the file\n");
				nbytes = sendto(sock, &fileDNE, strlen(fileDNE), 0, (struct sockaddr *) &remote, remote_length);
		  }
		}

		else if(!strcmp(buffer, "exit")) {
			close(sock);
			return 0;
		}
		else {
			nbytes = sendto(sock, &errCommand, strlen(errCommand), 0, (struct sockaddr *) &remote, remote_length);
		}
  }
}
