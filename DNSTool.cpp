#include "DNSTool.h"
#include<string>
void queryDNS(char *DNS){
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  DNSPack aDNSPack;
  /*aaDNSPack.header.flags[0]={0x01,0x00};
  aDNSPack.header.qdcount={0x12,0x34};
  aDNSPack.header.ancount={0x00,0x01};
  aDNSPack.header.nscount={0x00,0x00};
  aDNSPack.header.arcount={0x00,0x00};*/
  //aDNSPack.question.qname=new byte[DNSLength+1];
  char *domain=getDomain(DNS);
  int domainLength=strlen(domain);
  //memcpy()

  //memcpy(aDNSPack.question.qname,domain,domainLength);
  /*aDNSPack.question.qtype={0x00,0x01};
  aDNSPack.question.qclass={0x00,0x01};*/
  sockaddr_in dist;
  dist.sin_addr.s_addr = inet_addr("8.8.8.8");
  dist.sin_port = htons(53);
  dist.sin_family = AF_INET;
  byte buff[1024];
  memset(buff,0,sizeof(buff));
  memcpy(buff,&aDNSPack.header,12);

  memcpy(buff+12,domain,domainLength);
  memcpy(buff+12+domainLength,&aDNSPack.question,4);
  //5memcpy(buff+12+domainLength,&)
  //strcat(buff,aDNSPack.header);
  //strcat(buff,aDNSPack.question);
  //memcpy(buff,aDNSPack.header,sizeof(aDNSPack.header));
  //memcpy(buff,aDNSPack.question,sizeof(aDNSPack.question));
  int sendSuccessFlag=sendto(fd,buff,sizeof(buff),0, (sockaddr *) &dist, sizeof(dist));
  if (sendSuccessFlag<0){
    perror("error");
    return;
  }
  memset(buff,0,sizeof(buff));
  socklen_t distSize=sizeof(dist);
  int recvSuccessFlag = recvfrom(fd, buff, 1024, 0, (sockaddr *) &dist, &distSize);
  if(recvSuccessFlag<0){
    perror("error");
    return;
  }
  resolveDNS(buff,recvSuccessFlag,16+domainLength);
}

char *getDomain(char *DNS){
  int DNSLength=strlen(DNS);
  char * tmpDNS=DNS;
  char *domain;
  domain =new char[1024];
  int length=0;
  for(int i = 0;i<DNSLength;i++){
    if(tmpDNS[i]!='.') length++;
    else{
      strcat(domain,std::to_string(length).c_str());
      strncat(domain,DNS,length);
      DNS=DNS+length+1;
      length=0;
    }
  }
  strncat(domain,DNS,length);
  return  domain;
}

void resolveDNS(byte *buff,int buffSize,int ansPosition){
  DNSPack aDNSPack;
  memset(&aDNSPack,0,sizeof(aDNSPack));
  memcpy(&aDNSPack.header,buff,12);
  int ansCount=aDNSPack.header.ancount[0]*256+aDNSPack.header.ancount[1];
  if(ansCount==0) return;
  int tmpPosition=ansPosition;
  while(tmpPosition<buffSize){
    DNSPart ansPart;
    memcpy(&ansPart,buff+tmpPosition,sizeof(ansPart));
    tmpPosition+=sizeof(ansPart);
    int length=ansPart.RDLENGTH[0]*256+ansPart.RDLENGTH[1];
    byte *ansBuff=new byte[length];
    memcpy(ansBuff,buff+tmpPosition,length);
    //tmpPosition+=length;
    if(ansPart.TYPE[1]==0x05){
      printf("CNAME:");
      int tmpLen=0;
      bool comFlag=0;
      byte len;
      while(buff[tmpLen+tmpPosition+1]!=0x00){
        len=ansBuff[tmpLen+tmpPosition+1];
        if(ansBuff[tmpLen+len+1]==0xc0){
          comFlag=1;
          break;
        }
        ansBuff[len+tmpLen]='.';
        tmpLen+=len+1;
      }
      ansBuff[tmpLen+len+1]='\0';
      if (comFlag) {
        printf("%s.com\n", ansBuff+ 1);
      } else {
        printf("%s\n", ansBuff + 1);
      }
      delete ansBuff;
    }else if(ansPart.TYPE[1]==0x01){
      printf("IP: %d",buff[tmpPosition]);
      for(int i = 1;i<=3;i++) printf(".%d",buff[tmpPosition+i]);
    }else if(ansPart.TYPE[1]==0x1c){
      printf("IP: %02x",buff[tmpPosition]);
      for(int i = 1;i<=15;i++) printf(".%02x",buff[tmpPosition+i]);
    }
    tmpPosition+=length;
  }
}
