#include <stdio.h>
#include <stdlib.h>
#include "bm.h"

/*
 * bm 算法完整实现
 */


#ifndef MAX
#define MAX(a, b) ({ \
    const typeof(a) _a = a; \
    const typeof(b) _b = b; \
    _a > _b ? _a : _b; })
#endif

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

void BMs_Prepare(unsigned char *pat, int patlen, struct BmsS *bms)
{
    int i;
    
    for (i = 0; i < ASIZE; i++){
        bms->BmBc[i] = patlen;
    }

    for (i = 0; i < patlen - 1; i++){
        bms->BmBc[pat[i]] = patlen - 1 - i;
    }
}


void BMs_Search(unsigned char *text, int textlen, unsigned char *pat, int patlen, 
                int *count, struct BmsS *bms)
{
    int i = 0;
    unsigned char *pattail = &pat[patlen-1];
    int index = patlen - 1;
    while (index < textlen){
        if (*pattail == text[index] &&
                0 == _bm_memcpy(&text[index], pattail, patlen)){
            printf("find patten %d\n", index+1-patlen);
            if (i < COUNTMAX)
                count[i++] = index + 1 - patlen;
            else
                break;
        }
        index += bms->BmBc[text[index]];
    }
}


static inline void preBmBc(unsigned char *pat, int patlen, struct BmS *bms)
{
    int i;

    //未存在的字符偏移模式串长度
    for (i = 0; i < ASIZE; i++){
        bms->BmBc[i] = patlen;
    }
    //坏字符在模式串最右边的位置
    for (i = 0; i < patlen-1; i++){
        bms->BmBc[pat[i]] = patlen - 1 - i;
    }
}

/*
 * 从patter中获取每个位置从右到左相同的字符串个数
 * ohdhgjdhg
 * 000020009
 *
 */
static inline void suffix(unsigned char *pat, int patlen, int *suffix) 
{
    int i, j, g;
    suffix[patlen-1] = patlen;
    j = patlen - 1;
    for (i = patlen-2; i >= 0; i--){
#ifndef  DEFINE_BM_SUFFIX
        //可能多数的模式串中不存在好后缀，直接对每个做匹配
        j = i;
        g = i;
        while(j>=0 && pat[j] == pat[patlen - 1 - g + j]){
            j--;
        }
        suffix[i] = g - j;
#else
        //模式串中存在好后缀，从已经命中的后缀中直接获取好后缀偏移的个数
        if (i>j && suffix[patlen - 1 + i - g] < i -j){
            suffix[i] = suffix[patlen - 1 + i - g];
        }
        else{
            if (i < j)
                j = i;
            g = i;
            while(j >= 0 && pat[j] == pat[patlen - 1 - g + j]){
                g--;
            }
            suffix[i] = g - j;
        }
#endif
    }
}


/*
 * 获取good suffix
 * 1.先从pat中获取每一位和最右边具有相同字符串的个数
 * 2.
 */
static inline void preBmGs(unsigned char *pat, int patlen, struct BmS *bms)
{
    int i, j, suff[PATSIZE];

    //模式串从右向左，找到模式串中每个字符的好后缀长度
    suffix(pat, patlen, suff);

    //初始化good suff, 每个偏移为patlen
    for (i = 0; i < patlen; i++){
        bms->BmGs[i] = (unsigned char)patlen;
    }

    j = 0;
    //
    for (i = patlen - 1; i >= 0; i--){
        //从i开始到尾都能和pat从右开始匹配上
        if (suff[i] == i + 1)
            for (; j < patlen - 1 - i; j++){
                if (bms->BmGs[j] == patlen)
                    bms->BmGs[j] = patlen - 1 - i;
            }
    }

    for (i = 0; i < patlen - 2; i++){
        bms->BmGs[patlen-1-suff[i]] = patlen - 1 - i;
    }
}

void BM_Prepare(unsigned char *pat, int patlen, struct BmS *bms)
{
    preBmGs(pat, patlen, bms);
    preBmBc(pat, patlen, bms);
}

void BM_Search(unsigned char *text, int textlen, unsigned char *pat,
        int patlen,int *count; struct BmS *bms)
{
    int i, j, g;

    g = 0
    j = 0;
    while (j <= textlen - patlen){
        for (i = patlen - 1; i >= 0 && pat[i] == text[i+j]; i--);
        if (i < 0){
            printf("find pat index %d\n", j);
            if (g < COUNTMAX)
                count[g++] = j;
            else
                break;
            j += bms->BmGs[0];
        }
        else
            j += MAX(bms->BmGs[i], bms->BmBc[text[i+j]]-patlen+1+i);
    }
}


