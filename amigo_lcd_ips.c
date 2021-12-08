#include <fpioa.h>
#include <sysctl.h>
#include <gpiohs.h>
#include <spi.h>
#include <sleep.h>

/* TFT, IPS common */
static void 
dcx_control(void){
    gpiohs_set_pin(6, GPIO_PV_LOW);
}

static void
dcx_data(void){
    gpiohs_set_pin(6, GPIO_PV_HIGH);
}

static void
rst(int val){
    if(val){
        gpiohs_set_pin(7, GPIO_PV_HIGH);
    }else{
        gpiohs_set_pin(7, GPIO_PV_LOW);
    }
}

void
amigo_lcd_ips_cmd(unsigned int cmd){
    uint8_t d;
    dcx_control();
    d = cmd;
    spi_init(0, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(0, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
    spi_send_data_normal_dma(3 /* DMA ch */, 0, 3 /* SS3 */, 
                             &d, 1, SPI_TRANS_CHAR);
}

void
amigo_lcd_ips_data(const unsigned char* data, unsigned long size){
    dcx_data();
    spi_init(0, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(0, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
    spi_send_data_normal_dma(3 /* DMA ch */, 0, 3 /* SS3 */, 
                             data, size, SPI_TRANS_CHAR);
}

void
amigo_lcd_ips_boot(void){
    /* Pin configure */
    fpioa_set_function(36 /* LCD_CS */, FUNC_SPI0_SS3);
    fpioa_set_function(39 /* LCD_WR */, FUNC_SPI0_SCLK);
    fpioa_set_function(38 /* LCD_DC */, FUNC_GPIOHS6);
    fpioa_set_function(37 /* LCD_RST */, FUNC_GPIOHS7);
    sysctl_set_spi0_dvp_data(1);

    /* GPIO init */
    gpiohs_set_drive_mode(6, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(7, GPIO_DM_OUTPUT);
    rst(0);
    dcx_data();

    /* SPI0 init */
    spi_init(0, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_set_clk_rate(0, 20000000 /* 20MHz */);
}

/* IPS init */
void
amigo_lcd_ips_start(void){
    unsigned char t[16]; 

    rst(1);
    usleep(100000);

    /* 9481 */
    amigo_lcd_ips_cmd(0x01 /* SOFTWARE_RESET */);
    usleep(100000);


    amigo_lcd_ips_cmd(0xd1); /* VCOM control */
    t[0] = 0;
    amigo_lcd_ips_data(t, 1);

    amigo_lcd_ips_cmd(0xc8 /* gamma */);
    t[0] = 0;
    t[1] = 0x30;
    t[2] = 0x56;
    t[3] = 0;
    t[4] = 0;
    t[5] = 0x8;
    t[6] = 0x34;
    t[7] = 0x75;
    t[8] = 0x77;
    t[9] = 0;
    t[10] = 8;
    t[11] = 0;
    amigo_lcd_ips_data(t, 12);

    amigo_lcd_ips_cmd(0x21 /* invert */);

    amigo_lcd_ips_cmd(0x11 /* SLEEP_OFF */);
    usleep(100000);

    amigo_lcd_ips_cmd(0x3A); /* Interface Pixel Format */
    t[0] = (0x55);  //RGB565
    amigo_lcd_ips_data(t, 1);

    amigo_lcd_ips_cmd(0x36); /* Memory Access Control */
    t[0] = (1 << 3 /* BGR */ | 1 << 6 /* Invert horizontal */);
    amigo_lcd_ips_data(t, 1);

    amigo_lcd_ips_cmd(0x29 /* DISPALY_ON ... DISPLAY_ON?? */);
}

void
amigo_lcd_ips_push_pix(const void* pix, int pixlen){
    if(pixlen){
        amigo_lcd_ips_cmd(0x2c);
        dcx_data();
        spi_init(0, SPI_WORK_MODE_0, SPI_FF_OCTAL, 16, 0);
        spi_init_non_standard(0, 16, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
        spi_send_data_normal_dma(3 /* DMA ch */, 0, 3 /* SS3 */, 
                                 pix, pixlen, SPI_TRANS_SHORT);
    }
}

void
amigo_lcd_ips_push_rgnpix(int x, int y, int w, int h, 
                          const void* pix, int pixlen){
    unsigned char t[4];
    unsigned int x1;
    unsigned int x2;
    unsigned int y1;
    unsigned int y2;

    x1 = x;
    x2 = (x+w)-1; /* Final pixel */
    y1 = y;
    y2 = (y+h)-1; /* Final pixel */

    amigo_lcd_ips_cmd(0x2a /* Set X */);
    t[0] = x1 >> 8;
    t[1] = x1 & 0xff;
    t[2] = x2 >> 8;
    t[3] = x2 & 0xff;
    amigo_lcd_ips_data(t, 4);

    amigo_lcd_ips_cmd(0x2b /* Set Y */);
    t[0] = y1 >> 8;
    t[1] = y1 & 0xff;
    t[2] = y2 >> 8;
    t[3] = y2 & 0xff;
    amigo_lcd_ips_data(t, 4);

    amigo_lcd_ips_push_pix(pix, pixlen);
}

