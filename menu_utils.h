#pragma once

#include "button.h"
#include "menu.h"

//Returns true if button was found, false otherwise
bool findPressedButton(const Button* const* buttons,
                       const uint8_t num_buttons,
                       Button *const button_out,
                       const Point touch_point);

void initSelector(int16_t *const raw_select_val_in_out,
                  const Button* const* buttons,
                  const uint8_t num_buttons);

void adjustSelector(int16_t *const raw_select_val_in_out,
                    int16_t knob,
                    const Button* const* buttons,
                    const uint8_t num_buttons);

void endSelector(const Button *const button);
