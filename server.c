#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MYPORT 8080 /* Avoid reserved ports */
#define BACKLOG 10 /* pending connections queue size */
//#define BUFFER 512

char* HTTP = "HTTP/1.1 ";
char* INTER_ERROR_500 = "500 Internal Error\n";
char* INTER_ERROR_404 = "404 Not Found\n";
char* CONNECTION_CLOSE = "Connection: Close\n";
char* CONTENT_LENGTH = "Content-Length: 0\n";
char* SERVER_NAME = "Ethan's Server\n";
char* CONTENT_TYPE = "Content-Type: text/html;\n";
char* HTTP_200 = "200 OK\n";
char* CONTENT_LANGUAGE = "Content-Language: en-US\n";

int main(int argc, char *argv[])
{
	
	int sockfd, new_fd; /* listen on sock_fd, new connection on new_fd */
	int port;
	struct sockaddr_in my_addr; /* my address */
	struct sockaddr_in their_addr; /* connector addr */
	int sin_size;
	/* create a socket */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		fprintf(stderr, "socket\n");
		exit(1);
	}

	if (argc != 2)
	{
		fprintf(stderr, "port not specified\n");
		exit(1);
	}
	int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(enable), sizeof(int)) < 0)
    {
        fprintf(stderr, "setsockopt(SO_REUSEADDR) failed\n");
        exit(1);
    }




	port = atoi(argv[1]);
	/* set the address info */
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port); /* short, network byte order */
	//my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	/* INADDR_ANY allows clients to connect to any one of the host’s IP address. Optionally, use this
	line if you know the IP to use:
	my_addr.sin_addr.s_addr = inet_addr(“127.0.0.1”);
	*/
	//my_addr.sin_addr.s_addr = inet_addr(“127.0.0.1”);
	memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));
	/* bind the socket */
	
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) 
	{
		fprintf(stderr, "bind");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		fprintf(stderr, "listen");
		exit(1);
	}


	while (1) 
	{ /* main accept() loop */
		
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size)) == -1) 
		{
			fprintf(stdout, "accept\n");
			exit(1);
		}
		printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
		/*pid = fork();
		if (pid < 0)
		{
			perror("fork");
			exit(1);
		}
		else if (pid == 0) // child
		{
			close(sockfd);
			interpret(new_fd);
			exit(0);
		}
		else
		{
			close(new_fd);
		}*/

		char buf[512];
		char file[1024];
		memset(buf, 0, sizeof(buf));
		memset(file, 0, sizeof(file));
		int request;
		request = read(new_fd, buf, 512);
		if (request < 0)
		{
			fprintf(stderr, "read\n");
			exit(1);
		}
		buf[request - 1] = '\0';
		printf("HTTP Request: \n%s", buf);
		char* begin = strstr(buf, "GET /");
		if (begin == buf)
		{
			begin += 5;
		}
		else
		{
			write(new_fd, HTTP, sizeof(HTTP));
			write(new_fd, INTER_ERROR_500, sizeof(INTER_ERROR_500));
			fprintf(stderr, "Request not supported\n");
			exit(1);
		}

		char* finish = strstr(begin, " HTTP/");
		int length = finish - begin;
		char temp[10];
		memset(temp, 0, sizeof(temp));
		strncpy(file, begin, length);
		file[length] = '\0';

		char* space = strstr(begin, "%20");
		if (space != NULL)
		{
			int remainder = finish - space;
			remainder -= 3;
			space = strstr(file, "%20");
			strncpy(temp, space + 3, remainder);
			strcpy(space, " ");
			strcat(space, temp);
		}

		fprintf(stderr, "%s\n", file);
		char suffix[4];
		char type[20];

		char* beginSuffix = strstr(file, ".");
		if (beginSuffix != NULL)
		{
			strcpy(suffix, beginSuffix + 1);

			if(strcmp(suffix, "jpg") == 0)
			{
				strcpy(type, "image/jpeg");
			}
			else if(strcmp(suffix, "gif") == 0)
			{
				strcpy(type, "image/gif");
			}
			else if(strcmp(suffix, "html") == 0 || strcmp(suffix, "txt") == 0)
			{
				strcpy(type, "text/html");
			}
			/*else
			{
				strcpy(type, "application/octet-stream");
			}*/
		}
		else
		{
			strcpy(type, "application/octet-stream");
		}

		write(new_fd, HTTP, sizeof(HTTP));

		struct stat bufferStat;
		if (length <= 0 || stat(file, &bufferStat) != 0)
		{
			printf("404 Not Found\n");
			write(new_fd, INTER_ERROR_404, sizeof(INTER_ERROR_404));
			write(new_fd, SERVER_NAME, sizeof(SERVER_NAME));
			write(new_fd, CONTENT_LENGTH, sizeof(CONTENT_LENGTH));
			write(new_fd, CONNECTION_CLOSE, sizeof(CONNECTION_CLOSE));
			write(new_fd, CONTENT_TYPE, sizeof(CONTENT_TYPE));
			exit(1);
		}

		write(new_fd, HTTP_200, sizeof(HTTP_200));
		write(new_fd, CONTENT_LANGUAGE, sizeof(CONTENT_LANGUAGE));

		/*FILE *infile;
		if (beginSuffix == NULL)
		{
			infile = fopen(file, "rb");
		}
		else
		{
			infile = fopen(file, "r");
		}*/
		FILE *infile = fopen(file, "r");
		
		fseek(infile, 0L, SEEK_END);
		int filesize = (int)ftell(infile);
		fseek(infile, 0L, SEEK_SET);


		sprintf(buf, "Content-Length: %d\n", filesize); // copy to buf
		write(new_fd, buf, strlen(buf)); // copy buf to broswer

		if(strlen(type) > 0)
		{
			sprintf(buf, "Content-Type: %s\n\n", type); // copy to buf
			write(new_fd, buf, strlen(buf)); // copy buf to browser
		}

		if(ferror(infile))
		{
			fprintf(stderr, "could not read file\n");
			exit(1);
		}

		char* mem = (char*)malloc(sizeof(char) * filesize); // allocate space in memory for file contents
		fread(mem, 1, filesize, infile); // read file into memory 

		int test;
		test = write(new_fd, mem, filesize);
		if (test < 0)
		{
			fprintf(stderr, "write\n");
			exit(1);
		}

		free(mem);
		fclose(infile);
		close(new_fd);
	
		close(sockfd);
	}
}
