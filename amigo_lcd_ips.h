#ifndef __YUNI_AMIGO_LCD_IPS_H
#define __YUNI_AMIGO_LCD_IPS_H

void amigo_lcd_ips_cmd(unsigned int cmd);
void amigo_lcd_ips_data(const unsigned char* data, unsigned long size);
void amigo_lcd_ips_boot(void);
void amigo_lcd_ips_start(void);

#endif
