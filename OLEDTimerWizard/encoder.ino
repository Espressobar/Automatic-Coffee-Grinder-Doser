/* encoder */
/* encoder ISR pin A/B */
void encoderISR() {
  delay(1);  // wait a little until the bouncing is done

  long time = millis();
  if ( time > encoderIdleTime ) {
    encoderIdleTime = time + encoderIdleDuration;
    if (digitalRead(ENCODER_A_PIN) == digitalRead(ENCODER_B_PIN) ) {
      encoderVal = -1; // encoder direction 
    } else {
      encoderVal = 1;
    }
    if (stateIdx != SJON) {
      encoderAction();
      if (refreshDisplay) {
        refreshDisplay = false;
        drawDisplay();
      }
    }
  }
}

void encoderAction()
{
  if (encoderVal != 0) {  // encoder is rotating
      switch (stateIdx) {
        case WIZARD:      // Wizard-Mode: Step1, Step2_1, Step3, Step5
           if ((setupStateIdx == STEP1 ) || (setupStateIdx == STEP2_1) || (setupStateIdx == STEP3 ) || (setupStateIdx == STEP5 )) {   
           integerInput();
        break;
          } else {        // Wizard-Mode: Init, Step2, Step2_2, Step4
         chooser(0,2);
        break;
        }
   
        case SJOFF:       // normal operation mode
          chooser(1,0);
        break;
     
       case QB1SET:       // quickbutton 1 setup mode
          chooser(0,1);
       break;
        
       case QB2SET:       // quickbutton 2 setup mode
          chooser(0,1);
       break;        
        
       default:           // Time setup mode (SJSET)
          integerInput();
       break;
      }
      
    // refresh display
    refreshDisplay = true;
    
    #ifdef DEBUGENCODER
     Serial.print("stateIdx: ");Serial.print(stateIdx,DEC);Serial.print("  SetupStateIdx: ");Serial.print(setupStateIdx,DEC);Serial.print("  doseIdx:");Serial.print(doseIdx,DEC); Serial.print("  doseTime: ");Serial.println(doseTime[doseIdx]);
    #endif
  }
  // reset encoder value
  encoderVal = 0;
}

void integerInput(){                // input integer for time and weight
  doseTime[doseIdx] += (encoderVal > 0) ? 1 : -1;
  if (doseTime[doseIdx] <= 0) doseTime[doseIdx]=0;
}

void chooser(byte max, byte min){   // input integer from max to min e.g. YES/NO, number of doses, QB-setup...
  int ADOSES; 
  if ( setupStateIdx == INIT ) {
    ADOSES = totalPortions-1; 
  } else {
    ADOSES = totalPortions+max;
  }
  if ((setupStateIdx == STEP2) ||(setupStateIdx == STEP2_2) ||(setupStateIdx == STEP4)) {
    if ((totalPortions == 3) || (totalPortions == 5)) ++ADOSES;
  } 
  doseIdx += (encoderVal > 0) ? 1 : -1;
  doseIdx = doseIdx%ADOSES;
  if (doseIdx < min) doseIdx = doseIdx + ADOSES;
}

// add idle time
void addEncoderIdleTime( long time ) {
  encoderIdleTime = encoderIdleTime + time;
}

