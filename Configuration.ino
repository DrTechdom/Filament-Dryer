/*

User configuration

*/


/*
   I/O Options
*/

// Button input pins
#define DHTPIN       3   /* Temp humidity sensor */

// Relay Output pins
#define RELAY_FAN_1  46
#define RELAY_HEAT_1 44
#define RELAY_HEAT_2 40
#define RELAY_HEAT_3 42


/*
    Sensor options

    Uncomment whatever type you're using!
*/
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)



/*
    Heating Options
*/

/* Heating operation type
 *  0 = Relay
 *  1 = PID with SSR Relay
 */
byte settings_heater_relay_type = 0;


/* RELAY ONLY - Heating element configuration
   0 = heater elements are split as a "Y", relay 1 is low, relay 2 is medium and relay1 and relay2 together is high
   1 = heater elements are seperate, relay 1 is low, relay 2 is high
   2 = heater elements are seperate, relay 1 is low, relay 2 is medium, relay 3 is high
   3 = heater element is a single element and tempature high only
*/
byte settings_heater_style = 0;






/*
   Fan configuration

   0 = High speed only
*/
byte settings_fan_speed = 0;


/*
   Timers
*/
int time_maxPreheat = 30; //Emergency shut off after 30 seconds of failer to preheat



/*
   Tempature Settings
*/
int temp_max = 135;   // Max heat unit to produce
int temp_cooled = 80;     // Tempature at which the fan shuts off when cooled and finished
byte tempDebounce = 5;    // Debouce/float tempature



/*
   Misc
*/

//Debug mode - Serial output
#define debugMode

// If your relay set takes inverted signals, fix that here
bool settings_inverted_output = true;
