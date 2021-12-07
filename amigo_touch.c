#include <fpioa.h>
#include <i2c.h>

#include <bsp.h>

#include "amigo_touch.h"

static void
touch_i2c_cfg(void){
    i2c_init(I2C_DEVICE_0, 0x38, 7, 400*1000);
}

void
amigo_touch_boot(void){
    uint8_t reg;
    uint8_t dat[2];
    /* Pin configure */
    /* FIXME: Move this to common I2C configure */
    fpioa_set_function(24 /* ??? */, FUNC_I2C0_SCLK);
    fpioa_set_function(27 /* ??? */, FUNC_I2C0_SDA);

    /* Ident device */
    /* FIXME: Acquire I2C-0 lock */
    touch_i2c_cfg();
    dat[0] = 0xcc;
    reg = 0xa8; /* VendorID */
    i2c_recv_data_dma(DMAC_CHANNEL0, DMAC_CHANNEL1,
                      I2C_DEVICE_0, &reg, 1,
                      dat, 1);
    printf("VendorID = %x\n", dat[0]);
    dat[0] = 0xcc;
    reg = 0xa3; /* ChipID */
    i2c_recv_data_dma(DMAC_CHANNEL0, DMAC_CHANNEL1,
                      I2C_DEVICE_0, &reg, 1,
                      dat, 1);
    printf("ChipID = %x\n", dat[0]);
}

void
amigo_touch_start(void){
    uint8_t dat[2];
    /* FIXME: Acquire I2C-0 lock */
    touch_i2c_cfg();
    dat[0] = 0x00; /* device mode */
    dat[1] = 0;
    i2c_send_data_dma(DMAC_CHANNEL0, I2C_DEVICE_0, dat, 2);

    dat[0] = 0x80; /* threshold */
    dat[1] = 22;
    i2c_send_data_dma(DMAC_CHANNEL0, I2C_DEVICE_0, dat, 2);

    dat[0] = 0x88; /* touch rate */
    dat[1] = 0x0e;
    i2c_send_data_dma(DMAC_CHANNEL0, I2C_DEVICE_0, dat, 2);
}

void
amigo_touch_read(amigo_touch_data_t* out){
    uint8_t reg;
    uint8_t dat[16];

    reg = 0;

    /* FIXME: Acquire I2C-0 lock */
    touch_i2c_cfg();
    i2c_recv_data_dma(DMAC_CHANNEL0, DMAC_CHANNEL1,
                      I2C_DEVICE_0, &reg, 1,
                      dat, 15);

    out->point[0].valid = 0;
    out->point[1].valid = 0;

    if(dat[2] == 1 || dat[2] == 2){
        out->point[0].valid = 1;
        out->point[0].x = ((dat[0x3] & 0xf) << 8) + dat[0x4];
        out->point[0].y = ((dat[0x5] & 0xf) << 8) + dat[0x6];
        out->point[0].id = dat[0x5] >> 4;
        out->point[0].weight = dat[0x7];
        out->point[0].area = dat[0x8];
    }
    if(dat[2] == 2){
        out->point[1].valid = 1;
        out->point[1].x = ((dat[0x9] & 0xf) << 8) + dat[0xa];
        out->point[1].y = ((dat[0xb] & 0xf) << 8) + dat[0xc];
        out->point[1].id = dat[0xb] >> 4;
        out->point[1].weight = dat[0xd];
        out->point[1].area = dat[0xe];
    }
}
