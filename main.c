#include <bsp.h>
#include "amigo_powercfg.h"

static int errno;

int *__errno(void){
    // FIXME: turn it to TLS
    return &errno;
}

int
main(void){
    printf("Hello.\n");
    printf("Init powercfg.\n");
    powercfg_boot();
    printf("Done.\n");
    for(;;){}
    return 0;
}
