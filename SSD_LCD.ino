
void display_dry() {
  // Read temperature as Fahrenheit (isFahrenheit = true)
 // float f1 = dht_up.readTemperature(true);
//  float f2 = dht_low.readTemperature(true);
 // float h1 = dht_up.readHumidity();
//  float h2 = dht_low.readHumidity();

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  if (menuLocation == 4) {
    display.println("*Auto Dry*");
  }else{
    display.println("*Time Dry*");
  }
  display.setTextSize(1);
  display.setCursor(0, 17);
  if (menuLocation == 4) {
    display.print("Material: "); display.println(menu[materialNum].name);
  }else{
    display.println(" ");
  }

  // Screen info flip flop
  if (displayFlip == false) {
    display.print("Heat status: ");
    if (RelayStat[1] == LOW &&
        RelayStat[2] == LOW &&
        RelayStat[3] == LOW) { 
          display.println("IDLE"); 
    }else{
      if (systemStatus == 2) { display.println("PRE-HEAT"); }
      else if (RelayStat[3] == HIGH) { display.println("HIGH"); } 
      else if (RelayStat[2] == HIGH) { display.println("MED"); }
      else if (RelayStat[1] == HIGH) { display.println("LOW"); } 
    }
  
  
    display.print("Fan Status: ");
    if (RelayStat[0] == LOW) { display.println("IDLE"); } else { display.println("HIGH"); }
  }else{
    display.print("Temp: "); display.println(temp_up);

    display.print("Humidity %: "); display.println(hum_up);
  }
  
  long t = timerProgram - millis();
  display.println("Time left: "); 
  display.setTextSize(2);
  if (numberOfHours(t) < 10) { display.print("0"); }
  display.print(numberOfHours(t)); display.print(":");
  if (numberOfMinutes(t) < 10) { display.print("0"); }
  display.print(numberOfMinutes(t)); display.print(":");
  if (numberOfSeconds(t) < 10) { display.print("0"); }
  display.println(numberOfSeconds(t));
  
  display.display();
}

/* Tempature Function
void update_tempature() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h1 = dht_up.readHumidity();
  float h2 = dht_low.readHumidity();
  // Read temperature as Celsius (the default)
  float t1 = dht_up.readTemperature();
  float t2 = dht_low.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f1 = dht_up.readTemperature(true);
  float f2 = dht_low.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h1) || isnan(t1) || isnan(f1)) {
    Serial.println(F("Failed to read from upper DHT sensor!"));
    return;
  }
  if (isnan(h2) || isnan(t2) || isnan(f2)) {
    Serial.println(F("Failed to read from lower DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif_up = dht_up.computeHeatIndex(f1, h1);
  float hif_low = dht_low.computeHeatIndex(f2, h2);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic_up = dht_up.computeHeatIndex(t1, h1, false);
  float hic_low = dht_low.computeHeatIndex(t2, h2, false);

  Serial.print(F("Upper DHT -- "));
  Serial.print(F("Humidity: "));
  Serial.print(h1);
  Serial.print(F("%  Temperature: "));
  Serial.print(t1);
  Serial.print(F("°C "));
  Serial.print(f1);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic_up);
  Serial.print(F("°C "));
  Serial.print(hif_up);
  Serial.println(F("°F"));

  Serial.print("Lower DHT -- ");
  Serial.print(F("Humidity: "));
  Serial.print(h2);
  Serial.print(F("%  Temperature: "));
  Serial.print(t2);
  Serial.print(F("°C "));
  Serial.print(f2);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic_low);
  Serial.print(F("°C "));
  Serial.print(hif_low);
  Serial.println(F("°F"));

}
*/

void MenuSelection() {
  byte items = -1;
  byte f = 0;
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  
  //Get count of menu items
  for (byte i = 0; i < (sizeof(menu) / sizeof(menu[0])); i++) {
    if (menu[i].menuNum == menuLocation) {
        items++;
    }
    f++;
  }
  
  // Check status of up button button
  if (BtnStat[0] == HIGH && selectedPos != 0) {
    selectedPos--;
  }

  // Check Status of down
  if (BtnStat[1] == HIGH && selectedPos != items - 1) {
    selectedPos++;
  }

  f = 0;
  int menupage = (selectedPos / 3) * 6;
  for (byte i = menupage; i < (sizeof(menu) / sizeof(menu[0])); i++) {
    if (menu[i].menuNum == menuLocation) {
      // Display title of menu
      if (f == 0) {
     //   Serial.print("1st - "); Serial.print(f); Serial.print(" - "); Serial.println(menu[i].name);
        display.println(menu[i].name);
        display.setTextSize(1);
        display.setCursor(0, 17);
        display.setTextSize(1);
      }else{
     //   Serial.print("2nd - "); Serial.print(f); Serial.print(" - "); Serial.println(menu[i].name);
        //Menu Curser
        if (selectedPos == f-1) {
          display.print(F("> "));
        }else{
          display.print(F("  "));
        }
    
        // Menu item
        
          display.println(menu[i].name);
        }
    f++;
    }
  }
  
  display.display();

  // Check Status of enter button
  if (BtnStat[2] == HIGH) {
  f = 0;
  for (byte i = 0; i < (sizeof(menu) / sizeof(menu[0])); i++) {
    if (menu[i].menuNum == menuLocation) {
    //    Serial.print("22nd - "); Serial.print(f); Serial.print(" - "); Serial.println(menu[i].name);
        //Menu Curser
        if (selectedPos == f-1) {
          materialNum = i;
          menuLocation = menu[i].sMode; //Change to auto
        }
    f++;
    }
  }
    selectedPos = 0;
    BtnStat[2] = LOW;
    if (menuLocation < 4) { refresh = true; }
  }
}

byte ButtonStatus(byte btn) {
  byte r = LOW;
  byte statbtn = digitalRead(BTN[btn]);
  //Serial.print("BTN: "); Serial.print(btn); Serial.print(" Stat: "); Serial.println(statbtn);
  
  if (statbtn == LOW && previous[btn] == HIGH && (millis() - firstTime[btn]) > 50) {
    firstTime[btn] = millis();                            // Take time stamp when first pressed. 200 milliseconds acts as debouncing.
    r = HIGH;
  }
  
//  millis_held[btn] = (millis() - firstTime[btn]);         // Count how many milliseconds the button has been pressed for.
  
//  if (statbtn == LOW && previous[btn] == HIGH && millis_held[btn] >= 50) {
//    r = HIGH;
//  }
  
  previous[btn] = statbtn;
  return r;
}
