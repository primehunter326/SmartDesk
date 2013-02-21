/*
Ethan Eldridge
January 27th 2013

This file implements the common networking components for all
base modules. Defining an abstraction that will be easy to use
by including network.h

*/

//Types for compatability with BSD
#include <sys/types.h>
#include <sys/socket.h> 
//If we don't include netinet then we dont know the sizeof(sockaddr_in)
#include <netinet/in.h> 
//Get the correct header for inet_ntoa
#include <arpa/inet.h>
//For perror
#include <stdio.h>
//For binding in non blocking mode
#include <fcntl.h>
//Let's get our assert on
#include <assert.h>
//Rather random, but file descriptor sets are stored in time. who knows why... Aliens.
#include <sys/time.h> 
//For EAGAIN
#include <errno.h>
//Need mmap for interprocess comm
#include <sys/mman.h>
//malloc and free
#include <stdlib.h>
//Odd enough, memset is in string.h
#include <string.h>

//Non standard includes:
#include "conf.h"
#include "network.h"

//Work around for my apparently messed up header:
#ifndef SOL_TCP
	#define SOL_TCP 6
#endif
#ifndef TCP_NODELAY
	#define TCP_NODELAY 1
#endif



//Free's memory used by the NetworkModule
void destroyNetworkModule(NetworkModule * module){
	close(module->memShareAddr);
	close(module->memShareFD);
	close(module->serverSockFD);
	module->memSeekInt =0;
}


/*Creates a NetworkModule and binds a socket to 
	fd: File descriptor for file to write to
	module: NetworkModule struct to fill out with information
Really good example of nonblocking io.
http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzab6%2Frzab6xnonblock.htm 
Returns -1 on failure, 0 on success
*/
//TODO: this could take a parameter specifying whether this is a server or a client
int createNetworkModule(int fd, NetworkModule * module){
	module->memSeekInt = 0;
	//Set up the memory share:
	// Write only for the network, it doesn't need to read it at all
	module->memShareAddr = mmap(NULL, MEMSHARESIZE, PROT_WRITE, MAP_SHARED, fd, 0);
	if(module->memShareAddr == MAP_FAILED){
		puts("Failed to map memory share to network module");
		perror("heressomehelp");
        munmap(module->memShareAddr,MEMSHARESIZE);
		return -1;
	}
	module->memShareFD = fd;
    module->serverSockFD = socket(AF_INET, SOCK_STREAM, 0);
    if(module->serverSockFD < 0)
		return -1;
    return 0;
}

//starts the network module as a client to connect to a server
/*
int startClient(NetworkModule * module){
    char *host = "localhost";
    struct sockaddr_in serv_addr;
    struct hostent *server;
    server = gethostbyname(host);   
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(SERVERPORT); //SERVERPORT hardcoded in conf.h
    if (connect(module->serverSockFD,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        puts("Error connecting to server");
    //here's where stuff happens
    return 0;
}
*/

//starts the network module as a server that listens for and responds to connections
int startServer(NetworkModule * module){
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    socklen_t servlen = sizeof(serv_addr);
    int clientSockFD, childPID, result;

    //Set up serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVERPORT); //SERVERPORT hardcoded in conf.h
    
    result = bind(module->serverSockFD,(struct sockaddr *) &serv_addr,servlen);
    if (result < 0){
    	puts("Error binding socket");
        return -1;
    }
    listen(module->serverSockFD,5);
    puts("Listening for connections...");
 
    //fork the process every time a connection is established
    //so multiple clients can connect at once
    while(1){
        clientSockFD = accept(module->serverSockFD,
                            (struct sockaddr *) &cli_addr, &clilen);
        childPID = fork();
        if (childPID < 0)
            puts("Something broke when forking");
        if (childPID == 0){
            close(module->serverSockFD);
            handleIncoming(clientSockFD, module);
            return 0;
        }
        else
            close(clientSockFD);
    }
}

void handleIncoming(int fd, NetworkModule * module){
	char buffer[256];
	memset(buffer,0,256);

	int bytesRead = 0;
	bytesRead = read(fd,buffer,255);

	if(bytesRead < 0){
		puts("I read no bytes");
		return;
	}else{
		printf("I read %d bytes \n", bytesRead);
	}

	printf("%s\n", buffer);

	//Handle the Size of our file pointer getting crazy
	if(module->memSeekInt + bytesRead >= MEMSHARESIZE){
		//Essentially loop it around
		module->memSeekInt=0;
	}

	//Load the buffer to the shared memory
	//There should probably be some semaphore action going on
	int i;
	for(i =0; i < bytesRead; i++){
		//Write to the buffer at the current seek position
		*(((int *)module->memShareAddr)+module->memSeekInt+i) = buffer[i];	
	}
	module->memSeekInt = module->memSeekInt + bytesRead;	

	msync(module->memShareAddr,sizeof(int),MS_SYNC|MS_INVALIDATE);
}



//test everything out
//int main(){ 
//    
//   int fd = createMemShare();
//    NetworkModule module;
//    createNetworkModule(fd,&module);
//    startServer(&module);
//    destroyNetworkModule(&module);
//    return 0;
//}
