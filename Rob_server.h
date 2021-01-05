#ifndef ROB_SERVER_H
#define ROB_SERVER_H

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/wait.h>
#include <signal.h>
#include <thread>
#include <vector>



class Rob_server{
	
	public:
		int make_accept_sock(const char* port);
	
};

#endif 