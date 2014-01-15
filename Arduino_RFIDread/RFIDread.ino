/*****************
 * 
 * - kasper
 */




// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



// Access card:
#include <SoftwareSerial.h>
unsigned long accessCardCheckTime=0;
char get_readID[] = { 
  0xAA , 0x00, 0x03, 0x25, 0x26, 0x00, 0x00, 0xBB };
SoftwareSerial mySerial(6,5);
unsigned long currentCardID, lastCardID;





void setup()   {                
  Serial.begin(57600);
  Serial << "Started\n";
  delay(1000);
  mySerial.begin(9600);
  delay(1000);
}


void loop() {
  delay(100);  // There MUST be a delay here, otherwise the reading fails. ... 
  currentCardID = accessCardRead();
  if (currentCardID && hasTimedOut(accessCardCheckTime,1000))  {
    accessCardCheckTime = millis();
    lastCardID = currentCardID;
    Serial << "Access Card Number: " << currentCardID << "\n";
  }
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

