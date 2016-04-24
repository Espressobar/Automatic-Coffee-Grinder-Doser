/*

 La Pavoni Zip Timer(version 05/28/2015) by Ralf Jardon (user "Richy" @ http://www.kaffee-netz.de )
 
 for the most part based on Mazzer Timer(version 08/08/2014
 by Alexandros Pollakis (user "A1exandros" of http://www.kaffee-netz.de )
 
 Translated and adapted to Danish language by Steffe Lav (user "S.Lav" @ http://www.kaffee-netz.de or "Lav" @ http://www.espressobar.dk )

 changelog v03/03/16
 - added screensaver. Allows you to enable screensaver after a defined amount of seconds.
 - added light control. Allows you to add an LED to pin 10. Light will fade up and down according to defined values. Will fade down when screensaver is activated.
 - timer indicator, frame will only be drawn when actually grinding.
 
 changelog v05/28/15
 - bugfixes
 - code optimization
 - wizard: changed step4 against step5. Step4 is now "Factor YES/NO" and Step5 is now "factor input".
 
 changelog v05/27/15
 - improve encoder reliability

 changelog v05/26/15
 - bugfixes (round long var values in do_dosecalc and factor calculation to improve results)
 - added u8glib constructor for SPI displays

 changelog v05/25/15
 - bugfixes (2 doses yes/no problem solved)
 - added probeGrindTime as preselection in config section
 - added probeGrindTime input in wizard
 - added repeat function for probe grinding
 - recalculate times only if something has been changed (auto-factor = yes)

 changelog v05/24/15
 - flex- and fix-factor concept discarded
 - added Setup-Wizard:
 -- allows you to specify the number of doses
 -- allows you to specify the coffee quantity each dose
 -- offers a grinding sample (10s)
 -- automatically calculates a quotiont dependig on grinding sample (weight divided by grinding time)
 -- automatically calculates each time depending on quotient
 -- automatically calculates the factors depending on quantity of flour each dose
 -- allows you to verify and change the factors
 -- allows you to choose between factor automatic and manual programming mode
 - added soft-reset function and restart of wizard (5 short + 1 long encoder click)
 - added auto-factor setup (3 short + 1 long encoder click)

 changelog v04/22/15 (obsolete)
 - added flex-factor autocalc function

 changelog v04/17/15 (obsolete)
 - added fix-factor autocalc function

 changelog v04/14/15
 - expanded menu to 5 portions
 - changed direction of encoder (cw = inc values; ccw = dec values)
 - added two QuickButtons for grinding without using the encoder selection
 - added Setup-Menu for QuickButtons (you can choose what portion will be grinded)

 */

// include libraries
#include <MsTimer2.h>     // https://www.dropbox.com/sh/7uxorxk525zrtxk/AACaTxeZjhuq60BrImUm0_Lra?dl=0
#include <ClickButton.h>  // see above
#include <U8glib.h>       // see above
#include <avr/eeprom.h>   // enable eeprom read and write
#include <stdio.h>
#include <LEDFader.h>     // https://github.com/jgillick/arduino-LEDFader

// *** BEGIN OF CONFIG SECTION ************************************************************************************************************************
// define arduino pins
#define ENCODER_A_PIN  2
#define ENCODER_B_PIN  3
#define BTN_PIN        4  // Push button
#define QBTN_PIN_1     5  // pin quick-button 1
#define QBTN_PIN_2     6  // pin quick-button 2
#define RELAY_PIN      8
#define LED_PIN       10  //Pin for LED-light.
#define PINtoRESET     12 // connect wire from this pin to rst in order to use the soft-reset function

// read settings from EEPROM if version number is correct; change version number to reset settings
byte eepromVersionNumber = 20;

//                       *****************************************************
// usefull preselections ***** configure what you know and what you need *****
//                       *****************************************************

long doseTime[]           = { 3000, 35, 70, 85, 105, 140, 0 };  // time in 1/10s that is value 55 for 5.5s. First position is manual grind mode (3000ms = 5 minutes). Position 6 is reserved for wizard step 3.
long factor[]             = { 0, 116, 171, 116, 150 };         // factor pre-selection (usefull when skipping grind calibration). First position is a placeholder because of the manual grind mode!
byte dose[]               = { 0, 70, 140, 180, 210, 280 };      // pre-selected doses. First position is a placeholder because of the manual grind mode!
byte quickButtonPortion[] = { 1, 2 };                          // pre-selection quickbuttons
char *portion_name[]      = {" 7g", " 14g", "18g", "21g", "28g" }; // name of portions (the weight in gram) which is shown on display
int probeGrindTime        = 100;                               // time for calibration grinding (e.g. 100 = 10s)


