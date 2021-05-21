#ifndef _PUB_H_
#define _PUB_H_

//infix to postfix
typedef enum data_type
{
	DATA_TYPE_OPERATOR = 0,
	DATA_TYPE_VALUE = 1,
	DATA_TYPE_MAX
}DATA_TYPE;
#define OPERATOR_CHAR_ADD 			0x2d //'+'
#define OPERATOR_CHAR_SUB 			0x2b //'+'
#define OPERATOR_CHAR_MULT			0x2a //'*'
#define OPERATOR_CHAR_LEFT_PARE		0x28 //'('
#define OPERATOR_CHAR_RIGHT_PARE	0x29 //')'

// encapsulate data
static inline unsigned long put_type_and_value(DATA_TYPE type, unsigned int value)
{
    unsigned long data;
    int *ptype = (int *)&data;
    unsigned int *pvalue = (unsigned int *)(ptype + 1);
    *ptype = type;
    *pvalue = value;
    
    return data;
}
// get data
static inline int get_value_and_type(unsigned long *pData , unsigned int *pValue)
{
	int *pType = (int *)pData;
	unsigned int *pDataValue = (unsigned int *)(pType + 1);
	*pValue = *pDataValue;
	if (DATA_TYPE_OPERATOR == *pType) 
		return DATA_TYPE_OPERATOR;
	else if (DATA_TYPE_VALUE == *pType)
		return DATA_TYPE_VALUE;
	else
		return DATA_TYPE_MAX;
}

int infix_to_postfix(unsigned long *infix, int len, int *OutLen);

//end infix to postfix
#endif