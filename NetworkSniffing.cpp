#include <cstdio>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<cstring>
#include<iostream>
using std::cin;
using std::cout;
int main() {
    char hname[128];
    hostent *hent;
    gethostname(hname, sizeof(hname));
    hent = gethostbyname(hname);
    printf("hostname: %s\naddress list: ", hent->h_name);
    char *IP;
    for(int i = 0; hent->h_addr_list[i]; i++) {
        IP=inet_ntoa(*(in_addr*)(hent->h_addr_list[i]));
        for(int j = 0;j<=strlen(IP);j++){
          printf("%d %c\n",j,IP[j]);
        }
        cout<<"IP is"<<IP;
    }
    int ipLength=strlen(IP);
    int tmp=0;
    int cnt=0;
    int ipSection[5];
    for(int i = 0;i<ipLength;i++){
      if(IP[i]=='.'){
        ipSection[++cnt]=tmp;
        tmp=0;
      }else{
        tmp=tmp*10+IP[i]-'0';
      }
    }
    ipSection[++cnt]=tmp;
    for(int i = 1;i<255;i++){
      if(i==ipSection[cnt]) continue;
      char *ip;

      if(sendIsOK())
    }



    return 0;
}
