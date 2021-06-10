#include "include.h"

static void hi_init(){
    HI_S32 ret;
    ret = HI_SYS_Init();
    if (ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
        exit(-1);
    }
    ret = HI_UNF_GPIO_Init();
    if (ret != HI_SUCCESS)
    {
        printf("%s: %d ErrorCode=%d\n", __FILE__, __LINE__, ret);
        exit(-1);
    }
    //fbTest();
    osdTest();
    //vedioPlayerTest();
    //imageTest();
    //pwmTest();
    mainTest();  
    ipCheck();
    ledproc();
    uart485Test();
}

static void hi_uninit(){
    HI_SYS_DeInit();
    HI_UNF_GPIO_DeInit();
    HI_GO_Deinit();
}


int main(int argc, char *argv[])
{
    int ret;

    printf("------hiEnvBin Version20191202:%d.%d------\n",VERSION_MAX,VERSION_MIN);    

    hi_init();

    while (1){
        sleep(2);
    }
    
    hi_uninit();
    return 0;
}
