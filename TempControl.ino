// Tempature control
void tempControl() {
  /* Grab readings from sensors */
  temp = dht.readTemperature(true); // In F
  tempc = dht.readTemperature(true); // In F
  hum = dht.readHumidity();


  // Check if any reads failed and exit early (to try again).
  if (isnan(temp) || isnan(hum)) {
    #if defined(DebugMode)
      Serial.println("Failed to read from DHT sensor!");
    #endif
    error_lastStop = "DHT sensor is dissconnected or has stopped responding!";
    systemStatus = 9; // E-stop
  }
  
   // Check for over tempature
   if (temp >= temp_max) {
    error_lastStop = "Over limit tempature!";
    systemStatus = 9; // E-stop
   }

  //Turn the fan off when cooled
  if (fan_query() == true && temp < temp_cooled && systemStatus != 2 && systemStatus != 3) {
    #if defined(debugMode)
     Serial.print("Cooling satified - turning fan off.."); Serial.println(temp);
    #endif
    fan_operation(false);
  }

  //Turn fan on if its hot, maybe from a power failer or a restart
  if (fan_query() == false && temp > temp_cooled && systemStatus <= 2) {
    #if defined(debugMode)
     Serial.print("Temp above cut off - cooling..."); Serial.println(temp);
    #endif
    fan_operation(true);
  }
  
  switch(systemStatus) {
    case 1: // Normal Operation
        //Heat call
        /* Combine heating call and heating is taking too long into one function using timer and switches */
        if (temp <= menu[materialNum].tempature - tempDebounce && timer_check_heater_debounce()) {
          timer_set_heater_debounce();
          heat_setting++; //Bunp the heat setting up
          heat_opertation(heat_setting);
          /* NOTE: When using single element or without fan give more time */
          timer_high_on = millis() + 240000; //if he doesnt rise within 3 minutes, turn on high heat
          temp_lastPreheatCheck = temp; // Record last tempature
       //   timer_maxPreheat = millis() + (time_maxPreheat * 70000); //Set timer for heater issue

        #if defined(debugMode)
          Serial.print("Calling for heat.... "); Serial.println(temp);
        #endif
        }

        // Heat is taking too long to heat
        if (heat_query() != 0 && timer_high_on < millis() && timer_high_on != 0) { // && temp_lastPreheatCheck <= (temp_lastPreheatCheck + 2)
          /* Make sure we're not at high heat yet */
          if (heat_query() != 3) {
          heat_setting++; //Bunp the heat setting up
          heat_opertation(heat_setting);
          /* NOTE: When using single element or without fan give more time */
          timer_high_on = millis() + 240000; //if he doesnt rise within 4 minutres, turn on high heat
          temp_lastPreheatCheck = temp; // Record last tempature
         // timer_maxPreheat = millis() + (time_maxPreheat * 45000); //Set timer for heater issue

        #if defined(debugMode)
          Serial.print("Heat is taking too long, moving heat to: "); Serial.println(heat_setting);
           Serial.println(temp);
        #endif
          }else{
            /* Alarm if high heat is even taking too long*/
            error_lastStop = "Heat doesnt rise     after aloted time. Is the top on? Heater coil issues fan issues";
            systemStatus = 9; // E-stop
            #if defined(debugMode)
              Serial.println("Heater issues");
            #endif       
          }
        }

      // Quick and ditry hold
      if (temp <= menu[materialNum].tempature - 3.5) {
        heat_holding = false;
      }
      
      // Tempature satified
      if (!heat_holding && temp >= menu[materialNum].tempature - 2 && timer_check_heater_debounce() && heat_setting != 1) { // -2 due to the cool down of the coil
          heat_setting--;
          timer_high_on = 0;
          heat_holding = true;
          heat_opertation(heat_setting);
          timer_set_heater_debounce();

        #if defined(debugMode)
          Serial.println("Heat is satisfied");
        #endif
      }

      //Tempature over satisfied
      if (temp >= menu[materialNum].tempature + 4  && timer_check_heater_debounce() && heat_setting != 0) {
          heat_setting--;
          timer_high_on = 0;
          heat_opertation(heat_setting);
          timer_set_heater_debounce();

        #if defined(debugMode)
          Serial.println("Heat is over satisfied");
        #endif
      }

      // Tempature Running away
      if (temp >= menu[materialNum].tempature + 10) {
        error_lastStop = "Heater run away.. Heats wont shut off";
        systemStatus = 9; // E-stop
        #if defined(debugMode)
          Serial.print("Heater run away: "); Serial.println(temp);
        #endif
      }
      break;
    case 2: //Pre-heat
     // Pre-Heat Start
     if (temp < menu[materialNum].tempature && heat_setting != 3) {
      fan_operation(true);
      heat_opertation(3);
      temp_lastPreheatCheck = temp; // Record last tempature
      timer_high_on = millis(); //turn on high temp timer

       #if defined(debugMode)
         Serial.println("Pre-Heating");
       #endif
      }

      // Pre-Heat Complete
      if (temp > menu[materialNum].tempature) {
        env_hum = hum; // Set envirmental humidity stat
        env_temp = temp;
        heat_opertation(2); /* Move heat to medium */
        timer_high_on = 0; /* Reset heat timer */
        fan_operation(true);
        systemStatus = 1; /* Set system to drying */

       #if defined(debugMode)
         Serial.println("Pre-Heating Complete");
       #endif
      }  
      break;
    case 9: // Emergency
      // Turn heats off
      if (RelayStat[1] == HIGH || RelayStat[2] == HIGH|| RelayStat[3] == HIGH && settings_inverted_output == false) { 
       heat_opertation(0);
      }

      if (RelayStat[1] == LOW || RelayStat[2] == LOW|| RelayStat[3] == LOW && settings_inverted_output == true) { 
       heat_opertation(0);
      }

            
      // Make sure that the fan is on
      if (RelayStat[0] == HIGH && settings_inverted_output == true) { 
        fan_operation(true);
        }
      if (RelayStat[0] == LOW && settings_inverted_output == false) { 
        fan_operation(true);
        }
  }

  //If fan is off, shut heats off
  if (RelayStat[0] == LOW && settings_inverted_output == true) {    
    if (RelayStat[1] == HIGH || RelayStat[2] == HIGH || RelayStat[3] == HIGH) {
      heat_opertation(0);
      fan_operation(true);
  }}

  if (RelayStat[0] == HIGH && settings_inverted_output == false) {    
    if (RelayStat[1] == LOW || RelayStat[2] == LOW || RelayStat[3] == LOW) {
      heat_opertation(0);
      fan_operation(true);
  }}
}

