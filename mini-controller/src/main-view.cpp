#include <tnlog.h>

#include "main-view.h"

#define STATUS_AREA_X       0
#define STATUS_AREA_Y       16

#define VIEW_AREA_X         0
#define VIEW_AREA_Y         16

MainView::MainView(U8G2 & display) :
    _display(display)
{

}

void MainView::drawImageView(uint8_t width, uint8_t height, uint8_t * xbm, char * label)
{
    u8g2_uint_t displayWidth = _display.getWidth();
    u8g2_uint_t displayHeight = _display.getHeight();
    u8g2_uint_t occupiedHeight = 0;

    if (label != nullptr){
        _display.setFont(u8g2_font_6x12_t_cyrillic);        
        _display.enableUTF8Print();
        u8g2_uint_t textWidth = _display.getUTF8Width(label);
        u8g2_uint_t x = (displayWidth - textWidth - VIEW_AREA_X) / 2;
        u8g2_uint_t textHeight = _display.getMaxCharHeight();
        u8g2_uint_t y = displayHeight - textHeight + 5;

        _display.setCursor(x,y);
        _display.print(label);
        occupiedHeight = textHeight;
    }

    u8g2_uint_t availableHeight = displayHeight - occupiedHeight - VIEW_AREA_Y;
    u8g2_uint_t imageX = (displayWidth - width - VIEW_AREA_X) / 2 + VIEW_AREA_X;
    u8g2_uint_t imageY = (availableHeight - height) / 2 + VIEW_AREA_Y;

    _display.drawXBM(imageX, imageY, width, height, xbm);

}