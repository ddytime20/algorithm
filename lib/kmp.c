#include <stdlib.h>
#include "kmp.h"
#include "debug.h"

/*
 * KMP算法实现
 */


/*
 * 构造partial match table
 * next数组中存放的数据代表:patten在该位置失配时下次跳转patten的位置
 * 该位置是失配位置j的前j-1个字符串前缀和后缀共同的最大长度加1,也就是
 * 最大共同长度的下一位置开始匹配
 */
static inline void get_next(unsigned char *pat, int patlen, int *next)
{
    int i, j;

    next[0] = -1;
    j = -1; //判断当前搜索位置是否是pat开始位置
    i = 0;
    while (i < patlen - 1){
        if (j == -1 || pat[i] == pat[j]){
            i++;
            j++;
            next[i] = j;
        } else {
            //一定是失配时走到这里，失配时根据next表跳转
            j = next[j];
        }
    }
}

//只匹配最开始的一个
void KMP_Search(unsigned char *text, int textlen, unsigned char *pat,
        int patlen, int *count)
{
    int i, j;
    int *next;

    next = (int *)mallec(sizeof(int) * patlen);
    if (NULL == next)
        FatalError("out of memory");

    j = 0;
    i = 0;
    get_next(pat, patlen, next);
    while (i < textlen && j < patlen){
        //j为-1则是从头开始匹配
        if (-1 == j || text[i] == pat[j]){
            i++;
            j++;
        } else {
            //获取下一个跳转位置
            j = next[j];
        }
    }

    //命中返回匹配开始的位置
    if (j == patlen)
        return i-j;
        
    return -1;
}


