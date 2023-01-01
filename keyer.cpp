#include "toneAC2/toneAC2.h"
#include "pin_definitions.h"
#include "settings.h"
#include "tuner.h"

/**
 CW Keyer
 CW Key logic change with ron's code (ubitx_keyer.cpp)
 Ron's logic has been modified to work with the original uBITX by KD8CEC

 Original Comment ----------------------------------------------------------------------------
 * The CW keyer handles either a straight key or an iambic / paddle key.
 * They all use just one analog input line. This is how it works.
 * The analog line has the internal pull-up resistor enabled. 
 * When a straight key is connected, it shorts the pull-up resistor, analog input is 0 volts
 * When a paddle is connected, the dot and the dash are connected to the analog pin through
 * a 10K and a 2.2K resistors. These produce a 4v and a 2v input to the analog pins.
 * So, the readings are as follows :
 * 0v - straight key
 * 1-2.5 v - paddle dot
 * 2.5 to 4.5 v - paddle dash
 * 2.0 to 0.5 v - dot and dash pressed
 * 
 * The keyer is written to transparently handle all these cases
 * 
 * Generating CW
 * The CW is cleanly generated by unbalancing the front-end mixer
 * and putting the local oscillator directly at the CW transmit frequency.
 * The sidetone, generated by the Arduino is injected into the volume control
 */

 //CW ADC Range
//static const unsigned int cwAdcSTFrom = 0;
static const unsigned int cwAdcSTTo = 50;
static const unsigned int cwAdcBothFrom = cwAdcSTTo + 1;
static const unsigned int cwAdcBothTo = 300;
static const unsigned int cwAdcDotFrom = cwAdcBothTo + 1;
static const unsigned int cwAdcDotTo = 600;
static const unsigned int cwAdcDashFrom = cwAdcDotTo + 1;
static const unsigned int cwAdcDashTo = 800;

/**
 * Starts transmitting the carrier with the sidetone
 * It assumes that we have called cwTxStart and not called cwTxStop
 * each time it is called, the cwTimeOut is pushed further into the future
 */
void cwKeydown(){
  toneAC2(PIN_CW_TONE, globalSettings.cwSideToneFreq);
  digitalWrite(PIN_CW_KEY, 1);

  globalSettings.cwExpirationTimeMs = millis() + globalSettings.cwActiveTimeoutMs;
}

/**
 * Stops the cw carrier transmission along with the sidetone
 * Pushes the cwTimeout further into the future
 */
void cwKeyUp(){
  noToneAC2();
  digitalWrite(PIN_CW_KEY, 0);
  
  globalSettings.cwExpirationTimeMs = millis() + globalSettings.cwActiveTimeoutMs;
}

//Variables for Ron's new logic
#define DIT_L 0x01 // DIT latch
#define DAH_L 0x02 // DAH latch
#define DIT_PROC 0x04 // DIT is being processed
#define PDLSWAP 0x08 // 0 for normal, 1 for swap
#define IAMBICB 0x10 // 0 for Iambic A, 1 for Iambic B
enum KSTYPE {IDLE, CHK_DIT, CHK_DAH, KEYED_PREP, KEYED, INTER_ELEMENT };
static unsigned long ktimer;
unsigned char keyerState = IDLE;
uint8_t keyerControl = 0;

//Below is a test to reduce the keying error. do not delete lines
//create by KD8CEC for compatible with new CW Logic
char update_PaddleLatch(bool isUpdateKeyState) {
  unsigned char tmpKeyerControl = 0;
  unsigned int paddle = analogRead(PIN_ANALOG_KEYER);
  if (paddle >= cwAdcDashFrom && paddle <= cwAdcDashTo)
    tmpKeyerControl |= DAH_L;
  else if (paddle >= cwAdcDotFrom && paddle <= cwAdcDotTo)
    tmpKeyerControl |= DIT_L;
  else if (paddle >= cwAdcBothFrom && paddle <= cwAdcBothTo)
    tmpKeyerControl |= (DAH_L | DIT_L) ;
  else{
    if (KeyerMode_e::KEYER_STRAIGHT != globalSettings.keyerMode)
      tmpKeyerControl = 0 ;
    else if (paddle <= cwAdcDashTo)
      tmpKeyerControl = DIT_L ;
     else
       tmpKeyerControl = 0 ;
  }
  
  if (isUpdateKeyState)
    keyerControl |= tmpKeyerControl;

  return tmpKeyerControl;
}

