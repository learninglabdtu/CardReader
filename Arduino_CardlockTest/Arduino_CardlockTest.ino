/*****************
 * 
 * - kasper
 */



#include <MCP23017.h>
#include <SkaarhojGPIO2x8.h>

SkaarhojGPIO2x8 GPIOboard;
MCP23017 GPIOchip; 

// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}


#include <Wire.h>


// Temperature Sensor:
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


  Wire.begin(); // Start the wire library for communication with the GPIO chip.

    // GPIO Chip:
  GPIOchip.begin(B101);
  GPIOchip.init();
  GPIOchip.internalPullupMask(65535);	// All has pull-up
  GPIOchip.inputOutputMask(65535);	// All are inputs.

    // Temp sensor.
  dht.begin();

    // Serial for access card reader:
  mySerial.begin(9600);


    // Beep:
  pinMode(9, OUTPUT);     
  analogWrite(9,128);
  delay(100);
  analogWrite(9,0);

    // Set up GPIO chip for relays and input: 
  Serial << F("GPIO:\n");
  GPIOboard.begin(4);
  
    // Flip relays - should be audible
  flipLock();
  
  
  
  delay(2000);
}


void loop() {
  
    // Print humidity and temperature
  printTempHum();

    // Reading card:
  currentCardID = accessCardRead();
  Serial << "Access Card Number: " << currentCardID << "\n";

    // Flipping relations if inputs is set:
  uint8_t inputs = GPIOboard.inputIsActiveAll();
  Serial.print("Lock 1+2 and menu button inputs: ");
  Serial.println(inputs, BIN);
  GPIOboard.setOutputAll(inputs);
  
    // GPIO Chip readings:
  word buttonStatus = GPIOchip.digitalWordRead();
  Serial.print("Extra GPIO: ");
  Serial.println(buttonStatus, BIN);

    // Analog inputs:
  Serial.print("Analog A0/A1/A2/A3: ");
  Serial << analogRead(A0) << " / " << analogRead(A1) << " / " << analogRead(A2) << " / " << analogRead(A3) << "\n";

  delay(1000);
  Serial.println();
}



void printTempHum()  {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" % - ");

    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  }
}

void flipLock()  {
 Serial << "Triggering Relay on Lock 1...\n";
 GPIOboard.setOutput(1,HIGH);
 delay(500);
 Serial << "Triggering Relay on Lock 2...\n";
 GPIOboard.setOutput(2,HIGH);

  // Open
 delay(1000);
   // Close it
 Serial << "Releasing Relay on Lock 1...\n";
 GPIOboard.setOutput(1,LOW);
 delay(500);
 Serial << "Releasing Relay on Lock 2...\n";
 GPIOboard.setOutput(2,LOW);
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

