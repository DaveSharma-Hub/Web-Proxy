/*
Author: Dave Sharma 
Filename: proxy.c
Date: October 1, 2021
*/
/*
References:
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/examples/testserver.c // parts of Dr. Carey's server code used to create main server-side of Proxy
https://d2l.ucalgary.ca/d2l/le/content/401094/viewContent/4896480/View // TA's notes presented on HTTP used in server and client side of Proxy
https://www.geeksforgeeks.org/socket-programming-cc/ //Used in secondary server to telnet to Proxy
https://www.binarytides.com/socket-programming-c-linux-tutorial/ //Used in creating client-side of Proxy
https://sites.google.com/view/sinacpsc441/content //Sina Keshvadi's notes used to understand the required Proxy contents
https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/ //Used to retrieve the ip address of hostname url
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>

#define MAX_MESSAGE_LENGTH 10000
#define MAX_MESSAGE_LENGTH2 100
#define SERVERPORTNUM2 54321
#define PORT 8080
#define LENGTH 50000
/* Optional verbose debugging output */
#define DEBUG 1
/* Global variable */
	  int childsockfd; //accepts connection from parentsock

/* This is a signal handler to do graceful exit if needed */
void catcher( int sig )
  {
    close(childsockfd);
    exit(0);
  }

int hostname_to_ip(char *  , char *);


