/* button */
void buttonAction() {  
  // check button
  encoderButton.Update();
  quickButton1.Update();
  quickButton2.Update();

  // quit if button was not pressed
  if ( ( encoderButton.clicks == 0) && ( quickButton1.clicks == 0 ) && ( quickButton2.clicks == 0 ) ) return;
  
  // 5 short + 1 long clicks
  if ( encoderButton.clicks == -6 ) softReset();                    // delete eeprom and perform a soft reset
  
  // 3 short + 1 long clicks
  if ( ( encoderButton.clicks == -4 ) && ( stateIdx != WIZARD ) ) { // change factors and select auto-factor yes/no
    doseTime[6] = factor[1];                                        // load first factor
    doseIdx=6;       
    portion = 1;
    stateIdx = WIZARD;
    setupStateIdx = STEP4;
  }
 
  // ******************
  // ** Setup wizard **
  // ******************
  if ( (stateIdx == WIZARD) && (encoderButton.clicks == -1) ) { 
    if (setupStateIdx == INIT ) {                 // first wizard step - number of doses e.g. the sieves 
      portion = 1;
      totalPortions = doseIdx;                    // servings
      setTotalPortions();                         // write to eeprom
      doseTime[doseIdx] = dose[1];                // load preselection for the first dose
      setupStateIdx = STEP1;
    } else
    
    if (setupStateIdx == STEP1) {                 // second wizard step - amount of coffee flour (g) per dose 
        if (portion != totalPortions) {      
          dose[portion] = doseTime[doseIdx];      // weigth per serving   
          setPortion();
          doseTime[doseIdx] = dose[portion+1];    // load preselection for the other doses
          portion++;
      } else { 
          dose[portion] = doseTime[doseIdx];
          setPortion();
          portion = 1;
          setupStateIdx = STEP2;
       }
      } else
    
    if (setupStateIdx == STEP2) {             // third wizard step - probe grinding
      if ( (doseIdx+1) % 2 == 0 ) {           // even = YES
        doseTime[doseIdx] = probeGrindTime;   // load preselection as start value
        setupStateIdx = STEP2_1;
        } else {                              // odd = NO
          doseIdx = 6;                        // set start factor to 1.00
          doseTime[6] = factor[1];
          setupStateIdx = STEP4;              
        }
      } else
    
    if (setupStateIdx == STEP2_1) {           // probe grinding time inupt 
      probeGrindTime = doseTime[doseIdx];
      onRelay();
   } else 
    
    if (setupStateIdx == STEP2_2) {           // repeat probe grinding selection
      if ( (doseIdx+1) % 2 == 0 ) {           // even = YES
        setupStateIdx = STEP2_1;              // repeat probe grinding
        } else {                              // odd = NO
          doseTime[doseIdx] = probeGrindTime; // load preselection as start value
          setupStateIdx = STEP3;              // don't repeat - just go to weigth input              
        }
      } else
    
    if (setupStateIdx == STEP3) {             // fourth wizard step - weight input depending on probe grindig
      quotient = probeGrindTime * 100 / doseTime[doseIdx];   
      setQuotient();
      for (int i=1; i<=totalPortions; i++) {  // autocalc dosetime depending on quotient
        doseTime[i] = quotient * dose[i] / 100;
      }
      writeSettingsEEPROM();
      setupStateIdx = STEP4;  
     } else
    
     if (setupStateIdx == STEP4) {                // fifth wizard step - factor automatic on/off
      for (int i=1; i<=totalPortions-1; i++) {    // automatic factor calculation
         factor[i] = ((doseTime[i+1]*1000) / doseTime[i] + 5) / 10;
         setFactor(i);
         if ( i == 1) doseTime[6] = factor[i];    // save first factor for displaying it
        }           
      if ( (doseIdx+1) % 2 == 0 ) { 
        factorAutomatic = true;
        doseIdx=6;                                // doseIdx = 6 will show the first factor on display
        setupStateIdx = STEP5;                    // factor verification/input
      } else {
        factorAutomatic = false;
        setFactorAutomatic();
        laststep();
        }  
      setFactorAutomatic();     
      } else    
    
    if (setupStateIdx == STEP5) {                 // sixth wizard step - factor input
      if ((portion) != totalPortions-1) { 
        doseIdx=6;
        factor[portion] = doseTime[doseIdx];
        setFactor(portion);
        doseTime[doseIdx] = factor[portion+1];
        portion++;  
      } else {
        factor[portion] = doseTime[doseIdx];
        setFactor(portion);
        laststep();
      } 
    } 
  }
  
  // ON -> OFF
  if ( (stateIdx == SJON) && (encoderButton.clicks == 1) ) {
          offRelay();
        }
  
  // OFF -> ON
  else if ( (stateIdx == SJOFF) && (encoderButton.clicks == 1) ) {
          onRelay();
        }
  
  // quickbutton 1 pressed
  else if ( (stateIdx == SJOFF) && (quickButton1.clicks == 1) ) {
          doseIdx = quickButtonPortion[QB1];
          onRelay();
        }
  
  // quickbutton 2 pressed
  else if ( (stateIdx == SJOFF) && (quickButton2.clicks == 1) ) {
          doseIdx = quickButtonPortion[QB2];
          onRelay();
        }
  
  // OFF -> SET
  else if ( (stateIdx == SJOFF) && (doseIdx != DOSEM)&& (encoderButton.clicks == -1) ) {
          stateIdx = SJSET;
        }
  
  // SET -> OFF
  else if ( (stateIdx == SJSET) && (encoderButton.clicks == -1) ) {
          stateIdx = SJOFF;
          if (factorAutomatic) do_dosecalc();
          writeSettingsEEPROM();
        }
  
  // setup Quickbutton 1
  else if ( (stateIdx == SJOFF) && (quickButton1.clicks == -1) ) {
          stateIdx = QB1SET;
        }    
  
  // setup Quickbutton 2
  else if ( (stateIdx == SJOFF) && (quickButton2.clicks == -1) ) {
          stateIdx = QB2SET;
        }
  
  // SET -> OFF
  else if ( (stateIdx == QB1SET ) && (quickButton1.clicks == -1) ) {
          writeQB1SettingsEEPROM();
          stateIdx = SJOFF;
        }
  
  else if ( (stateIdx == QB2SET ) && (quickButton2.clicks == -1) ) {
          writeQB2SettingsEEPROM();
          stateIdx = SJOFF;
        }
    
  refreshDisplay = true;
}