// Screensaver
// Uncomment the following line to activate screensaver:
#define screensaver

#ifdef screensaver
  boolean isPaused = false;
  const long interval = 60;        // Seconds of idletime before activating screensaver.
  unsigned long previousMillis = 0;
#endif

// LIGHT
// Connect LED power + to pin 10.
// uncomment the following line to activate light control:
#define LIGHT

#ifdef LIGHT
  #define FADE_TIME 3000          // amount of milliseconds it takes to fade light. Fade time when grind is finished is fixed to 10 seconds.
  #define MAX_LIGHT 255           // 0-255. Amount of light when grinding.
  #define MIN_LIGHT 128           // 0-255. Amount of light when idle.
  #define DIR_UP 1
  #define DIR_DOWN -1
  LEDFader led;
  int direction = DIR_UP;
#endif

// *** DEBUGING ***

//#define DEBUG          // remove leading "//" in order to activate serial monitor
//#define DEBUGBUTTON
//#define DEBUGEEPROM
//#define DEBUGENCODER

// *** END OF CONFIG SECTION **************************************************************************************************************************

// relay mode is digital ralay(s)
#define ON  LOW                 // pin state for acting relay as ON
#define OFF HIGH                // pin state for acting relay as OFF

// state
#define SJOFF             0     // grinder off
#define SJON              1     // grinder on
#define SJSET             2     // time setup
#define QB1SET            3     // quickbuttons setup (button1)
#define QB2SET            4     // quickbuttons setup (button2)
#define WIZARD            5     // wizard setup mode 
#define INIT              6     // wizard: number of doses
#define STEP1             7     // wizard: amount of coffee flour (g) per dose
#define STEP2             8     // wizard: probe grinding (yes/no)
#define STEP2_1           9     // wizard: input probe time
#define STEP2_2           10    // wizard: repeat probe grinding (yes/no)
#define STEP3             11    // wizard: time input depending on probe grindig
#define STEP4             12    // wizard: factor automatic (yes/no)
#define STEP5             13    // wizard: factor input
#define PAUSE             14    // Screensaver

// Quickbuttons
#define QUICKBUTTONS 2          // number of programable quickbuttons
#define QB1 0                   // quickbuttons numbering
#define QB2 1

// Doses
#define DOSEM 0                 // grind manually

#define DOSE1 1                 // dose 1 e.g.  7g
#define DOSE2 2                 // dose 2 e.g. 14g
#define DOSE3 3                 // dose 3 e.g. 18g
#define DOSE4 4                 // dose 4 e.g. 21g
#define DOSE5 5                 // dose 5 e.g. 28g

volatile int stateIdx = 0;      // state-index      - controlls the operation mode
volatile int doseIdx = 1;       // dose-index       - controlls which dose will be processed
volatile int setupStateIdx = 0; // setupstate-index - controlls the setup mode

volatile long quotient;         // quotient = how many grams coffeeflour per second

int one, ten, hundred, n;       // helper
byte wizardDone;                // if true, the timer will be in normal operation mode
byte portion;                   // number of serving (the sieves)
byte totalPortions;             // how many servings in timer
byte factorAutomatic;           // if true, the timer will recalculate each dose time after altering a single time

volatile int currentTime = 0;
volatile long timerIdleTime;
static long timerIdleDuration = 100;

// encoder
volatile int encoderVal;
volatile long encoderIdleTime;
volatile long encoderIdleDuration;

// eeprom
EEMEM byte eepromVersion, eepromButtonPortion[QUICKBUTTONS], eepromTotalPortions, eepromWizardDone, eepromFactorAutomatic;
EEMEM unsigned long eepromDoseTime[7 - 1], eepromFactor[4], eepromPortion[5], eepromQuotient;

// display
//U8GLIB_SSD1306_128X64 u8g(10, 9);             // SPI
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // I2C
#define display_width 128
#define display_height 64
volatile boolean refreshDisplay = true;

// button
ClickButton encoderButton(BTN_PIN, LOW, CLICKBTN_PULLUP);
ClickButton quickButton1(QBTN_PIN_1, LOW, CLICKBTN_PULLUP);
ClickButton quickButton2(QBTN_PIN_2, LOW, CLICKBTN_PULLUP);

