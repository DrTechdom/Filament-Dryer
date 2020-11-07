/**************************************************
 *                  Filament Dryer                *
 *              Created by. Ryan Vesely           *  
 *                                                *
 *               Created on  07/27/2020           *
 *          Last updated on  11/06/2020           */
float         sw_version = 0.58; 
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
 *
 *  8/22/20 - Version 0.37 - Ryan Vesely
 *    - Created low heat fail function to keep low heat on if its not keeping up
 *    - Created heater control function
 *    - Created fan control function
 *    - Created run away protection
 *    - Created a heater status query function
 *    - Moved all heating functions to a switch of systemStatus to clean things up
 *    - Created cool down function if unit resumes from reset and is greated than the cooling temp
 *    
 *  9/19/20 - Version 0.40 - Ryan Vesely
 *    - Changed heating plan to fluxuate the heat when calling and satisfied but not to shut the heat off  
 *      and hopfully find a sweet spot to keep the heat on all the time for heating elements type 0 - 3
 *    - Fixed medium heating for type 0
 *    - Finished todo "Create cool down function if unit is hot from a reset/power outage or finsihing cycle"
 *    - Finsihed todo "Create a relay function to keep track of the relay status"
 *    
 *  10/9/20 - Version 0.53 - Ryan Vesely
 *    BUG FIX: over saturation heat and satified heat not turning to or is too sensitive, added debounce timer
 *    BUG FIX: heating taking too long function calls too often and only waits for a time period, moved time to 45 seconds from 25 and added a tempature variable check
 *    BUG FIX: over run e-stop fails to check if high heat is on and to switch off heats
 *    BUG FIX: heat issue timer too sensitive, changed to 60 seconds from 10 seconds
 *    BUG FIX: heat_query function never outputed high heat causing an over run
 *    BUG FIX: LCD heater tempature status is incorrectly displayed previously using relay stats, now uses heat_query function
 *    BUG FIX: over staturation heat and satofied heat debounce too sensitive, change from 10 seconds to 45 seconds
 *    Moved heater issue check to taking too long check
 *    BUG FIX: estop fails to shut off inverted relays correctly
 *    Changed lcd fan speed to grab from fan_query function
 *    BUG FIX: Still... issues with rising temp vs time... changed heat taking too long to 2 min
 *    Changed the satisfied heat to -2 from +1 as the coil is still got extra heat on it and pushing over the heat setting
 *    
 *  11/6/20 - Version 0.58 - Ryan Vesely
 *    Changed database time from int to double to allow for less than an hour if needed... this came in handy for debugging
 *    Added ablity to disable/ignore the outside sensor if not if use
 *    BUG FIX: Heater does not bump up when it supose to call - Forgot to remove heat check
 *    BUG FIX: Heat setting changes to high when calling - forgot to add a debounce
 *    BUG FIX: When holding heat tempature after a rise and its satisfied the satisfied function triggers and drops the temp to low - added 
 *             a quick and dirty hold is true/false to the statement.
 */


 /*           TODO
  *  - Add heating option of (high, med, low) , (high, low) , (high)
  *  - Auto dry have humidity predict the time
  *  - Menu will not scroll to next page
  *  - While running create menu to stop process
  *  - Check if in menu and dont refresh status
  *  - Add a buzzer for alarms and when finished
  *  - Process bar behind time
  *  - Center time and titles using a function
  *  - Fan muti-speed?
  *  - Check if heat is still changing while heating after x seconds?
  *  - create heat lower and raise function instead of changing and pushing variable
  *  - create some type of reset after alarm has cleared or an ack button after alarm has cleared
  *  - automaticly change the extra satisifed heat if it goes under or over
  *  - cooling down screen after a drying run
  *  - add timer to check for temps
  *  - Add a menu when in a run to do things such as stopping the run
  *  - Is print(F("")) even neccessary??
  *  BUG: drier goes into estop when accessing the menu while drying
  */
