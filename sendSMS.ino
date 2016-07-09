char incomingByte[128];
char phoneNumber[16];
int i=0,j=0;
bool number=true;
void setup() 
{
  Serial.begin(9600);
  Serial2.begin(9600);
}
 void sendGPSData()
 {
    delay(1000);
    Serial2.println("AT");
    //Serial.println(Serial2.read());
    delay(1000);
    Serial2.println("AT+CMGF=1");
    delay(1000);
    Serial2.print("AT+CMGS=\"");
    Serial2.println(phoneNumber);
    delay(1000);
    Serial2.println(incomingByte);
    Serial2.write(0x1A);
    Serial2.write(0x1D);
    Serial2.write(0x0A);
    //Serial.println(Serial2.read());
 }
void loop() 
{
  //sendGPSData();
  //delay(5000);
  if(Serial.available()>0)
  {
    if(number==true)
    {
      phoneNumber[i]=(char)Serial.read();
      if(phoneNumber[i]=='"')
      {
        number=false;
        i=0;
      }
      else
      {
        i++;
      }
    }
    else
    {
      incomingByte[i]=(char)Serial.read();
      if(incomingByte[i]=='\n')
      {
        sendGPSData();
        i=0;
        number=true;
      }
      else
      {
        i++;
      }
    }
  };
}
