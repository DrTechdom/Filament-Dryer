/* End of program timer */
void timer_main() {
  long t = timerProgram - millis();

  if (t <= 0 && systemStatus == 2 || t <= 0 && systemStatus == 1) {
    env_hum      = 0;    // Reset enviorment test
    env_temp     = 0;    
    systemStatus = 0;    // Change system to idle
    timerProgram = 0;    // Reset program time
    menuLocation = 1;    // Goto main menu
    refresh      = true; // Redraw menu
     
    // Turn off heat
    heat_opertation(0);

    #if defined(debugMode)
      Serial.println("Program finished!");
    #endif
  }

}

/* Heater debounce timer set */
void timer_set_heater_debounce() {
  timer_heat_debounce = millis() + 60000; /* Set 60 second debounce */
  #if defined(debugMode)
    Serial.println("Setting heater bebounce timer to 60 seconds");
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

/* Enviorment test timer */

bool timer_envTest() {
  bool r = false;
  // Check for a reset
  if (env_hum == 0) {
     fan_operation(true); // Turn only fan on
     
     // Check to see if were at nominal temp
     if (fan_query() == true && temp < temp_cooled) {
       env_hum = 1;
       timer_env_Test = millis() + 30000; // Set 30 second 
       #if defined(debugMode)
        Serial.println("Starting env test for 30 seconds...");
       #endif
    }
  }

  // Stop fan after 15 seconds
  if (timer_env_Test - 15000 <= millis() && env_hum == 1) {
    fan_operation(false); // Turn fan off
  }
  
  // Test is over
  if (timer_env_Test <= millis() && env_hum == 1) {
    env_hum = hum;
    env_temp = temp;
     #if defined(debugMode)
      Serial.println("Env test is over");
      Serial.print("Room Temp/hum: "); Serial.print(env_temp); Serial.print(" / "); Serial.println(env_hum);
     #endif
    timer_env_Test = 0;
    r = true;
  }
  return r;
}

/* Display flip flop function */
bool timer_flipflop() {
  // Flip flop the display
  if (millis() >= timer_display_flip) {
    timer_display_flip = millis() + 5000; // add 5 seconds
    if (displayFlip == true) { displayFlip = false; }else{ displayFlip = true; }
  }
}
