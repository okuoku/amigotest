#include <bsp.h>
#include <sysctl.h>

static int errno;

int *__errno(void){
    // FIXME: turn it to TLS
    return &errno;
}

int
main(void){
    printf("Hello.\n");
    for(;;){}
    return 0;
}
