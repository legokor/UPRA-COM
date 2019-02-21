void EEPROM_read_callsign()
{
  char i;
  wdt_reset();
  for(i=0; i<7; i++)
  {
    wdt_reset();
    callsign[i] = EEPROM.read(i);
    delay(100);
  }
}

void EEPROM_store_callsign()
{
  char i;
  wdt_reset();
  for(i=0; i<7; i++)
  {
    wdt_reset();
    EEPROM.write(i, new_callsign[i]);
    delay(100);
  }
  
}

void EEPROM_callsign_set(void)
{
    wdt_reset();
    EEPROM_read_callsign();
    memcpy(telemetry +2, callsign, 7);
    telemetry[9] = ',';  
}

