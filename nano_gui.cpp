#include <Arduino.h>
#include "nano_gui.h"
#include "colors.h"
#include "pin_definitions.h"
#include "push_button.h"
#include "scratch_space.h"
#include "settings.h"
#include "touch.h"
#include "config.h"

#include <SPI.h>
#include <avr/pgmspace.h>


/*****************
 * Begin TFT functions
 *****************/
#define ILI9341_CS_PIN PIN_TFT_CS
#define ILI9341_DC_PIN PIN_TFT_DC
#define	ILI9341_SAVE_SPCR (1) //Save state before/after to play nice with the touch screen
#include "PDQ_MinLib/PDQ_ILI9341.h"
PDQ_ILI9341 tft;

#include "nano_font.h"

void displayInit(void){
  //Pulling this low 6 times should exit deep sleep mode
  pinMode(PIN_TFT_CS,OUTPUT);
  for(uint8_t i = 0; i < 6; ++i){
    digitalWrite(PIN_TFT_CS,HIGH);
    digitalWrite(PIN_TFT_CS,LOW);
  }
  digitalWrite(PIN_TFT_CS,HIGH);//Disable writing for now

  tft.begin();
  tft.setFont(ubitx_font);
  tft.setTextWrap(true);
  tft.setTextColor(DISPLAY_GREEN,DISPLAY_BLACK);
  tft.setTextSize(1);
  tft.setRotation(1);
}

void displayPixel(unsigned int x, unsigned int y, unsigned int c){
  tft.fillRect(x,y,1,1,c);
}

void displayHline(unsigned int x, unsigned int y, unsigned int w, unsigned int c){
  tft.fillRect(x,y,w,1,c);
}

void displayVline(unsigned int x, unsigned int y, unsigned int l, unsigned int c){
  tft.fillRect(x,y,1,l,c);
}

void displayClear(unsigned int color){
  tft.fillRect(0,0,320,240,color);
}

void displayRect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c){
  tft.fillRect(x,y,w,1,c);
  tft.fillRect(x,y,1,h,c);
  tft.fillRect(x,y+h-1,w,1,c);
  tft.fillRect(x+w-1,y,1,h,c);
}

void displayCorner(unsigned int x, unsigned int y, unsigned int w, unsigned int c){
  for (unsigned int i = 0; w > 0; --w) {
    tft.drawFastHLine(x, y+i, w, c);
    ++i;
  }
}

void displayRoundrect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int r,unsigned int c){
  tft.drawRoundRect(x, y, w, h, r, c);
}

void displayFillrect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c){
  tft.fillRect(x,y,w,h,c);
}

void displayFillroundrect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int r,unsigned int c){
  tft.fillRoundRect(x,y,w,h,r,c);
}

void displayFillcircle(unsigned int x, unsigned int y, unsigned int r, unsigned int c){
  tft.fillCircle(x, y, r, c);
}

void displayChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg) {
  tft.drawCharGFX(x,y,c,color,bg,1);
}

void displayRawText(const char *text, int x1, int y1, int w, int color, int background){
  tft.setTextColor(color,background);
  tft.setCursor(x1,y1);
  tft.setBound(x1,x1+w);
  tft.print(text);
}

void displayAlignedText(const char *const text, int x1, int y1, int w, int h, int color, int background, TextJustification_e justification)
{
  int16_t x1_out;
  int16_t y1_out;
  uint16_t width_out;
  uint16_t height_out;
  tft.getTextBounds(text,x1,y1,&x1_out,&y1_out,&width_out,&height_out,w);
  if(TextJustification_e::Center == justification){
    x1 += (w - ( (int32_t)width_out + (x1_out-x1)))/2;
  }
  else if(TextJustification_e::Right == justification){
    x1 += w - ((int32_t)width_out + (x1_out-x1));
  }
  else{
    x1 += 2;//Give a little bit of padding from the border
  }
  y1 += (ubitx_font->yAdvance + h - ( (int32_t)height_out))/2;
  displayRawText(text,x1,y1,w,color,background);
}

void displayText(const char *const text, int x1, int y1, int w, int h, int color, int background, int border, TextJustification_e justification)
{
#if GUI_THEME == 0
  displayFillrect(x1, y1, w ,h, background);
  displayRect(x1, y1, w ,h, border);
#else
  if (background != border) {
    displayRect(x1, y1, w ,h, border);
  }
#endif
  displayAlignedText(text, x1, y1, w, h, color, background, justification);
}

void displayButtonText(const char *const text, int x1, int y1, int w, int h, int color, int background, int border, TextJustification_e justification)
{
#if GUI_THEME == 0
  displayFillrect(x1, y1, w ,h, background);
  displayRect(x1, y1, w ,h, border);
#else
  displayFillroundrect(x1, y1, w ,h, h/2, background);
#endif
  displayAlignedText(text, x1, y1, w, h, color, background, justification);
}

