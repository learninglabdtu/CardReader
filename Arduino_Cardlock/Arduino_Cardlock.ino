/*****************
 * 
 * - kasper
 */



#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EEPROM.h>      // For storing IP numbers


#include <MCP23017.h>
#include <SkaarhojGPIO2x8.h>

SkaarhojGPIO2x8 GPIOboard;

// Configure the IP addresses and MAC address with the sketch "ConfigEthernetAddresses":
uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)
uint8_t ip[4];        // Will hold the Arduino IP address
uint8_t ElearningServerIP[4];  // Will hold the Elearning Server IP address (polls, questions, web-controller video production, auto-snapshots)


// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}


EthernetClient client;


//#include <utility/w5100.h>



unsigned long adminUsers[] = {
  1439152219,  // mads
  1373867451,  // franz
  3038927467,    // jan

  2646479370,   // kasper
  2431723322,  // georgi
  2646517914,  // Filip
  2648319610,  // Bjørn
  2182974427,  // Aske
  
  3015030732,  // ohas
  2431700378,   // peej
  3015909660,  // mmor
  1365395131,   // jasc
  3015144556, // jobh
  3038915835, // mkai
  3220358428,  // cbfj
  
  555318875, // ksch
  
  
  1148227963, // Leif temporary
};




#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 8
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif






#include "DHT.h"
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);






// Access card:
#include <SoftwareSerial.h>
unsigned long accessCardCheckTime=0;
char get_readID[] = { 
  0xAA , 0x00, 0x03, 0x25, 0x26, 0x00, 0x00, 0xBB };
SoftwareSerial mySerial(6,5);
unsigned long currentCardID, lastCardID;





void setup()   {  
    // Debug serial:
  Serial.begin(9600);
  Serial << F("\n---------\nSerial Started\n");

  delay(1000);
/*
  // Setting the Arduino IP address:
  ip[0] = EEPROM.read(0+2);
  ip[1] = EEPROM.read(1+2);
  ip[2] = EEPROM.read(2+2);
  ip[3] = EEPROM.read(3+2);

  Serial << F("Device IP Address: ") << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3] << "\n";

  // Setting MAC address:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
  char buffer[18];
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << F("Device MAC address: ") << buffer << F(" - Checksum: ")
    << ((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF) << "\n";
  if ((uint8_t)EEPROM.read(16)!=((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF))  {
    Serial << F("MAC address not found in EEPROM memory!\n") <<
      F("Please load example sketch ConfigEthernetAddresses to set it.\n") <<
      F("The MAC address is found on the backside of your Ethernet Shield/Board\n (STOP)");
    while(true);
  }

  // Elearnign Server IP address:
  ElearningServerIP[0] = EEPROM.read(200);
  ElearningServerIP[1] = EEPROM.read(201);
  ElearningServerIP[2] = EEPROM.read(202);
  ElearningServerIP[3] = EEPROM.read(203);
  Serial << F("Elearning Server IP Address: ") << ElearningServerIP[0] << "." << ElearningServerIP[1] << "." << ElearningServerIP[2] << "." << ElearningServerIP[3]  << F(" - Checksum: ") 
    << ((ElearningServerIP[0]+ElearningServerIP[1]+ElearningServerIP[2]+ElearningServerIP[3]) & 0xFF) << "\n";
  if ((uint8_t)EEPROM.read(204)!=((ElearningServerIP[0]+ElearningServerIP[1]+ElearningServerIP[2]+ElearningServerIP[3]) & 0xFF))  {
    Serial << F("FAILED! did not match ") << EEPROM.read(204);
    while(true);
  }
  
*/
  Ethernet.begin(mac, ip);

//  W5100.setRetransmissionTime(0xD0);  // Milli seconds
//  W5100.setRetransmissionCount(1);


  Wire.begin(); // Start the wire library for communication with the GPIO chip.


    // Init display:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.setRotation(2);
  display.clearDisplay();   // clears the screen and buffer
  display.display(); // show splashscreen

    // Temp sensor.
  dht.begin();

    // Serial for access card reader:
  mySerial.begin(9600);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display << F("DTU Access Card Reader");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display << F("by LearningLab");

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display << "BOOTING...";
  display.display();
  delay(2000);


 Serial << F("Ready to beep\n");

    // Beep:
  pinMode(9, OUTPUT);     
  analogWrite(9,128);
  delay(100);
  analogWrite(9,0);
 
 Serial << F("GPIO:\n");
   //  
  GPIOboard.begin(4);
        openLock();

 Serial << F("Done\n");

}



