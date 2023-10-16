#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

/* Menu Structure */
struct menuDB {
  byte menuNum;
  char name[20];
  int sMode;
  double hours;
  int humitity;
  int tempature;
} menu[] = {
    /* Menu#  Name/Material,    Screen#   Hours   Humitity  Tempature */
    {  1,     "Main Menu",      0,        0,      0,        0}, // Menu Title
    {  1,     "Auto Drying",    2,        0,      0,        0},
    {  1,     "Manual Drying",  3,        0,      0,        0},
    {  2,     "Programs",       0,        0,      0,        0}, // Menu Title
    {  2,     "<Main Menu>",    1,        0,      0,        0},
    {  2,     "ABS",            4,        2,      50,       140},
    {  2,     "TEST",           4,        0.3,    20,       115},
    {  2,     "PLA",            4,        4,      13,       115},
    {  2,     "PETG",           4,        4,      30,       140},
    {  2,     "Nylon",          4,        12,     30,       140},
    {  2,     "TPU",            4,        4,      30,       120},
    {  2,     "PVA",            4,        4,      30,       150},
    {  3,     "Time",           0,        0,      0,        0}, // Menu Title
    {  3,     "<Main Menu>",    1,        0,      0,        0},
    {  3,     "1 Hour",         5,        1,      0,        90},
    {  3,     "2 Hour",         5,        2,      0,        90},
    {  3,     "4 Hour",         5,        4,      0,        90},
    {  3,     "8 Hour",         5,        8,      0,        90},
    {  3,     "12 Hour",        5,        12,     0,        90},
    {  3,     "18 Hour",        5,        18,     0,        90},
    {  3,     "20 Hour",        5,        20,     0,        90},
    {  3,     "24 Hour",        5,        24,     0,        90},
    {  3,     "48 Hour",        5,        48,     0,        90},
    {  6,     "Menu",           0,        0,      0,        0}, // Menu Title 0
    {  6,     "Back",           4,        0,      0,        0}, // 4
    {  6,     "Stop Process",   1,        0,      0,        0}, // 0
    {  7,     "Menu",           0,        0,      0,        0}, // Menu Title
    {  7,     "Back",           5,        0,      0,        0}, // 5
    {  7,     "Stop Process",   1,        0,      0,        0} //  0
};

/* Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) */
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

byte menuLocation = 1;// 1 =  Main Menu, 2 = programs menu, 3 = Manual Timer, 4 = In Program display (auto), 5 = In Program display (manual)
byte selectedPos = 0;
byte BTN[] = {31, 29, 27}; //0= down 1=up 2= enter 
byte BtnStat[] = {LOW, LOW, LOW};
byte RelayStat[] = {LOW, LOW, LOW, LOW}; //FAN1, Heat1, Heat2, Heat3
byte previous[] = {LOW, LOW, LOW};               // Last button status
unsigned long millis_held[] = {0, 0, 0};            // How long the button was held (milliseconds)
unsigned long firstTime[] = {0, 0, 0};              // Time Stamp when button is first pressed
byte menupage = 0;
bool refresh = false;
menuDB *mAddress;
byte materialNum = 0;
unsigned long timerProgram = 0; //Program timer in seconds
byte systemStatus = 0; //0 = idle , 1 = running , 2 = pre-heat , 3 = Enviorment test , 9 = emergency stop
bool displayFlip = false;
String error_lastStop;
float temp, tempc, hum;
byte heat_setting = 1;
byte heat_failed = 0;
bool heat_holding = false;
float env_hum, env_temp = 0;

/* Temp and humidity settings */
int temp_lastPreheatCheck = 0;

/* Timers */
unsigned long timer_display_flip = 0;
unsigned long timer_high_on = 0;
unsigned long timer_heat_debounce = 0;
unsigned long timer_env_Test = 0;


