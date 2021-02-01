#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <strings.h>
#include <sstream>
#include <iomanip>

#define buff_size 256 //max length for the name passed
#define mntrPort 25543
#define sa struct sockaddr

using namespace std;

void mntrMain(int sock);

int main() {
	int soc_o, conn_o, flag, mdynPort;
	struct sockaddr_in mntrAddr;
    socklen_t ms_len;

    // TCP socket creation and verification
	soc_o = socket(AF_INET, SOCK_STREAM, 0); //TCP connection
	if (soc_o == -1) {
		cout<<"Socket creation failed."<<endl;
		exit(0);
	}
	/*else
		cout<<"Socket successfully created."<<endl;*/
    bzero(&mntrAddr, sizeof(mntrAddr));

	// assign IP, PORT
	mntrAddr.sin_family = AF_INET;
	mntrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	mntrAddr.sin_port = htons(mntrPort);

    // Binding newly created socket to given IP and its verification
	if ((bind(soc_o, (sa*)&mntrAddr, sizeof(mntrAddr))) != 0) {
		cout<<"Socket binding failed."<<endl;
		exit(0);
	}
	/*else
		cout<<"Socket successfully binded."<<endl;*/

	// Server listening and its verification
	if ((listen(soc_o, 20)) != 0) {
		cout<<"Listen failed."<<endl;
		exit(0);
	}
	/*else
		cout<<"Monitor listening."<<endl;*/

	//Dynamic PORT assignment
	ms_len=sizeof(mntrAddr);
	flag=getsockname(soc_o, (sa*)&mntrAddr, &ms_len);
	if (flag==-1) {
		perror("getsockname() failed");
		exit(1);
	}
	else
		mdynPort=ntohs(mntrAddr.sin_port);

	conn_o = accept(soc_o, (sa*)&mntrAddr, &ms_len);
	cout<<"The monitor is up and running."<<endl;

    while(1)
		mntrMain(conn_o);

    return 0;
}

void mntrMain(int sock){
    char buf[buff_size];
    bzero(buf, sizeof(buf));
	read(sock, buf, sizeof(buf));
	cout<<buf<<endl;

    if ((strncmp(buf, "exit", 4)) == 0) {
		cout<<"Monitor Exit."<<endl;
		exit(1); //break;
	}
}
