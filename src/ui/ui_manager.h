#ifndef _UI_MANAGER_H_
#define _UI_MANAGER_H_

#include <M5EPD.h>
#include <Arduino.h>
#include <FS.h>
#include <vector>
#include "../data_struct.h"
#include "id_generator.h"
#include "ui_object.h"
#include "ui_label.h"
#include "ui_line.h"

class ui_manager
{
public:
    ui_manager(M5EPD_Canvas *canvas);

    void create_button(int x, int y, int w, int h, const uint8_t *default_image_data, const uint8_t *tapped_image_data);
    void create_button2(int x, int y, int w, int h, const uint8_t *default_icon_data);
    void create_object(int x, int y, int w, int h, const uint8_t *image_data);
    void create_label(int x, int y, uint8_t size, uint16_t color, uint16_t cache_size, String title);
    void create_line(int x0, int y0,int x1, int y1, uint16_t color, uint16_t thickness); 
    void create_Hline(int x, int y, int h, uint16_t color, uint16_t thickness);
    void create_Vline(int x, int y, int h, uint16_t color, uint16_t thickness);
    void initialize();

    void draw_all(m5epd_update_mode_t mode);
    void draw_all_periodic(m5epd_update_mode_t mode, unsigned long period_sec);
    void push_button(int id, m5epd_update_mode_t mode);
    void release_button(int id, m5epd_update_mode_t mode);
    void clear();
    int judge_touched_button_id(int x, int y);
    int check_executable_button_id();

    int get_button_num();

private:
    std::vector<ui_object> button_list_;
    std::vector<ui_object> object_list_;
    std::vector<ui_label> label_list_;
    std::vector<ui_line> line_list_;

    int button_num_ = 0;

    id_generator button_id_generator;

    M5EPD_Canvas *canvas_;

    position tmp_touch_point_ = {0, 0};
    position target_touch_point_ = {0, 0};
    bool touch_flag_ = false;
    int touch_id_ = -1;
};

#endif
