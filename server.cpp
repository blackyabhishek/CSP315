//
//  main.cpp
//  train_server
//
//  Created by Vansh Pahwa & Utkarsh Sharma on 3/29/15.
//  Copyright (c) 2015 Vansh Pahwa & Utkarsh Sharma. All rights reserved.
//

#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>
#include <fstream>
#include <map>
#include "arduino_handle.h"

using namespace std;

#define MIN_DISTANCE_TO_CHECK_IN_DATABASE 150
#define MIN_DISTANCE 100
#define conversion_scale_latitude   185300
#define conversion_scale_longitude  119000
#define crossingfilename "crossings.txt"
#define FILE_DIRECTORY ""


int USB2;


int eucledian_distance(float lat_1,float long_1,float latitude2,float longitude2) // not commutative
{
    float latitude1     =((int)lat_1)/100 + ((int)(lat_1*10000)%1000000)/(10000*60.0);
    float longitude1    =((int)long_1)/100 + (((int) (long_1*10000))%1000000)/(60.0 *10000);
    ///cout<<"LAT1:"<< latitude1<<" LONG1:"<<longitude1<<" LAT2:"<<latitude2<<" LONG2:"<<longitude2<<endl;
    return (int)111194.93*sqrt((latitude1-latitude2)*(latitude1-latitude2) + cos((latitude1+latitude2)*0.00872663)*cos((latitude1+latitude2)*0.00872663)*(longitude1-longitude2)*(longitude1-longitude2));
}
struct train_info
{
    int trainId;
    float latitude;
    float longitude;
    float velocity; // velocity in km/hr
    string train_phone_no;
    int time_last_updated = 0;
};

struct crossing_point_info
{
    float latitude;
    float longitude;
    int distance;
    bool to_right;
};

struct message_received
{
    int timestamp;
    train_info train_data;
};

struct message_sent_to_crossing
{
    train_info traindata;
};

struct crossing_data_point_info
{
    float latitude;
    float longitude;
    int distance;
    bool to_right;
};
struct crossing_info
{
    int crossing_id;
    string crossing_phone_no;
    float latitude;
    float longitude;
    
    vector<crossing_data_point_info> local_geographic_data;
};
struct message_sent_to_train
{
    crossing_info crossing;
};

train_info latest_info[1000];
////crossing_info crossing_db[1000];

int number_of_trains = 1;
int TrainId[1] = {123456};
crossing_info *crossings;
int number_of_crossings = 0;
map <int, int> mymap;

fstream *train_files = new fstream[number_of_trains];


void fileopener()
{
    
    string filee = "train";
    for (int i=0;i<number_of_trains;i++)
    {
        string filee2 = filee + to_string(i) + ".txt";
        cout << filee2 << "\n";
        mymap.insert(make_pair(TrainId[i],i));
        
        train_files[i].open(filee2);
    }
}




void readCrossingFile(char *filename)
{
    ifstream fin(filename,ios::in);
    string phone_no;
    int crossing_id;
    float latitude;
    float longitude;
    fin >> number_of_crossings;
    crossings = new crossing_info[number_of_crossings];
    for (int i=0;i<number_of_crossings;i++)
    {
        fin >> phone_no >> crossing_id >> latitude >> longitude;
        crossings[i].crossing_phone_no = phone_no;
        crossings[i].crossing_id = crossing_id;
        crossings[i].latitude = latitude;
        crossings[i].longitude = longitude;
    }
    fin.close();
}




void WriteTrainData(message_received mes)
{
    int i = mymap.find(mes.train_data.trainId)->second;
    string filee = "train";
    filee = filee+ to_string(0) + ".txt";
    ofstream fileopen(FILE_DIRECTORY+filee);
    fileopen << mes.train_data.train_phone_no << " " << mes.train_data.latitude << " " <<mes.train_data.longitude << " "<< mes.timestamp << "\n";
    ///train_files[i].seekg(0,train_files[i].beg);
    fileopen.close();
}
void print_debug(crossing_data_point_info cdpi)
{
    printf("Latitude : %f \n Longitude : %f \n Distance %d \n To_Right: %d \n",cdpi.latitude,cdpi.longitude,cdpi.distance,cdpi.to_right);
}
void print_crossings()
{
    for (int i=0;i<number_of_crossings;i++)
    {
        cout<<"CROSSING :"<<i<<endl;
        cout << crossings[i].crossing_phone_no << " " << crossings[i].crossing_id << " "<< crossings[i].latitude << " " << crossings[i].longitude << "\n";
        for (int j=0;j<crossings[i].local_geographic_data.size();j++)
            print_debug(crossings[i].local_geographic_data[j]);
    }
}

