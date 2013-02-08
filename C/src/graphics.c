/*
*Ethan Eldridge
*January 30 2013
*
*The purpose of this file is to provide graphics to the program
*and to define how to read the memory mapped file
*
* To be honest I've gotta set up some type of protocol for reading from the
*thing. It's easy enough to sync the memory on each call, but how do I know
*if I'm reading 'too fast' and the networking isn't giving me anything? There
*needs to be some null value coming in from the file or something. Or a global
*count of how many bytes I've written so far. Then graphics reads up to that, if
*the reading count is higher than the writting count than the writer looped around
*the file and began writing at the beginning, so we need to reflect that...
*
* I could have the writer write the offset it's on at the 0th byte so the reader 
*could just msync, read that, and then continue on... 
*/

//System includes
#include <sys/mman.h>		//memory mapped files
#include <stdlib.h>			//malloc and free
#include <stdio.h>			//puts

//Local includes
#include "conf.h"

typedef struct{
	void * memShareAddr;
	int memShareFD;
} GraphicModule;


int setupGraphicsModule(int fd, GraphicModule * module){
	//Setup the void pointer for the mapped file
	module->memShareAddr = (void*)malloc(sizeof(void*));
	module->memShareFD = -1;
	if(module->memShareAddr == NULL){
		puts("Failed allocating memory for Graphics Module");
		return -1;
	}

	module->memShareAddr = mmap(NULL, MEMSHARESIZE, PROT_READ, MAP_SHARED, fd, 0);
	if(module->memShareAddr == MAP_FAILED){
		puts("Failed to map memory share to network module");
		free(module->memShareAddr);
		return -1;
	}
	module->memShareFD = fd;
	return 0;
}
