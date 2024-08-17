#ifndef _UI_LINE_H_
#define _UI_LINE_H_

#include <M5EPD.h>
#include <Arduino.h>
#include <FS.h>
#include "../data_struct.h"

class ui_line
{
public:
    ui_line(int x0, int y0,int x1, int y1, uint16_t color, uint16_t thickness);
    position get_position();
    uint16_t get_color();
    uint16_t get_thickness();
    

private:
    position line_pos_;
    uint16_t line_color_;
    uint16_t line_thickness_;
};

#endif
