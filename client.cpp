#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <strings.h>
#include <sstream>
#include <iomanip>

#define buff_size 256 //max length for the name passed
#define cliPort 21543
#define sa struct sockaddr

char inputData[buff_size] = {0};
char recvRes[buff_size] = {0};

using namespace std;

// Function declaration for message exchange between client and server
void sendInputData(int sock, char* id, char* sz);
void displayRes(string);

int main(int argc, char* argv[]) {
	int soc_c, flag, dynPort;
	char* id = argv[1]; //stores id entered during execution
    char* size = argv[2]; //stores size entered during execution
	struct sockaddr_in serverAddr;
    socklen_t sa_len;

	// socket creation and verification
	soc_c = socket(AF_INET, SOCK_STREAM, 0); //TCP connection
	if (soc_c == -1) {
		cout<<"Socket creation failed."<<endl;
		exit(0);
	}
	/*else
		cout<<"Socket successfully created."<<endl;*/
	bzero(&serverAddr, sizeof(serverAddr));

	// assign IP, PORT
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(cliPort);

	// Client connection to Server and its verification
	if (connect(soc_c, (sa*)&serverAddr, sizeof(serverAddr)) != 0) {
		cout<<"The Client is down."<<endl;
		exit(0);
	}
	else
	   cout<<"The Client is up and running."<<endl;

	//Dynamic PORT assignment
	sa_len=sizeof(serverAddr);
	flag=getsockname(soc_c, (sa*)&serverAddr, &sa_len);
	if (flag==-1) {
		perror("getsockname() failed");
		exit(1);
	}
	else
		dynPort=ntohs(serverAddr.sin_port);

	// Function call for message exchange between client and server
	sendInputData(soc_c, id, size);

	// Closing the socket after exchange
	close(soc_c);

    return 0;
}

// Function designed for message exchange between client and server.
void sendInputData(int sock, char* id, char* sz) {
	char buf[buff_size];
    strcat (inputData, id);
    strcat (inputData, " ");
    strcat (inputData, sz);
    strcat (inputData, " ");
	//int n;
	cout<<"Sent link "<<id<<" and File Size "<<sz<<"MB to Main Server."<<endl;
	write(sock, inputData, sizeof(inputData));
    bzero(buf, sizeof(buf));
	read(sock, buf, sizeof(buf));
	if ((strncmp(buf, "No", 2)) == 0)
		cout<<"No match found."<<endl;
	else{
		cout<<"Received ";
		string result(buf);
		displayRes(result);
	}

	if ((strncmp(buf, "exit", 4)) == 0) {
		cout<<"Client Exit."<<endl;
		exit(1); //break;
	}
}

void displayRes(string r){
    float tt,tp,sum;
    string t, p, s, dat;
    //cout<<endl<<"Res: "<<r<<endl;

    int pos=r.find(" ");
    t=r.substr(0,(pos));
    dat=r.substr(pos+1);
    stringstream stt(t);
    stt >> tt;

    int pos1=dat.find(" ");
    p=dat.substr(0,(pos1));
    dat=dat.substr(pos1+1);
    stringstream stp(p);
    stp >> tp;

    int pos2=dat.find(" ");
    s=dat.substr(0,(pos2));
    stringstream ssum(s);
    ssum >> sum;

    cout<<"Transmission Delay "<<std::fixed<<std::setprecision(2)<<tt<<"ms, ";
    cout<<"Propagation Delay "<<std::fixed<<std::setprecision(2)<<tp<<"ms, ";
    cout<<"and Total Delay "<<std::fixed<<std::setprecision(2)<<sum<<"ms."<<endl<<endl;

}
