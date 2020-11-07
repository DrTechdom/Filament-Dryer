/* End of program timer */
void timer_main() {
  long t = timerProgram - millis();

  if (t <= 0 && systemStatus != 0) {
     systemStatus = 0; // Change system to idle
     timerProgram = 0; // Reset program time
     menuLocation = 1; // Goto main menu
     refresh = true; // Redraw menu
     
     // Turn off heat
     if (settings_inverted_output == true) {
      digitalWrite(RELAY_HEAT_1,HIGH);
      digitalWrite(RELAY_HEAT_2,HIGH);
      digitalWrite(RELAY_HEAT_3,HIGH);      
     }else{
      digitalWrite(RELAY_HEAT_1,LOW);
      digitalWrite(RELAY_HEAT_2,LOW);
      digitalWrite(RELAY_HEAT_3,LOW);
     }
  }
}

/* Heater debounce timer set */
void timer_set_heater_debounce() {
  timer_heat_debounce = millis() + 60000; /* Set 60 second debounce */
  #if defined(debugMode)
    Serial.println(F("Setting heater bebounce timer to 60 seconds"));
  #endif
}

/* Heater debounce timer */
bool timer_check_heater_debounce() {
  bool n = false;
  if (timer_heat_debounce < millis()) {
    n = true;
  }
  return n;
}
