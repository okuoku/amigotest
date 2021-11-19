#include <sysctl.h>

void
powercfg_boot(void){
    // LCD digital I/O pins
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    // DVP(Camera)
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
}