void laststep() {                                                              // last wizard step
      setupStateIdx = false;
      stateIdx = SJOFF; 
      setWizardDone(); 
      doseIdx = DOSEM;
      totalPortions = totalPortions - 1;
      readSettingsEEPROM(eepromVersionNumber);
    }   

void do_dosecalc() { 
  if (doseTime[doseIdx] != eeprom_read_dword(&eepromDoseTime[doseIdx])) {      // check if a time has been changed
    if (doseIdx == totalPortions) {                                            // last time was changed
      for (int i=totalPortions-1; i>=1; i--) {
        doseTime[i] = (doseTime[i+1]*1000 / factor[i] + 5) / 10;
      }
    } else
    if (doseIdx == 1) {
      for (int i=1; i<=totalPortions-1; i++) {                                 // first time was changed
        doseTime[i+1] = (doseTime[i]*10 + 5) * factor[i] / 1000;
      }
    } else {                                                                   // time between first and last time was changed
         for (int i=doseIdx; i<=totalPortions-1; i++) {     
           doseTime[i+1] = (doseTime[i]*10 + 5) * factor[i] / 1000;
         }
         for (int i=doseIdx-1; i>=1; i--) {
           doseTime[i] = (doseTime[i+1]*1000 / factor[i] + 5) / 10;
         }   
    }
    #ifdef DEBUGBUTTON                                                         // show me all times in log
      for (int i=1; i<=totalPortions; i++){
       Serial.print("Time: "); Serial.println(doseTime[i]);
    }
  #endif
  }
}

/* turn relay on */
void onRelay() {
  stateIdx = SJON;
  currentTime = doseTime[doseIdx];
}

/* turn relay off */
void offRelay() {
  if ( (setupStateIdx == STEP2) || (setupStateIdx == STEP2_1) ) { // Probe grinding
    stateIdx = WIZARD;            // back to wizard
    if (setupStateIdx == STEP2) { 
      setupStateIdx = STEP3; 
    } else setupStateIdx = STEP2_2;
  } else stateIdx = SJOFF;        // else: back to normal menu
  addEncoderIdleTime(1000);
}

void softReset() {
  eepromClear();
  pinMode(PINtoRESET, OUTPUT);   // lights out. Assuming it is jumper-ed correctly.
  while(1);                      // never gets here.
}




