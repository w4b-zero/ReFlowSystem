/*  HOT PLATE CODE - base source from My Circuits 2023, SD comunication is based on code of David Bird 2018
    
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
 
*/
//int cTest = 1; // Fast Temp to debug
int cTest = 0; // Normal Temp
int debugsrc = 0;
//Web ESP32 + SD
#include <WiFi.h>              //Built-in
#include <ESP32WebServer.h>    //https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
#include <ESPmDNS.h>
#include <SD.h> 
#include <SPI.h>
//Hot Plate
#include <Wire.h>
#include <GyverMAX6675.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define servername "reflowserver" //Define the name to your server... 
String sName = "ReFlow System";
String refreshSite = "";
int startCommand = 0;
  String refreshStatus = "";

#include "CSS.h" //Includes headers of the web and de style file
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SSD1306_NO_SPLASH
/* VARIABLES SERVER + SD */
String pageReflowStatus = "";
#define SD_pin 5 

#define CLK_PIN   17  // Пин SCK
#define DATA_PIN  16  // Пин SO
#define CS_PIN    15  // Пин CS
String CONFIGfile = "/CONFIG.TXT";
String wlanmode = "ap";
String ssid     = "";
String password = "";
String apssid     = "reflowserver";
String appassword = "12345678";
  String newconfig = "no";

bool   SD_present = false; //Controls if the SD card is present or not
String fileName0 = "/CURVE.TXT"; //File name to save the data
String fileName1 = "/DCURVE.TXT"; //File name to save the data
String fDEBUG = "/DEBUG.TXT"; //File name to save the data
String fName = ""; //File name to save the data

String img_download = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAAsgAAALIBa5Ro4AAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAJ6SURBVFiF7ZfPi01xGMY/zwz5NdMYDWFW7NyykBKJ/GZI8geoyUZ+LCmrsaBslGIzdmPMbDQLNAtCikiJxhjJQpEIRUrDDDPzWHyP6cyZO/fec85VFp56u6dzvu/7PN9z3/f9vgfbJA04BDwHRgGntD5gfrG4xUwR4TgknQeOAJ+ATuAzlWMfUIhEbLT9taxHYudrol1cBmZUuouY/xVgEBgBHgB15XxqEnq2Rr+nbA+n2Hkcb4FWYDVwVdLMUouTApYAH233ZyQHwHYXIY82AT2SplcqQHmIEyLagWPALqBbUm2xddOqRRjhO9Aoqdb2qO0zkuqBNmBQ0n4nsj75BvLiMbAAOCBJALZPAGcJeXFukkciizuAD2mzP+Y/C3hGqKTXwMOY/ekpR0tVQS7Y/kHI/jZgABiK2T1CiW6J++TKAUkNhFp/FxMxCJycYn1f8l7eJLwALAQ2ZA2Q9y+YG1lmVLsK/gv4ewIU0CVpXYk1BUmXJM2rugDCObEK6JW0sgj5UuAmsD1FzMoF2B4jHNffgOuSlsXIm4FbQB2ww/aXqguIRLyJRIiw28VAQ3S9CNht+0mamKkbke0XklqA20BzdHsE2Gv7btp4marA9iNgDzBMOGBabfdmiZW5Fdu+I2kz0JiVPJeASMT9PP7wDzSism9AUg1h3F6fMnY/sM32UC4Btsck9QDvUwoYKEdeTMAYMGl6td1J+ErKi9qIYxzJHHgJNElaWwWyCYhadQF4NeFBYqhcDvwCbgBNWYfTIsNqPXCR0DNayn2cHgdOE5rMNaDivj4F5hCaVj3QbvvgBL6kgEjETuAwsAKYnVPAT+Ap0GG7O/nwN3kByObnvXUsAAAAAElFTkSuQmCC";
String img_upload = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAAsQAAALEBxi1JjQAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAANfSURBVFiFxddfiNVVEMDxz+j2x9ywwMVKIfsDUlGRVhhFFFFZD0VaQYFhZgURQeBLCulLhCzBRlRYSom0ZYkFvViZglBYmWAhQSEswRYWRkKKW+r08Dt393q96717F2vgcO9v5pzz+86cc2bOT2bKTDgHqzGIbLMdxcraHJ20yEwRMRFf4WK8hW+0J69gKpZl5ottjjleivf3F49uHAs9Bgps4plOItBVOGbjp8z8ogMf3sH76IuIQ5m5ZiyDawBn4Y8OXg4yszciurG6QPSPFaBT+QuTC8SKiJiMdQXio3YmmDBOgH4sjIgZBWIp1mBDRMz7LwDWYggDETEQEQO4G6djU0TMbDXBuJYgM/fh6oi4GTcgiqkbyzFTdVJODUAdyHZsrz1HxHkFoKWMdwnGLf87wKhLEBGzcOso5n2Z+eEpBcBiPDmK7beI2JaZf9YrI6ILl+G6ojqjJUHJ6X3Y0UkuV+38xdiJw46vlkN4F7e0qgUnSKmQZ7fgn4WXMQfr8GoBGcCVJRJ3YltEbMTCzDzcVgTwmvbuBHswp0WUHlLVmi/R0+w+0Ie5mTm3LgLTcM0onk9XpdxPcU9mDtWN68IVmbm7IaLT8TX2ZOYdLSPQwqP1xaMLGvQTVKU58USTcfOK7d529sCDmN/ENBEL8Ehm/tJgW40Hyv/XI+JAZm6oc3ZzRKzFSxHxcWYeO1ki6sKZTdocHMEHDcC9eBRvFtUbWB8RdzXM249LShv7EuA97GrQLcMxLDIS5kmqk3EQN9X1nVL6zs/MjlLxDHxb5/lEvIBnM/Ptun6Jx7AFK4aVmQewV3VMh/dAjgHgd5UXtQmPRkR3Zh5s7JiZRyJiAaY1mKZgPyPFaBCXR0SrxAO7jKTa2otOeHk9RGYO1p4j4kL0qI7kMMAnqovpynK5bAUwMyJ62oBtJtfjb+ymLEFmfh8RS9GLJRHxw0kmmFR+V+Dpsbw5Ik7Dc9hSS17DeSAz+yJiM25XZbqTyV48FRGbMnNrg+1HfK7yslGW41LcN6zpsAJOwGf4GbPbHPMw/sGS4/SdAJQJp2JH8fR5dI3SrwcbVR+yqxrtUTp1JGVNV6n2wn7Vzq6V46tUp+Va/IpF2eTTb1wAdSDn43HchotwLr5TJayd2JiZh5qN/RfT7gbtXn6VVAAAAABJRU5ErkJggg==";
String img_delete = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAACXBIWXMAAADsAAAA7AF5KHG9AAAAGXRFWHRTb2Z0d2FyZQB3d3cuaW5rc2NhcGUub3Jnm+48GgAAAd9JREFUWIW9lr9KA0EQxn8JCqKQCAH/Bgu1sPBPrSApFC3zAL6EorWFpVZ5CEF8FVuxsIyCTWJIMCLRXCx2DzZze8luLpeB5ciX73a+m5mdHfCzC6AG9GJWDTj32TDjwd0CXoAf4COGswzMaO6rjxAXK6O+sjKAU9GcsuumUxasAGwD0wLf088icByzX9HgtsV/v8AzUB8k6EoT43KcdHWAyzjnu0A3Refh6gI7odOsIeBA/E7LstpXREBjAs4jvsxjmAH2gdmUnTeBp5R9uJutEZ0BNyn5uwbuTcDWB1rAekoCmhKwVf2bBQtQR0habwAeWPD3QepCKxA9uydAyYIfAYcWvAScWvCCiwCAL/HiKipdgcDn9TKxQHOLApetGYhvPDJUOeAP+HYQ39bcnMCrPgJkHeT1s+UgIOTkBW6rLW8BkSq2WMhJJECGKwynjwCZgrFEIEkKxlIDE0uBJIfh9IlAohTIcI0jAtYuGCegTf98kDQCn6jm5iwA+kOWNALW8A8TYKYh6SmwnoBhAkzVSfvASBGYSApsA4ntpSVgDlgQnDWiH7GIuiFXDMxpDpC2Sf91WkfdhibWIHp1d1BVb2IbowgAeCQ6VPiuh1GdgxrR71BV7Ou4CtwyZMz/B6vd494SzA63AAAAAElFTkSuQmCC";
File file;
File dataCurve;
File okCurve;
File isDEBUG;

