#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>

using namespace std;

# define TERM_CHAR '@'

pthread_t threads[1];

/* Allocate memory for read buffer */
char buf [128];
char inp[128];

void readUSB(int USB)
{
    /* *** READ *** */
    long n;
    for(int i=0,j=0;;)
    {
        memset (&buf, '\0', sizeof buf);
        //memset (&inp, '\0', sizeof inp);
        n= read(USB, &buf , sizeof buf );
        if(n>0)
        {
            for(j=0;j<n-1;j++)
                inp[i++]=buf[j];
            if(buf[n-1]==TERM_CHAR)
            {
                inp[i]='\0';
                break;
            }
            inp[i++]=buf[j];
        }
    }
    /* Error Handling */
    if (n < 0)
    {
        cout << "Error reading : " << strerror(errno) << endl;
    }
    /* Print what I read... */
    cout << "Read: " << inp << endl;
}
void *readUSBCont(void *USB)
{
    while(1)
        readUSB(*(int*)USB);
}
void writeUSB(int USB,char str[],int size)
{
    /* *** WRITE *** */
    //char str1[]= {'9','7','1','7','4','7','1','1','7','5','"','I','N','I','T','I','A','L','L','Y',' ','O','K','\n','\0'};
    long n=write( USB, str, size);
    cout<<"N : "<<n<<"\n";
    if(n!=size)
        cout<<"ERROR : Can not write string\n";
    cout<<"DONE : "<<size<<"\n";
}
void flush(int USB)
{
    sleep(2); //required to make flush work, for some reason
    tcflush(USB, TCIOFLUSH);
}
int initializeUSB(char *USBFile)
{
    /* Open File Descriptor */
    int USB = open( USBFile, O_RDWR| O_NONBLOCK | O_NDELAY );
    
    /* Error Handling */
    if (USB<0)
    {
        cout << "Error " << errno << " opening " << USBFile << ": " << strerror (errno) << endl;
    }
    
    /* *** Configure Port *** */
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    
    /* Error Handling */
    if ( tcgetattr ( USB, &tty ) != 0 )
    {
        cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
    }
    
    /* Set Baud Rate */
    cfsetospeed (&tty, B9600);
    cfsetispeed (&tty, B9600);
    
    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;        // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_cflag     &=  ~CRTSCTS;       // no flow control
    tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
    tty.c_oflag     =   0;                  // no remapping, no delays
    tty.c_cc[VMIN]      =   0;                  // read doesn't block
    tty.c_cc[VTIME]     =   0;                  // 0.5 seconds read timeout
    
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
    tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
    tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    tty.c_oflag     &=  ~OPOST;              // make raw
    
    /* Flush Port, then applies attributes */
    flush(USB);
    
    if ( tcsetattr ( USB, TCSANOW, &tty ) != 0)
    {
        cout << "Error " << errno << " from tcsetattr" << endl;
    }
    
    //writeUSB(USB);
    
    //while(1)
      //  readUSB(USB);
    
    return USB;
    
    //close(USB);
}
int main()
{
    int USB1=initializeUSB("/dev/tty.usbmodem1411\0"); // receive sms
    int USB2=initializeUSB("/dev/tty.usbmodem1421\0");
    pthread_create(&threads[0],NULL,readUSBCont,&USB1); //receive sms
    char str[]= {'9','7','1','7','4','7','1','1','7','5','"','I','N','I','T','I','A','L','L','Y',' ','O','K','\n','\0'};
    
    writeUSB(USB2,str,sizeof(str));
    while(1);
    //writeUSB(USB2,"Done\n\0");
    return 0;
}
