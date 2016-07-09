  #include <GSM.h>

  #define PINNUMBER ""

  GSM gsmAccess;
  GSM_SMS sms;
  char c='\0';
  int state=0;
  bool timeGPS=true;
  char senderNumber[16];
  char crossingPhoneNumber[16];
  char myPhoneNumber[]={'9','9','5','8','2','2','6','5','8','7','\0'};
  char trainID[]={'1','2','3','4','5','6','\0'};
  char serverPhoneNumber[]={'9','5','9','9','5','3','9','9','8','4','\0'};
 int ledPin = 13;
 
 int rxPin1GPS = 19;
 int txPin1GPS = 18;
 int rxPin2GSM = 17;
 int txPin2GSM = 16;
 int byteGPS = -1;
 char charGPS = ' ';
 char stringGPS[100] = "";
 char sendData [100] = "";
 int c1 = 0;
 int c2 = 0;
 int ind = 0;
 
 unsigned long timeStamp = 0;
 char validity = ' ';
 float latitude = 0.0;
 char NorS = ' ';
 float longitude = 0.0;
 char EorW = ' ';
 float speedKnots = 0.0;
 float trueCourse = 0.0;
 unsigned long dateStamp = 0;
 float magneticVariation = 0.0;
 char magneticDirection = ' ';
 char mode = ' ';
 unsigned long checkSum = 0;
 
 char extractWord [12] = "";
 int pos = 0;
 void reset()
{
  sms.print("AT\r");
  delay(4000);
  sms.print("AT&F\r" );
  delay(4000);
  sms.print("AT+CMGD=0,4\r");
  delay(4000);
}
 void setup() 
 {
   pinMode(ledPin, OUTPUT);
   pinMode(rxPin1GPS, INPUT);
   pinMode(txPin1GPS, OUTPUT);
   pinMode(rxPin2GSM, INPUT);
   pinMode(txPin2GSM, OUTPUT);
   Serial.begin(9600);
   Serial1.begin(9600);
   Serial2.begin(9600);
   for (int i=0;i<100;i++)
   {
     stringGPS[i]=' ';
   }
   Serial.println("Starting\n");
   
  boolean notConnected = true;
  while (notConnected)
  {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY)
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  //reset();
  Serial.println("GSM initialized");
 }
 
 unsigned long charToInt()
 {
   unsigned long num=0;
   for(int i=0;extractWord[i]!=' ' && extractWord[i]!='.';i++)
   {
     num=num*10+(int)extractWord[i]-48;
   }
   return num;
 }
 float charToFloat()
 {
   float num=0.0;
   int p=0;
   int i=0;
   for(;extractWord[i]!=' ';i++)
   {
     if(extractWord[i]=='.')
     {
       p=i+1;
     }
     else
     {
       num=num*10.0+(int)extractWord[i]-48;
     }
   }
   num=num/pow(10.0,i-p);
   return num;
 }
 
 void printGPSData()
 {
   Serial.print("Time in UTC (HHMMSS) : ");
   Serial.println(timeStamp);
   
   Serial.print("Status (A=OK,V=KO) : ");
   Serial.println(validity);
   
   Serial.print("Latitude : ");
   Serial.println(latitude,4);
   
   Serial.print("Direction (N/S) : ");
   Serial.println(NorS);
   
   Serial.print("Longitude : ");
   Serial.println(longitude,4);
   
   Serial.print("Direction (E/W) : ");
   Serial.println(EorW);
   
   Serial.print("Speed in Knots : ");
   Serial.println(speedKnots);
   
   Serial.print("Heading in degrees : ");
   Serial.println(trueCourse);
   
   Serial.print("Date UTC (DDMMYY) : ");
   Serial.println(dateStamp);
   
   Serial.print("Magnetic degrees: ");
   Serial.println(magneticVariation);
   
   Serial.print("Magnetic direction (E/W) : ");
   Serial.println(magneticDirection);
   
   Serial.print("Mode : ");
   Serial.println(mode);
   
   Serial.print("Checksum : ");
   Serial.println(checkSum);
 }
 
 void parseGPRMC()
 {
   pos=0;
   sendData[ind++]='T';
   sendData[ind++]=' ';
   for(int i=0,j=0;stringGPS[i]!=10 && stringGPS[i]!=13;i++)
   {
     if(stringGPS[i]==',' || stringGPS[i]=='*')
     {
       c2=0;
       switch(pos)
       {
         case 1 : timeStamp=charToInt();;
                   for(j=0;extractWord[j]!=' ';ind++)
                     sendData[ind]=extractWord[j++];
                   sendData[ind]=' ';
                   ind++;
                   break;
         case 2 : validity=extractWord[0]; break;
         case 3 : latitude=charToFloat();
                   for(j=0;extractWord[j]!=' ';ind++)
                     sendData[ind]=extractWord[j++];
                   sendData[ind]=' ';
                   ind++;
                   break;
         case 4 : NorS=extractWord[0]; 
                  for(j=0;extractWord[j]!=' ';ind++)
                     sendData[ind]=extractWord[j++];
                   sendData[ind]=' ';
                   ind++;
                   break;
         case 5 : longitude=charToFloat(); 
                  for(j=0;extractWord[j]!=' ';ind++)
                     sendData[ind]=extractWord[j++];
                   sendData[ind]=' ';
                   ind++;
                   break;
         case 6 : EorW=extractWord[0];
                  for(j=0;extractWord[j]!=' ';ind++)
                     sendData[ind]=extractWord[j++];
                   sendData[ind]=' ';
                   ind++;
                 break;
         case 7 : speedKnots=charToFloat(); 
                  for(j=0;extractWord[j]!=' ';ind++)
                     sendData[ind]=extractWord[j++];
                   sendData[ind]=' ';
                   ind++;
                   break;
         case 8 : trueCourse=charToFloat(); break;
         case 9 : dateStamp=charToInt(); break;
         case 10 : magneticVariation=charToFloat(); break;
         case 11 : magneticDirection=extractWord[0]; break;
         case 12 : mode=extractWord[0]; break;
         case 13 : checkSum=charToInt(); break;
       }
       for(int i=0;i<12;i++)
       {
         extractWord[i]=' ';
       }
       pos++;
     }
     else
     {
       extractWord[c2]=stringGPS[i];
       c2++;
     }
   }
   checkSum=charToInt();
 }
 void appendNumberAndID()
 {
   int j;
   for(j=0;myPhoneNumber[j]!='\0';ind++)
     sendData[ind]=myPhoneNumber[j++];
   sendData[ind]=' ';
   ind++;
   for(j=0;trainID[j]!='\0';ind++)
     sendData[ind]=trainID[j++];
   sendData[ind]=' ';
   ind++;
 }
 void sendGPSData1()
 {
   if(state==0)
   {
     Serial.println(serverPhoneNumber);
     sms.beginSMS(serverPhoneNumber);
   }
   else if(state==1)
   {
     Serial.println(crossingPhoneNumber);
     sms.beginSMS(crossingPhoneNumber);
   }
  sms.print(sendData);
  Serial.println(sendData);
  sms.endSMS();
 }
 void receiveGPSData()
 {
   digitalWrite(ledPin, HIGH);
   byteGPS=Serial1.read();
   if(byteGPS!=-1)
   {
     charGPS=(char)byteGPS;
     Serial.print(charGPS);
     if(charGPS=='$')
     {
       if(stringGPS[1]=='G' && stringGPS[2]=='P' && stringGPS[3]=='R' && stringGPS[4]=='M' && stringGPS[5]=='C')
       {
         parseGPRMC();
         appendNumberAndID();
         //printGPSData();
         //Serial.println("@@@@@@@@@@@@@@@@@@@@@@");
         sendGPSData1();
         timeGPS=false;
         ind=0;
         for(int i=0;i<100;i++)
           sendData[i]='\0';
         //Serial.println("@@@@@@@@@@@@@@@@@@@@@@");
         delay(30000);
       }
       for(int i=1;i<c1;i++)
         stringGPS[i]=' ';
       c1=1;
     }
     else
     {
       stringGPS[c1]=charGPS;
       c1++;
     }
   }
 }
 
 void sendGPSData()
 {
    delay(1000);
    Serial2.println("AT");
    Serial.println(Serial2.read());
    delay(1000);
    Serial2.println("AT+CMGF=1"); 
    delay(1000);
    Serial2.println("AT+CMGS=\"9717471175\"");
    delay(1000);
    Serial2.println(sendData);
    Serial.println(sendData);
    Serial2.write(0x1A);
    Serial2.write(0x1D);
    Serial2.write(0x0A);
    Serial.println(Serial2.read());
 }
 
 void receiveSMS()
 {
  if(sms.available())
  {
    Serial.print("Message received from:");
    sms.remoteNumber(senderNumber,20);
    Serial.println(senderNumber);
    if (sms.peek() == '#')
      sms.flush();
    if(!((senderNumber[0]=='1' && senderNumber[1]=='2' && senderNumber[2]=='1') || (senderNumber[0]=='A' && senderNumber[1]=='D')))
    {
      c=sms.read();
      Serial.print(c);
      if(c=='Y' || c=='N' || c=='y' || c=='n')
        state=0;
      else if(c=='S')
        state=1;
      if(state==1)
      {
        int i;
        c = sms.read();
        for(i=0;c = sms.read();i++)
        {
          if(c=='\n')
            break;
          Serial.print(c);
          crossingPhoneNumber[i]=c;
        }
        crossingPhoneNumber[i]='\0';
      }
      Serial.println("\nEND OF MESSAGE");
    }
    else
    {
      for(int i=0;c=sms.read();i++)
        Serial.print(c);
       Serial.println("\nEND OF AIRTEL MESSAGE");
    }
    sms.flush();
    
    //sms.print("AT+CMGD=0,4\r");
    //delay(1000);
  }
  timeGPS=true;
  delay(1000);
 }
 
 void loop() 
 {
   if(timeGPS)
     receiveGPSData();
   else
     receiveSMS();
 }
