#ifndef __DEF_H__
#define __DEF_H__
#include<errno.h> 
#include <unistd.h>
#include<stdio.h>
#include <strings.h>
#define print_error() do\
{\
    perror("perror:");\
    printf("%s,errno=%d\n",__FUNCTION__,errno);\
}while(0)
#define printfStart() (printf("%s start\n",__FUNCTION__))
#define printfEnd() (printf("%s end\n",__FUNCTION__))
#define printfError() (printf("%s error\n",__FUNCTION__))
#define printfStr(s) (printf("%s %s\n",__FUNCTION__,(s)))
#define printfInt(i) (printf("%s %d\n",__FUNCTION__,(i)))
#endif