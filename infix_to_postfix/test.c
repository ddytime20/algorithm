#include <stdio.h>
#include <stdlib.h>
//#include "stack.h"
#include "pub.h"

// 1 + 3 * 8 + ( 2 * 5 + 7 ) + 10 ==> 1 3 8 * + 2 5 * 7 + + 10 +
// 1 + 3 * 8 + ( 2 * 5 + 7 ) * 10 ==> 1 3 8 * + 2 5 * 7 + 10 * +
// 1 + (3 * 8) + ( 2 * 5 + 7 ) * 10 ==> 1 3 8 * + 2 5 * 7 + + 10 +
int main(int argc, char *argv[])
{
    unsigned long data[30];
    int len;
    int num = 0;
    
    data[num++] = put_type_and_value(DATA_TYPE_VALUE, 1);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_ADD);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_LEFT_PARE);
    data[num++] = put_type_and_value(DATA_TYPE_VALUE, 3);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_MULT);
    data[num++] = put_type_and_value(DATA_TYPE_VALUE, 8);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_RIGHT_PARE);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_ADD);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_LEFT_PARE);
    data[num++] = put_type_and_value(DATA_TYPE_VALUE, 2);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_MULT);
    data[num++] = put_type_and_value(DATA_TYPE_VALUE, 5);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_ADD);
    data[num++] = put_type_and_value(DATA_TYPE_VALUE, 7);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_RIGHT_PARE);
    data[num++] = put_type_and_value(DATA_TYPE_OPERATOR, OPERATOR_CHAR_ADD);
    data[num++] = put_type_and_value(DATA_TYPE_VALUE, 10);
    
    if (-1 == infix_to_postfix(data, num, &len)){
        printf("infix to postfix error");
        return -1;
    }
    printf("infix to postfix success \n");
    int i;
    DATA_TYPE type;
    unsigned int value;
    for (i = 0; i < len; i++) {
        type = get_value_and_type(&data[i], &value);
        if (DATA_TYPE_MAX == type) {
            printf("index %d, value failed \n", i);
        } else if (DATA_TYPE_VALUE == type) {
            printf("index %d, valude %d \n", i, value);
        } else {
            char ovalue = value;
            printf("index %d, valude %c \n", i, ovalue);
        }
    }
    return 0;
}
