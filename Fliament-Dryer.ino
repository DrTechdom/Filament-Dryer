/**************************************************
 *                  Filament Dryer                *
 *              Created by. Ryan Vesely           *  
 *                                                *
 *               Created on 7/27/20               *
 *          Last updated on  08/16/2020           */
String         sw_version = "0.32"; 
 /**************************************************/


/*              Change log
 *  
 *  7/27/20 - Version 0.0 - Ryan Vesely
 *   - Init of project
 *   - Implamented SSD1604 screen and 3 buttons using Arduino nano
 *   - Created menu system for navigation
 *   - Started having memory issues with LCD screen as memory got to 62% full, switched to mega2560
 *   
 *  7/29/20 - Version 0.3 - Ryan Vesely
 *   - Mega2560 works with no issues with SSD1604 LCD screen
 *   - Created proto type mainboard and used thingaverse.com 3D printed case for LCD
 *   - Started auto dry function and manual time dry function
 *   - Implamented sensors and basic functions
 *  
 *  7/30/20 - Version 0.5 - Ryan Vesely
 *   - Created drying functions
 *   - Created heater and fan control with rise, and low of heat
 *   - Added emergency monitoring features to heaters
 *   - Added low heat feature
 *   
 *  7/31/20 - Version 0.9 - Ryan Vesely
 *   - Created timer and macros. Added to run screen
 *   - Added screen dual flipping to add more data to screen
 *   - Started to create a proto type dryer
 *   
 *  8/15/20 - Version 0.20 - Ryan Vesely
 *   - Temperarly setup proto type dryer
 *   - BUG FIX: Fan shuts off when pre temp is reached. Less than symbol was backwards
 *   - Added medium/high heat function timer
 *   - Added heating element configuration and functions in heating
 *   - Added pre-heat to heater status on run screen
 *   - BUG FIX: heating status only shows low heat, had to change order of if else statments
 *   - Moved heater estop timer outside of pre-heat to monitor the heating elements when active at preheat and normal heat
 *   - Added inverted relay outputs configutation option
 *   - Added timer to turn on medium/high heat
 *   
 *   8/16/20 - Version 0.32 - Ryan Vesely
 *    - Added main timer function
 *    - BUG FIX: added 0 if timer displayed digit is less than 10
 *    - BUG FIX: after timer finishes the previous program restarts
 *      - Forgot to change menu back to main menu
 *      - Added heater shut off
 *    - Changed fan cooling to anytime
 *    - BUG FIX: Fan will not turn on after moving cooling to anytime.. forgot to specify not to monitor when pre-heating *face palm*
 *
 */


 /*           TODO
  *  - Add heating option of (high, med, low) , (high, low) , (high)
  *  - Auto dry have humidity predict the time
  *  - Menu will not scroll
  *  - While running create menu to stop process
  *  - Create cool down function if unit is hot from a reset/power outage or finsihing cycle
  *  - Check if in menu and dont refresh status
  *  - Add a buzzer for alarms and when finished
  *  - Process bar behind time
  *  - Center time and titles using a function
  */
