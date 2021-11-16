#include <bsp.h>
#include <sysctl.h>

int __errno;

int
main(void){
    printf("Hello.\n");
    for(;;){}
    return 0;
}