/* SERVER */
ESP32WebServer server(80);

//Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //IR Temperature sensor
GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> sens;

float targetTemp = 0; //Track the target temperature in each time - Target Temperature
float temp = 0.00; //Temperature from the sensor - Current Temperature
char tempstr[20]; //Temperature from the sensor - Current Temperature
int ledStatus = LOW; //Rele state
int i = 0; //Variable for tracking the rele state
int tAs = 0; //Variable for tracking the rele state
int down = 0; //Relative Time delay for the rele
int extraTemp = 1; //Time delay for the rele in seconds - Compensation Temperature
int start = 0; //Tracks the push of the botton
int currentMillis = 0; //Current milliseconds
unsigned long ssdPreviousMillis = 0; // Store the time of the last LED toggle
int digitalButton = 0; //Web interface start
int firstTime = 1; //Tracks the first start
int reflowon = 0;

// the number of the LED pin
const int ledPinOrange = 2;  //Rele
const int ledPinWhite =  13; //On
const int ledPinGreen =  14; //Hot plate cycle
const int ledPinRed =  12; //Plate hot

const int button = 26; //Push button

void setup() 
{
  Serial.begin(9600);

 
  pinMode(19, INPUT_PULLUP);
  
  pinMode(ledPinWhite, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinOrange, OUTPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinWhite, LOW);
  digitalWrite(ledPinOrange, LOW);
  digitalWrite(ledPinRed, LOW);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
//  delay(2000); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();
    display.drawLine(0, 0, 127, 0, SSD1306_WHITE); //top line
    display.drawLine(0, 63, 127, 63, SSD1306_WHITE); //bottom line
    display.drawLine(0, 0, 0, 63, SSD1306_WHITE); //left line
    display.drawLine(127, 0, 127, 63, SSD1306_WHITE); // right line


    display.display(); // Update screen with each newly-drawn line
display.cp437(true);
  display.setTextSize(1);             
//  display.setTextColor(SSD1306_WHITE);
//  display.setCursor(0,0);             // Start at top-left corner
//debugtest();
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("REFLOW",62 ,5 ); //Center on Display
drawCentreText("PLATE V1.0",62 ,25 ); //Center on Display
              //123456789012345678901 size1
              //1234567890            size2
              //1234567               size3
display.setCursor(0,0);             // Start at top-left corner
drawCentreText("by zero",62 ,45 ); //Center on Display  
display.setCursor(102,43);             // Start at top-left corner
  display.setTextSize(1);             
  display.println(F("TM"));
  display.display();
  Serial.println("*********************");
  Serial.println("* REFLOW PLATE V1.0 *");
  Serial.println("*     by zeroTM     *");
  Serial.println("*********************");
//  Serial.println("");
  delay(2000); // Pause for 5 seconds
  // Clear the buffer
  display.clearDisplay();


    display.drawLine(0, 0, 127, 0, SSD1306_WHITE); //top line
    display.drawLine(0, 63, 127, 63, SSD1306_WHITE); //bottom line
    display.drawLine(0, 0, 0, 63, SSD1306_WHITE); //left line
    display.drawLine(127, 0, 127, 63, SSD1306_WHITE); // right line

    display.drawLine(105, 0, 105, 63, SSD1306_WHITE); // v-trenner
    display.drawLine(0, 41, 105, 41, SSD1306_WHITE); // h-trenner

    display.display(); // Update screen with each newly-drawn line

  // textarea
//  display.setCursor(4,4);             // Start at top-left corner
//  display.println(F("System ready!"));
//  display.setCursor(4,17);             // Start at top-left corner
//  display.println(F("Press Button"));
//  display.setCursor(4,28);             // Start at top-left corner
//  display.println(F("to Start!"));
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextSize(1);             
//drawCentreText("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB",48 ,4 ); //Center on Display
drawCentreText("System start",50 ,17 ); //Center on Display
//drawCentreText("\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB",48 ,29 ); //Center on Display
  //display.display();
  Serial.println("- System start");


  // temp area
  display.setTextSize(2);             
  display.setCursor(0,0);             // Start at top-left corner
  display.setCursor(6,45);             
  display.print("220.00");
  display.print("\xF8");
  display.print("C");

  // reflow sign
//  display.setTextSize(3);             
//  display.setCursor(0,0);             // Start at top-left corner
//  display.setCursor(109,2);             
//  display.print("\xF7");
reflowOn();
  // relay sign
//  display.setTextSize(3);             
//  display.setCursor(0,0);             // Start at top-left corner
//  display.setCursor(109,17);             
//  display.print("\x07");
relaySignOn();
  // hot plate sign
//  display.setTextSize(3);             
//  display.setCursor(0,0);             // Start at top-left corner
//  display.setCursor(109,38);             
//  display.print("\x13");
plateHot();
  display.setTextSize(1);             

  
  display.display();
  delay(1000); // Pause for 5 seconds
clearSign1();   
clearSign2();   
clearSign3();   
clearTextArea();
  display.setTextColor(SSD1306_WHITE);
clearTempArea();
display.setTextSize(2);             
  display.setCursor(0,0);             // Start at top-left corner
  display.setCursor(6,45); 
  dtostrf (temp, 6, 2, tempstr);
  drawCentreText(tempstr,39 ,45 ); //Center on Display
  display.setTextSize(1);             
  display.display();

display.setCursor(0,0);
drawCentreText("check LEDs",50 ,17 ); //Center on Display
  display.display();
  Serial.print("- check LEDs ");

  checkLeds(); //Nice Led pattern

  //mlx.begin(); //IR Temperature sensor
  // wait for MAX chip to stabilize
  //delay(500);

  //SD
  if (!SD.begin(SD_pin))
  { 
    //Serial.println(F("Card failed or not present, no SD Card data logging possible..."));
  Serial.println("F SD-Card Error or not present, use Standart Temp Data!");
    SD_present = false; 
clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("SD-Card Err!",50 ,4 ); //Center on Display
drawCentreText("use Standart",50 ,17 ); //Center on Display
drawCentreText("Temp Data!",50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
delay(1000);
  } 
  else
  {
read_file_info();
    //Serial.println(F("Card initialised... file access enabled..."));
    SD_present = true; 
      //isDEBUG = SD.open(fDEBUG);
      if(debugsrc == 1){
        cTest = 1;
          Serial.println("!! DEBUG Mode ");
          Serial.println("!! "+fileName1+" wird genutzt!");
      }
      else {cTest = 0;}


      
          if (cTest == 0){
      okCurve = SD.open(fileName0);
      fName = fileName0;
      }
      if (cTest == 1){
      okCurve = SD.open(fileName1);
      fName = fileName1;
      }
      clearTextArea();
      display.setCursor(0,0);             // Start at top-left corner
      drawCentreText("SD-Card OK!",50 ,4 ); //Center on Display
            String useCfile = "use "+fName;
      if (okCurve){
          if (cTest == 0){
            drawCentreText(useCfile.c_str(),50 ,17 ); //Center on Display
            drawCentreText("from SD-Card",50 ,29 ); //Center on Display
            Serial.println("- SD-Card OK, "+useCfile+" from SD-Card!");
          }
          if (cTest == 1){
            drawCentreText(useCfile.c_str(),50 ,17 ); //Center on Display
            drawCentreText("from SD-Card",50 ,29 ); //Center on Display
            Serial.println("- SD-Card OK, "+useCfile+" from SD-Card!");
         }
      }
      else {
        Serial.println("- SD-Card OK, File not found! use default Data");
        drawCentreText("File not found!",50 ,17 ); //Center on Display
        drawCentreText("use default Data",50 ,29 ); //Center on Display
      }
      display.display(); // Update screen with each newly-drawn rectangle
      delay(2000);
    }

 if (wlanmode == "station"){ 
  //SERVER
//  WiFi.softAP("MyCircuits", "12345678"); //Network and password for the access point genereted by ESP32
  Serial.print("- Connecting to Router: ");
  Serial.println(ssid);
clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("Connecting",50 ,4 ); //Center on Display
drawCentreText("to",50 ,17 ); //Center on Display
drawCentreText("Router",50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
  WiFi.begin(ssid, password);
    Serial.print("- ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("*");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("- WiFi connected.");
  Serial.print("- IP address: ");
  Serial.println(WiFi.localIP());
  //const char* ipWifi = WiFi.localIP().toString().c_str() ;
clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("WiFi connected",50 ,4 ); //Center on Display
drawCentreText("IP address",50 ,17 ); //Center on Display
drawCentreText(WiFi.localIP().toString().c_str(),50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
delay(1000);
 }


else {
  //SERVER
  Serial.print("- Starting Accesspoint: ");
  Serial.println(apssid);
  Serial.print("Password: ");
  Serial.println(appassword);
clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("Starting",50 ,4 ); //Center on Display
drawCentreText("the",50 ,17 ); //Center on Display
drawCentreText("Accesspoint",50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
  WiFi.softAP(apssid, appassword); //Network and password for the access point genereted by ESP32
//  WiFi.begin(ssid, password);
//    Serial.print("- ");
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print("*");
//  }
  // Print local IP address and start web server
//  Serial.println("");
  Serial.println("- AP Started");
  Serial.print("- IP address: ");
  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);
  //const char* ipWifi = WiFi.localIP().toString().c_str() ;
clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("AP started",50 ,4 ); //Center on Display
drawCentreText("IP address",50 ,17 ); //Center on Display
drawCentreText(myIP.toString().c_str(),50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
delay(1000);
 }

 
  if (!MDNS.begin(servername)) 
  {          
    Serial.println(F("Error setting up MDNS responder!")); 
    ESP.restart(); 
  } 

  /*********  Server Commands  **********/
//  server.on("/",         SD_dir);
  server.on("/",         startCycle);
  server.on("/files",         SD_dir);
  server.on("/fileDel",         fileDel);
  server.on("/upload",   File_Upload);
  server.on("/fupload",  HTTP_POST,[](){ server.send(200);}, handleFileUpload);
  server.on("/config",  editconfig);
  server.on("/wconfig",  writeconfig);
  server.on("/rfcommand", ReFlowCommand);
  server.on("/newstart", newstart);
  server.on("/gonewstart", gonewstart);


  
  server.begin();
  
  Serial.println("- HTTP server started");
  Serial.print("- Servername: ");
  Serial.println(servername);
clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("HTTP server",50 ,4 ); //Center on Display
drawCentreText("started",50 ,17 ); //Center on Display
//drawCentreText(WiFi.localIP(),48 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
delay(1000);

clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("System ready!",50 ,4 ); //Center on Display
drawCentreText("Press Button",50 ,17 ); //Center on Display
drawCentreText("to Start!",50 ,29 ); //Center on Display
  Serial.println("************************");
  Serial.println("- System ready!");
  Serial.println("- Press Button to Start!");
  Serial.println("************************");

///clearTempArea();
//clearSign1();   
//clearSign2();   
  // relay sign
//  display.setTextSize(3);             
//  display.setCursor(109,17);             
//  display.print("\x09");
//  display.setTextSize(1);             
//  display.display();
//clearSign3();   
}

void loop() 
{
while (start == 0) 
  {
    server.handleClient(); //Listen for client connections
//    temp = sens.getTemp(); //Obtain IR temperature
      reflowon = 0;
    
    if (digitalRead(button) == 0 || digitalButton == 1) 
    {
      start = 1;
    if (digitalRead(button) == 0){ 
      Serial.println("Button pressed!");
    }
    else {
      Serial.println("Webinterface Start!");
      
    }
      Serial.println("Reflowing started!");
      Serial.println("!!! WARNING !!! - PLATE IS HOT!!!");
clearTextArea();
drawCentreText("Reflowing...",50 ,4 ); //Center on Display
drawCentreText("!!WARNING!!",50 ,17 ); //Center on Display
drawCentreText("!PLATE VERY HOT!",50 ,29 ); //Center on Display
 //   display.display(); // Update screen with each newly-drawn rectangle
reflowOn();
reflowon = 1;
}
    currentMillis = millis();
//    ssdCurrentMillis = millis();
    firstTime = 1;
    temp = sens.getTemp(); //Obtain IR temperature
    //HOT - Red
ssdTemp();
     
    if (temp>39.99)
    {
      digitalWrite(ledPinRed, HIGH);
    Serial.print("! Plate is HOT!!! Temp:");
    Serial.println(temp);
    plateHot();
    //delay(1000); //Step every 1 second
    }
    else if (temp > 38)
    {
    if (reflowon == 0){  
      digitalWrite(ledPinRed, LOW);
    Serial.print("- READY! PLATE IS COLD!");
    Serial.println(temp);
    plateCold();
clearTextArea();
drawCentreText("READY!",50 ,4 ); //Center on Display
drawCentreText("------",50 ,17 ); //Center on Display
drawCentreText("PLATE IS COLD!",50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
    delay(5000); //Step every 1 second
    tAs=1;
    }
    }
    else if (temp<37)
    {
  digitalWrite(ledPinRed, LOW);
        if (reflowon == 0){  
      if(tAs == 1){
clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("System ready!",50 ,4 ); //Center on Display
drawCentreText("Press Button",50 ,17 ); //Center on Display
drawCentreText("to Start!",50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
  Serial.println("************************");
  Serial.println("- System ready!");
  Serial.println("- Press Button to Start!");
  Serial.println("************************");
    tAs=0;
      }
        }
    }
    delay(250); //Step every 1/4 second
  }
  
  while (start==1)
  {
    server.handleClient(); //Listen for client connections
    if (firstTime==1) 
    {
      firstTime = 0;
      if (cTest == 0){
      dataCurve = SD.open(fileName0);
      }
      if (cTest == 1){
      dataCurve = SD.open(fileName1);
      }

    }
    
//    temp = mlx.readObjectTempC(); //Obtain IR temperature
    temp = sens.getTemp(); //Obtain IR temperature

    if (!dataCurve)
    {
      checkTemp (); //Gets current cycle temperature
    }
    else
    {
      String row = dataCurve.readStringUntil('\n');
      reflowon = 0;
      if (row.indexOf("e") >= 0) //until "end"
      {
        targetTemp = 0;
        start = 0;
        digitalButton = 0;
        digitalWrite(ledPinGreen, LOW);
        reflowon = 0;
       // Serial.println("end detected");
  Serial.println("*******************************");
  Serial.println("- Reflow finish!");
  Serial.println("! WAIT UNTIL THE PLATE IS COLD!");
  Serial.println("*******************************");
clearTextArea();      
drawCentreText("Reflow finish!",50 ,4 ); //Center on Display
drawCentreText("WAIT UNTIL THE",50 ,17 ); //Center on Display
drawCentreText("PLATE IS COLD!",50 ,29 ); //Center on Display
        reflowOff();
      }
      else
      {
        targetTemp = row.toFloat();
        digitalWrite(ledPinGreen, HIGH);
        reflowon = 1;
      }
    }
    
    downCheck(); //Definition for the relay delay
    //HOT - Red
    if (temp>39.99)
    {
      digitalWrite(ledPinRed, HIGH);
    if (reflowon == 0){  
    Serial.print("! Plate is HOT!!! Temp:");
    Serial.println(temp);
    }
    if (reflowon == 1){  
    Serial.print("! Plate is HOT!!! ");
    }
    plateHot();
    }
    else if (temp > 38)
    {
    if (reflowon == 0){  
    digitalWrite(ledPinRed, LOW);
    Serial.print("- READY! PLATE IS COLD!");
    Serial.println(temp);
    plateCold();
clearTextArea();
drawCentreText("READY!",50 ,4 ); //Center on Display
drawCentreText("------",50 ,17 ); //Center on Display
drawCentreText("PLATE IS COLD!",50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
    delay(5000); //Step every 1 second
    tAs=1;
    reflowon = 0;
    }
    if (reflowon == 1){  
    Serial.print("- Plate is COLD. ");
    }

    }
    else if (temp<38)
    {
    if (reflowon == 1){  
    Serial.print("- Plate is COLD. ");
    }
}
    else if (temp<37)
    {
      digitalWrite(ledPinRed, LOW);
    if (reflowon == 0){  
      if(tAs == 1){
clearTextArea();
  display.setCursor(0,0);             // Start at top-left corner
drawCentreText("System ready!",50 ,4 ); //Center on Display
drawCentreText("Press Button",50 ,17 ); //Center on Display
drawCentreText("to Start!",50 ,29 ); //Center on Display
    display.display(); // Update screen with each newly-drawn rectangle
  Serial.println("************************");
  Serial.println("- System ready!");
  Serial.println("- Press Button to Start!");
  Serial.println("************************");
    tAs=0;
      }
    }
    if (reflowon == 1){  
    Serial.print("- Plate is COLD. ");
    }
}
  
    Serial.print("TargetTemp:"); 
    Serial.print(targetTemp); 
    Serial.print(", Temp:");
    Serial.print(temp);
  // temp area
clearTempArea();
display.setTextSize(2);             
  display.setCursor(0,0);             // Start at top-left corner
  display.setCursor(6,45); 
  dtostrf (temp, 6, 2, tempstr);
  drawCentreText(tempstr,39 ,45 ); //Center on Display
  display.setTextSize(1);             
  display.display();
    
    if(temp<(targetTemp-extraTemp)) //Rele ON 
    {
      ledStatus = HIGH;
      i = 10; //Rele State
      relaySignOn();
    }
    else //Rele OFF
    {
      ledStatus = LOW;
      i = 0; //Rele State
      clearSign2();
    }
  
  
    digitalWrite(ledPinOrange, ledStatus); //Rele control
    
    //Serial.println("???????????????????????????????????");
    Serial.print(", Relay:O");
    if(i == 10) //Rele ON 
    {
    Serial.print("n");
    } else {
    Serial.print("ff");
    }
    Serial.print(", extraTemp:");
    Serial.println(extraTemp*10); //Compensation Temperature * 10
    //Serial.println("???????????????????????????????????");
    
    delay(250); //Step every 1/4 second
  }
    //delay(250); //Step every 1/4 second
}

void ssdTemp() //default reflow curve - If there is no CURVE.TXT still works...
{
unsigned long ssdCurrentMillis = millis(); // Get the current time
  if (ssdCurrentMillis - ssdPreviousMillis >= 1000)
  {
    temp = sens.getTemp(); //Obtain IR temperature
      // temp area
clearTempArea();
display.setTextSize(2);             
  display.setCursor(0,0);             // Start at top-left corner
  display.setCursor(6,45); 
  dtostrf (temp, 6, 2, tempstr);
             
drawCentreText(tempstr,39 ,45 ); //Center on Display
  display.setTextSize(1);             
  display.display();
ssdPreviousMillis = ssdCurrentMillis;
  }
}


void checkLeds()
{

  digitalWrite(ledPinWhite, HIGH);
  Serial.print("*");
  delay(300);
  digitalWrite(ledPinGreen, HIGH);
  reflowOn();
  display.display();
  Serial.print("*");
  delay(300);
  digitalWrite(ledPinOrange, HIGH);
  relaySignOn();
  display.display();
  Serial.print("*");
  delay(300);
  digitalWrite(ledPinRed, HIGH);
  plateHot();
  display.display();
  Serial.print("*");
  delay(300);
  digitalWrite(ledPinRed, LOW);
  clearSign1();   
  display.display();
  Serial.print("*");
  delay(300);
  digitalWrite(ledPinOrange, LOW);
  clearSign2();   
  display.display();
  Serial.print("*");
  delay(300);
  digitalWrite(ledPinGreen, LOW);  
  clearSign3();   
  display.display();
  Serial.print("*");
  delay(1000);
  display.setTextSize(1);             
  display.display();
  Serial.println(" OK");
}

void downCheck() //Rele delay calculation - little twisted but seems to make the job
{
    temp = sens.getTemp(); //Obtain IR temperature
  if (temp<targetTemp && ledStatus == HIGH ) //needs to go up
  {
    down = down - 1; //need to continue
  }
  else if (temp<targetTemp && ledStatus == LOW ) 
  {
    down = down + 1; //need to continue
  }
  else if (temp>targetTemp && ledStatus == LOW ) 
  {
    down = down - 1; //need to continue
  }
  else //(temp>targetTemp && ds == 255 ) 
  {
    down = down + 5; //need to continue
  }

  if (down<0)
  {
    if (down < -200)
    {
      down = -200;
    } 
    extraTemp = -down/20; //converts to seconds
  }
  else
  {
    extraTemp = 1;
  }
}

void checkTemp () //default reflow curve - If there is no CURVE.TXT still works...
{
  if (millis()<=75000+currentMillis)
  {
    targetTemp = 2.00*(millis()-currentMillis)/1000.00;
    digitalWrite(ledPinGreen, HIGH);

  }
  else if (millis()<=155000+currentMillis)
  {
    targetTemp = 150;
  }
  else if (millis()<=190000+currentMillis)
  {
    targetTemp = 150.00+2.00*(millis()-155000-currentMillis)/1000.00;
  }
  else if (millis()<=230000+currentMillis)
  {
    targetTemp = 220;
  }
  else
  {
    targetTemp = 0;
    digitalWrite(ledPinGreen, LOW);
    start = 0;
  Serial.println("*******************************");
  Serial.println("- Reflow finish!");
  Serial.println("! WAIT UNTIL THE PLATE IS COLD!");
  Serial.println("*******************************");
clearTextArea();      
drawCentreText("Reflow finish!",50 ,4 ); //Center on Display
drawCentreText("WAIT UNTIL THE",50 ,17 ); //Center on Display
drawCentreText("PLATE IS COLD!",50 ,29 ); //Center on Display
   reflowOff();
  }
}

/*********  FUNCTIONS  **********/


//Download a file from the SD, it is called in void SD_dir()
void SD_file_download(String filename)
{
  if (SD_present) 
  { 
    File download = SD.open("/"+filename);
    if (download) 
    {
      server.sendHeader("Content-Type", "text/text");
      server.sendHeader("Content-Disposition", "attachment; filename="+filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } else ReportFileNotPresent("download"); 
  } else ReportSDNotPresent();
}
//Upload a file to the SD
void File_Upload()
{
  sName = "File Upload";  
  refreshSite = "";  
  refreshStatus = "";
  append_page_header();

          if(start==0){
   pageReflowStatus = "<span style='color:green'><b>Off</b></span>";  
    }
    
    if(start==1){
   pageReflowStatus = "<span style='color:red'><b>On</b></span>";  
     }
  webpage += F("<div class='inhalt'>");
    webpage  += refreshStatus;
  webpage += F("<p class='status'>");
  webpage += pageReflowStatus;
  webpage += F("</p ><br>");

  webpage += F("<h3>Select File to Upload</h3>"); 
  webpage += F("<FORM action='/fupload' method='post' enctype='multipart/form-data'>");
  webpage += F("<input class='buttons' style='width:25%' type='file' name='fupload' id = 'fupload' value=''>");
  webpage += F("<button class='buttons' style='width:10%' type='submit'><img src='");
  webpage += img_upload;
  webpage += F("' width='32' height='32'></button><br><br>");
        webpage += F("<a href='/files'>Back to File System</a><br><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");
  webpage += F("</div>");
  append_page_footer();
  server.send(200, "text/html",webpage);
}

//Handles the file upload a file to the SD
File UploadFile;
//Upload a new file to the Filing system
void handleFileUpload()
{ 
  HTTPUpload& uploadfile = server.upload(); //See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            //For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if(uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SD.remove(filename);                         //Remove a previous version, otherwise data is appended the file again
    UploadFile = SD.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if(UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  } 
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if(UploadFile)          //If the file was successfully created
    {                                    
      UploadFile.close();   //Close the file again
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);
      webpage = "";
  sName = "File Upload - successfull!";  
    webpage  += refreshStatus;
  refreshSite = "";  
  refreshStatus = "";
      append_page_header();

          if(start==0){
   pageReflowStatus = "<span style='color:green'><b>Off</b></span>";  
    }
    
    if(start==1){
   pageReflowStatus = "<span style='color:red'><b>On</b></span>";  
     }
  webpage += F("<div class='inhalt'>");
  webpage += F("<p class='status'>");
  webpage += pageReflowStatus;
  webpage += F("</p ><br>");

      webpage += F("<h3>File was successfully uploaded</h3>"); 
      webpage += F("<p class='status1'>Uploaded File Name: "); webpage += uploadfile.filename+"</p >";
      webpage += F("<p class='status1'>File Size: "); webpage += file_size(uploadfile.totalSize) + "</p ><br><br>"; 
        webpage += F("<a href='/files'>Back to File System</a><br><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");
          webpage += F("</div>");

      append_page_footer();
      server.send(200,"text/html",webpage);
    } 
    else
    {
      ReportCouldNotCreateFile("upload");
    }
  }
}

void editconfig()
{
sName = "System Config";  
  refreshSite = "";  
  refreshStatus = "";
  append_page_header();

          if(start==0){
   pageReflowStatus = "<span style='color:green'><b>Off</b></span>";  
    }
    
    if(start==1){
   pageReflowStatus = "<span style='color:red'><b>On</b></span>";  
     }
  webpage += F("<div class='inhalt'>");
    webpage  += refreshStatus;
  webpage += F("<p class='status'>");
  webpage += pageReflowStatus;
  webpage += F("</p >");
    String cfile = fileName0.substring(1);
    String dcfile = fileName1.substring(1);



  webpage += F("<h3>ReFlowServer System Config</h3>"); 
  webpage += F("<FORM action='/wconfig' method='post' enctype='multipart/form-data'>");
webpage += "<table align='center'><tr><th>Name</th><th>Value</th><th>Default</th></tr>";
webpage += "<tr><td>Wlanmode: </td><td><select name='wlanmode' id='wlanmode'>";
webpage += "  <option value='station'";
  if (wlanmode == "station"){webpage += "selected ";} 
webpage += " >Station</option>";
webpage += "  <option value='ap'";
  if (wlanmode == "ap"){webpage += "selected ";} 
webpage += " >AcessPoint</option>";
webpage += "</select></td><td>AcessPoint</td></tr>";
  webpage += F("<tr><td>AP ssid: </td><td><input class='text' type='text' name='apssid' id = 'apssid' value='");
  webpage += apssid;
  webpage += F("'></td><td>reflowserver</td></tr>");

  webpage += F("<tr><td>AP pass: </td><td><input class='text' type='text' name='appassword' id = 'appassword' value='");
  webpage += appassword;
  webpage += F("'></td><td>12345678</td></tr>");

  webpage += F("<tr><td>Station ssid: </td><td><input class='text' type='text' name='ssid' id = 'ssid' value='");
  webpage += ssid;
  webpage += F("'></td><td></td></tr>");

  webpage += F("<tr><td>Station Pass: </td><td><input class='text' type='text' name='password' id = 'password' value='");
  webpage += password;
  webpage += F("'></td><td></td></tr>");

webpage += "<tr><td>Debugmode: </td><td><select name='debugmode' id='debugmode'>";
webpage += "  <option value='0'";
  if (String(debugsrc) == "0"){webpage += "selected ";} 
webpage += " >Off</option>";
webpage += "  <option value='1'";
  if (String(debugsrc) == "1"){webpage += "selected ";} 
webpage += " >On</option>";
webpage += "</select></td><td>Off</td></tr>";

  webpage += F("<tr><td>Temp Data File: </td><td><input class='text' type='text' name='curvefile' id = 'curvefile' value='");
  webpage += cfile;
  webpage += F("'></td><td>CURVE.TXT</td></tr>");

  webpage += F("<tr><td>Debug Temp File: </td><td><input class='text' type='text' name='dcurvefile' id = 'dcurvefile' value='");
  webpage += dcfile;
  webpage += F("'></td><td>DCURVE.TXT</td></tr></table>");

  webpage += F("<a href='/'>Back to Startsite</a> | <button class='buttons' type='submit'>Save Config</button><br><br>");
  webpage += F("</div>");
  append_page_footer();
  server.send(200, "text/html",webpage);
}
void writeconfig()
{ 
   sName = "Save Config";  
   webpage  += refreshStatus;
  refreshSite = "";  
  refreshStatus = "";
      append_page_header();

          if(start==0){
   pageReflowStatus = "<span style='color:green'><b>Off</b></span>";  
    }
    
    if(start==1){
   pageReflowStatus = "<span style='color:red'><b>On</b></span>";  
     }
  webpage += F("<div class='inhalt'>");
  webpage += F("<p class='status'>");
  webpage += pageReflowStatus;
  webpage += F("</p ><br>");
  if (server.args() > 0 ) //Arguments were received, ignored if there are not arguments
  { 
webpage += F("Number of args received:");
webpage += server.args();
webpage += F("<br>");
for (int i = 0; i < server.args(); i++) {

webpage += F("Arg nr");
webpage += i;
webpage += F(" > ");
webpage += server.argName(i);
webpage += F(": ");
webpage += server.arg(i);
webpage += F("<br>");

} 
  }
    String filename = CONFIGfile;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("write config: "); Serial.println(filename);
    SD.remove(filename);
    File ConfigFile;
    //Remove a previous version, otherwise data is appended the file again
    ConfigFile = SD.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    filename = String();

    if(ConfigFile) 
    {
    String configTXT0 = "// *** Configfile for ReFlowServer ***";  
    String configTXT1 = "wlanmode = "+server.arg(0)+";       // ap/station";  
    String configTXT2 = "apwifissid = "+server.arg(1)+";    // ssid for Accesspoint";  
    String configTXT3 = "apwifipass = "+server.arg(2)+";      // password for Accesspoint";  
    String configTXT4 = "wifissid = "+server.arg(3)+";  // ssid of your Router";  
    String configTXT5 = "wifipass = "+server.arg(4)+";    // passwort of your Router";  
    String configTXT6 = "debugmode = "+server.arg(5)+";        // Debugmode 0/1 = 0 for curvefile or 1 for dcurvefile  ";  
    String cfile = fileName0.substring(1);
    String dcfile = fileName1.substring(1);

    String configTXT7 = "curvefile = "+server.arg(6)+";      // ReFlow Temp File";  
    String configTXT8 = "dcurvefile = "+server.arg(7)+";    // Debugmode ReFlow Temp File";  
    ConfigFile.println(configTXT0); // write a line to esp32.txt
    ConfigFile.println(configTXT1); // write another line to esp32.txt
    ConfigFile.println(configTXT2); // write another line to esp32.txt
    ConfigFile.println(configTXT3); // write another line to esp32.txt
    ConfigFile.println(configTXT4); // write another line to esp32.txt
    ConfigFile.println(configTXT5); // write another line to esp32.txt
    ConfigFile.println(configTXT6); // write another line to esp32.txt
    ConfigFile.println(configTXT7); // write another line to esp32.txt
    ConfigFile.println(configTXT8); // write another line to esp32.txt
    ConfigFile.close();
    Serial.println(F("SD Card: "));
    Serial.println(filename);
    Serial.println(" was successfully written");
      webpage += F("<h3>Configfile was successfully written</h3>"); 
  } else {
    Serial.print(F("SD Card: Issue encountered while attempting to open the file esp32.txt"));
      webpage += F("<h3>SD Card: Issue encountered while attempting to open the file esp32.txt</h3>"); 
  }
        webpage += F("<a href='/files'>Back to File System</a><br><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");
        webpage += F("<a href='/newstart'>RESTART SYSTEM</a><br><br>");
          webpage += F("</div>");

      append_page_footer();
      server.send(200,"text/html",webpage);
read_file_info();
    }

//Initial page of the server web, list directory and give you the chance of deleting and uploading
void SD_dir()
{
  if (SD_present) 
  {
    //Action acording to post, dowload or delete, by MC 2022
    if (server.args() > 0 ) //Arguments were received, ignored if there are not arguments
    { 
      Serial.println(server.arg(0));
  
      String Order = server.arg(0);
      Serial.println(Order);
      
      if (Order.indexOf("download_")>=0)
      {
        Order.remove(0,9);
        SD_file_download(Order);
        Serial.println(Order);
      }
  
      if ((server.arg(0)).indexOf("delete_")>=0)
      {
        Order.remove(0,7);
        SD_file_delete(Order);
        Serial.println(Order);
      }
    }

    File root = SD.open("/");
    if (root) {
      root.rewindDirectory();
  sName = "File System";  
  refreshSite = "";  
  refreshStatus = "";
      SendHTML_Header();    
      webpage += F("<table align='center'>");
      webpage += F("<tr><th>Name/Type</th><th style='width:20%'>Type File/Dir</th><th>File Size</th></tr>");
      printDirectory("/",0);
      webpage += F("</table><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");

      SendHTML_Content();
      root.close();
    }
    else 
    {
  sName = "File System - No Files!";  
  refreshSite = "";  
  refreshStatus = "";
      SendHTML_Header();
      webpage += F("<h3>No Files Found</h3>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");
    }
  webpage += F("</div>");
    append_page_footer();
    SendHTML_Content();
    SendHTML_Stop();   //Stop is needed because no content length was sent
  } else ReportSDNotPresent();
}

//Prints the directory, it is called in void SD_dir() 
void printDirectory(const char * dirname, uint8_t levels)
{
  
  File root = SD.open(dirname);

  if(!root){
    return;
  }
  if(!root.isDirectory()){
    return;
  }
  File file = root.openNextFile();

  int i = 0;
  while(file){
    if (webpage.length() > 1000) {
      SendHTML_Content();
    }
    if(file.isDirectory()){
      webpage += "<tr><td>"+String(file.isDirectory()?"Dir":"File")+"</td><td>"+String(file.name())+"</td><td></td></tr>";
      printDirectory(file.name(), levels-1);
    }
    else
    {
      String fdName = String(file.name());
      webpage += "<tr><td>"+fdName+"</td>";
      String type = String(file.isDirectory()?"Dir":"File");
    String cfile = fileName0.substring(1);
    String dcfile = fileName1.substring(1);
      if (type == "File"){
        if (fdName =="CONFIG.TXT"){
          type = "<font style='color:red'>Systemfile</font>";
        } 
        else if (fdName == cfile){
          type = "<font style='color:red'>Systemfile</font>";
        } 
        else if (fdName == dcfile){
          type = "<font style='color:red'>Systemfile</font>";
        } 
      }
      webpage += "<td>"+type+"</td>";
      int bytes = file.size();
      String fsize = "";
      if (bytes < 1024)                     fsize = String(bytes)+" B";
      else if(bytes < (1024 * 1024))        fsize = String(bytes/1024.0,3)+" KB";
      else if(bytes < (1024 * 1024 * 1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
      else                                  fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
      webpage += "<td>"+fsize+"</td>";
      webpage += "<td class='test'>";
      webpage += F("<FORM action='/files' method='post'>"); 
      webpage += F("<button type='submit' name='download'"); 
      webpage += F("' value='"); webpage +="download_"+String(file.name()); webpage +=F("'><img src='");
      webpage += img_download;
      webpage += F("' width='32' height='32'></button>");
      webpage += "</FORM></td>";
      webpage += "<td class='test'>";
      webpage += F("<FORM action='/fileDel' method='post'>"); 
      webpage += F("<button type='submit' name='delete'"); 
      webpage += F("' value='"); webpage +="delete_"+String(file.name()); webpage +=F("'><img src='");
      webpage += img_delete;
      webpage += F("' width='32' height='32'></button></FORM>");
      webpage += "</td>";
      webpage += "</tr>";

    }
    file = root.openNextFile();
    i++;
  }
  file.close();

 
}

//Delete a file from the SD, it is called in void SD_dir()
void SD_file_delete(String filename) 
{ 
  if (SD_present) {
      sName = "File System - Delete File";  
  refreshSite = "";  
  refreshStatus = "";
     SendHTML_Header();
   File dataFile = SD.open("/"+filename, FILE_READ); //Now read data from SD Card 
    if (dataFile)
    {
      if (SD.remove("/"+filename)) {
        Serial.println(F("File deleted successfully"));
        webpage += "<h3>File '"+filename+"' has been erased</h3>"; 
        webpage += F("<a href='/files'>Back to File System</a><br><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");
      }
      else
      { 
        webpage += F("<h3>File was not deleted - error</h3>");
        webpage += F("<a href='/files'>Back to File System</a><br><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");
      }
    } else ReportFileNotPresent("delete");
  webpage += F("</div>");
    append_page_footer(); 
    SendHTML_Content();
    SendHTML_Stop();
  } else ReportSDNotPresent();
} 

//SendHTML_Header
void SendHTML_Header()
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate"); 
  server.sendHeader("Pragma", "no-cache"); 
  server.sendHeader("Expires", "-1"); 
  server.setContentLength(CONTENT_LENGTH_UNKNOWN); 
  server.send(200, "text/html", ""); //Empty content inhibits Content-length header so we have to close the socket ourselves. 
  append_page_header();

            if(start==0){
   pageReflowStatus = "<span style='color:green'><b>Off</b></span>";  
    }
    
    if(start==1){
   pageReflowStatus = "<span style='color:red'><b>On</b></span>";  
     }
  webpage += F("<div class='inhalt'>");
    webpage  += refreshStatus;
  webpage += F("<p class='status'>");
  webpage += pageReflowStatus;
  webpage += F("</p ><br>");

  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Content
void SendHTML_Content()
{
  server.sendContent(webpage);
  webpage = "";
}

//SendHTML_Stop
void SendHTML_Stop()
{
  server.sendContent("");
  server.client().stop(); //Stop is needed because no content length was sent
}

void ReFlowCommand()
{
    if(server.args() > 0  and server.arg(0).toInt() == 0){
    digitalButton = 0;
           targetTemp = 0;
  digitalWrite(ledPinOrange, LOW);
  clearSign2();   
  digitalWrite(ledPinGreen, LOW);  
  clearSign3();   
      start = 0;
  Serial.println("*******************************");
  Serial.println("- Webinterface Abort!");
  Serial.println("! WAIT UNTIL THE PLATE IS COLD!");
  Serial.println("*******************************");
clearTextArea();      
drawCentreText("Reflow Abort!!",50 ,4 ); //Center on Display
drawCentreText("WAIT UNTIL THE",50 ,17 ); //Center on Display
drawCentreText("PLATE IS COLD!",50 ,29 ); //Center on Display
   reflowOff();

    }
    if(server.args() > 0  and server.arg(0).toInt() == 1){
      //start = 1;
    digitalButton = 1;
    }
  webpage = "";
  sName = "ReFlowing System";
  refreshSite = "<meta http-equiv='refresh' content='0; url=/'>";  
    refreshStatus = "";
  append_page_header();

            if(start==0){
   pageReflowStatus = "<span style='color:green'><b>Off</b></span>";  
    }
    
    if(start==1){
   pageReflowStatus = "<span style='color:red'><b>On</b></span>";  
     }
  webpage += F("<div class='inhalt'>");
    webpage  += refreshStatus;
  webpage += F("<p class='status'>");
  webpage += pageReflowStatus;
  webpage += F("</p ><br>");
  webpage += F("</div>");

  append_page_footer();

  SendHTML_Content();
  startCommand = 1;
  if (server.args() > 0 ) //Arguments were received, ignored if there are not arguments
  { 
String message = "Number of args received:";
message += server.args() + "\n";
for (int i = 0; i < server.args(); i++) {

message += "Arg nº" + (String)i + " –> ";
message += server.argName(i) + ": ";
message += server.arg(i) + "\n";

} 
    Serial.println(message);


    //Configuration parameters
/*    if(server.arg(0).toInt() == 0){
      start=0;
    digitalButton = 0;
    Serial.println("ReFlowing Stopped!");
    }
    if(server.arg(0).toInt() == 1){
    digitalButton = server.arg(0).toInt();
    Serial.println("ReFlowing Started!");
    }*/
    //Serial.println(server.arg(0));
    //Serial.println(digitalButton);
  }

}
void startCycle()
{

  webpage = "";
  sName = "ReFlowing System";
  refreshSite = "";  
  refreshStatus = "";
    if(start==0){
   pageReflowStatus = "<span style='color:green'><b>Off</b></span>";  
    digitalButton = 0;
           targetTemp = 0;
  digitalWrite(ledPinOrange, LOW);
  clearSign1();   
  clearSign2();   
  digitalWrite(ledPinGreen, LOW);  
      start = 0;
    }
    
    if(start==1){
   pageReflowStatus = "<span style='color:red'><b>On</b></span>";  
  refreshSite = "<meta http-equiv='refresh' content='5; url=/'>";
  refreshStatus = "<center>Refresh Site every 5 Seconds!</center>";
     }
  append_page_header();
    webpage += F("<div class='inhalt'>");
    webpage  += refreshStatus;
  webpage += F("<p class='status'>");
  webpage += pageReflowStatus;
  webpage += F("</p ><br>");

  webpage += F("<FORM action='/rfcommand' method='post'>");

    if(startCommand == 1 and start==0){
  webpage += F("ReFlowing Stopped!<br><br>");
  webpage += F("Press Button to start ReFlowing!<br>");
  webpage += F("<button type='submit' name='Start' value='1'>Start</button><br><br>");
    }
    if(startCommand == 0 and start==0){
  webpage += F("Press Button to start ReFlowing!<br>");
  webpage += F("<button type='submit' name='Start' value='1'>Start</button><br><br>");
    }
    if(startCommand == 0 and start==1){
    webpage += F("ReFlowing Active!<br>");
    webpage += F("<h3 style='color:red'>Hot plate working...</h3>"); 
  webpage += F("<button type='submit' name='Start' value='0'>Stop</button><br><br>");
    }
    if(startCommand == 1 and start==1){
    webpage += F("ReFlowing Started!<br>");
    webpage += F("<h3 style='color:red'>Hot plate working...</h3>"); 
  webpage += F("<button type='submit' name='Start' value='0'>Stop</button><br><br>");
    }

  webpage += F("</FORM>");

  webpage += F("<a href='/'>Refresh Site</a><br>");
    webpage += F("</div>");

  append_page_footer();

  SendHTML_Content();
  if (server.args() > 0 ) //Arguments were received, ignored if there are not arguments
  { 
String message = "Number of args received:";
message += server.args() + "\n";
for (int i = 0; i < server.args(); i++) {

message += "Arg nº" + (String)i + " –> ";
message += server.argName(i) + ": ";
message += server.arg(i) + "\n";

} 
    Serial.println(message);


    //Configuration parameters
/*    if(server.arg(0).toInt() == 0){
      start=0;
    digitalButton = 0;
    Serial.println("ReFlowing Stopped!");
    }
    if(server.arg(0).toInt() == 1){
    digitalButton = server.arg(0).toInt();
    Serial.println("ReFlowing Started!");
    }*/
    //Serial.println(server.arg(0));
    //Serial.println(digitalButton);
  }
startCommand = 0;
}

//ReportSDNotPresent
void ReportSDNotPresent()
{
  sName = "! Error ! - No SD CARD found!";  
  refreshSite = "";  
  refreshStatus = "";
  SendHTML_Header();
  webpage += F("<h3>No SD Card present</h3>"); 
        webpage += F("<a href='/'>to Startsite</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//ReportFileNotPresent
void ReportFileNotPresent(String target)
{
  sName = "! Error ! - File not found!";  
  refreshSite = "";  
  refreshStatus = "";
  SendHTML_Header();
  webpage += F("<h3>File does not exist</h3>"); 
  webpage += F("<a href='/files?"); webpage += target + "'>Back</a><br><br>";
        webpage += F("<a href='/files'>Back to File System</a><br><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//ReportCouldNotCreateFile
void ReportCouldNotCreateFile(String target)
{
  sName = "! Error ! - File not saved!";  
  refreshSite = "";  
  refreshStatus = "";
  SendHTML_Header();
  webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>"); 
  webpage += F("<a href='/files?"); webpage += target + "'>Back</a><br><br>";
        webpage += F("<a href='/files'>Back to File System</a><br><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");
  append_page_footer();
  SendHTML_Content();
  SendHTML_Stop();
}

//File size conversion
String file_size(int bytes)
{
  String fsize = "";
  if (bytes < 1024)                 fsize = String(bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}



void drawCentreText(const char *buf, int x, int y)
{
int16_t x1;
int16_t y1;
uint16_t w;
uint16_t h;
display.getTextBounds(buf, 0, y, &x1, &y1, &w, &h); //calc width of new string
display.setCursor((x+3) - w / 2, y);
display.print(buf);
}


void clearTextArea(void) {
//    display.fillRect(3, 3, 100, 36, SSD1306_WHITE);
//    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
  display.setCursor(0,0);             // Start at top-left corner
    display.fillRect(2, 3, 101, 36, SSD1306_BLACK);
    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
}

  // temp area
//  display.setTextSize(2);             
//  display.setCursor(6,45);             
//  display.print("220.00");
//  display.print("\xF8");
//  display.print("C");

  // reflow sign on
void reflowOn(void){
  display.setTextSize(3);             
  display.setCursor(109,2);             
  display.print("\xF7");
display.setTextSize(1);             
    display.display(); // Update screen with each newly-drawn rectangle
  
}
  // reflow sign off
void reflowOff(void){
  clearSign1();             
}
  // relay sign on
void relaySignOn(void){
//  clearSign2();
  display.setTextSize(3);             
  display.setCursor(109,17);             
  display.print("\x07");
display.setTextSize(1);             
    display.display(); // Update screen with each newly-drawn rectangle
}
  // relay sign off
void relaySignOff(void){
//  clearSign2();
  display.setTextSize(3);             
  display.setCursor(109,17);             
  display.print("\x09");
display.setTextSize(1);             
    display.display(); // Update screen with each newly-drawn rectangle
}
  // hot plate sign on
void plateHot(void){
  display.setTextSize(3);             
  display.setCursor(109,38);             
  display.print("\x13");
display.setTextSize(1);             
    display.display(); // Update screen with each newly-drawn rectangle
}
  // hot plate sign off
void plateCold(void){
  clearSign3();             
}

void debugtest(void){
  Serial.println("DebugTest");
  int testloop = 50;
  
  for(int tl = 0; tl <= testloop; tl++){
    Serial.print("Test No: ");
    Serial.println(tl);

    clearSign21(); // WHITE BIG
  display.setTextColor(SSD1306_BLACK);
    relaySignOn();
    delay(1000);
    clearSign2(); // BLACK

    clearSign22(); // BLACK BIG
  display.setTextColor(SSD1306_WHITE);
    relaySignOn();
    delay(1000);
    clearSign20(); // WHITE
    
    clearSign21(); // WHITE BIG
  display.setTextColor(SSD1306_BLACK);
    relaySignOff();
    delay(1000);
    clearSign2(); // BLACK

    clearSign22(); // BLACK BIG
  display.setTextColor(SSD1306_WHITE);
    relaySignOff();
    delay(1000);
    clearSign20(); // WHITE
  }
  display.clearDisplay();
}

void clearTempArea(void){
  display.setCursor(0,0);             // Start at top-left corner
    display.fillRect(3, 45, 73, 16, SSD1306_BLACK);
    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
}
void clearSign1(void){
  display.setCursor(0,0);             // Start at top-left corner
    display.fillRect(109, 4, 15, 16, SSD1306_BLACK);
    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
}
void clearSign2(void){
  display.setCursor(0,0);             // Start at top-left corner
//    display.fillRect(109, 22, 15, 13, SSD1306_BLACK);
    display.fillRect(115, 26, 3, 6, SSD1306_BLACK);
    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
}
void clearSign20(void){
  display.setCursor(0,0);             // Start at top-left corner
//    display.fillRect(109, 22, 15, 13, SSD1306_BLACK);
    display.fillRect(115, 26, 3, 6, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
}
void clearSign21(void){
  display.setCursor(0,0);             // Start at top-left corner
    display.fillRect(109, 22, 15, 14, SSD1306_WHITE);
//    display.fillRect(115, 26, 3, 5, SSD1306_BLACK);
    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
}
void clearSign22(void){
  display.setCursor(0,0);             // Start at top-left corner
    display.fillRect(109, 22, 15, 14, SSD1306_BLACK);
//    display.fillRect(115, 26, 3, 5, SSD1306_BLACK);
    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
}
void clearSign3(void){
  display.setCursor(0,0);             // Start at top-left corner
    display.fillRect(109, 38, 15, 21, SSD1306_BLACK);
    display.display(); // Update screen with each newly-drawn rectangle
//    delay(1000);
}



void read_file_info() {
  String buffer;
  String configFile = CONFIGfile;
  file = SD.open(configFile);
  if (!file){      Serial.println(configFile + " not found!!");

writenewconfig();
}
  else{
      Serial.println("load "+configFile);

  Serial.print("\nFile size: ");
  Serial.println(file.size());

  Serial.print("Content inside file: \n");
  while (file.available()){
    buffer = file.readStringUntil('\n');
//    buffer = file.readStringUntil(';');
//  Serial.print("Zeile ");
//  Serial.println(buffer);

    if (buffer.startsWith("wifissid")){
      int index = buffer.indexOf("=");
      int dend = buffer.indexOf(";");
      index = index + 2;
      String sub = buffer.substring(index,dend);
//      Serial.println(buffer);
      Serial.print("wifissid = ");
      Serial.println(sub);
      ssid = sub;
    } 
    else if (buffer.startsWith("wifipass")){
      int index = buffer.indexOf("=");
      int dend = buffer.indexOf(";");
      index = index + 2;
      String sub = buffer.substring(index,dend);
//      Serial.println(buffer);
      Serial.print("wifipass = ");
      Serial.println(sub);
      password = sub;
    } 
    else if (buffer.startsWith("apwifissid")){
      int index = buffer.indexOf("=");
      int dend = buffer.indexOf(";");
      index = index + 2;
      String sub = buffer.substring(index,dend);
//      Serial.println(buffer);
      Serial.print("apwifissid = ");
      Serial.println(sub);
      apssid = sub;
    } 
    else if (buffer.startsWith("apwifipass")){
      int index = buffer.indexOf("=");
      int dend = buffer.indexOf(";");
      index = index + 2;
      String sub = buffer.substring(index,dend);
//      Serial.println(buffer);
      Serial.print("apwifipass = ");
      Serial.println(sub);
      appassword = sub;
    } 
    else if (buffer.startsWith("wlanmode")){
      int index = buffer.indexOf("=");
      int dend = buffer.indexOf(";");
      index = index + 2;
      String sub = buffer.substring(index,dend);
//      Serial.println(buffer);
      Serial.print("wlanmode = ");
      Serial.println(sub);
      wlanmode = sub;
      //wlanmode = "station";
    } 
    else if (buffer.startsWith("debugmode")){
      int index = buffer.indexOf("=");
      int dend = buffer.indexOf(";");
      index = index + 2;
      String sub = buffer.substring(index,dend);
//      Serial.println(buffer);
      Serial.print("debugmode = ");
      Serial.println(sub);
      debugsrc = sub.toInt();
    } 
    else if (buffer.startsWith("curvefile")){
      int index = buffer.indexOf("=");
      int dend = buffer.indexOf(";");
      index = index + 2;
      String sub = buffer.substring(index,dend);
//      Serial.println(buffer);
      Serial.print("curvefile = ");
      Serial.println(sub);
      fileName0 = "/"+sub;
    } 
    else if (buffer.startsWith("dcurvefile")){
      int index = buffer.indexOf("=");
      int dend = buffer.indexOf(";");
      index = index + 2;
      String sub = buffer.substring(index,dend);
//      Serial.println(buffer);
      Serial.print("dcurvefile = ");
      Serial.println(sub);
      fileName1 = "/"+sub;
    } 
    else if (buffer.startsWith("//")){
      Serial.println("comment row");
    } 
    else{
      Serial.println("Unknown Row!");
    }
  }
  }

  Serial.println("\n");

  file.close();
  //delay(3000);
}

void writenewconfig()
{ 
    String filename = CONFIGfile;
 //   if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("write config: "); Serial.println(filename);
    SD.remove(filename);
    File ConfigFile;
    //Remove a previous version, otherwise data is appended the file again
    ConfigFile = SD.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    filename = String();

    if(!ConfigFile) 
    {
      
    }
    if(ConfigFile) 
    {
    String configTXT0 = "// *** Configfile for ReFlowServer ***";  
    String configTXT1 = "wlanmode = ap;       // ap/station";  
    String configTXT2 = "apwifissid = reflowserver;    // ssid for Accesspoint";  
    String configTXT3 = "apwifipass = 12345678;      // passord for Accesspoint";  
    String configTXT4 = "wifissid = ;  // ssid of your Router";  
    String configTXT5 = "wifipass = ;    // passwort of your Router";  
    String configTXT6 = "debugmode = 0;        // Debugmode 0/1 = 0 for curvefile or 1 for dcurvefile";  
    String configTXT7 = "curvefile = CURVE.TXT;      // ReFlow Temp File";  
    String configTXT8 = "dcurvefile = DCURVE.TXT;    // Debugmode ReFlow Temp File";  

    ConfigFile.println(configTXT0); // write a line to esp32.txt
    ConfigFile.println(configTXT1); // write another line to esp32.txt
    ConfigFile.println(configTXT2); // write another line to esp32.txt
    ConfigFile.println(configTXT3); // write another line to esp32.txt
    ConfigFile.println(configTXT4); // write another line to esp32.txt
    ConfigFile.println(configTXT5); // write another line to esp32.txt
    ConfigFile.println(configTXT6); // write another line to esp32.txt
    ConfigFile.println(configTXT7); // write another line to esp32.txt
    ConfigFile.println(configTXT8); // write another line to esp32.txt
    ConfigFile.close();
    Serial.println(F("SD Card: "));
    Serial.println(filename);
    Serial.println(" was successfully written");
  } else {
    Serial.print(F("SD Card: Issue encountered while attempting to open the file esp32.txt"));
  }
read_file_info();
    }

void fileDel(){
  if (server.args() > 0 ) //Arguments were received, ignored if there are not arguments
  { 
String message = "Number of args received:";
message += server.args() + "\n";
for (int i = 0; i < server.args(); i++) {

message += "Arg nº" + (String)i + " –> ";
message += server.argName(i) + ": ";
message += server.arg(i) + "\n";

} 
    Serial.println(message);
  }
  webpage = "";
  sName = "File System";
  refreshSite = "";
  refreshStatus = "";
  SendHTML_Header();
  webpage += F("<h3>Delete File??</h3>"); 
  String delFileName = String(server.arg(0)).substring(7);
        webpage += delFileName+"<br>";
        webpage += F("SURE???<br><br>");
      webpage += F("<FORM action='/files' method='post'>"); 
      webpage += F("<button type='submit' name='delete'"); 
      webpage += F("' value='"); webpage +=String(server.arg(0)); webpage +=F("' style='background-color:red'>delete File</button>");
        webpage += F(" &nbsp;&nbsp;|&nbsp;&nbsp; <a href='/'>back to Startsite</a></FORM>");

      webpage += F("</div>");

  append_page_footer();

  SendHTML_Content();
  SendHTML_Stop();
}

void newstart(){
  webpage = "";
  sName = "Restart System";
  refreshSite = "";
  refreshStatus = "";
  SendHTML_Header();
  webpage += F("<h3>SYSTEM RESTART!!!</h3>"); 
        webpage += F("SURE???<br><br>");
        webpage += F("<a href='/gonewstart' style='background-color:red'>Restart now!</a> &nbsp;&nbsp;|&nbsp;&nbsp; <a href='/'>back to Startsite</a>");

      webpage += F("</div>");

  append_page_footer();

  SendHTML_Content();
  SendHTML_Stop();

}

void gonewstart(){
  webpage = "";
  sName = "Restart System";
  refreshSite = "<meta http-equiv='refresh' content='30; url=/'>";
  refreshStatus = "<center>Startpage load in 30 Seconds!</center>";
  SendHTML_Header();
  webpage += F("<h3>SYSTEM RESTART!!!</h3>"); 
        webpage += F("Please wait ca.30 Sec. and click to Startpage.<br><br>");
        webpage += F("<a href='/'>to Startsite</a><br><br>");

      webpage += F("</div>");

  append_page_footer();

  SendHTML_Content();
  SendHTML_Stop();


    Serial.println("!!! SYSTEM RESTART in 10 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 9 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 8 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 7 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 6 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 5 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 4 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 3 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 2 sec!");
delay(1000);
    Serial.println("!!! SYSTEM RESTART in 1 sec!");
delay(1000);
    Serial.println("!!! RESTART !!!!");
delay(1000);
      ESP.restart(); 

}
