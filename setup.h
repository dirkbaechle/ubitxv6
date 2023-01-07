#pragma once

#include "menu.h"
#include "config.h"

extern Menu_t* const setupMenu;

#if GUI_THEME == 0
void setupTouch();
void runLocalOscSetting();
void runBfoSetting();
#endif
void runCwSpeedSetting();