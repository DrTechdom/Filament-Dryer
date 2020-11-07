/*
 * In opertaion screen
 */
void display_dry() {
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
    if (heat_query() == 0) { 
          display.println("IDLE"); 
    }else{
      if (systemStatus == 2) { display.println("PRE-HEAT"); }
      else if (heat_query() == 3) { display.println("HIGH"); } 
      else if (heat_query() == 2) { display.println("MED"); }
      else if (heat_query() == 1) { display.println("LOW"); } 
    }
  
  
    display.print("Fan Status: ");
    if (fan_query() == false) { display.println("IDLE"); } else { display.println("HIGH"); }
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

/* 
 *  Menu Structure
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

  /* Display the menu */
  f = 0;
  int menupage = (selectedPos / 7) * 5;
  for (byte i = menupage; i < (sizeof(menu) / sizeof(menu[0])); i++) {
    if (menu[i].menuNum == menuLocation) {
      
      // Display title of menu
      if (f == 0) {
        display.println(menu[i].name);
        display.setTextSize(1);
        display.setCursor(0, 17);
        display.setTextSize(1);
      }else{
        // Menu items
        //Set menu curser on selected item
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
        //Menu Curser
        if (selectedPos == f-1) {
          materialNum = i;
          menuLocation = menu[i].sMode; //Change menu
        }
    f++;
    }
  }
    selectedPos = 0;
    BtnStat[2] = LOW;
    if (menuLocation < 4) { refresh = true; }
  }
}

/*
 * User input monitor
 */
byte ButtonStatus(byte btn) {
  byte r = LOW;
  byte statbtn = digitalRead(BTN[btn]);
  //Serial.print("BTN: "); Serial.print(btn); Serial.print(" Stat: "); Serial.println(statbtn);

  // Debounce
  if (statbtn == LOW && previous[btn] == HIGH && (millis() - firstTime[btn]) > 50) {
    firstTime[btn] = millis();
    r = HIGH;
  }  
  previous[btn] = statbtn;
  return r;
}
