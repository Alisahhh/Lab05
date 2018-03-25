#include"type.h"

void askARP(IAM *pIAM);

void multThreadBroadcast(IAM *pIAM,int beg,int end,countType &count);

void ARPAttack(int target,IAM *pIAM);

void IAMPrint(IAM* pIAM);

void queryDNS(char *DNS);

void ARPUnAttack(int target,IAM *pIAM);

char *getDomain(char *DNS);

void resolveDNS(byte *buff,int buffSize,int ansPosition);
