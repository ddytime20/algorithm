#ifndef _BM_H_

#define _BM_H_

#define PATSIZE 128
#define ASIZE 256

struct BmS
{
    unsigned char BmGs[PATSIZE];
    unsigned char BmBc[ASIZE];
};

void BM_Prepare(unsigned char *pat, int patlen, struct BmS *bms);

void BM_Search(unsigned char *text, int textlen, unsigned char *pat,
        int patlen, struct BmS *bms);


#endif
