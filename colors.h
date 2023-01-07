// Color definitions
//
// Note that the colors are stored with 5 bit for each
// component of R, G and B. When expanding a uint16_t
// like 0x001F (for BLUE), the 5 bit get shifted left
// by 3 places (multiplied by 8), and then 0x7 gets added!
static const uint16_t DISPLAY_BLACK       = 0x0000;  ///<   0,   0,   0
static const uint16_t DISPLAY_NAVY        = 0x000F;  ///<   0,   0, 123
static const uint16_t DISPLAY_DARKGREEN   = 0x03E0;  ///<   0, 125,   0
static const uint16_t DISPLAY_DARKCYAN    = 0x03EF;  ///<   0, 125, 123
static const uint16_t DISPLAY_MAROON      = 0x7800;  ///< 123,   0,   0
static const uint16_t DISPLAY_PURPLE      = 0x780F;  ///< 123,   0, 123
static const uint16_t DISPLAY_OLIVE       = 0x7BE0;  ///< 123, 125,   0
static const uint16_t DISPLAY_LIGHTGREY   = 0xC618;  ///< 198, 195, 198
static const uint16_t DISPLAY_DARKGREY    = 0x7BEF;  ///< 123, 125, 123
static const uint16_t DISPLAY_BLUE        = 0x001F;  ///<   0,   0, 255
static const uint16_t DISPLAY_GREEN       = 0x07E0;  ///<   0, 255,   0
static const uint16_t DISPLAY_CYAN        = 0x07FF;  ///<   0, 255, 255
static const uint16_t DISPLAY_RED         = 0xF800;  ///< 255,   0,   0
static const uint16_t DISPLAY_MAGENTA     = 0xF81F;  ///< 255,   0, 255
static const uint16_t DISPLAY_YELLOW      = 0xFFE0;  ///< 255, 255,   0
static const uint16_t DISPLAY_WHITE       = 0xFFFF;  ///< 255, 255, 255
static const uint16_t DISPLAY_ORANGE      = 0xFD20;  ///< 255, 165,   0
static const uint16_t DISPLAY_GREENYELLOW = 0xAFE5;  ///< 173, 255,  41
static const uint16_t DISPLAY_PINK        = 0xFC18;  ///< 255, 130, 198