/* Macros for getting elapsed time */
#define numberOfSeconds(_time_) ((_time_ / 1000) % 60)  
#define numberOfMinutes(_time_) (((_time_ / 1000) / 60) % 60)
#define numberOfHours(_time_) (((_time_ / 1000) / 60) / 60)
#define numberOfDays(_time_) ((((_time_ / 1000) / 60) / 60) / 60)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  #if defined(debugMode) 
    Serial.begin(9600);
    Serial.println("Filament Dryer Starting . . .");
  #endif
  
  pinMode(RELAY_FAN_1,OUTPUT);
  pinMode(RELAY_HEAT_1,OUTPUT);
  pinMode(RELAY_HEAT_2,OUTPUT);
  pinMode(RELAY_HEAT_3,OUTPUT);

  if (settings_inverted_output == true) {
    digitalWrite(RELAY_FAN_1,HIGH); 
    digitalWrite(RELAY_HEAT_1,HIGH);
    digitalWrite(RELAY_HEAT_2,HIGH);
    digitalWrite(RELAY_HEAT_3,HIGH);      
   }

  for (int i = 0; i < 3; i++) {
    pinMode(BTN[i], INPUT_PULLUP);
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    #if defined(debugMode)
      Serial.println("SSD1306 allocation failed");
    #endif
    for(;;); // Don't proceed, loop forever
  }
  display.display(); // Splash screen
  display.clearDisplay();
  delay(2);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display_centerString("Filament",17);
  display_centerString("Dryer",34);
  display.setCursor(3, 57);
  display.setTextSize(1);
  display.print("Version: "); display.println(sw_version);
  display.display();
  delay(1500);

  dht.begin();
  
  // Initial fetch of tempature
  delay(300);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h1 = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t1 = dht.readTemperature();
  
  // Check if any reads failed and exit early
  if (isnan(h1) || isnan(t1)) {
    #if defined(debugMode)
      Serial.println("Failed to read from upper DHT sensor!");
    #endif
    error_lastStop = "Upper DHT sensor is dissconnected or has stopped responding!";
    systemStatus = 9; // E-stop
  }
  
  MenuSelection();
  timer_display_flip = millis();
}

void program_start() {
      #if defined(debugMode)
        Serial.println("program start");
      #endif
      env_hum = hum;
      env_temp = temp;
      systemStatus = 2; //set to preheat
      heat_setting = 1; //set heat to low
      long t = menu[materialNum].hours * 3600; //Convert hours to seconds
      timerProgram = t * 1000 + millis(); // Load time into memory
}
   
void loop() {
  delay(100);

  // Check if the time has expired
  timer_main();
  
   //Drying mode
   switch (systemStatus) {
    case 0: //Normal operations
      // Check status of button input
      for (int i = 0; i < 3; i++) {
        BtnStat[i] = ButtonStatus(i);
        if (BtnStat[i] == HIGH) {
          MenuSelection();
        }
      }
    case 1: //Pre Heat
    case 2: //Drying
      // Check status of button input
      for (int i = 0; i < 3; i++) {
        BtnStat[i] = ButtonStatus(i);
        if (BtnStat[i] == HIGH) {
          if (menuLocation == 4) { menuLocation = 6; } // Auto mode
          if (menuLocation == 5) { menuLocation = 7; } // Manual mode
          MenuSelection();
        }
      }

      //Force a screen refresh
      if (refresh == true) {
         MenuSelection();
         refresh = false;
      }
      
       if (menuLocation == 4 || menuLocation == 5) {
        // Load program if not done and start
        if (systemStatus == 0) {

          //Check to see if we have run enviormental check yet
          if (env_temp == 0) {
            systemStatus = 3; // Start enviorment test
            display_envTest();
            #if defined(debugMode)
             Serial.println("enviormental check started");
            #endif
          }else{
            program_start();
          }
          
         // Moved to enviromental check * systemStatus = 2; //set to preheat
         // Moved to enviromental check * heat_setting = 1; //set heat to low
         // Moved to enviromental check * long t = menu[materialNum].hours * 3600; //Convert hours to seconds
         // Moved to enviromental check * timerProgram = t * 1000 + millis(); // Load time into memory
        }else{
          display_dry(); // Update info
        }
     }
     break;
    case 3: //Enviorment test
      display_envTest(); // Update the display

      // Check if timer is done
      if (timer_envTest() == true) {
          program_start();
      }
      
      break;
    case 9: //Emergency
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.println("* E-STOP *");
      display.setTextSize(1);
      display.setCursor(0, 18);
      display.println(error_lastStop);
      display.display();
      display.startscrollright(0x00, 0x01);
      systemStatus = -1;
      break;
   }

   // Tempature control monitoring
   tempControl();

   //Flip flop display
   timer_flipflop();
}
