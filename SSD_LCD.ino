void display_centerString(String str, int y) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(str, 0, 0, &x1, &y1, &width, &height);

  // display on horizontal and vertical center
  //display.clearDisplay(); // clear display
  display.setCursor((SCREEN_WIDTH - width) / 2, y);
  display.println(str); // text to display
 // display.display();
}
/*
 * In opertaion screen
 */
void display_dry() {
  display.clearDisplay();
  display.setTextSize(2);
  
  if (menuLocation == 4) {
    display_centerString("*Auto Dry*",0);
  }else{
    display_centerString("*Timed Dry*",0);
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
      if (systemStatus == 2)      { display.println("PRE-HEAT"); }
      else if (heat_query() == 3) { display.println("HIGH");     } 
      else if (heat_query() == 2) { display.println("MED");      }
      else if (heat_query() == 1) { display.println("LOW");      } 
    }
  
  
    display.print("Fan Status: ");
    if (fan_query() == false) { display.println("IDLE"); } else { display.println("HIGH"); }
  }else{
    display.print("Temp: "); display.println(temp);

    display.print("Humidity %: "); 

    // Wait until we're out of pre-heat
    if (systemStatus == 2) {
      display.println("Calc..");
    }else{
      display.println(hum - env_hum);
    }
  }
  
  long t = timerProgram - millis();
  display.println("Time left: "); 
  display.setTextSize(2); 
  String time;
  if (numberOfHours(t) < 10) { time = "0"; }
  time += numberOfHours(t); time += ":";
  if (numberOfMinutes(t) < 10) { time += "0"; }
  time += numberOfMinutes(t); time += ":";
  if (numberOfSeconds(t) < 10) { time += "0"; }
  time += numberOfSeconds(t);
  display_centerString(time, 50);
  display.display();
}

/*
 * In opertaion - Enviorment test
 */
void display_envTest() {

  display.clearDisplay();
  display.setTextSize(2);
  display_centerString("*Env Test*",0);
  
  display.setTextSize(1);
  display_centerString("Testing the current",17); 
  display_centerString("Humidity and temp",25);
  display.println("");


  //Enviormental status display
  if (temp < temp_cooled && env_hum == 1) {
    //Running
    
    display.print("Temp: "); display.println(temp);
  
    display.print("Humidity %: "); display.println(hum);
      long t = timer_env_Test - millis();
      display.print("Time left: "); 
      if (numberOfSeconds(t) < 10) { display.print("0"); }
      display.print(numberOfSeconds(t)); display.println(" seconds");
  }else{
    //Cooling
    
    if (displayFlip == false) {
      display_centerString("*=*=*=*=*=*=*=*=*=*",40);
      display_centerString("Cooling",48);
      display_centerString("*=*=*=*=*=*=*=*=*=*",56);
    }else{
      display_centerString("=*=*=*=*=*=*=*=*=*=",40);
      String strTemp = "Temp: "; strTemp += temp; strTemp += "/"; strTemp += temp_cooled;
      display_centerString(strTemp,48);
      display_centerString("=*=*=*=*=*=*=*=*=*=",56);
    }
  }
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
        display_centerString(menu[i].name,0);
        display.setCursor(0, 17);
        display.setTextSize(1);
      }else{
        // Menu items
        //Set menu curser on selected item
        if (selectedPos == f-1) {
          display.print("> ");
        }else{
          display.print("  ");
        }
    
        // Menu item
        
          display.println(menu[i].name);
        }
    f++;
    }
  }
  
  display.display();

  // Menu Selection chosen
  if (BtnStat[2] == HIGH) {
  f = 0;
  for (byte i = 0; i < (sizeof(menu) / sizeof(menu[0])); i++) {
    if (menu[i].menuNum == menuLocation) {
        //Menu Curser
        if (selectedPos == f-1) {
          
          if (menuLocation < 5) { materialNum = i; }
          menuLocation = menu[i].sMode; //Change menu
          
          //Stop process when in process
          if (menuLocation == 1 && systemStatus == 1 || menuLocation == 1 && systemStatus == 2) {
            timerProgram = millis();
          }

          #if defined(debugMode)
            Serial.print("Menu location changed to "); Serial.println(menuLocation);
          #endif
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
 // Serial.print("BTN: "); Serial.print(btn); Serial.print(" Stat: "); Serial.println(statbtn);

  // Debounce
  if (statbtn == LOW && previous[btn] == HIGH && (millis() - firstTime[btn]) > 100) {
    firstTime[btn] = millis();
    r = HIGH;
  }  
  previous[btn] = statbtn;
  return r;
}
