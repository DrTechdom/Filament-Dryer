/**************************************************
 *                  Filament Dryer                *
 *              Created by. Ryan Vesely           *
 *                                                *
 *               Created on  07/27/2020           *
 *          Last updated on  10/15/2023           */
float         sw_version = 0.80;
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
 *
 *  01/29/22 - Version 0.60 - Ryan Vesely
 *    BUG FIX: Unit goes into estop when accessing the menu from in process. Created a statement only in process that the material type can be changed
 *    Add: Created the stop function when in process to cool the unit down and to stop the process
 *    Add: Dont refresh the menu if in menu when in process
 *    
 *  02/13/22 - Version 0.62 - Ryan Vesely
 *    Change: Moved all print(F( statements to print statements (we have the space to spare)
 *    Change: Removed outside temp/hum sensor
 *    
 *  04/13/22 - Version 0.65 - Ryan Vesely
 *    Change: Moved Estop to systemstatus 9 to make room for AI features
 *    Add: Created enviormental test - get the current room temp and humidity to guage the moisture in the material
 *    Change: Cleaned up debugging output
 *    Change: Replaced disable heat command with 0 program timer for abort command to trigger a reset
 *  
 *  04/14/22 - Version 0.68 - Ryan Vesely
 *    Change: Added calculated humidity to drying display
 *    Change: Added humidity wait function until temp was up to 100o to display actual humidity of material
 *    Change: Pushed temp change time out from 2 to 3 minutes
 *    Change: Moved display flip flop timer to timer functions
 *    BUG FIX: Forgot to add systemStatus == 1 to program finish counter, program never finishes when aborting program with new setup
 *    Change: Added cooling function to enviorment test
 *    Change: Added cooling status with flip flop to enviorment test cooling function
 *    Change: Added count down clock to enviorment test
 *    Change: Moved finished pre-heat temp from low to medium
 *    
 *  05/27/23 - Version 0.70 - Ryan Vesely
 *    Change: Removed Enviromental Check to test cold air so its irelevant
 *    Change: Moved enviromental check to end of preheat
 *  
 *  09/21/23 - Version 0.79 - Ryan Vesely
 *    Change: Changed mind on enviormental check and reinstated to get room temp
 *    Change: Added enviormental skip if a cycle has already been ran
 *    Change: Made enviormental time 30 seconds and cut fan at 15 seconds
 *    FINSIHED TODO: Created string centering function and started converting strings
 *    BUG FIX: When fan shut off after 15 seconds the cooling display would appear instead. 
 *             - Changed display if to watch for env_test = 1 instead of fan status
 *    
 * 10/15/23 - Version 0.80 - Ryan Vesely
 *    BUG FIX: When in pre-heat humidity shows high negitive, change to say processing or something
 *             - Changed statment waiting for tempature to wait until we're out of pre-heat
 */


 /*           TODO
 *   - BUG: When in drying mode if the enter button is pressed the screen only flickers
  *  - Finish relay functions
  *  - If heat comes on when not calling alarm as relay is not wired correctly
  *  - PID heat with SSRs
  *  - Add heating option of (high, med, low) , (high, low) , (high)
  *  - Auto dry have humidity predict the time
  *  - BUG: Menu will not scroll to next page
  *  - Add a buzzer for alarms and when finished
  *  - Fan muti-speed
  *  - cooling down screen after a drying run???
  *  - Inital setup program to find highest temp, temp change time, time to temp,...
  *  - Monitor negitive humidity, once platod concirder the material dry, move timer to 5 or 10 min remaining
  *  - If temp doesnt change for more than 2 mins +/- 5o preheating goto drying mode at highest heat
  */
