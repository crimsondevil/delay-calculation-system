#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <strings.h>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#define buff_size 256 //max length for the name passed
#define cliPort 21543
#define mainPort 22543
#define dbPort 23543
#define calcPort 24543
#define mntrPort 25543
#define sa struct sockaddr

char inputData[buff_size] = {0};
char outputData[buff_size] = {0};
char logData[buff_size] = {0};

using namespace std;

// Function declaration for message exchange between client and server
void recvInputData(int sock);
void splitInput(int&,int&);
void showData(string);
void res(string);

int main() {
    int soc_s, conn_s, soc_m, soc_o, conn_o, n, id=0, size=0, response, bnd, flag=0;
    struct sockaddr_in serverAddr, clientAddr, dbaddr, calcaddr, mainaddr, mntrAddr;
    char buffer[buff_size] = {0}, data[buff_size], calc[buff_size];
    socklen_t len;

	// TCP client socket creation and verification
	soc_s = socket(AF_INET, SOCK_STREAM, 0);
	if (soc_s == -1) {
		cout<<"Client socket creation failed."<<endl;
		exit(0);
	}
	/*else
		cout<<"Client socket successfully created."<<endl;*/
	bzero(&serverAddr, sizeof(serverAddr));

	// assign IP, PORT
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(cliPort);

	// Binding newly created socket to given IP and its verification
	if ((bind(soc_s, (sa*)&serverAddr, sizeof(serverAddr))) != 0) {
		cout<<"Socket binding failed."<<endl;
		exit(0);
	}
	/*else
		cout<<"Socket successfully binded."<<endl;*/

	// Server listening and its verification
	if ((listen(soc_s, 20)) != 0) {
		cout<<"Listen failed."<<endl;
		exit(0);
	}
	/*else
		cout<<"Server listening."<<endl;*/

    socklen_t db_addr_len = sizeof(dbaddr);

    // Running self as server (For listening)
	bzero(&mainaddr, sizeof(mainaddr));
  	mainaddr.sin_family = AF_INET;
	mainaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	mainaddr.sin_port = htons(mainPort);

    // UDP socket creation and verification
	soc_m = socket(PF_INET, SOCK_DGRAM, 0);
    if (soc_m < 0) {
        cout << "ERROR: Setting socket" << endl;
    }
    bnd = bind(soc_m, (sa *)&mainaddr, sizeof(mainaddr));
    if (bnd < 0) {
        cout << "ERROR: Binding socket" << endl;
    }
    cout<<"The Main Server is up and running."<<endl;

    bzero(&dbaddr, sizeof(dbaddr));
  	dbaddr.sin_family = AF_INET;
	dbaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	dbaddr.sin_port = htons(dbPort);

	socklen_t calc_addr_len = sizeof(calcaddr);

	bzero(&calcaddr, sizeof(calcaddr));
  	calcaddr.sin_family = AF_INET;
	calcaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	calcaddr.sin_port = htons(calcPort);

    // TCP monitor socket creation and verification
	soc_o = socket(AF_INET, SOCK_STREAM, 0);
	if (soc_o == -1) {
		cout<<"Monitor socket creation failed."<<endl;
		exit(0);
	}
	/*else
		cout<<"Socket successfully created."<<endl;*/
	bzero(&mntrAddr, sizeof(mntrAddr));

	// assign IP, PORT
	mntrAddr.sin_family = AF_INET;
	mntrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	mntrAddr.sin_port = htons(mntrPort);

    // Client connection to Server and its verification
	conn_o = connect(soc_o, (sa*)&mntrAddr, sizeof(mntrAddr));
    //cout<<"The monitor is ready to log."<<endl;

    //Loop for client requests to connect until listen() hasn't exhausted
    while (1) {
        len = sizeof(clientAddr);
        // Server connection and its verification
        conn_s = accept(soc_s, (sa*)&clientAddr, &len);
        if (conn_s < 0) {
            cout<<"The Client server is down."<<endl;
            exit(0);
        }
        /*else
            cout<<"The Server is up and running."<<endl;*/

        // Function call for message exchange between client and server
        recvInputData(conn_s);
        splitInput(id,size);

        string d=to_string(id);
        string z=to_string(size);
        strcpy (logData, "Main server receives Link ");
        strcat (logData ,d.c_str());
        strcat (logData, " and file size ");
        strcat (logData ,z.c_str());
        strcat (logData, " MB from client.");
        write (soc_o, logData, sizeof(logData));

        cout << "Received Link " << id << ", File Size "<< size << "MB."<<endl;
        response = sendto(soc_m, (char*)&id, buff_size, 0, (sa *)&dbaddr, sizeof(dbaddr));
        if (response > 0) {
            cout << "Sent Link "<< id <<" to Database Server." << endl;

            strcpy (logData, "Main server sends Link ");
            strcat (logData ,d.c_str());
            strcat (logData, " to database server.");
            write (soc_o, logData, sizeof(logData));

            response = recvfrom(soc_m, buffer, sizeof(buffer), 0, (sa *)&dbaddr, &db_addr_len);
            if (response > 0) {
                buffer[response] = '\0';
                strcpy(data,buffer);
               /*for (int i = 0; buffer[i]!='\0'; i++ ) {
                   //cout << buffer[i];
                   data[i]=buffer[i];
               }*/
                if(strncmp(data,"NA",2)==0){
                    cout<<"Received no match found."<<endl<<endl;
                    strcpy(outputData, "No match found.");
                    strcpy(logData, "Main server receives information from database server: No match found.");
                    flag=-1;
                }
                else{
                    cout<<"Received ";
                    showData(data);
                    write (soc_o, logData, sizeof(logData));
                }
            } else {
                cout << "ERROR: Receiving data" << endl;
            }
        } else {
            cout << "ERROR: Sending data" << endl;
        }

        if(flag!=-1){
            char sendCalcData[buff_size] = {0};
            string sz=to_string(size);
            strcat (sendCalcData, data);
            strcat (sendCalcData, " ");
            strcat (sendCalcData,sz.c_str());
            strcat (sendCalcData, " ");

            response = sendto(soc_m, (char*)&sendCalcData, buff_size, 0, (sa *)&calcaddr, sizeof(calcaddr));
            if (response > 0) {
                cout << endl <<"Sent information to Calculation Server." << endl;
                strcpy (logData, "Main server sends information to calculation server.");
                write (soc_o, logData, sizeof(logData));

                response = recvfrom(soc_m, buffer, sizeof(buffer), 0, (sa *)&calcaddr, &calc_addr_len);
                if (response > 0) {
                    buffer[response] = '\0';
                    for (int i = 0; buffer[i]!='\0'; i++ ) {
                        //cout << buffer[i];
                        calc[i]=buffer[i];
                    }
                    strcpy(outputData,calc);
                    //cout<<"outputData"<<outputData<<endl<<endl;
                    strcpy (logData, "Main server receives information from calculation server.");
                    write (soc_o, logData, sizeof(logData));
                    cout<<"Received ";
                    string result(calc);
                    res(result);
                } else {
                    cout << "ERROR: Receiving data" << endl;
                }
            } else {
                cout << "ERROR: Sending data" << endl;
            }
        }
        flag=0;

        write (conn_s, outputData, sizeof(outputData));
        write (soc_o, logData, sizeof(logData));

        close(conn_s);
    }

    return 0;
}

