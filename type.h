
#include<cstdio>
#include<iostream>
#include<thread>
#include<mutex>
#include <sys/types.h>
#include <sys/socket.h>
#include<arpa/inet.h>
#include<cstring>
#include<netpacket/packet.h>
typedef unsigned char byte;
using std::mutex;
struct IAM{
  byte IP[4];
  byte MAC[6];
};

struct etherPack {
  struct etherHeader{
    byte etherDhost[6]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};//Broadcast MAC
    byte etheShost[6] = {0xc8, 0x21, 0x58, 0x9e, 0xbf, 0x1f};//Host MAC
    byte etherType[2] = {0x08, 0x06};
  }etherHeader;
  struct ARP{
    byte hardType[2] = {0x00, 0x01};
    byte proType[2] = {0x08, 0x00};
    byte hardLen = 0x06;
    byte proLen = 0x04;
    byte opt[2] = {0x00, 0x01};
    byte hostMAC[6] = {0xc8, 0x21, 0x58, 0x9e, 0xbf, 0x1f};
    byte hostIP[4] = {0xc0, 0xa8, 0x01, 0x76};
    byte distMAC[6] = {0x00};
    byte distIP[4] = {0xc0, 0xa8, 0x01, 0x00};
  }ARP;
  //40:b0:34:51:c1:5e
  //c8:21:58:9e:bf:1f
  //c8:21:58:9e:bf:1f
};


struct countType{
  int count=0;
  mutex mtx;
};

struct DNSPart{
  byte NAME[2];
  byte TYPE[2];
  byte CLASS[2];
  byte TTL[4];
  byte RDLENGTH[2];
  //byte *RDATA;

};

struct DNSPack{
  struct header{
    byte ID[2]={0x11,0x11};
    byte flags[2]={0x01,0x00};
    byte qdcount[2]={0x00,0x01};
    byte ancount[2]={0x00,0x00};
    byte nscount[2]={0x00,0x00};
    byte arcount[2]={0x00,0x00};
  }header;
  struct question{
    //byte *qname;
    byte qtype[2]={0x00,0x01};
    byte qclass[2]={0x00,0x01};
  }question;
};
