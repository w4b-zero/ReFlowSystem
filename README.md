# ReFlowSystem
ReFlowSystem with Display 

HOT PLATE CODE - base source from My Circuits 2023, SD comunication is based on code of David Bird 2018
    
 Upgrades by zeroTM - https://github.com/w4b-zero
 SD + Configfile CONFIG.TXT (if not on SD=auto create configfile)
 SD + Second Curve Temp File (for Debugmode)

 +SSD1306 Status and infos

 WebServer + Activ-Status
 WebServer + ask before delete File
 WebServer + editable Configfile
 WebServer + stop running ReFlow
 WebServer + System Restart

 +Configfile AccessPoint or Stationmode
 +Configfile APssid/APpassword STAssid/STApassword
 +Configfile Debugmode On/Off (to use second Curve Temp File!)
 +Configfile CURVE.TXT DCURVE.TXT

 Default Config:
 Wlanmode = AccessPoint
 APssid = reflowserver
 APpassword = 12345678

 USE MAX6675 Temp Sensor!
 
 Requiered libraries:
 
 ESP32WebServer - https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
 SSD1306 - Adafruit_SSD1306 + Adafruit_GFX Arduino-Library
 MAX6675 - GyverMAX6675 Arduino-Library

 ************************
 * Original Source Info *
 ************************
 Information from David Birds original code - not from My Circuits contribution (with my contribution/simplification to the code do whatever you wish, just mention us!):
  
 Part of the SD software, the ideas and concepts is Copyright (c) David Bird 2018. All rights to this software are reserved.
 
 Any redistribution or reproduction of any part or all of the contents in any form is prohibited other than the following:
 1. You may print or download to a local hard disk extracts for your personal and non-commercial use only.
 2. You may copy the content to individual third parties for their personal use, but only if you acknowledge the author David Bird as the source of the material.
 3. You may not, except with my express written permission, distribute or commercially exploit the content.
 4. You may not transmit it or store it in any other website or other form of electronic retrieval system for commercial purposes.

 The above copyright ('as annotated') notice and this permission notice shall be included in all copies or substantial portions of the Software and where the
 software use is visible to an end-user.
 
 THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT. FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY 
 OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 See more at http://www.dsbird.org.uk
 