void print_message_received(message_received msg)
{
    cout << "ts = "<<msg.timestamp << " latitude = " << msg.train_data.latitude << " longitude = " << msg.train_data.longitude << " phone number =  " << msg.train_data.train_phone_no << "\n";
}
message_received convert_to_message(string in)
{
    int trainId, time;char train_phone[20];
    float latitude, longitude, velocity; // note that velocity is in knots
    sscanf(in.c_str(), "T %d.000 %f N %f E %f %s %d", &time, &latitude, &longitude, &velocity, train_phone, &trainId);
    train_info arduino_message;
    arduino_message.latitude = latitude;
    arduino_message.longitude = longitude;
    arduino_message.velocity = 1.852*velocity; // factor of 1.852 to convert knots to kmph
    arduino_message.trainId = trainId;
    arduino_message.train_phone_no = (string)train_phone;
    arduino_message.time_last_updated=-1;
    message_received msg;
    msg.timestamp=time;
    msg.train_data = arduino_message;
    return msg;
}

string convert_to_string(crossing_info msg)
{
    string a;
    a="S "+msg.crossing_phone_no;
    return a;
}

string convert_to_string(train_info msg)
{
    string b;
    b="S "+msg.train_phone_no;
    return b;
}

void sendMsg_to_train(crossing_info crossing,string train_Phone_No,int USB)
{
    message_sent_to_train msg;
    msg.crossing=crossing;
    string a=train_Phone_No+"\""+ convert_to_string(crossing)+"\n\0";char ans[128];
    for (int i=0;i<a.length();i++)
    {
        ans[i]=a[i];
    }
    ans[a.length()]='\0';
    writeUSB(USB,ans,a.length()+1);
    
}

void sendMsg_to_crossing(train_info traindata,string crossing_Phone_No, int USB,bool locator)
{
    message_sent_to_crossing msg;
    msg.traindata=traindata;
    
    string locator_string;
    if (locator)
        locator_string = " 1";
    else
        locator_string = " 0";
    
    string a=crossing_Phone_No+"\""+convert_to_string(traindata)+ locator_string +"\n\0";char ans[128];
    for (int i=0;i<a.length();i++)
    {
        ans[i]=a[i];
    }
    ans[a.length()]='\0';
    cout<<"MESSAGE WRITTEN :"<<ans<<" : "<<a.length()+1<<"\n";
    writeUSB(USB,ans,a.length()+1);
    
}

void parser_crossing_points_data(string filename,vector<crossing_data_point_info> &crossing_complete_data)
{
    string no_records;string crossing_phone_no;string crossing_id;
    ifstream fin(filename);
    
    fin>>crossing_phone_no>>crossing_id>>no_records;
    int p=stoi(no_records);
    for (int i=0;i<p;i++)
    {
        crossing_data_point_info cinfo;
        string a;
        fin>>a;
        cinfo.latitude=stof(a);
        fin>>a;
        cinfo.longitude=stof(a);
        fin>>a;
        cinfo.distance=stoi(a);
        fin>>a;
        cinfo.to_right=stoi(a);
        crossing_complete_data.push_back(cinfo);
    }
    
}
void complete_transfer_info()
{
    readCrossingFile(crossingfilename);
    for (int i=0;i<number_of_crossings;i++)
    {
        string filename = string(FILE_DIRECTORY) + "c" + to_string(i)+".txt";
        parser_crossing_points_data(filename,crossings[i].local_geographic_data);
    }
    
}
struct minner
{
    int min;
    bool locator;
    
};
minner distance_database(float latitude,float longitude,crossing_info crossing)
{
    int min=MIN_DISTANCE_TO_CHECK_IN_DATABASE;
    bool locator = 1;
    int indexer = 0;
    for (int i=0;i<crossing.local_geographic_data.size();i++)
    {
        int distance = eucledian_distance(latitude, longitude, crossing.local_geographic_data[i].latitude, crossing.local_geographic_data[i].longitude);
        if (distance < min)
        {
            indexer =i;
            min=distance;
            locator = crossing.local_geographic_data[i].to_right;
            
        }
    }
    minner a;
    a.min = crossing.local_geographic_data[indexer].distance + min-20;
    a.locator = locator;
    return a;
}

