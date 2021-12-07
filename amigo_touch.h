#ifndef __YUNI_AMIGO_TOUCH_H
#define __YUNI_AMIGO_TOUCH_H

struct amigo_touch_point_s {
    int valid;
    int id;
    int x;
    int y;
    int weight;
    int area;
};

struct amigo_touch_data_s {
    struct amigo_touch_point_s point[2];
};

typedef struct amigo_touch_data_s amigo_touch_data_t;

void amigo_touch_boot(void);
void amigo_touch_start(void);
void amigo_touch_read(amigo_touch_data_t* out);

#endif
