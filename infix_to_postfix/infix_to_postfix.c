/*
 * 
 * infix to postfix
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"
#include "pub.h"

static inline int get_value_and_type_from_stack(struct StackRecord *Stack, unsigned int *pValue)
{
	unsigned long data;
	data = StackTop(Stack);
    if (0 == data) {
        // empty stack
        return -1;
    }
	return get_value_and_type(&data, pValue);
}

/*
 * 
 */
static inline int infix_to_postfix_proc(unsigned long date, struct StackRecord *Stack,
                                unsigned long *buff , int *pos)
{
	unsigned int Value;
	DATA_TYPE type = get_value_and_type(&date, &Value);
	if (DATA_TYPE_VALUE == type) {
		buff[(*pos)++] = date;
		return 0;
	}
	
	if (DATA_TYPE_OPERATOR == type) {
		unsigned char Operator = (unsigned char)Value;
		unsigned long PreOpt;
		/*
		 * '*' '(' and empty stack ,must put data in stack
		 */
		if ((OPERATOR_CHAR_MULT == Operator) || (OPERATOR_CHAR_LEFT_PARE == Operator) \
			|| StackIsEmpty(Stack)) {
			StackPush(Stack, date);
			return 0;
		}
		else if (OPERATOR_CHAR_ADD == Operator) {
			do {
                // if Operator is '+' , pop all until '(' , '+' , because they have lower priority
				if (-1 == get_value_and_type_from_stack(Stack, &Value) ||
                    OPERATOR_CHAR_LEFT_PARE == Value) {
                    StackPush(Stack, date);
					break;
                }
                else {
                    PreOpt = StackPop(Stack);
					buff[(*pos)++] = PreOpt;
                }
			}while(1);
		} // must ')'
		else {
			do {
				(void)get_value_and_type_from_stack(Stack, &Value);
				Operator = (unsigned char)Value;
				if (OPERATOR_CHAR_LEFT_PARE == Operator) {
					PreOpt = StackPop(Stack);
					break;
				}
				else {
					PreOpt = StackPop(Stack);
					buff[(*pos)++] = PreOpt;
				}
			}while(1);
		}
	}
	else
		return -1;
	
	return 0;
}

/*
 * unsigned long 64位编译器，前4位表示类型，后4位是数值
 */
int infix_to_postfix(unsigned long *infix, int len, int *OutLen)
{
	struct StackRecord *Stack = CreateStack(100);
	unsigned long *buff;
	int i, pos, ret;
	pos = ret = 0;
	buff = (unsigned long *)malloc(sizeof(unsigned long) * len);
	if (NULL == buff) {
		StackFree(Stack);
        return -1;
	}
    // turn infix to postfix
    for (i = 0; i < len; i++) {
		if (-1 == infix_to_postfix_proc(infix[i], Stack, buff, &pos)) {
			ret = -1;
			break;
		}
	}
    // 将栈中剩余的符号放入buff
    if (0 == ret) {
        while(!StackIsEmpty(Stack)) {
            buff[pos++] = StackPop(Stack);
        }
        memcpy(infix, buff, sizeof(unsigned long) * pos);
        *OutLen = pos;
    }
    StackFree(Stack);
    free(buff);
    
    return ret;
}

