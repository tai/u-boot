#ifndef _SECU_BOOT_H_
#define _SECU_BOOT_H_

void UpdateExternalRSAKey(unsigned char *InputBuf);

int SignatureVerify(unsigned char *InputBuf, 
                    int Size, 
                    unsigned char *SigBuf);

int SHAVerify(unsigned char *InputBuf, 
              int Size, 
              unsigned char *SigBuf);

int HybridSignatureVerify( unsigned char hash[32], unsigned char *SigBuf);


#endif
