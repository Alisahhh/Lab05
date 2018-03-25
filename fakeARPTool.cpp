
#include "ARPLib.h"
int main(){
  IAM *pIAM=nullptr;
  map();
  while(1){
    int opt;
    scanf("%d",&opt);

    if(opt==1){
      pIAM=networkSuffing(pIAM);
    }
    if(opt==2){
      int target;
      std::cout<<"Please enter the target you want to attack:"<<std::endl;
      scanf("%d",&target);
      attack(target,pIAM);
    }
    /*if(opt==3){
      int target;
      std::cout<<"Please enter the target you want to unattack:"<<std::endl;
      scanf("%d",&target);
      unAttack(target,pIAM);
    }*/
    if(opt==3){
      char DNS[100]="";
      printf("Please enter the DNS\n");
      std::cin>>DNS;
      queryDNS(DNS);
    }
    if(opt==4){
      delete []pIAM;
      break;
    }
  }
}
