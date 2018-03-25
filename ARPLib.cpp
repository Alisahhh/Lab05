#include"ARPLib.h"

IAM *networkSuffing(IAM *pIAM){
  if(pIAM!=nullptr){
    memset(pIAM,0,sizeof(pIAM));
    askARP(pIAM);
  }else{
    pIAM =new IAM[256];
    memset(pIAM,0,sizeof(pIAM));
    askARP(pIAM);
  }
  IAMPrint(pIAM);
  return pIAM;
}

void attack(int target,IAM *pIAM){
  ARPAttack(target,pIAM);
}

void unAttack(int target,IAM *pIAM){
  ARPUnAttack(target,pIAM);
}

void map(){
    printf("Welcome to use this fake network tool\n");
    printf("Please enter 1~4 to use it\n");
    printf("1: Broadcast  2: attack\n");
    printf("3: queryDNS   4:quit\n");
    printf("(The tool hasn't been finished and you may meet some problem,I am so sorry)\n");
    printf("-----------------------------------------------\n");
}