/* SET UP */
void setup() {



  pinMode(PINtoRESET, INPUT);    // Just to be clear, as default is INPUT. Not really needed.
  digitalWrite(PINtoRESET, LOW); // Prime it, but does not actually set output.
  // Does disable 10K pull Up, but who cares.
  // debugging
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("Arduino is ready!");
#endif

  // relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, OFF);

  // encoder
  pinMode(ENCODER_A_PIN, INPUT);
  pinMode(ENCODER_B_PIN, INPUT);
  digitalWrite(ENCODER_A_PIN, HIGH);
  digitalWrite(ENCODER_B_PIN, HIGH);
  attachInterrupt(0, encoderISR, LOW);

  readSettingsEEPROM( eepromVersionNumber);

  // encoder parameter
  encoderIdleTime = millis() + 1000;
  encoderIdleDuration = 5;
  encoderVal = 0;

  // button parameter
  encoderButton.debounceTime   = 1;    // Debounce timer in ms
  encoderButton.multiclickTime = 250;  // Time limit for multi clicks
  encoderButton.longClickTime  = 1000; // time until "held-down clicks" register

  quickButton1.debounceTime   = 1;    // Debounce timer in ms
  quickButton1.multiclickTime = 250;  // Time limit for multi clicks
  quickButton1.longClickTime  = 1000; // time until "held-down clicks" register

  quickButton2.debounceTime   = 1;    // Debounce timer in ms
  quickButton2.multiclickTime = 250;  // Time limit for multi clicks
  quickButton2.longClickTime  = 1000; // time until "held-down clicks" register


  // timer interrupt
  MsTimer2::set(1, timerISR);  // period to check relay state: every 100ms
  MsTimer2::start();
  timerIdleTime = 0;

  // Setup wizard
  if (!wizardDone) {
    totalPortions = 5;      // allow max. 5 doses in setup mode
    doseIdx = 2;            // start value min. 2 doses
    stateIdx = WIZARD;      // activate wizard
    setupStateIdx = INIT;   // select first wizard step
  } else {
    doseIdx = 1;
  }

  // display bug workaround (not needed on my display)
  //stateIdx = SJSET; drawDisplay();
  //stateIdx = SJOFF; drawDisplay();
    #ifdef LIGHT
    led = LEDFader(LED_PIN);
    onLight();
  #endif
}

/* MAIN */
void loop() {

  #ifdef LIGHT
    led.update();
  #endif
    
  #ifdef screensaver
    pauseTimer();
  #endif

  // rebuild display
  if ( refreshDisplay ) {
    refreshDisplay = false;
    drawDisplay();
  }
}

/* timer ISR to execute the thread controller*/
void timerISR() {

  // button
  buttonAction();

  // timer
  long time = millis();
  if ( time > timerIdleTime ) {
    timerIdleTime = time + timerIdleDuration;
    timerAction();
  }

  // write actual state to relay pin
  digitalWrite(RELAY_PIN, stateIdx == SJON ? ON : OFF);
}

/* timer action*/
void timerAction() {
  // check currentTime
  if ( stateIdx == SJON ) {
    currentTime--;
    
    #ifdef screensaver
      previousMillis = millis();
    #endif
    
    refreshDisplay = true;
    if (currentTime < 0) {
      

      
      offRelay();
    }
  }
}

#ifdef screensaver 

  void pauseTimer() {
  
    if (isPaused !=true || stateIdx != SJON || stateIdx !=WIZARD) {
      // || stateIdx !=SJSET || stateIdx !=QB1SET || stateIdx !=QB2SET
      //Ovenstående skal kun være med, hvis man ikke ønsker pause, mens man er ved at indstille noget.
  
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval*1000) {
      previousMillis = currentMillis;
      
    onPause();
    
       }
     }
   }


  void onPause() {
    isPaused = true;
    stateIdx = PAUSE;
    //fade light to 0
    refreshDisplay = true;   
      // LED no longer fading, switch direction
    #ifdef LIGHT
    direction = DIR_DOWN;
      led.fade(0, FADE_TIME);
    #endif
  }
  
  void offPause() {
    isPaused = false;
    stateIdx = SJOFF;
    previousMillis = millis();
    #ifdef LIGHT
      onLight();
    #endif
  }
#endif

#ifdef LIGHT
  void onLight(){
    direction = DIR_UP;
     led.fade(MIN_LIGHT, FADE_TIME);
    }
#endif


