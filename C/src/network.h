#ifndef __NETWORK_H__
	#define __NETWORK_H__

#include <inttypes.h>		//for uint16
#include <sys/time.h>    	//for fd_set
#include <netinet/in.h>  	//struct sockaddr

typedef struct {
	void * memShareAddr;
	int memShareFD;
	int memSeekInt;
	int serverSockFD;
	//Probably gonna end up putting an fd_set in here... the master set anyway, not the working set
} NetworkModule;

//Free's memory used by the NetworkModule
void destroyNetworkModule(NetworkModule * module);

//Instantiates the networkModule, returns -1 on failure 0 on success
int createNetworkModule(NetworkModule * module);

/*Set's up the memory share for the module and initializes network connections
	fd: File descriptor for file to write to
	module: NetworkModule struct to fill out with information
Really good example of nonblocking io.
http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=%2Frzab6%2Frzab6xnonblock.htm 

Returns -1 on failure, 0 on success
*/
int setupNetworkModule(int fd, NetworkModule * module);



/*Creates a socket for the server to run on
*	module NetworkModule structure to be filled out with the server information
*	returns -1 on failure, 0 on success
*/
int createServerSocket(NetworkModule * module);

/*
createSocket takes the port and string form of an ip address and returns
the socket descriptor (int) if successful in creating the port, or -1 on 
failure. Note that this binds the port. If you're trying to create a port
from for a listen command, this id one by the listen function
port: The port to be bound to
ip_address: The string representation of the ip address
address: The sockaddr_in struct to fill with the information for the socket (needed to be used by accept)
*/
int createSocket(uint16_t port, const char * ip_address,struct sockaddr_in * address);

/*Checks to see if there is any data ready to be accepted on the current set of sockets
	sockSet: 	The set of sockets we might accept data on
	maxFileDescriptor: The highest numbered file descriptor so far, this is needed for the polling
  This function will poll the fd_set given in sockSet, and possibly modify it. It is wise to create
  a working set from whatever socket set you're wanting to poll. Returns the number of ready sockets
  in the set, or -1 if there was an error
*/
int isDataReady(fd_set * sockSet, const int maxFileDescriptor );

/*Core logic loop of the Network Module
*	module: The NetworkModule structure to use to accept
*			incoming connections and write to mmaped file
*/
void runServer(NetworkModule * module);

void handleIncoming(int fd, NetworkModule * module);

#endif