void update_ui(string status, string train_lat, string train_long)
{
    // currently only for one train. File name at UI is a.txt.
    // modify for more trains!!!
    ofstream fs;
    fs.open("a.txt", ofstream::trunc);
    if (!fs)
    {
        cout << "ERROR: UI File not found";
        return;
    }
    string write = status+","+train_lat+","+train_long;
    fs << write;
    fs.close();
}

void update_and_check(message_received msg,int USB)
{
    int trainid=msg.train_data.trainId;
    trainid = mymap.find(trainid)->second;
    WriteTrainData(msg);
    if (latest_info[trainid].time_last_updated < msg.timestamp || (latest_info[trainid].time_last_updated - msg.timestamp)>230000)
        // make sure that comparision of timestamp is working correctly
    {
        latest_info[trainid].latitude=msg.train_data.latitude;
        latest_info[trainid].longitude=msg.train_data.longitude;
        latest_info[trainid].velocity=msg.train_data.velocity;
        latest_info[trainid].time_last_updated=msg.timestamp;
        ///string crossing_light_status = "0";
        for (int i=0;i<number_of_crossings;i++)
        {
            
            cout << "Distance is = "<<eucledian_distance(latest_info[trainid].latitude,latest_info[trainid].longitude,crossings[i].latitude,crossings[i].longitude) << "\n";
            
            if (eucledian_distance(latest_info[trainid].latitude,latest_info[trainid].longitude,crossings[i].latitude,crossings[i].longitude) < MIN_DISTANCE_TO_CHECK_IN_DATABASE)
            {
                cout<<"ENTERED STATE WHERE WE ARE CHECKING IN DATABASE"<<endl;
                cout<<"TRAIN PHONE NO :"<<msg.train_data.train_phone_no<<endl;
                cout<<"CROSSING PHONE NO:"<<crossings[i].crossing_phone_no<<endl;
                cout<<"Eucledian Distance from crossing is :"<<eucledian_distance(latest_info[trainid].latitude,latest_info[trainid].longitude,crossings[i].latitude,crossings[i].longitude);
                int dis=distance_database(latest_info[trainid].latitude,latest_info[trainid].longitude,crossings[i]).min;
                bool locator = distance_database(latest_info[trainid].latitude,latest_info[trainid].longitude,crossings[i]).locator;
                if (dis< MIN_DISTANCE)
                {
                    cout<<"DISTANCE FROM CROSSING AT WHICH CONNECTION ESTABLISHED :"<<dis<<endl;
                    sendMsg_to_train(crossings[i],msg.train_data.train_phone_no,USB);
                    
                    flush(USB2);
                    close(USB2);
                    sleep(4);
                    USB2=initializeUSB("/dev/tty.usbmodem1421\0");
                    
                    sendMsg_to_crossing(msg.train_data,crossings[i].crossing_phone_no,USB,locator);
                    ///crossing_light_status = "1";
                }
                
            }
        }
        ///update_ui(crossing_light_status, to_string(msg.train_data.latitude), to_string(msg.train_data.longitude));
    }
}


//message_received msg_rec()
//{
//    string MSG;
//    //INSERT CODE HERE TO RECEIVE STRING;
//    return convert_to_message(MSG);
//}

//void receive_message()
//{
//    message_received msg;
//    msg=msg_rec();
//    update_and_check(msg);
//
//}





int main()
{
 ////'   fileopener();
    complete_transfer_info();
    print_crossings();
    
    int USB1=initializeUSB("/dev/tty.usbmodem1411\0"); // receive sms
    USB2=initializeUSB("/dev/tty.usbmodem1421\0");
    //
    while(1)
    {
        string s = readUSB(USB1);
        if(s[0] == 'T')
        {
            if(s.length() > 0)
            {
                message_received msg = convert_to_message(s);
                print_message_received(msg);
                update_and_check(msg,USB2);
            }
        }
        else
        {
            cout << " Message received from somewhere else. ERROR. " << s << endl;
        }
    }
}

//print_crossings();
//initiate_train_communication();
//    initiate_train_present_position();
//    while(1)
//    {
//       receive_message();
//    }
//
//    cout<<cos(1.57);
//}

