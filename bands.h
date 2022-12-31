#include <stdint.h>

// Set to your preferred region
// 1 - Africa, Europe, Middle East, and northern Asia
// 2 - the Americas
// 3 - the rest of Asia and the Pacific
#define IARU_REGION 2

void getBandString(const uint32_t frequency,
                   char* band_string_out,
                   const uint16_t max_string_length);

uint32_t getFreqInBand(const uint32_t frequency,
                       const uint8_t target_band);

bool isFreqInBand(const uint32_t frequency,
                  const uint8_t band);
