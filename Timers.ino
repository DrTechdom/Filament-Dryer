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