/*****************************************************************************
// New logic, by RON
// modified by KD8CEC
******************************************************************************/
void cwKeyer(void){
  bool continue_loop = true;
  char tmpKeyControl = 0;
  
  if(KeyerMode_e::KEYER_STRAIGHT == globalSettings.keyerMode){
    while(1){
      tmpKeyControl = update_PaddleLatch(0);
      //Serial.println((int)tmpKeyControl);
      if ((tmpKeyControl & DIT_L) == DIT_L) {
        // if we are here, it is only because the key is pressed
        if (!globalSettings.txActive){
          startTx(TuningMode_e::TUNE_CW);
          globalSettings.cwExpirationTimeMs = millis() + globalSettings.cwActiveTimeoutMs;
        }
        cwKeydown();
        
        while ( tmpKeyControl & DIT_L == DIT_L){
          tmpKeyControl = update_PaddleLatch(0);
          //Serial.println((int)tmpKeyControl);
        }
          
        cwKeyUp();
      }
      else{
        if (0 < globalSettings.cwExpirationTimeMs && globalSettings.cwExpirationTimeMs < millis()){
          globalSettings.cwExpirationTimeMs = 0;
          stopTx();
        }
        return;//Tx stop control by Main Loop
      }

      checkCAT();
    } //end of while
    
  }
  else{//KEYER_IAMBIC_*
    while(continue_loop){
      switch(keyerState){
        case IDLE:
          tmpKeyControl = update_PaddleLatch(0);
          if((tmpKeyControl == DAH_L)//Currently dah
           ||(tmpKeyControl == DIT_L)//Currently dit
           ||(tmpKeyControl == (DAH_L | DIT_L))//Currently both
           ||( keyerControl  & (DAH_L | DIT_L))){//Resolving either
             update_PaddleLatch(true);
             keyerState = CHK_DIT;
          }
          else{
            if (0 < globalSettings.cwExpirationTimeMs && globalSettings.cwExpirationTimeMs < millis()){
              globalSettings.cwExpirationTimeMs = 0;
              stopTx();
            }
            continue_loop = false;
          }
          break;
    
        case CHK_DIT:
          if (keyerControl & DIT_L) {
            keyerControl |= DIT_PROC;
            ktimer = globalSettings.cwDitDurationMs;
            keyerState = KEYED_PREP;
          }else{
            keyerState = CHK_DAH;
          }
          break;
    
        case CHK_DAH:
          if (keyerControl & DAH_L) {
            ktimer = 3*globalSettings.cwDitDurationMs;
            keyerState = KEYED_PREP;
          }else{
            keyerState = IDLE;
          }
          break;
    
        case KEYED_PREP:
          //modified KD8CEC
          if (!globalSettings.txActive){
            globalSettings.cwExpirationTimeMs = millis() + globalSettings.cwActiveTimeoutMs;
            startTx(TuningMode_e::TUNE_CW);
          }
          ktimer += millis(); // set ktimer to interval end time
          keyerControl &= ~(DIT_L + DAH_L); // clear both paddle latch bits
          keyerState = KEYED; // next state
          
          cwKeydown();
          break;
    
        case KEYED:
          if (millis() > ktimer) { // are we at end of key down ?
            cwKeyUp();
            ktimer = millis() + globalSettings.cwDitDurationMs; // inter-element time
            keyerState = INTER_ELEMENT; // next state
          }
          else if(KeyerMode_e::KEYER_IAMBIC_B == globalSettings.keyerMode){
            update_PaddleLatch(1); // early paddle latch in Iambic B mode
          }
          break;
    
        case INTER_ELEMENT:
          // Insert time between dits/dahs
          update_PaddleLatch(1); // latch paddle state
          if (millis() > ktimer) { // are we at end of inter-space ?
            if (keyerControl & DIT_PROC) { // was it a dit or dah ?
              keyerControl &= ~(DIT_L + DIT_PROC); // clear two bits
              keyerState = CHK_DAH; // dit done, check for dah
            }else{
              keyerControl &= ~(DAH_L); // clear dah latch
              keyerState = IDLE; // go idle
            }
          }
          break;
      }
  
      checkCAT();
    } //end of while
  }//end of KEYER_IAMBIC_*
}


