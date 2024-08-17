#include "ui_line.h"

ui_line::ui_line(int x0, int y0,int x1, int y1, uint16_t color, uint16_t thickness)
{
    line_pos_.x = x0;
    line_pos_.y = y0;
    line_pos_.x1 = x1;
    line_pos_.y1 = y1;

    
    line_color_ = color;
    line_thickness_ = thickness;
}

position ui_line::get_position()
{
    return line_pos_;
}

uint16_t ui_line::get_color()
{
    return line_color_;
}

uint16_t ui_line::get_thickness()
{
    return line_thickness_;
}
