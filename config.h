#pragma once
//
// Header file that keeps some global configuration values
//

// The callsign that appears on the main menu
#define CALLSIGN_TEXT "CALLSIGN"

// The software version number of the ubitx firmware
#define VERSION_TEXT "R2.0.1"

// Set to your preferred region for the correct band frequencies
// 1 - Africa, Europe, Middle East, and northern Asia
// 2 - the Americas
// 3 - the rest of Asia and the Pacific
#define IARU_REGION 2

// Display CW speed as
// 0 - words per minute (wpm)
// 1 - chars per minute (cpm)
// 2 - Buchstaben pro Minute (BpM, German special)
#define DISPLAY_CW_SPEED 0

// Sets the default tuning mode to
// 0 - SSB
// 1 - CW
#define CW_IS_DEFAULT 0

// Sets the default tuning step size to
// 0 - 10Hz
// 1 - 20Hz
// 2 - 50Hz
// 3 - 100Hz
#define DEFAULT_TUNING_STEP_SIZE 2

// Sets the GUI theme to use
// 0 - default
// 1 - more oriented towards Google Materials (e.g. rounded buttons), be aware that for this
//     option, the calibration and CAT functionalities had to be removed and aren't available! 
#define GUI_THEME 0
