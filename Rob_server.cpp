#include "Rob_server.h"

int Rob_server::make_accept_sock(const char* port){
	
	struct addrinfo hints;
	memset(&hints, 0, sizeof(int));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	struct addrinfo* servinfo;
	int rv = getaddrinfo(NULL, port, &hints, &servinfo);
	
	if(rv != 0){
		std::cerr<<"getaddrinfo: "<<gai_strerror(rv)<<"\n";
		exit(EXIT_FAILURE);
	}
	struct addrinfo *p;
	int sockfd;
	int yes{1};
	
	for(p = servinfo; p!=NULL; p=p->ai_next){
		
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			std::cerr<<"server: failed to create socket \n";
			exit(EXIT_FAILURE);
			continue;
		}
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			std::cerr<<"server failed setsockopt\n";
			exit(EXIT_FAILURE);
			//continue;
		}
		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			std::cerr<<"server failed bind \n";
			continue;
			//exit(EXIT_FAILURE);
		}
		break;
	}
	
	if(p == NULL){
		std::cerr<<"server failed to bind \n";
		exit(EXIT_FAILURE);
	}
	
	freeaddrinfo(servinfo);
	
	const int BACKLOG{10};
	if(listen(sockfd, BACKLOG) == -1){
		std::cerr<<"server failed to listen \n";
		exit(EXIT_FAILURE);
	}
	return sockfd;
}