bool recordingState = false;

void loop() {
  
  // Display stuff:
  display.clearDisplay();   // clears the screen and buffer
  display.setCursor(0,0);

  if (lastCardID && !hasTimedOut(accessCardCheckTime,5000))  {
    display << "Card ID: " << lastCardID << "\n";
    if (recordingState)  {
      display << "ADMIN: Add/Remove User!\n";
    }
  } else {
    printTempHum();
  }
  



    // Reading card:
  currentCardID = accessCardRead();
  if (currentCardID && hasTimedOut(accessCardCheckTime,1000))  {
    lastCardID = currentCardID;
    Serial << "Access Card Number: " << currentCardID << "\n";
     
       // Changes: 
    if (recordingState && !isAdmin(currentCardID))  {
      if (isUser(currentCardID))  {
         removeUser(currentCardID);
      } else {
         addUser(currentCardID);
      }
    } 
    
      // Opening
    else if ((isUser(currentCardID) || isAdmin(currentCardID)))  {
      analogWrite(9,128);
      delay(100);
      analogWrite(9,0);
      openLock();
    } else {  // No access
      analogWrite(9,128);
      delay(2000);
      analogWrite(9,0);
    }
    
      // Setting recording state
    if (isAdmin(currentCardID))  {
      recordingState = true;
    }

    // Everytime a card is presented, update this
    accessCardCheckTime = millis();
  }

    // Resetting recording state after some time when no cards was presented:
  if (recordingState && hasTimedOut(accessCardCheckTime, 5000))  {
    recordingState=false;
  }

  display.display();

  delay(20);
}



void printTempHum()  {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("Failed to read from DHT");
  } else {
    display.clearDisplay();   // clears the screen and buffer
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Humidity: ");
    display.print(h);
    display.println(" %");

    display.print("Temperature: "); 
    display.print(t);
    display.println(" *C");
  }
}

void removeUser(unsigned long cardID)  {
  
}
void addUser(unsigned long cardID)  {
  
}
bool isUser(unsigned long cardID)  {
  
}
bool isAdmin(unsigned long cardID)  {
  for(uint8_t i=0; i< sizeof(adminUsers); i++)  {
    Serial << "Checking admin index " << i << "...";
    if (adminUsers[i]==cardID)  {
      Serial << " OK!\n";
      return true;
    }  
    Serial << "\n";
  }
  return false;
}
void openLock()  {
 Serial << "Opening...\n";
 GPIOboard.setOutput(0,HIGH);
 GPIOboard.setOutput(1,HIGH);
  // Open
 delay(3000);
   // Close it
 Serial << "Closing again...\n";
 GPIOboard.setOutput(0,LOW);
 GPIOboard.setOutput(1,LOW);
}



unsigned long accessCardRead()  {
  int counter=0;
  for (counter = 0 ; counter < 8 ; counter++){
    mySerial.write(get_readID[counter]);
  }
  int i = 0;
  unsigned long cardID = 0;
  while (mySerial.available()) {
    if(i >= 5 && i < 9) {
      cardID |= (unsigned long) mySerial.read() << (i-5)*8;
    } 
    else {
      mySerial.read();
    }
    i++; 
  }
  if(i == 11) {
    return cardID;
  } else return 0;
}



bool hasTimedOut(unsigned long time, unsigned long timeout)  {
  if ((unsigned long)(time + timeout) < (unsigned long)millis())  {  // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true; 
  } 
  else {
    return false;
  }
}

