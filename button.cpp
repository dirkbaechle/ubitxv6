#include "button.h"

#include <avr/pgmspace.h>

#include "color_theme.h"
#include "nano_gui.h"
#include "scratch_space.h"

void drawButton(Button* button)
{
  if(nullptr != button->text){
    strncpy_P(b,button->text,sizeof(b));
  }
  else if(nullptr != button->text_override){
    button->text_override(b,sizeof(b));
  }
  else{
    //Something's messed up
    //Serial.println(F("No text for button!"));
    return;
  }
#if GUI_THEME == 0
  if (button->status() == ButtonStatus_e::Active) {
    displayText(b, button->x, button->y, button->w, button->h, COLOR_ACTIVE_TEXT, COLOR_ACTIVE_BACKGROUND, COLOR_INACTIVE_BORDER);
  } else {
    displayText(b, button->x, button->y, button->w, button->h, COLOR_INACTIVE_TEXT, COLOR_INACTIVE_BACKGROUND, COLOR_INACTIVE_BORDER);
  }
#else
  displayFillrect(button->x, button->y, button->w, button->h, COLOR_BACKGROUND);
  if (button->status() == ButtonStatus_e::Active) {
    displayFillroundrect(button->x, button->y, button->w, button->h, button->h/2, COLOR_ACTIVE_BACKGROUND);
    displayText(b, button->x, button->y, button->w, button->h, COLOR_ACTIVE_TEXT, COLOR_ACTIVE_BACKGROUND, COLOR_ACTIVE_BACKGROUND);
  } else {
    displayText(b, button->x, button->y, button->w, button->h, COLOR_INACTIVE_TEXT, COLOR_INACTIVE_BACKGROUND, COLOR_INACTIVE_BACKGROUND);
    displayRoundrect(button->x, button->y, button->w, button->h, button->h/2, COLOR_INACTIVE_TEXT);
  }
#endif
}

void extractAndDrawButton(Button* button_out, const Button* button_P)
{
  memcpy_P(button_out,button_P,sizeof(*button_out));
  drawButton(button_out);
}
