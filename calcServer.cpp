#include <strings.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>
#include <iomanip>

#define calcPort 24543
#define mainPort 22543
#define buff_size 256
#define MAXLINE 100000
#define sa struct sockaddr

using namespace std;

void calc(string, string&);

int main() {
    struct sockaddr_in calcServerAddr, mainServerAddr;
	char calcRecv[buff_size] = {0}, calcData[buff_size];
    string calcRply = "";
	int soc_c, bnd, response;
	socklen_t addr_len = sizeof(mainServerAddr);

   bzero(&calcServerAddr, sizeof(calcServerAddr));
  	calcServerAddr.sin_family = AF_INET;
	calcServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	calcServerAddr.sin_port = htons(calcPort);

	soc_c = socket(PF_INET, SOCK_DGRAM, 0);
    if (soc_c < 0) {
        cout << "ERROR: Setting socket" << endl;
    }
    bnd = bind(soc_c, (sa *)&calcServerAddr, sizeof(calcServerAddr));
    if (bnd < 0) {
        cout << "ERROR: Binding socket" << endl;
    }
    cout << "The Calculation Server is up and running." << endl;


	bzero(&mainServerAddr, sizeof(mainServerAddr));
  	mainServerAddr.sin_family = AF_INET;
	mainServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	mainServerAddr.sin_port = htons(mainPort);

   while(1){
	    response = recvfrom(soc_c, calcRecv, buff_size, 0, (sa *)&mainServerAddr, &addr_len);
       if (response > 0) {
           cout << "Received request from Main Server."<<endl;
           for (int i = 0; calcRecv[i] != '\0'; i++ ) {
               //cout << calcRecv[i];
               calcData[i]=calcRecv[i];
           }
           cout<< "Sent ";
           string cdata(calcData);
           calc(cdata,calcRply);
           char arr[buff_size];
           strcpy(arr,calcRply.c_str());
           //cout<< "calcRply: "<< arr <<endl;
           response = sendto(soc_c, (const char*)&arr, sizeof(arr), 0, (sa *)&mainServerAddr, sizeof(mainServerAddr));
           if (response > 0) {
               cout << endl;
           } else {
               cout << "ERROR: Sending reponse" << endl;
           }
       } else {
           cout << "ERROR: Receiving data" << endl;
       }
     }
  return 0;
}

void calc(string data, string &rply){
    int s,c,l,v;
    float tt,tp,sum;
    string size, cap, len, vel, dat;
    //cout<<endl<<"Data: "<<data<<endl;

    int pos=data.find(" ");
    cap=data.substr(0,(pos));
    dat=data.substr(pos+1);
    stringstream cp(cap);
    cp >> c;
    //cout<<"C: "<<c<<endl;

    int pos1=dat.find(" ");
    len=dat.substr(0,(pos1));
    dat=dat.substr(pos1+1);
    stringstream ln(len);
    ln >> l;
    //cout<<"L: "<<l<<endl;

    int pos2=dat.find(" ");
    vel=dat.substr(0,(pos2));
    dat=dat.substr(pos2+1);
    stringstream vp(vel);
    vp >> v;
    //cout<<"Vp: "<<v<<endl;

    int pos3=dat.find(" ");
    size=dat.substr(0,(pos3));
    stringstream sz(size);
    sz >> s;
    //cout<<"Size: "<<s<<endl;

    tt=(s*8*1000)/(float)c;
    tp=(l*1000)/(float)v;
    sum=tt+tp;
    cout<<"transmission delay "<<std::fixed<<std::setprecision(2)<<tt<<"ms, ";
    cout<<"propagation delay "<<std::fixed<<std::setprecision(2)<<tp<<" ms, ";
    cout<<"total delay "<<std::fixed<<std::setprecision(2)<<sum<<"ms."<<endl;

    string t=to_string(tt);
    string p=to_string(tp);
    string sm=to_string(sum);

    rply=t;
    rply.append(" ");
    rply.append(p);
    rply.append(" ");
    rply.append(sm);
}
