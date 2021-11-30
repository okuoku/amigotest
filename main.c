#include <bsp.h>
#include "amigo_powercfg.h"
#include "amigo_lcd_ips.h"

static int errno;

int *__errno(void){
    // FIXME: turn it to TLS
    return &errno;
}

static unsigned char fb[320*480*2];

int
main(void){
    int curx;
    printf("Hello.\n");
    printf("Init powercfg.\n");
    powercfg_boot();
    printf("Init LCD.\n");
    amigo_lcd_ips_boot();
    printf("Start LCD.\n");
    amigo_lcd_ips_start();
    printf("Done.\n");
    curx = 0;
    for(;;){
        memset(fb, 0xcc, 320*480*2);
        for(int i=0;i!=480;i++){
            fb[320*2*i + curx*2] = 0xff;
            fb[320*2*i + curx*2+1] = 0xff;
        }
        printf("curx = %d, %p, %x\n", curx, fb, (unsigned int)fb[0]);
        amigo_lcd_ips_push_rgnpix(0,0,320,480,fb,320*480);
        curx ++;
        if(curx == 320){
            curx = 0;
        }
    }
    return 0;
}