// Function designed for message exchange between client and server.
void recvInputData(int sock) {
	char buf[buff_size];
	//int n;
	bzero(buf, buff_size);
	// read the message from client and copy it in buffer
	read(sock, buf, sizeof(buf));
    strcpy(inputData,buf);

	// if msg contains "Exit" then server exit and chat ended.
	if (strncmp("exit", buf, 4) == 0) {
		cout<<"Server Exit."<<endl;
		exit(1); //break;
	}
}

void splitInput(int &id, int &sz) {
    string iddt, szdt;
    string iData(inputData);
    int pos=iData.find(" ");
    iddt=iData.substr(0,(pos));
    szdt=iData.substr(pos+1);
    stringstream i(iddt);
    i >> id;
    //cout<<"ID: "<<id<<", ";
    stringstream s(szdt);
    s >> sz;
    //cout<<"Size: "<<sz<<"MB. ";
}

void showData(string dt) {
    int c,l,v;
    string cap, len, vel, dat;
    //cout<<endl<<"Data: "<<data<<endl;

    int pos=dt.find(" ");
    cap=dt.substr(0,(pos));
    dat=dt.substr(pos+1);
    stringstream cp(cap);
    cp >> c;
    cout<<"Link Capacity "<<c<<"Mbps, ";

    int pos1=dat.find(" ");
    len=dat.substr(0,(pos1));
    stringstream ln(len);
    ln >> l;
    cout<<"Link Length "<<l<<"km, ";

    vel=dat.substr(pos1+1);
    stringstream vp(vel);
    vp >> v;
    cout<<"and Propagation Velocity "<<v<<"km/s.";

    strcpy (logData, "Main server receives information from database server: link capacity ");
    strcat (logData ,cap.c_str());
    strcat (logData, "Mbps, link length ");
    strcat (logData ,len.c_str());
    strcat (logData, "km, and propagation velocity ");
    strcat (logData ,vel.c_str());
    strcat (logData, "km/s.");
}

void res(string r){
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

    string tr=to_string(tt);
    string pr=to_string(tp);
    string su=to_string(sum);
    strcpy (logData, "Main server sends information to client: transmission delay ");
    strcat (logData ,tr.c_str());
    strcat (logData, "ms, propagation delay ");
    strcat (logData ,pr.c_str());
    strcat (logData, "ms, and total delay ");
    strcat (logData ,su.c_str());
    strcat (logData, "ms.");
}