/* Tempature control
 *  
 *  0 = all off
 *  1 = low
 *  2 = med
 *  3 = high
 */

int heat_query() { 
  return heat_setting;
}

bool fan_query() {
  bool r = false;
  
  if (RelayStat[0] == HIGH && settings_inverted_output == true ) { r = true; }
  if (RelayStat[0] == LOW && settings_inverted_output == false ) { r = true; }
  return r;
}

void heat_opertation(byte temp) {
  
  #if defined(debugMode)
    Serial.print("Changing heat to: "); 
  #endif
  
  switch (temp) {
    case 0: // Off
      #if defined(debugMode)
        Serial.println("Off"); 
      #endif
      if (settings_inverted_output == true) {
        digitalWrite(RELAY_HEAT_1,HIGH);
        digitalWrite(RELAY_HEAT_2,HIGH);
        digitalWrite(RELAY_HEAT_3,HIGH);
      }else{
        digitalWrite(RELAY_HEAT_1,LOW);
        digitalWrite(RELAY_HEAT_2,LOW);
        digitalWrite(RELAY_HEAT_3,LOW);
      }
      RelayStat[1] = LOW;
      RelayStat[2] = LOW;
      RelayStat[3] = LOW;
      break;
    case 1: // Low
      #if defined(debugMode)
        Serial.println("Low"); 
      #endif
      // Heating element configuration selection
      switch (settings_heater_style) {
        case 0: //heater elements are split as a y, relay 1 is low, relay 2 is med, relay 1 and 2 is high
          if (settings_inverted_output == true) {
            digitalWrite(RELAY_HEAT_1,LOW);
            digitalWrite(RELAY_HEAT_2,HIGH);
          }else{
            digitalWrite(RELAY_HEAT_1,HIGH);
            digitalWrite(RELAY_HEAT_2,LOW);
          }
          RelayStat[1] = HIGH;
          RelayStat[2] = LOW;
          break;
       case 1: //heater elements are seperate, relay 1 is low, relay 2 is high
        break;
       case 2: //heater elements are seperate, relay 1 is low, relay 2 is medium, relay 3 is high
        break;
      }
      break;
    case 2: // Medium
      #if defined(debugMode)
        Serial.println("Medium"); 
      #endif
      // Heating element configuration selection
      switch (settings_heater_style) {
        case 0: //heater elements are split as a y, relay 1 is low, relay 2 is med, relay 1 and 2 is high
          if (settings_inverted_output == true) {
            digitalWrite(RELAY_HEAT_1,HIGH);
            digitalWrite(RELAY_HEAT_2,LOW);
          }else{
            digitalWrite(RELAY_HEAT_1,LOW);
            digitalWrite(RELAY_HEAT_2,HIGH);
          }
          RelayStat[1] = LOW;
          RelayStat[2] = HIGH;
          break;
       case 1: //heater elements are seperate, relay 1 is low, relay 2 is high
        break;
       case 2: //heater elements are seperate, relay 1 is low, relay 2 is medium, relay 3 is high
        break;
      }
      break;
    case 3: // High
      #if defined(debugMode)
        Serial.println("High"); 
      #endif
      // Heating element configuration selection
      switch (settings_heater_style) {
        case 0: //heater elements are split as a y, relay 1 is low, relay 2 is med, relay 1 and 2 is high
          if (settings_inverted_output == true) {
            digitalWrite(RELAY_HEAT_1,LOW);
            digitalWrite(RELAY_HEAT_2,LOW);
          }else{
            digitalWrite(RELAY_HEAT_1,HIGH);
            digitalWrite(RELAY_HEAT_2,HIGH);
          }
          RelayStat[1] = HIGH;
          RelayStat[2] = HIGH;
          break;
        case 1: //heater elements are seperate, relay 1 is low, relay 2 is high
          if (settings_inverted_output == true) {
            digitalWrite(RELAY_HEAT_1,HIGH);
            digitalWrite(RELAY_HEAT_2,LOW); 
          }else{
            digitalWrite(RELAY_HEAT_1,LOW); 
            digitalWrite(RELAY_HEAT_2,HIGH); 
          }
          RelayStat[1] = LOW;
          RelayStat[2] = HIGH;
          break;
       case 2: //heater elements are seperate, relay 1 is low, relay 2 is medium, relay 3 is high
          if (settings_inverted_output == true) {
            digitalWrite(RELAY_HEAT_1,HIGH);
            digitalWrite(RELAY_HEAT_2,HIGH);
            digitalWrite(RELAY_HEAT_3,LOW);
          }else{
            digitalWrite(RELAY_HEAT_1,LOW); 
            digitalWrite(RELAY_HEAT_2,LOW);
            digitalWrite(RELAY_HEAT_3,HIGH);
          }
          break;
      }
      break;
  }
    heat_setting = temp;
}

void fan_operation(bool state) {
  if (state != fan_query()) {
     #if defined(debugMode)
      Serial.print("Fan changed to: "); if (state == 1) { Serial.println("On"); }else{ Serial.println("Off"); }
     #endif
    if (settings_inverted_output == true) {
      if (state == true) {
        digitalWrite(RELAY_FAN_1,LOW);
        RelayStat[0] = HIGH;
      }else{
        digitalWrite(RELAY_FAN_1,HIGH);
        RelayStat[0] = LOW;
      }
    }else{
      if (state == true) {
        digitalWrite(RELAY_FAN_1,HIGH);
        RelayStat[0] = HIGH;
      }else{
        digitalWrite(RELAY_FAN_1,LOW);
        RelayStat[0] = LOW;
      }
    }
  }
}