/* Main program for Proxy */
int main()
  {
	  int blockedCounter=0; //Determine the size of blocked list

puts("Please enter port number: "); //asks user to enter the port number that they want to use for Proxy
char serverportnum_string[50]; //Stores proxy port number
scanf("%s",serverportnum_string);
int SERVERPORTNUM=atoi(serverportnum_string);
	struct timeval tv; // Used to set the timer on the proxy

	int bytes;
	int done = 0;
	int socket_desc;
	struct sockaddr_in serverClient; //creating the struct for client side of Proxy
	char serverClient_reply[LENGTH]; //stores the message to send back to web browser

	int client2;
	struct sockaddr_in serverClient2;
	

 int server_fd, new_socket, valread;
	 struct sockaddr_in addressD;
	 int opt=1;
	 int addrlen = sizeof(addressD);
	 char buffer[1024]={0};
	 //char *hello="Hello from server"; 
	 char blocked[10][20]; // Blocked list
	
	int val=0;
    struct sockaddr_in server, client;
    static struct sigaction act;
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
    int parentsockfd, c, pid, conns;

    /* Set up a signal handler to catch any unusual termination conditions. */
    act.sa_handler = catcher;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, NULL);

    /* Initialize server sockaddr structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVERPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* set up the transport-level end point to use TCP */
    if( (parentsockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1 )
      {
	fprintf(stderr, "testserver: socket() call failed!\n");
	exit(1);
      }

    /* bind a specific address and port to the end point */
    if( bind(parentsockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in) ) == -1 )
      {
	fprintf(stderr, "testserver: bind() call failed!\n");
	exit(1);
      }

    /* start listening for incoming connections from clients */
    if( listen(parentsockfd, 5) == -1 )
      {
	fprintf(stderr, "testserver: listen() call failed!\n");
	exit(1);
      }

    /* initialize message strings just to be safe (null-terminated) */
    bzero(messagein, MAX_MESSAGE_LENGTH);
    bzero(messageout, MAX_MESSAGE_LENGTH);

    fprintf(stderr, "Welcome! I am the CPSC 441 test server for L01!!\n");
    fprintf(stderr, "Server listening on TCP port %d...\n\n", SERVERPORTNUM);

    conns = 0;
    c = sizeof(struct sockaddr_in);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
   // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


	tv.tv_sec = 5;
	tv.tv_usec = 0;
	//Set timer for the server telnet socket
		if(setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv)){
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

	addressD.sin_family = AF_INET;
    addressD.sin_addr.s_addr = INADDR_ANY;
    addressD.sin_port = htons( PORT );
	/*Bind the server created for telneting*/
	if (bind(server_fd, (struct sockaddr *)&addressD, sizeof(addressD))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	/*Listen to server telnet*/ 
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
	
	/* Accept the socket from the parentsock for telneting*/
    if ((new_socket = accept(server_fd, (struct sockaddr *)&addressD,(socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
	
    /* Main loop: server loops forever listening for requests */
    for( ; ; ){
     		bzero(messagein, MAX_MESSAGE_LENGTH);
	
		int bytes2;
	    int done2 = 0;
	
			  int testBlock=0;
			  int testUnblock;
		val++;
		printf("%d",val);
		bzero(buffer,1024);
		valread= read( new_socket , buffer, 1024);
		printf("%s\n",buffer );
    	printf("Hello message sent\n");
				char* testing1=buffer;
				char* testing2=buffer;	
		if(strlen(buffer)>=7){//Parse through telnet to determine if BLOCK or UNBLOCK
			 if(buffer[0]=='U' &&buffer[1]=='N'&&buffer[2]=='B'&&buffer[3]=='L' &&buffer[4]=='O'&&buffer[5]=='C' &&buffer[6]=='K'){
				testUnblock=1;
				testBlock=0;
				//blockedCounter=0;
				blockedCounter--;
			}
		}
		if(strlen(buffer)>=5){
			if(buffer[0]=='B' &&buffer[1]=='L'&&buffer[2]=='O'&&buffer[3]=='C' &&buffer[4]=='K'){
				testBlock=1;
				testUnblock=0;
			}
		}
		
		if(testBlock==1){// If BLOCK, determine what to BLOCK
			testBlock=0;
			int strleng=(strlen(buffer));
			int i=0;
			while(testing1[i]!='.'){
				i++;
			}
			//i--;
			char blockstring[i-6];
			int test1Counter=0;
			bzero(blockstring,i-6);
		
			test1Counter=6;
			for(int i=0;testing1[test1Counter]!='.';i++){
				blockstring[i]=testing1[test1Counter];
				test1Counter++;
			}
			
				puts(blockstring);
				strcpy(blocked[blockedCounter],blockstring);
				puts(blocked[blockedCounter]);
				blockedCounter++;
				printf("%d",blockedCounter);
		}
		
		
		//Childsockfd accepts from the parentsockfd 
		if( (childsockfd = accept(parentsockfd, (struct sockaddr *)&client, (socklen_t *) &c)) == -1 )
	  {
	    fprintf(stderr, "testserver: accept() call failed!\n");
	    exit(1);
	  }

		bytes = recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0);//recieve message from web browser

	socket_desc=socket(AF_INET,SOCK_STREAM,0);
	if(socket_desc==-1){
		printf("Could not create socket");
	}
		char *hostname = messagein;
	char ip[100];
	struct hostent *he;
	struct in_addr **addr_list;
	//int i;
/*Parse through message to find the url without http:// */
	char *tmpHolder=hostname;
	int tmpHolderSize=strlen(tmpHolder);
    char parsing[500];
    int counterChecker=0;
    char checker[400];
    for(int i=0;i<tmpHolderSize-1;i++){
        if(tmpHolder[i]=='/'&&tmpHolder[i+1]=='/'){
            counterChecker=i+1;
            break;
        }
    }
    counterChecker++;
    for(int i=0;i<tmpHolderSize;i++){
        if(tmpHolder[counterChecker]=='a'&&tmpHolder[counterChecker-1]=='c'&&tmpHolder[counterChecker-2]=='.'){
            parsing[i]=tmpHolder[counterChecker];
            break;
        }
        else{
         parsing[i]=tmpHolder[counterChecker];
         counterChecker++;
        }
    }

	hostname=parsing;
	
	hostname_to_ip(hostname,ip);//determine the ip address using the url
	
	serverClient.sin_addr.s_addr=inet_addr(ip);
	serverClient.sin_family=AF_INET;
	serverClient.sin_port=htons( 80 );
//Connect the client-side of proxy to http servers
	if(connect(socket_desc,(struct sockaddr*) &serverClient ,sizeof(serverClient)) <0){
		puts("connect error");
		return 1;
	}
	
	char *message;
	
	char messageHolder[MAX_MESSAGE_LENGTH];
		
    int checkerBlocked=0;
/*Look through blocked list to determine if url, GET request has any blocked words */
	char messageinHolder[strlen(messagein)];
	strcpy(messageinHolder,messagein);
		for(int index=0;index<blockedCounter;index++){				
		 	if((strstr(messagein,(blocked[index])))!=NULL){
				checkerBlocked= 1;
			}
		}

	puts("checkerBlocked");
	printf("%d",checkerBlocked);
	/*Send an error page request to HTTP server*/
	if(checkerBlocked==1){ 
	   
	bzero(messageHolder,strlen(messageHolder));
	strcpy(messageHolder,"GET /~carey/CPSC441/ass1/error.html HTTP/1.1\r\nHost: 136.159.2.17\r\nContent-Type: text/plain\r\n\r\n");
	
	if(send(socket_desc,messageHolder,strlen(messageHolder),0)<0){
            puts("send failed");
            return 1;
		}
	}
	else{
		strcpy(messageHolder,messagein);
		if(send(socket_desc,messageHolder,strlen(messageHolder),0)<0){
            puts("send failed");
            return 1;
		}
	}
	
			bzero(serverClient_reply,LENGTH);
			int webData;
			int bonusCheck=0;
/*While have not recieved the entire message keep looping, especially for images*/
		while((webData=recv(socket_desc ,serverClient_reply ,LENGTH,0))>0){ //,0 MSG_WAITALL
	
		int checkerBlockedBack=0;

		char serverTMP[strlen(serverClient_reply)];
		strcpy(serverTMP,serverClient_reply);
		//Determine if return message has a word in the blocked list
		for(int i=0;i<blockedCounter;i++){
				
			char *tmp1=strstr(serverClient_reply,(blocked[i]));
		 		if((tmp1)!=NULL){
				checkerBlockedBack=1;
			}
		}
	if(checkerBlockedBack==1){ //if message does have the blocked list send back the error page html code

		bzero(serverClient_reply,strlen(serverClient_reply));
		strcpy(serverClient_reply,"HTTP/1.1 200 OK\r\nContent-Length: 334\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n<html>\r\n\r\n<title>\r\nSimple URL Censorship Error Page for CPSC 441 Assignment 1\r\n</title>\r\n\r\n<body>\r\n\r\n<h1>NO!!</h1>\r\n\r\n<p>\r\nSorry, but the Web page that you were trying to access\r\nhas a <b>URL</b> that is inappropriate.\r\n</p>\r\n<p>\r\nThe page has been blocked to avoid insulting your intelligence.\r\n</p>\r\n\r\n<p>\r\nWeb Censorship Proxy\r\n</p>\r\n\r\n</body>\r\n\r\n</html>");
		if(send(childsockfd,serverClient_reply,strlen(serverClient_reply),0)<0){ //0
            puts("send failed");
            return 1;
		}
		webData=-1;
		goto L1;
	}
		else{
			bonusCheck=0;
		int clientlength=strlen(serverClient_reply);
		if(send(childsockfd,serverClient_reply,webData,0)<0){ //0
            puts("send failed");
            return 1;
			}
		}
	}
L1:
	    
	} /* end of then part for child */
	
	  {
	    /* the parent process is the one doing this part */
	    fprintf(stderr, "Created child process %d to handle that client\n", pid);
	    fprintf(stderr, "Parent going back to job of listening...\n\n");

	    /* parent doesn't need the childsockfd */
	    close(childsockfd);
	  }
  	}
/*Used to determine the ip from the hostname. Code from https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/ */
  int hostname_to_ip(char *hostname , char *ip)
{
	int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		h = (struct sockaddr_in *) p->ai_addr;
		strcpy(ip , inet_ntoa( h->sin_addr ) );
	}
	
	freeaddrinfo(servinfo); // all done with this structure
	return 0;
}