#include <stdio.h>
#include <stdlib.h>
#include "bms.h"

/*
 * bm simple 
 * 将bad character和good suffix当成一种情况实现
 * 简化bm的实现方式，每次检测需要匹配的最右边的字符，
 * 跳转也按照字符串中匹配的最右边的字符，最右边的字符
 * 未命中当bad character跳转，命中了当good suffix跳转
 * 这两种跳转方式一样，根据字符在跳转表的位置跳转。
 */

//patter compare from right to left
static inline unsigned int _bm_memcpy(unsigned char *text, unsigned char *pat, unsigned int len)
{
   unsigned char *c1, *c2;
   for (c1 = text, c2 = pat; 0 < len; c1--, c2--, len--){
       if (*c1 != *c2)
           break;
   }

   return len;
}

//init jump patter map
void BM_Prepare(unsigned char *pat, int patlen, struct bmsbc *patbms)
{
   int index; 
   if (0 == patlen)
           return; 

   for (index = 0; index < BMCSIZE; index++){
        patbms->bmsbc[index] = (unsigned short)patlen;
   }

    for (index = 0; index < patlen - 1; index++){
        patbms->bmsbc[pat[index]] = (unsigned short)(patlen - index - 1);
    }

    return;
}

int BM_Search(unsigned char *text, unsigned int textlen, unsigned char *pat, unsigned int patlen, 
                struct bmsbc *patbms)
{
    if (!patlen)
        return -1;

    unsigned char *pattail = &pat[patlen-1];
    int index = patlen - 1;
    while (index < textlen){
        if (*pattail == text[index] &&
                0 == _bm_memcpy(&text[index], pattail, patlen)){
            printf("find patten %d\n", index);
            //break;
        }
        index += patbms->bmsbc[text[index]];
    }

    return 0;
}
