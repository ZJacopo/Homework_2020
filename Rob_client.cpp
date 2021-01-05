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

const int MAXDATASIZE{100};

void *get_in_addr(struct sockaddr *sa){
	
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]){
	
	if(argc != 3){
		std::cerr<<"usage: ./client hostname port\n";
		exit(EXIT_FAILURE);
	}
	
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0){
		std::cerr<<"getaddrinfo: "<<gai_strerror(rv)<<"\n";
		exit(EXIT_FAILURE);
	}
	
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			std::cerr<<"failed to create socket: \n";
			continue;
		}
		
		if(connect(sockfd, p->ai_addr, p->ai_addrlen) == 1){
			close(sockfd);
			std::cerr<<"failed to connetc socket...\n";
			continue;
		}
		break;		
	}
	
	if(p == NULL){
		std::cerr<<"client: failed to connect\n";
		exit(EXIT_FAILURE);
	}
	
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
	
	std::cout<<"client: connetcting to "<<s<<"\n";
	
	freeaddrinfo(servinfo);
	
	
	std::cout<<"waiting for instructions \n";
	std::string robot_state="not operating";
	
	again:
	{	
		
		if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1){
				std::cerr<<"client failed to receive data \n";
				exit(EXIT_FAILURE);
			 }
		buf[numbytes] = '\0';
		std::cout<<buf<<"\n";
		
		if(numbytes==8){
			robot_state="operating";
			std::cout<<"robot state is: "<<robot_state<<" \n";			
		}
		if(numbytes==9){
			robot_state="not operating";
			std::cout<<"robot state is: "<<robot_state<<" \n";	
		}
	
	}
	if(numbytes!=11){goto again;}
	robot_state="OFF, not operating, today I worked hard";
	std::cout<<"robot state is: "<<robot_state<<" \n";
	close(sockfd);
	return 0;
}






