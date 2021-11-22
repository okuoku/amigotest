#include <bsp.h>
#include "amigo_powercfg.h"
#include "amigo_lcd_ips.h"

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
    printf("Init LCD.\n");
    amigo_lcd_ips_boot();
    printf("Start LCD.\n");
    amigo_lcd_ips_start();
    printf("Done.\n");
    for(;;){}
    return 0;
}
