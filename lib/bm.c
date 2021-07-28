#include <stdio.h>
#include <stdlib.h>
#include "bm.h"

/*
 * bm 算法实现
 */


#ifndef MAX
#define MAX(a, b) ({ \
    const typeof(a) _a = a; \
    const typeof(b) _b = b; \
    _a > _b ? _a : _b; })
#endif

/*
 * bm算法简单实现,只考虑bad character,匹配时每次失配都把匹配的第一个字符当作bad character
 * 该实现在短字符串匹配中可能效率较高，实现简单
 */

//
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

//bm简单实现匹配接口
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



/*
 * bm算法完整实现，将bad character和good suffix加入
 * 在较长的字符串匹配中可能性能较好
 */
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
 * 获取good suffix:从pat中获取已经匹配上的字符串子集都是good suffix;
 * 分两种；子集在pat中间，这种只针对已匹配固定位置失配后偏移;子集到结尾，
 * 这种对固定范围内失配后可以偏移到该位置，若两种情况都符合则以第一种偏
 * 移为准因为第一种偏移的比第二种短
 * 1.先从pat中获取每一位和最右边具有相同字符串的个数,可以确定子集到结尾
 * 属于第二种情况,对于这种情况需要确定子集的长度（也就是到结尾的长度）
 * 也就是说失配的位置长度要大于等于这个长度则可以从这里移动到这里开始，
 * （但是若命中第二种情况则以第二种情况移动）
 * 2.这种情况是pat中间（good suffix未到最后）存在good suffix，失配的位置
 * 一定和good suffix长度一样长
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
    //第二种情况
    for (i = patlen - 1; i >= 0; i--){
        //这个判断可以确定一定good suffix一定到尾了
        if (suff[i] == i + 1)
            for (; j < patlen - 1 - i; j++){
                if (bms->BmGs[j] == patlen)
                    bms->BmGs[j] = patlen - 1 - i;
            }
    }

    //第一种情况
    for (i = 0; i < patlen - 2; i++){
        //失配位置等于good suffix长度，这种也包括上面的情况若存在等于
        bms->BmGs[patlen-1-suff[i]] = patlen - 1 - i;
    }
}

void BM_Prepare(unsigned char *pat, int patlen, struct BmS *bms)
{
    //prepare good suffix
    preBmGs(pat, patlen, bms);
    //prepare bad character
    preBmBc(pat, patlen, bms);
}

void BM_Search(unsigned char *text, int textlen, unsigned char *pat,
        int patlen,int *count, struct BmS *bms)
{
    int i, j, g;

    g = 0;
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


