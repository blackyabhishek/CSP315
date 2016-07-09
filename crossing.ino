#include <GSM.h>

#define PINNUMBER ""

GSM gsmAccess;
GSM_SMS sms;

double timeLimit=60.0;
bool serverSent=false;
int state=0;
int currentTrains=0;
double distanceTrains[3];
double speedTrains[3];
double timeLeftTrains[3];
char trainPhoneNumber[3][16];
bool flagTrains[3];
char sendData[2];
char c='\0';
int temp5,temp2,temp3,temp4,temp6,temp7;
char temp8[16];
int errorPin=11;
int error=LOW;
char receivedWord[16];
double latitude[3],longitude[3],timeStamp[3];
int trainNumber[3];
char senderNumber[16];
char smsPhoneNumber[16];
double lati1,lati2,long1,long2;
bool opposite[3];
int left_maintain[3];
int tempLeftMaintain;
double latitude_crossing[3];
double longitude_crossing[3];
int number_of_crossing_points = 3;
int crossing_data_left[3];
double crossing_points_distance[3];
double temp;

//Remeber to initialise the above arrays in setup

void setup() 
{
  Serial.begin(9600);
  pinMode(errorPin,INPUT);
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
  Serial.println("GSM initialized");
  
  //Initialisation of vairables
  sendData[1]='\0';
  for (int i=0;i<3;i++)
  {
    distanceTrains[i] = 100000000.0;
    speedTrains[i] = 0.0;
    timeLeftTrains[i] = 100000000.0;
    flagTrains[i] = false;
  }
  
  crossing_points_distance[0]= 77;
  latitude_crossing[0] = 28.545007;
  longitude_crossing[0] = 77.191623;
  crossing_data_left[0] = 0;
  
  crossing_points_distance[1] = 0;
  latitude_crossing[1] = 28.5452773 ;
  longitude_crossing[1] =77.19234028;
  crossing_data_left[1] = 0;
  
  crossing_points_distance[2] = 57;
  latitude_crossing[2] = 28.54541 ;
  longitude_crossing[2] = 77.192907;
  crossing_data_left[2] = 1;
  
}
int findIndexByPN()
{
  int i = 0;
  bool valid = false;
  for (i=0;i<3;i++)
  {
    valid = true;
    for (int j = 0;trainPhoneNumber[i][j] != '\0';j++)
    {
      if(trainPhoneNumber[i][j] != senderNumber[j+3])
      {
        valid = false;
        break;
      }
    }
    if(valid)
      break;
  }
  return i;
}
void swap(int i,int j)
{
    temp2 = distanceTrains[i];
    temp3 = speedTrains[i];
    temp4 = timeLeftTrains[i];
    temp5 = flagTrains[i];
    temp6 = trainNumber[i];
    temp7 = left_maintain[i];
    for(int ind=0;ind<16;ind++)
      temp8[ind]=trainPhoneNumber[i][ind];
    
    distanceTrains[i] = distanceTrains[j];
    speedTrains[i] = speedTrains[j];
    timeLeftTrains[i] = timeLeftTrains[j];
    flagTrains[i] = flagTrains[j];
    trainNumber[i]=trainNumber[j];
    left_maintain[i]=left_maintain[j];
    for(int ind=0;ind<16;ind++)
      trainPhoneNumber[i][ind]=trainPhoneNumber[j][ind];
    
    distanceTrains[j] = temp2;
    speedTrains[j] = temp3;
    timeLeftTrains[j] = temp4;
    flagTrains[j] = temp5;
    trainNumber[j]=temp6;
    left_maintain[j]=temp7;
    for(int ind=0;ind<16;ind++)
      trainPhoneNumber[j][ind]=temp8[ind];
}
void sort()
{
  if(!flagTrains[0] && flagTrains[1])
    swap(0,1);
  if(!flagTrains[1] && flagTrains[2])
    swap(1,2);
  if(!flagTrains[0] && flagTrains[1])
    swap(0,1);
}
int charToInt()
{
   int num=0;
   for(int i=0;receivedWord[i]!='\0';i++)
     num=num*10+(int)receivedWord[i]-48;
   return num;
}
 double charTodouble()
 {
   double num=0.0;
   int p=0;
   int i=0;
   for(;receivedWord[i]!='\0';i++)
   {
     if(receivedWord[i]=='.')
       p=i+1;
     else
       num=num*10.0+(int)receivedWord[i]-48;
   }
   num=num*pow(10.0,p-i);
   return num;
 }
