#include "Arduino.h"
#include "heltec.h"
#include <string.h>
#include <RadioLib.h>
#include <ArduinoJson.h>
#include <string.h>
String jsonString;
DynamicJsonDocument parsedJson(200);
const char *data[] = {

  "Welcome To Bitsilica",

  "Available Slot : ",

  "Slot 1 : ",

  "Slot 2 : ",

  "Slot 3 : ",

  "Slot 4 : ",

};
#define DEMO_DURATION 4000
String str1; // Data
SX1262 radio = new Module(8,14,12,13); // Connecting the Module with Pins
String newstr = str1;
String oldstr= "3AAA";
volatile bool receivedFlag = false;  // flag to indicate that a packet was received 
// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // we got a packet, set the flag
  receivedFlag = true;
}
void setup_Module() {
  Serial.begin(9600);
// initialize SX1262 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  radio.setPacketReceivedAction(setFlag); // set the function that will be called   // when new packet is received
  Serial.print(F("[SX1262] Starting to listen ... ")); // start listening for LoRa packets
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
  Serial.println(F("success!"));
  } 
  else {
      Serial.print(F("failed, not connected successfully "));
      Serial.println(state);
      while (true);
  }
}
void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10); 
  Heltec.display->clear(); // clear the display
  drawFontFaceDemo();
  Heltec.display->display();  // write the buffer to the display
  delay(3000);
  Heltec.display->clear();
  setup_Module();
}

void drawFontFaceDemo() { // Font Demo1    // create more fonts at http://oleddisplay.squix.ch/
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->drawString(0, 10, "BITSILICA");
  Heltec.display->setFont(ArialMT_Plain_10); 
  Heltec.display->drawString(0, 35, "ePARKING-SYSTEM");
}
void slotData(uint8_t pos, uint8_t height) {
  if(pos == 1)
  {
    switch(newstr[pos-1]){  // slot availability
      /*case '4':
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->setFont(ArialMT_Plain_10);
        Heltec.display->drawString(80, height, "4");
        break;*/
      case '3':
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->setFont(ArialMT_Plain_10);
        Heltec.display->drawString(80, height, "3");
        break;
      case '2':
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->setFont(ArialMT_Plain_10);
        Heltec.display->drawString(80, height, "2");
        break;
      case '1':
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->setFont(ArialMT_Plain_10);
        Heltec.display->drawString(80, height, "1");
        break;
      case '0':
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->setFont(ArialMT_Plain_10);
        Heltec.display->drawString(80, height, "0");
        break;
      default:
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->setFont(ArialMT_Plain_10);
        Heltec.display->drawString(80, height, "----");
        break;
    }
  }
  else
  {
  switch(newstr[pos-1]) // update the slots on OLED
  {
    case 'A':
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_10);
      Heltec.display->drawString(80, height, "EMPTY");
      break;
    case 'B':
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_10);
      Heltec.display->drawString(80, height, "PARKED");
      break;
    case 'Z':
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_10);
      Heltec.display->drawString(80, height, "WRONG");
      break;
    default:
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_10);
      Heltec.display->drawString(80, height, "----");
      break;
  }
  }
}

void printParkingData() { // Printing the Data on OLED
  for(uint8_t i=0, j=0; i<6; i++, j=j+10){
    if(i == 0) {
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_10);
      Heltec.display->drawString(0, j, data[i]);
    }
    else{
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_10);
      Heltec.display->drawString(0, j, data[i]);
      slotData(i, j);
    }
  }
oldstr=newstr;
Heltec.display->display();
}
void Module_loop()
{
 if(receivedFlag) {   // check if the flag is set
    receivedFlag = false;     // reset flag
    int state = radio.readData(str1);
    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(F("[SX1262] Received packet!"));       // packet was successfully received
      Serial.print(F("[SX1262] Data:\t\t"));       // print data of the packet
      Serial.println(str1);
      Serial.print(F("[SX1262] RSSI:\t\t"));       // print RSSI (Received Signal Strength Indicator)
      Serial.print(radio.getRSSI());
      Serial.println(F(" dBm"));
      Serial.print(F("[SX1262] SNR:\t\t")); //      // print SNR (Signal-to-Noise Ratio)
      Serial.print(radio.getSNR());
      Serial.println(F(" dB"));
      Serial.print(F("[SX1262] Frequency error:\t"));       // print frequency error
      Serial.print(radio.getFrequencyError());
      Serial.println(F(" Hz"));

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));

    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
  }
}
bool myStrCmp(const String &str1, const String &str2) {
  // Check if the lengths are different, if so, they can't be the same
  if (str1.length() != str2.length()) {
    return false;
  }

  // Compare character by character
  for (size_t i = 0; i < str1.length(); i++) {
    if (str1[i] != str2[i]) {
      return false; // Characters don't match, strings are not equal
    }
  }
  return true;
}
void loop() {
 Module_loop();
 newstr=str1;
 Serial.println("After Module_Loop");
 Serial.println(newstr);
 Serial.println(oldstr);
 if(myStrCmp(newstr, oldstr)) //Heltec.display->clear();
  {
    Heltec.display->clear();
    printParkingData();
    delay(2000);
    Serial.println("in StrCmp else statement -> After Module_Loop");
  }
  oldstr=newstr;
  delay(5000);
}