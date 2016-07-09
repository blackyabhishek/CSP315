/*
 SMS receiver

 This sketch, for the Arduino GSM shield, waits for a SMS message
 and displays it through the Serial port.

 Circuit:
 * GSM shield attached to and Arduino
 * SIM card that can receive SMS messages

 created 25 Feb 2012
 by Javier Zorzano / TD

 This example is in the public domain.

 http://arduino.cc/en/Tutorial/GSMExamplesReceiveSMS

*/

// include the GSM library
#include <GSM.h>

// PIN Number for the SIM
#define PINNUMBER ""

// initialize the library instances
GSM gsmAccess;
GSM_SMS sms;

// Array to hold the number a SMS is retreived from
char senderNumber[20];

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  
  delay(4000);

  Serial.print("SMS Messages Receiver@");
  

  // connection state
  boolean notConnected = true;

  // Start GSM connection
  while (notConnected)
  {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY)
      notConnected = false;
    else
    {
      Serial.print("Not connected@");
      delay(1000);
    }
  }

  Serial.print("GSM initialized\n");
  Serial.print("Waiting for messages@");
}

void loop()
{
  char c;

  // If there are any SMSs available()
  if (sms.available())
  {
    //Serial.println("Message received from : @");

    // Get remote number
    sms.remoteNumber(senderNumber, 20);
    Serial.print(senderNumber);
    Serial.print("@");
    delay(1000);

    // An example of message disposal
    // Any messages starting with # should be discarded
    if (sms.peek() == '#')
    {
      Serial.print("Discarded SMS@");
      sms.flush();
    }

    // Read message bytes and print them
    while (c = sms.read())
      Serial.print(c);
    Serial.print("@");
    delay(1000);
    Serial.print("END OF MESSAGE@");

    // Delete message from modem memory
    sms.flush();
    Serial.print("MESSAGE DELETED@");
  }

  delay(1000);

}


