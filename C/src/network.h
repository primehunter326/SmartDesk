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
int createNetworkModule(int fd, NetworkModule * module);

int startServer(NetworkModule * module);

int startClient(NetworkModule * module);

void handleIncoming(int fd, NetworkModule * module);

#endif
