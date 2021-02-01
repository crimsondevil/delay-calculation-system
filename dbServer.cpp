#include <strings.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>


#define dbPort 23543
#define mainPort 22543
#define buff_size 256
#define MAXLINE 100000
#define sa struct sockaddr

using namespace std;

void dbSearch(int, string&);
void showData(string);

int main() {
    struct sockaddr_in dbServerAddr, mainServerAddr;
	char dbRecv[buff_size] = {0}, dbIndex;
    string dbRply = "";
	int soc_d, bnd, response, dbId;
	socklen_t addr_len = sizeof(mainServerAddr);

    bzero(&dbServerAddr, sizeof(dbServerAddr));
  	dbServerAddr.sin_family = AF_INET;
	dbServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	dbServerAddr.sin_port = htons(dbPort);

	soc_d = socket(PF_INET, SOCK_DGRAM, 0);
    if (soc_d < 0) {
        cout << "ERROR: Setting socket" << endl;
    }
    bnd = bind(soc_d, (sa *)&dbServerAddr, sizeof(dbServerAddr));
    if (bnd < 0) {
        cout << "ERROR: binding socket" << endl;
    }
    cout << "The Database Server is up and running." << endl;

    bzero(&mainServerAddr, sizeof(mainServerAddr));
  	mainServerAddr.sin_family = AF_INET;
    mainServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    mainServerAddr.sin_port = htons(mainPort);

    while(1){
       response = recvfrom(soc_d, dbRecv, buff_size, 0, (sa *)&mainServerAddr, &addr_len);
       if (response > 0) {
           cout << "Received request from Main Server."<<endl;
           for (int i = 0; dbRecv[i] != '\0'; i++ ) {
              // cout << (int)dbRecv[i] << endl;
               dbIndex=(int)dbRecv[i];
           }
           dbId=(int)dbIndex;
           dbSearch(dbId,dbRply);
           char arr[buff_size];
           strcpy(arr,dbRply.c_str());
           response = sendto(soc_d, (const char*)&arr, sizeof(arr), 0, (sa *)&mainServerAddr, sizeof(mainServerAddr));
           if (response > 0) {
               if(strncmp(arr,"NA",2)==0)
    				   cout<<"No match found."<<endl<<endl;
    			   else{
    			      cout << "Sent link "<<dbId;
    			      showData(dbRply);
    			   }
           } else {
               cout << "ERROR: Sending response" << endl;
           }
       } else {
           cout << "ERROR: Receiving data" << endl;
       }
    }
    return 0;
}

void dbSearch(int id, string &data){
    string myText, myTextArr[10], fid;
    int i=0,flag=0,lid=0;
    ifstream MyReadFile("database_final.txt");
    while (getline (MyReadFile, myText)) {
        myTextArr[i++]=myText;
        flag++;
      }
    MyReadFile.close();
    for(i=0;i<flag;i++){
        int pos=myTextArr[i].find(" ");
        fid=myTextArr[i].substr(0,(pos));
        stringstream sid(fid);
        sid >> lid;
        if (lid==id){
            data= myTextArr[i].substr(pos+1);
            break;
        }
        else
            data = "NA";
    }
   // cout<<"Data:"<<data<<endl;
}

void showData(string data){
    int c,l,v;
    string cap, len, vel, dat;
    //cout<<endl<<"Data: "<<data<<endl;

    int pos=data.find(" ");
    cap=data.substr(0,(pos));
    dat=data.substr(pos+1);
    stringstream cp(cap);
    cp >> c;
    cout<<", capacity "<<c<<"Mbps";

    int pos1=dat.find(" ");
    len=dat.substr(0,(pos1));
    stringstream ln(len);
    ln >> l;
    cout<<", link length "<<l<<"km";

    vel=dat.substr(pos1+1);
    stringstream vp(vel);
    vp >> v;
    cout<<", propagation velocity "<<v<<"km/s."<<endl<<endl;
}
