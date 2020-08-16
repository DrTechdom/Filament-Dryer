
// Tempature control
void tempControl() {
  temp_up = dht_up.readTemperature(true); // In F
  temp_low = dht_low.readTemperature(true);// in F
  hum_up = dht_up.readHumidity();
  hum_low = dht_low.readHumidity();

      // Check if any reads failed and exit early (to try again).
  if (isnan(temp_up) || isnan(hum_up)) {
    #if defined(DebugMode)
      Serial.println(F("Failed to read from upper DHT sensor!"));
    #endif
    error_lastStop = "Upper DHT sensor is dissconnected or has stopped responding!";
    systemStatus = 3; // E-stop
  }
  if (isnan(temp_low) || isnan(hum_low)) {
    #if defined(DebugMode)
      Serial.println(F("Failed to read from lower DHT sensor!"));
    #endif
    error_lastStop = "Lower DHT sensor is dissconnected or has stopped responding!";
    systemStatus = 3; // E-stop
  }
  
  // Check to see if the system is running or in pre-heat
  if (systemStatus == 1 || systemStatus == 2) {
      /* Fan Control
       *  ToDo:
       *     - Multi speed fan
       */
      //Fan On
      if (RelayStat[0] == LOW) {
        if (settings_inverted_output == true) {
          digitalWrite(RELAY_FAN_1,LOW); 
        }else{
          digitalWrite(RELAY_FAN_1,HIGH); 
        }
        RelayStat[0] = HIGH;
      }
      
      //Fan Off
      if (RelayStat[0] == HIGH && temp_up < temp_cooled) { 
        if (settings_inverted_output == true) {
          digitalWrite(RELAY_FAN_1,HIGH); 
        }else{
          digitalWrite(RELAY_FAN_1,LOW); 
        }
        RelayStat[0] = LOW;
      }

      /* Heat Control
       *  ToDo:
       *     - High temp alarm
       *     - Temp control
       *     - If temp doesnt change for x seconds shut down heat, cool, and alarm
       */
       // Check for an emergency situation
       if (temp_up >= temp_top_max) {
        error_lastStop = "Over limit tempature!";
        systemStatus = 3; // E-stop
       }else{
        //Pre Heat
        if (systemStatus == 2) {
         // Init heater 1
         if (RelayStat[1] == LOW && temp_up < menu[materialNum].tempature) {
          
          if (settings_inverted_output == true) {
            digitalWrite(RELAY_HEAT_1,LOW); 
          }else{
            digitalWrite(RELAY_HEAT_1,HIGH); 
          }
          temp_lastPreheatCheck = temp_up; // Record last tempature
          timer_maxPreheat = millis() + (time_maxPreheat * 1000); //Set timer
          RelayStat[1] = HIGH;
          timer_high_on = millis(); //turn on high temp timer
          }
        }
        //Pre-Heat off
        if (RelayStat[1] == HIGH && temp_up >= menu[materialNum].tempature && systemStatus == 2) { 
          if (settings_inverted_output == true) {
            digitalWrite(RELAY_HEAT_1,HIGH);
            digitalWrite(RELAY_HEAT_2,HIGH);
            digitalWrite(RELAY_HEAT_3,HIGH);
          }else{
            digitalWrite(RELAY_HEAT_1,LOW);
            digitalWrite(RELAY_HEAT_2,LOW); 
            digitalWrite(RELAY_HEAT_3,LOW); 
          }
          systemStatus = 1;
          RelayStat[1] = LOW;
          RelayStat[2] = LOW;
          RelayStat[3] = LOW;
          timer_high_on = 0;
        }      

        //Heat on normally running
        if (RelayStat[1] == LOW && temp_up <= menu[materialNum].tempature - tempDebounce && systemStatus == 1) { 
          if (settings_inverted_output == true) {
            digitalWrite(RELAY_HEAT_1,LOW); 
          }else{
            digitalWrite(RELAY_HEAT_1,HIGH); 
          }
          systemStatus = 1; 
          RelayStat[1] = HIGH;
          timer_high_on = millis() + 25000; //if he doesnt rise within 25 seconds, turn on high heat
          temp_lastPreheatCheck = temp_up; // Record last tempature
          timer_maxPreheat = millis() + (time_maxPreheat * 1000); //Set timer
        }      

        //High temp limit
        if (RelayStat[1] == HIGH && temp_up >= menu[materialNum].tempature) { 
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
        }

        // Low heat is taking too long to heat, turn on high heat
        if (RelayStat[1] == HIGH && timer_high_on <= millis() && RelayStat[2] == LOW) {
          // Heating element configuration selection
          switch (settings_heater_style) {
            case 0: //heater elements are split as a y, relay 1 and 2 is high
              if (settings_inverted_output == true) {
                digitalWrite(RELAY_HEAT_1,LOW);
                digitalWrite(RELAY_HEAT_2,LOW);
              }else{
                digitalWrite(RELAY_HEAT_1,HIGH);
                digitalWrite(RELAY_HEAT_2,HIGH);
              }
              break;
            case 1: //heater elements are seperate, relay 1 is low, relay 2 is high
              if (settings_inverted_output == true) {
                digitalWrite(RELAY_HEAT_1,HIGH);
                digitalWrite(RELAY_HEAT_2,LOW); 
              }else{
                digitalWrite(RELAY_HEAT_1,LOW); 
                digitalWrite(RELAY_HEAT_2,HIGH); 
              }
              break;
          }
          temp_lastPreheatCheck = temp_up; // Record last tempature
          timer_maxPreheat = millis() + (time_maxPreheat * 1000); //Set timer
          RelayStat[2] = HIGH;
        }

       // time heater if there is an issue
       if (RelayStat[1] == HIGH && millis() >= timer_maxPreheat && (temp_lastPreheatCheck + 2) >= temp_up) {
        error_lastStop = "Heat doesnt rise after aloted time. Is the top on? Heater coil issues fan issues";
        systemStatus = 3; // E-stop
       }
        
       }//End of non-emergency situation
       
  }//end of normal operation

  // Emergency Stop
  if (systemStatus == 3) {
     if (settings_inverted_output == true) {
      digitalWrite(RELAY_HEAT_1,HIGH);
      digitalWrite(RELAY_HEAT_2,HIGH);
      digitalWrite(RELAY_HEAT_3,HIGH);      
     }else{
      digitalWrite(RELAY_HEAT_1,LOW);
      digitalWrite(RELAY_HEAT_2,LOW);
      digitalWrite(RELAY_HEAT_3,LOW);
     }
    // Make sure that the fan is on
    if (RelayStat[0] == LOW) { 
      if (settings_inverted_output == true) {
        digitalWrite(RELAY_FAN_1,LOW); 
      }else{
        digitalWrite(RELAY_FAN_1,HIGH);
      }
      RelayStat[0] = HIGH;
      }
  }

  //If fan is off, shut heats off
  //High temp limit
  if (RelayStat[0] == LOW) {
    if (RelayStat[1] == HIGH || RelayStat[2] == HIGH || RelayStat[3] == HIGH) {
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
  }}
}
