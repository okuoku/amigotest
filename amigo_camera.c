#include <fpioa.h>
#include <dvp.h>
#include <sleep.h>

#include <bsp.h>

#include <sipeed_i2c.h>

#include "amigo_camera.h"

static void
dcmi_rst(int val){
    /* FIXME: Not in SDK? */
    if(val){
        dvp->cmos_cfg &= ~DVP_CMOS_RESET;
    }else{
        dvp->cmos_cfg |= DVP_CMOS_RESET;
    }
}

static void
dcmi_pwdn(int val){
    /* FIXME: Not in SDK? */
    if(val){
        dvp->cmos_cfg &= ~DVP_CMOS_POWER_DOWN;
    }else{
        dvp->cmos_cfg |= DVP_CMOS_POWER_DOWN;
    }
}

static void
camera_i2c_cfg(int addr){
    i2c_init(I2C_DEVICE_2, addr, 7, 100*1000);
}

static int
camera_i2c_readreg(int addr, int reg){
    int r;
    uint8_t dat[1];
    camera_i2c_cfg(addr);
    dat[0] = reg & 0xff;
    r = maix_i2c_send_data(2, addr, dat, 1, 10);
    if(r){
        return -1;
    }
    dat[0] = 0xff;
    r = maix_i2c_recv_data(2, addr, NULL, 0, dat, 1, 10);
    if(r){
        return -1;
    }
    return dat[0];
}

static void
camera_i2c_writereg(int addr, int reg, int val){
    uint8_t dat[2];
    camera_i2c_cfg(addr);
    dat[0] = reg & 0xff;
    dat[1] = val & 0xff;
    i2c_send_data_dma(DMAC_CHANNEL2, I2C_DEVICE_2,
                      dat, 2);
}

void
amigo_camera_boot(void){
    int addr;
    unsigned int dat;
    unsigned char ids[4];
    /* FIXME: Acquire I2C-0 lock, and move this somewhere else.. */
    unsigned char icfg[2];
    i2c_init(I2C_DEVICE_0, 0x34, 7, 400*1000);
    //AXP173: LDO4 - 0.8V (default 0x48 1.8V)
    icfg[0] = 0x27;
    icfg[1] = 0x20;
    i2c_send_data_dma(DMAC_CHANNEL0, I2C_DEVICE_0, icfg, 2);
    i2c_init(I2C_DEVICE_0, 0x34, 7, 400*1000);
    //AXP173: LDO2/3 - LDO2 1.8V / LDO3 3.0V
    icfg[0] = 0x28;
    icfg[1] = 0x0c;
    i2c_send_data_dma(DMAC_CHANNEL0, I2C_DEVICE_0, icfg, 2);

    fpioa_set_function(47 /* DVP_PCLK */, FUNC_CMOS_XCLK);
    fpioa_set_function(45 /* DVP_HSYNC */, FUNC_CMOS_HREF);
    fpioa_set_function(43 /* DVP_VSYNC */, FUNC_CMOS_VSYNC);
#if 0 // Use I2C instead of SCCB
    fpioa_set_function(41 /* CAMERA_SCL */, FUNC_SCCB_SCLK);
    fpioa_set_function(40 /* CAMERA_SDA */, FUNC_SCCB_SDA);
#else
    fpioa_set_function(41 /* CAMERA_SCL */, FUNC_I2C2_SCLK);
    fpioa_set_function(40 /* CAMERA_SDA */, FUNC_I2C2_SDA);
    maix_i2c_init(2, 7, 100*1000);
#endif
    fpioa_set_function(42 /* DVP_RST */, FUNC_CMOS_RST);
    fpioa_set_function(44 /* DVP_PWDN */, FUNC_CMOS_PWDN /* FUNC_CMOS_PWND ?? */);
    fpioa_set_function(46 /* DVP_XCLK */, FUNC_CMOS_XCLK);
    dvp_init(8);
    dvp_enable_burst();
    dvp_disable_auto();
    //dvp_set_output_enable(DVP_OUTPUT_AI, 0);
    //dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 0);

    // Reset camera
    dcmi_rst(1);
    usleep(20*1000);
    dcmi_rst(0);
    usleep(20*1000);

    dcmi_pwdn(0);
    usleep(20*1000);
    dcmi_pwdn(1); /* Toggle to GC0328 */
    usleep(20*1000);

    for(addr = 8; addr != 0x78; addr ++){
        /* Manuf */
        ids[0] = camera_i2c_readreg(addr, 0x1c);
        ids[1] = camera_i2c_readreg(addr, 0x1d);
        /* Prod */
        ids[2] = camera_i2c_readreg(addr, 0x0a);
        ids[3] = camera_i2c_readreg(addr, 0x0b);
        printf("addr: %x = %x,%x:%x,%x\n",addr,ids[0],ids[1],ids[2],ids[3]);

    }
    //dvp_sccb_send_data(0x21, 0xfe, 0);
    ids[0] = camera_i2c_readreg(0x21, 0xf0);
    printf("Q: %x\n", ids[0]);

    printf("\nflip power\n");

    dcmi_pwdn(0); /* Toggle to OV7740 */
    usleep(200*1000);

    // Reset camera
    dcmi_rst(1);
    usleep(20*1000);
    dcmi_rst(0);
    usleep(20*1000);

    for(addr = 8; addr != 0x78; addr ++){
        /* Manuf */
        ids[0] = camera_i2c_readreg(addr, 0x1c);
        ids[1] = camera_i2c_readreg(addr, 0x1d);
        /* Prod */
        ids[2] = camera_i2c_readreg(addr, 0x0a);
        ids[3] = camera_i2c_readreg(addr, 0x0b);
        printf("addr: %x = %x,%x:%x,%x\n",addr,ids[0],ids[1],ids[2],ids[3]);
    }
}
