#include <bsp.h>
#include "amigo_powercfg.h"
#include "amigo_lcd_ips.h"
#include "amigo_touch.h"
#include "amigo_camera.h"

#include "sysctl.h" // For tick()
#include "lvgl.h"

static int errno;

int *__errno(void){
    // FIXME: turn it to TLS
    return &errno;
}

static unsigned char fb[320*480*2];

static int /* bool */
map_touch(amigo_touch_data_t* dat, int idx, int* out_x, int* out_y){
    int rx,ry,px,py;
    if(dat->point[idx].valid){
        rx = dat->point[idx].x;
        ry = dat->point[idx].y;
        px = rx;
        py = ry;
        /* Clip */
        if(px < 0){
            px = 0;
        }
        if(px >= 320){
            px = 319;
        }
        if(py < 0){
            py = 0;
        }
        if(py >= 480){
            py = 479;
        }
        *out_x = px;
        *out_y = py;
        return 1;
    }else{
        return 0;
    }
}

static lv_disp_draw_buf_t disp_draw_buf;
static lv_disp_drv_t disp_drv;

static void
flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p){
    lv_disp_flush_ready(drv);
}

static uint64_t
tick(void){
    uint64_t freq_ms = sysctl_clock_get_freq(SYSCTL_CLOCK_CPU) / 1000UL;
    uint64_t c = read_cycle();
    return c / freq_ms;
}


int
main(void){
    int x,y;
    uint64_t tp,tn;

    lv_style_t style;
    lv_obj_t* p0;
    lv_obj_t* p1;
    lv_obj_t* p0_x;
    lv_obj_t* p0_y;
    lv_obj_t* p1_x;
    lv_obj_t* p1_y;

    lv_point_t p_x_p[2] = {{0,0},{319,0}};
    lv_point_t p_y_p[2] = {{0,0},{0,479}};

    amigo_touch_data_t touch;
    printf("Hello.\n");
    printf("Init powercfg.\n");
    powercfg_boot();
    printf("Init LCD.\n");
    amigo_lcd_ips_boot();
    printf("Start LCD.\n");
    amigo_lcd_ips_start();
    printf("Start Touch.\n");
    amigo_touch_boot();
    amigo_touch_start();
    // FIXME: Touch inits I2C
    amigo_camera_boot();
    printf("Done.\n");
    /* LVGL Init */
    tp = tick();
    lv_init();
    lv_disp_draw_buf_init(&disp_draw_buf, fb, NULL, 320*480);
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_draw_buf;
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 480;
    disp_drv.flush_cb = flush_cb;
    disp_drv.direct_mode = 1;
    lv_disp_drv_register(&disp_drv);

    lv_style_init(&style);
    lv_style_set_line_width(&style, 5);
    lv_style_set_line_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    p0_x = lv_line_create(lv_scr_act());
    p0_y = lv_line_create(lv_scr_act());
    p1_x = lv_line_create(lv_scr_act());
    p1_y = lv_line_create(lv_scr_act());
    p0 = lv_label_create(lv_scr_act());
    p1 = lv_label_create(lv_scr_act());
    lv_label_set_text(p0, "p0");
    lv_label_set_text(p1, "p1");
    lv_obj_add_style(p0_x, &style,0);
    lv_obj_add_style(p0_y, &style,0);
    lv_obj_add_style(p1_x, &style,0);
    lv_obj_add_style(p1_y, &style,0);
    lv_line_set_points(p0_x, p_x_p, 2);
    lv_line_set_points(p1_x, p_x_p, 2);
    lv_line_set_points(p0_y, p_y_p, 2);
    lv_line_set_points(p1_y, p_y_p, 2);

    /* Instantiate lines */
    lv_obj_set_align(p0, LV_ALIGN_TOP_LEFT);
    lv_obj_set_align(p1, LV_ALIGN_TOP_LEFT);
    lv_obj_set_align(p0_x, LV_ALIGN_TOP_LEFT);
    lv_obj_set_align(p0_y, LV_ALIGN_TOP_LEFT);
    lv_obj_set_align(p1_x, LV_ALIGN_TOP_LEFT);
    lv_obj_set_align(p1_y, LV_ALIGN_TOP_LEFT);

    /* Initially hide */
    lv_obj_add_flag(p0_x, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(p0_y, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(p1_x, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(p1_y, LV_OBJ_FLAG_HIDDEN);

    for(;;){
        x = y = 0;
        amigo_touch_read(&touch);
        if(map_touch(&touch, 0, &x, &y)){
            lv_obj_set_pos(p0_x, 0, y);
            lv_obj_set_pos(p0_y, x, 0);
            lv_obj_set_pos(p0, x, y);
            lv_obj_clear_flag(p0_x, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(p0_y, LV_OBJ_FLAG_HIDDEN);
        }else{
            lv_obj_add_flag(p0_x, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(p0_y, LV_OBJ_FLAG_HIDDEN);
        }

        x = y = 0;
        if(map_touch(&touch, 1, &x, &y)){
            lv_obj_set_pos(p1_x, 0, y);
            lv_obj_set_pos(p1_y, x, 0);
            lv_obj_set_pos(p1, x, y);
            lv_obj_clear_flag(p1_x, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(p1_y, LV_OBJ_FLAG_HIDDEN);
        }else{
            lv_obj_add_flag(p1_x, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(p1_y, LV_OBJ_FLAG_HIDDEN);
        }

#if 0
        if(touch.point[0].valid){
            printf("0(%d): %04d %04d @%d %d\n",
                   touch.point[0].id,
                   touch.point[0].x,
                   touch.point[0].y,
                   touch.point[0].weight,
                   touch.point[0].area);
        }
        if(touch.point[1].valid){
            printf("1(%d): %04d %04d @%d %d\n",
                   touch.point[1].id,
                   touch.point[1].x,
                   touch.point[1].y,
                   touch.point[1].weight,
                   touch.point[1].area);
        }
#endif

        tn = tick();
        lv_tick_inc(tn - tp);
        tp = tn;
        lv_timer_handler();

        amigo_lcd_ips_push_rgnpix(0,0,320,480,fb,320*480);
    }
    return 0;
}
