#ifndef _PUB_H_

#define _PUB_H_

#define BMCSIZE 256

typedef struct bmpat
{
    unsigned short badc[BMCSIZE]; //坏字符跳转表
};

void BM_Prepare(unsigned char *pat, int patlen, struct bmpat *patbms);

int BM_Search(unsigned char *text, unsigned int textlen, unsigned char *pat, unsigned int patlen, 
                struct bmpat *patbms);
#endif
