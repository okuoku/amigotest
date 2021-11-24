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
                             &data, size, SPI_TRANS_CHAR);
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

    /* From MaixPy */
    amigo_lcd_ips_cmd(0x01 /* SOFTWARE_RESET */);
    usleep(100000);

    amigo_lcd_ips_cmd(0x11 /* SLEEP_OFF */);
    usleep(100000);

#if 0
    /* Removed following because fbcp-ili9341 do not do this
     * and it resolves flickering display (perhaps sort-of power save?) */
    amigo_lcd_ips_cmd(0xF1); /* Unk */
    t[0] = (0x36);
    t[1] = (0x04);
    t[2] = (0x00);
    t[3] = (0x3C);
    t[4] = (0x0F);
    t[5] = (0x8F);
    amigo_lcd_ips_data(t, 6);

    amigo_lcd_ips_cmd(0xF2); /* Unk */
    t[0] = (0x18);
    t[1] = (0xA3);
    t[2] = (0x12);
    t[3] = (0x02);
    t[4] = (0xB2);
    t[5] = (0x12);
    t[6] = (0xFF);
    t[7] = (0x10);
    t[8] = (0x00);
    amigo_lcd_ips_data(t, 9);

    amigo_lcd_ips_cmd(0xF8); /* Unk */
    t[0] = (0x21);
    t[1] = (0x04);
    amigo_lcd_ips_data(t, 2);

    amigo_lcd_ips_cmd(0xF9); /* Unk */
    t[0] = (0x00);
    t[1] = (0x08);
    amigo_lcd_ips_data(t, 2);
#endif

    amigo_lcd_ips_cmd(0x36); /* Memory Access Control */
    t[0] = (0x28);
    amigo_lcd_ips_data(t, 1);

    amigo_lcd_ips_cmd(0xB4); /* Display Inversion Control */
    t[0] = (0x00);
    amigo_lcd_ips_data(t, 1);

    amigo_lcd_ips_cmd(0xC1); /* Power Control 2 */
    t[0] = (0x41);
    amigo_lcd_ips_data(t, 1);

    amigo_lcd_ips_cmd(0xC5); /* Vcom Control */
    t[0] = (0x00);
    t[1] = (0x18);
    amigo_lcd_ips_data(t, 2);

    amigo_lcd_ips_cmd(0xE0); /* Positive Gamma Control */
    t[0] = (0x0F);
    t[1] = (0x1F);
    t[2] = (0x1C);
    t[3] = (0x0C);
    t[4] = (0x0F);
    t[5] = (0x08);
    t[6] = (0x48);
    t[7] = (0x98);
    t[8] = (0x37);
    t[9] = (0x0A);
    t[10] = (0x13);
    t[11] = (0x04);
    t[12] = (0x11);
    t[13] = (0x0D);
    t[14] = (0x00);
    amigo_lcd_ips_data(t, 15);

    amigo_lcd_ips_cmd(0xE1); /* Negative Gamma Control */
    t[0] = (0x0F);
    t[1] = (0x32);
    t[2] = (0x2E);
    t[3] = (0x0B);
    t[4] = (0x0D);
    t[5] = (0x05);
    t[6] = (0x47);
    t[7] = (0x75);
    t[8] = (0x37);
    t[9] = (0x06);
    t[10] = (0x10);
    t[11] = (0x03);
    t[12] = (0x24);
    t[13] = (0x20);
    t[14] = (0x00);
    amigo_lcd_ips_data(t, 15);

    amigo_lcd_ips_cmd(0x3A); /* Interface Pixel Format */
    t[0] = (0x55);  //RGB565
    amigo_lcd_ips_data(t, 1);
    /*display on*/
    amigo_lcd_ips_cmd(0x29 /* DISPALY_ON ... DISPLAY_ON?? */);
}
