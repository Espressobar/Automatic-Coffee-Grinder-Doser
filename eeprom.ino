/* initial read settings from EEPROM if version number is correct */
void readSettingsEEPROM( byte eepromVersionNumber ) {
  byte eepromReadVersion = eeprom_read_byte( &eepromVersion );
  if ( eepromReadVersion == eepromVersionNumber ) {
    doseTime[DOSE1] = eeprom_read_dword( &eepromDoseTime[DOSE1] );
    doseTime[DOSE2] = eeprom_read_dword( &eepromDoseTime[DOSE2] );
    doseTime[DOSE3] = eeprom_read_dword( &eepromDoseTime[DOSE3] );
    doseTime[DOSE4] = eeprom_read_dword( &eepromDoseTime[DOSE4] );
    doseTime[DOSE5] = eeprom_read_dword( &eepromDoseTime[DOSE5] );
    
    quickButtonPortion[QB1] = eeprom_read_byte( &eepromButtonPortion[QB1] );   // read quickbutton1 assignment
    quickButtonPortion[QB2] = eeprom_read_byte( &eepromButtonPortion[QB2] );   // read quickbutton2 assignment

    wizardDone = eeprom_read_byte( &eepromWizardDone );  // read state of timer (true = run normally, false = start wizard)
    totalPortions = eeprom_read_byte( &eepromTotalPortions );
    factorAutomatic = eeprom_read_byte( &eepromFactorAutomatic );
    factor[1] = eeprom_read_dword( &eepromFactor[1] );
    factor[2] = eeprom_read_dword( &eepromFactor[2] );
    factor[3] = eeprom_read_dword( &eepromFactor[3] );
    factor[4] = eeprom_read_dword( &eepromFactor[4] );
  } else {
    eeprom_write_byte( &eepromVersion, eepromVersionNumber );
    writeSettingsEEPROM();
    eeprom_write_byte ( &eepromButtonPortion[QB1], quickButtonPortion[QB1] );
    eeprom_write_byte ( &eepromButtonPortion[QB2], quickButtonPortion[QB2] );
    eeprom_write_byte ( &eepromWizardDone, false );
  }
}

/* write settings to EEPROM */
void writeSettingsEEPROM() {
  #ifdef DEBUGEEPROM
    Serial.println("EEPRom written..."); 
  #endif

    eeprom_write_dword( &eepromDoseTime[DOSE1], doseTime[DOSE1] );
    eeprom_write_dword( &eepromDoseTime[DOSE2], doseTime[DOSE2] );
    eeprom_write_dword( &eepromDoseTime[DOSE3], doseTime[DOSE3] );
    eeprom_write_dword( &eepromDoseTime[DOSE4], doseTime[DOSE4] );
    eeprom_write_dword( &eepromDoseTime[DOSE5], doseTime[DOSE5] );
    }
    
/* write Quickbutton1 settings to EEPROM */
void writeQB1SettingsEEPROM() {   
      eeprom_write_byte( &eepromButtonPortion[QB1], doseIdx );
      quickButtonPortion[QB1] = doseIdx;
}

/* write Quickbutton2 settings to EEPROM */
void writeQB2SettingsEEPROM() {
      eeprom_write_byte( &eepromButtonPortion[QB2], doseIdx );
      quickButtonPortion[QB2] = doseIdx;     
}

void setWizardDone() {
  eeprom_write_byte ( &eepromWizardDone, true );
}

void activateWizard() {
  eeprom_write_byte ( &eepromWizardDone, false );
}

void setTotalPortions() {
  #ifdef DEBUGEEPROM 
    Serial.print("TotalPortion written: ");Serial.println(totalPortions); 
  #endif
  eeprom_write_byte ( &eepromTotalPortions, totalPortions);
}

void setPortion() {
  #ifdef DEBUGEEPROM 
    Serial.print("Dose written: ");Serial.print(portion);Serial.print(" Amount: ");Serial.println(dose[portion]); 
  #endif
  eeprom_write_dword( &eepromPortion[portion], dose[portion] );
}

void setFactor(int i) {
  #ifdef DEBUGEEPROM 
    Serial.print("Factor written: ");Serial.print(i);Serial.print(" Value: ");Serial.println(factor[i]); 
  #endif
  eeprom_write_dword( &eepromFactor[i], factor[i] );
}

void setQuotient() {
  #ifdef DEBUGEEPROM 
    Serial.print("Quotient written: ");Serial.println(quotient); 
  #endif
  eeprom_write_dword( &eepromQuotient, quotient);
}

void setFactorAutomatic() {
  #ifdef DEBUGEEPROM 
    Serial.print("Automatik written: ");Serial.println(factorAutomatic); 
  #endif
  eeprom_write_byte( &eepromFactorAutomatic, factorAutomatic);
}

void eepromClear() {
  #ifdef DEBUGEEPROM 
    Serial.println("del EEprom..."); 
  #endif
 uint8_t sreg, i;
 uint16_t addr;
 uint8_t clear[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
 uint8_t data[8];

 sreg = SREG;
 cli();

 // Write page by page using the block writing method
 for(addr = 0; addr < 512; addr += 8)
 {
   eeprom_read_block((void *)&data[0], (const void *)addr, 8);
   for(i = 0; i < 8; i++)
     if(data[i] != 0xFF)
     {
       eeprom_write_block((void*)&clear[0], (void*)addr, 8);
       break;
     }
 }
		
 SREG = sreg;
}

