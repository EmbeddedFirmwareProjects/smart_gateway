#ifndef DATA_TYPES_H
#define DATA_TYPES_H

typedef unsigned long u32;
typedef short unsigned int u16;
typedef unsigned char u8;

typedef long s32;
typedef short int s16;
typedef char s8;

#define true 1
#define false 0

typedef void (*pVoidFuncVoidpData)(void *);
typedef void (*pVoidFuncVoidpDataU16Len)(void *, u16);


#endif
