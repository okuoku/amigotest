#ifndef __YUNI_AMIGO_LCD_IPS_H
#define __YUNI_AMIGO_LCD_IPS_H

void amigo_lcd_ips_cmd(unsigned int cmd);
void amigo_lcd_ips_data(const unsigned char* data, unsigned long size);
void amigo_lcd_ips_boot(void);
void amigo_lcd_ips_start(void);
void amigo_lcd_ips_push_pix(const void* pix, int pixlen);
void amigo_lcd_ips_push_rgnpix(int x, int y, int w, int h, const void* pix, int pixlen);

#endif