void sendSMS(int index)
{
  Serial.println(trainPhoneNumber[index]);
  sms.beginSMS(trainPhoneNumber[index]);
  sms.print(sendData);
  Serial.println(sendData);
  sms.endSMS();
}
void store(int index,int tN)
{
  switch(index)
  {
    case 0 : 
      timeStamp[tN]=charTodouble();
      break;
    case 1 : 
      latitude[tN]=charTodouble();
      break;
    case 2 :
      break;
    case 3 :
      longitude[tN]=charTodouble();
      break;
    case 4:
      break;
    case 5 :
      speedTrains[tN]=charTodouble();
      break;
    case 6 :
      break;
    case 7 :
      trainNumber[tN]=charToInt();
      break;
    default : 
      break;
  }
      
}
void receiveSMS()
{
  if(sms.available())
  {
    Serial.print("Message received from:");
    sms.remoteNumber(senderNumber,20);
    /*if(senderNumber[0]=='+' && senderNumber[1]=='9' && senderNumber[2]=='1')
      for(int i=3;i<16;i++)
        senderNumber[i-3]=senderNumber[i];*/
    Serial.println(senderNumber);
    if (sms.peek() == '#')
      sms.flush();
    if(!((senderNumber[0]=='1' && senderNumber[1]=='2' && senderNumber[2]=='1') || (senderNumber[0]=='A' && senderNumber[1]=='D')))
    {
      c=sms.read();
      Serial.print(c);
      if(c=='S')
      {
        c=sms.read();
        Serial.print(c);
        serverSent=true;
        int i;
        for(i=0;c = sms.read();i++)
        {
          if(c!=' ')
          {
            Serial.print(c);
            smsPhoneNumber[i]=c;
          }
          else
            break;
        }
        smsPhoneNumber[i]='\0';
        tempLeftMaintain=(int)sms.read()-48;
        Serial.println("");
        Serial.print(tempLeftMaintain);
      }
      else if(c=='T')
      {
        c=sms.read();
        Serial.print(c);
        int i,ind;
        for(i=0,ind=0;c=sms.read();i++)
        {
          if(c==' ')
          {
            receivedWord[i]='\0';
            Serial.println(receivedWord);
            store(ind,findIndexByPN());
            ind++;
            i=-1;
          }
          else
          {
            receivedWord[i]=c;
          }
        }
        distanceUpdater(findIndexByPN());
        for(i=0;i<16;i++)
          receivedWord[i]='\0';
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
  }
  delay(1000);
}
double calculate_distance(int index,int crossing_point)
{
  lati1 = ((int)latitude[index])/100 + (((long)(latitude[index]*10000))%1000000)/600000.0;
  lati2 = latitude_crossing[crossing_point];
  long1 = ((int)longitude[index])/100 + (((long)(longitude[index]*10000))%1000000)/600000.0;
  long2 = longitude_crossing[crossing_point];
  return 111194.93*sqrt((lati1-lati2)*(lati1-lati2) + cos((lati1+lati2)*0.00872665)*cos((lati1+lati2)*0.00872665)*(long1-long2)*(long1-long2));
}
void distanceUpdater(int index)
{
  Serial.print("Index : ");
  Serial.println(index);
  double mini=100000000.0;
  int ind = 0;
  for (int i=0;i<number_of_crossing_points;i++)
  {
    temp = calculate_distance(index,i);
    Serial.print("Temp ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(temp);
    if(temp < mini)
    {
      mini = temp;
      ind = i;
    }
  }
  if(left_maintain[ind] != crossing_data_left[ind])
    opposite[index] = true;
  else if(left_maintain[ind] == crossing_data_left[ind])
    opposite[index] = false;
  distanceTrains[index] = crossing_points_distance[ind] + mini;
}
bool trainPassed(int index)
{
  if(distanceTrains[index] > 100.0 && opposite[index] && flagTrains[index])
    return true;
  return false;
}

void loop() 
{
  error=digitalRead(errorPin);
  receiveSMS();
  Serial.print(state);
  Serial.print(" ");
  Serial.print(distanceTrains[0]);
  Serial.print(" ");
  Serial.print(opposite[0]);
  Serial.print(" ");
  Serial.println(flagTrains[0]);
  delay(5000);
  switch(state)
  {
    case 0 : 
      if(serverSent==true)
      {
        state++;
        currentTrains++;
        serverSent=false;
        flagTrains[0] = true;
        for (int i=0;i<16;i++)
          trainPhoneNumber[0][i] = smsPhoneNumber[i];
        left_maintain[0]=tempLeftMaintain;
      }
      break;
     case 1 :
       if(error==HIGH)
       {
         sendData[0] = 'N';       
         sendSMS(0);
         state=20;
       }
       else if(serverSent==true)
       {
         state++;
         currentTrains++;
         flagTrains[1] = true;
         for (int i=0;i<16;i++)
          trainPhoneNumber[1][i] = smsPhoneNumber[i];
         left_maintain[1]=tempLeftMaintain;
         serverSent=false;
       }
       else if(trainPassed(0)==true)
       {
         state--;
         sendData[0] = 'Y';
         sendSMS(0);
         flagTrains[0] = false;
         opposite[0] = false;
         sort();
         currentTrains--;
       }
       break;
     case 2 :
       if(error==HIGH)
       {
         sendData[0] = 'N';        
         sendSMS(0);
         sendSMS(1);
         state=20;
       }
       else if(serverSent==true)
       {
         state++;
         currentTrains++;
         flagTrains[2] = true;
         for (int i=0;i<16;i++)
          trainPhoneNumber[2][i] = smsPhoneNumber[i];
         left_maintain[2]=tempLeftMaintain;
         serverSent=false;
       }
       else if(trainPassed(0)==true)
       {
         state--;
         sendData[0] = 'Y';
         sendSMS(0);
         flagTrains[0] = false;
         opposite[0] = false;
         sort();
         currentTrains--;
       }
       else if(trainPassed(1)==true)
       {
         state--;
         sendData[0] = 'Y';
         sendSMS(1);
         flagTrains[1] = false;
         opposite[1] = false;
         sort();
         currentTrains--;
       }
       break;
     case 3 :
       if(error==HIGH)
       {
         sendData[0] = 'N';         
         sendSMS(0);
         sendSMS(1);
         sendSMS(2);
         state=20;
       }
       else if(serverSent==true)
       {
         Serial.println("Error");
         /*state=4;
         currentTrains++;
         flagTrains[3] = 1;*/
         serverSent=false;
       }
       else if(trainPassed(0)==true)
       {
         state--;
         sendData[0] = 'Y';
         sendSMS(0);
         flagTrains[0] = false;
         opposite[0] = false;
         sort();
         currentTrains--;
       }
       else if(trainPassed(1)==true)
       {
         state--;
         sendData[0] = 'Y';       
         sendSMS(1);
         flagTrains[1] = false;
         opposite[1] = false;
         sort();
         currentTrains--;
       }
       else if(trainPassed(2)==true)
       {
         state--;
         sendData[0] = 'Y';       
         sendSMS(2);
         flagTrains[2] = false;
         opposite[2] = false;
         sort();
         currentTrains--;
       }
       break;
     case 20 :
       if(currentTrains==1)
         state=11;
       else if(currentTrains==2)
         state=12;
       else if(currentTrains==3)
         state=13;
       break;
     case 11 :
       if(serverSent==true)
       {
         state++;
         currentTrains++;
         flagTrains[1] = true;
         for (int i=0;i<16;i++)
          trainPhoneNumber[1][i] = smsPhoneNumber[i];
         left_maintain[1]=tempLeftMaintain;
         serverSent=false;
       }
       else if(trainPassed(0)==true)
       {
         state--;
         flagTrains[0] = 0;
         opposite[0] = false;
         sort();
         currentTrains=0;
       }
       break;
     case 12 :
       if(serverSent==true)
       {
         state++;
         currentTrains++;
         flagTrains[2] = true;
         for (int i=0;i<16;i++)
          trainPhoneNumber[2][i] = smsPhoneNumber[i];
         left_maintain[2]=tempLeftMaintain;
         serverSent=false;
       }
       else if(trainPassed(0)==true)
       {
         state--;
         sendData[0] = 'Y';         
         sendSMS(0);
         flagTrains[0] = false;
         opposite[0] = false;
         sort();
         currentTrains--;
       }
       else if(trainPassed(1)==true)
       {
         state--;
         sendData[0] = 'Y';       
         sendSMS(1);
         flagTrains[1] = false;
         opposite[1] = false;
         sort();
         currentTrains--;
       }
       break;
     case 13 :
       if(serverSent==true)
       {
         Serial.println("Error");
         /*state=14;
         currentTrains++;
         flagTrains[3] = 1;*/
         serverSent=false;
       }
       else if(trainPassed(0)==true)
       {
         state--;
         sendData[0] = 'Y';
         sendSMS(0);
         flagTrains[0] = false;
         opposite[0] = false;
         sort();
         currentTrains--;
       }
       else if(trainPassed(1)==true)
       {
         state--;
         sendData[0] = 'Y';
         sendSMS(1);
         flagTrains[1] = false;
         opposite[1] = false;
         sort();
         currentTrains--;
       }
       else if(trainPassed(2)==true)
       {
         state--;
         sendData[0] = 'Y';      
         sendSMS(2);
         flagTrains[2] = false;
         opposite[2] = false;
         sort();
         currentTrains--;
       }
       break;
     default : 
       break;
  }
}
