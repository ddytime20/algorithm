#include <stdio.h>
#include <stdlib.h>
#include "kmp.h"


/*
 * KMP算法实现
 */


//获取partial match table
static inline void get_next(unsigned char *pat, int patlen, int *next)
{
    int i, j;

    next[0] = -1;
    j = -1;
    i = 0;
    while (i < patlen - 1){
        if (j == -1 || pat[i] == pat[j]){
            i++;
            j++;
            next[i] = j;
        } else {
            //一定是失配时走到这里
            j = next[j];
        }
    }
}

void KMP_Search(unsigned char *text, int textlen, unsigned char *pat,
        int patlen, int *count)
{
    int i, j;
    int *next;

    next = (int *)mallec(sizeof(int) * patlen);
    if (NULL == next)
        return;

    j = 0;
    i = 0;
    get_next(pat, patlen, next);
    while (i < textlen && j < patlen){
        if (-1 == j || text[i] == pat[j]){
            i++;
            j++;
        }
        else
            j = next[j];
                
    }

    if (j == patlen)
        

}

int main(int argc, char *argv[]){
    char tmp[] = "abababcab";
    int next[10];
    int len;
    len = strlen(tmp);

    get_next(tmp, len, next);

    int i;
    for (i = 0; i < len; i++){
        printf("next[%d] = %d\n", i, next[i]);
    }
    return 0;
}