void drawCross(int16_t x_center,int16_t y_center,uint16_t color)
{
  constexpr uint8_t HALF_SIZE = 10;
  displayHline(x_center-HALF_SIZE,y_center,2*HALF_SIZE,color);
  displayVline(x_center,y_center-HALF_SIZE,2*HALF_SIZE,color);
}

void setupTouch(){
  constexpr int16_t CROSS_CORNER_OFFSET = 20;
  constexpr Point CROSS_CORNER_POINTS [] = {
    {CROSS_CORNER_OFFSET,CROSS_CORNER_OFFSET},//Top left
    {PDQ_ILI9341::ILI9341_TFTHEIGHT-CROSS_CORNER_OFFSET,CROSS_CORNER_OFFSET},//Top right
    {CROSS_CORNER_OFFSET, PDQ_ILI9341::ILI9341_TFTWIDTH-CROSS_CORNER_OFFSET},//Bottom left
    {PDQ_ILI9341::ILI9341_TFTHEIGHT-CROSS_CORNER_OFFSET,PDQ_ILI9341::ILI9341_TFTWIDTH-CROSS_CORNER_OFFSET}//Bottom right
  };
  
  displayClear(DISPLAY_BLACK);
  strncpy_P(b,(const char*)F("Click on the cross\nPush tune to cancel"),sizeof(b));
  displayText(b, 20,100, 200, 50, DISPLAY_WHITE, DISPLAY_BLACK, DISPLAY_BLACK);

  Point cal_points[sizeof(CROSS_CORNER_POINTS)/sizeof(CROSS_CORNER_POINTS[0])];

  for(uint8_t i = 0; i < sizeof(CROSS_CORNER_POINTS)/sizeof(CROSS_CORNER_POINTS[0]); ++i){
    drawCross(CROSS_CORNER_POINTS[i].x,CROSS_CORNER_POINTS[i].y,DISPLAY_WHITE);
    while(!readTouch(&cal_points[i])){
      if(ButtonPress_e::NotPressed != CheckTunerButton()){
        return;
      }
      delay(100);
    }
    while(readTouch(&cal_points[i])){
      delay(100);
    }
    drawCross(CROSS_CORNER_POINTS[i].x,CROSS_CORNER_POINTS[i].y,DISPLAY_BLACK);
    delay(1000);//Ensure that nobody is pressing the screen before we do the next point
  }

  //We can get nicer scaling if we allow more resolution on the divisor
  constexpr int32_t SCALE_SENSITIVITY_MULTIPLIER = 10;

  const int16_t diff_x_top = cal_points[1].x - cal_points[0].x;
  const int16_t diff_x_bottom = cal_points[3].x - cal_points[2].x;
  constexpr int32_t diff_x_target = CROSS_CORNER_POINTS[1].x - CROSS_CORNER_POINTS[0].x;

  //Average the measured differences
  globalSettings.touchSlopeX = SCALE_SENSITIVITY_MULTIPLIER*(diff_x_top + diff_x_bottom) / (2*diff_x_target);

  const int16_t diff_y_left = cal_points[2].y - cal_points[0].y;
  const int16_t diff_y_right = cal_points[3].y - cal_points[1].y;
  constexpr int32_t diff_y_target = CROSS_CORNER_POINTS[2].y - CROSS_CORNER_POINTS[0].y;

  //Average the measured differences
  globalSettings.touchSlopeY = SCALE_SENSITIVITY_MULTIPLIER*(diff_y_left + diff_y_right) / (2*diff_y_target);

  globalSettings.touchOffsetX = cal_points[0].x - ((CROSS_CORNER_OFFSET * globalSettings.touchSlopeX)/SCALE_SENSITIVITY_MULTIPLIER);
  globalSettings.touchOffsetY = cal_points[0].y - ((CROSS_CORNER_OFFSET * globalSettings.touchSlopeY)/SCALE_SENSITIVITY_MULTIPLIER);


/*
  for(uint8_t i = 0; i < sizeof(cal_points)/sizeof(cal_points[0]); ++i){
    Serial.print(cal_points[i].x);Serial.print(':');Serial.println(cal_points[i].y);
  }
  
  //for debugging
  Serial.print(globalSettings.touchSlopeX); Serial.print(' ');
  Serial.print(globalSettings.touchSlopeY); Serial.print(' ');
  Serial.print(globalSettings.touchOffsetX); Serial.print(' ');
  Serial.println(globalSettings.touchOffsetY); Serial.println(' ');
*/

  SaveSettingsToEeprom();
}


