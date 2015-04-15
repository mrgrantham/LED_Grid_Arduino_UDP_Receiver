#include "SPI.h"
#include "Adafruit_WS2801.h"
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <TimerOne.h>

int dataPin  = 2;    // White wire on my Grid
int clockPin = 3;    // Green wire on my Grid

const int nLEDs = 49;

Adafruit_WS2801 strip = Adafruit_WS2801(49, dataPin, clockPin);

bool newMessage = false;
int rMessage;
int gMessage;
int bMessage;
int iMessage;

// Network Setup

byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

unsigned int localPort = 8888;      // local port to listen on

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char remoteIPAddy[50];
char addyPart[8];

void setup() {

    // start the Ethernet and UDP:
  Ethernet.begin(mac);
  Udp.begin(localPort);
  
  Serial.begin(9600);
  
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    sprintf(addyPart,"%d.",Ethernet.localIP()[thisByte]);
    sprintf(remoteIPAddy,"%s%s",remoteIPAddy,addyPart);

    
  // Timer1 is used so the strip will update at a known fixed frame rate.
  // Each effect rendering function varies in processing complexity, so
  // the timer allows smooth transitions between effects (otherwise the
  // effects and transitions would jump around in speed...not attractive).
  Timer1.initialize();
  Timer1.attachInterrupt(callback, 1000000 / 60); // 60 = 60 frames/second
  
    strip.begin();
    strip.show();
  }
  Serial.print(remoteIPAddy); 
}

void loop() {

    // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
  // CLear the packet buffer of last message
    memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
    
    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.println(packetBuffer);

    String message = String(packetBuffer);


    iMessage = message.substring(1,4).toInt();
    rMessage = message.substring(5,8).toInt();
    gMessage = message.substring(9,12).toInt();
    bMessage = message.substring(13,16).toInt();

    Serial.println(iMessage);
    Serial.println(rMessage);
    Serial.println(gMessage);
    Serial.println(bMessage);
    
      }
  delay(10);
    
}

// Timer1 interrupt handler.  Called at equal intervals; 60 Hz by default.
void callback() {
  // Very first thing here is to issue the strip data generated from the
  // *previous* callback.  It's done this way on purpose because show() is
  // roughly constant-time, so the refresh will always occur on a uniform
  // beat with respect to the Timer1 interrupt.  The various effects
  // rendering and compositing code is not constant-time, and that
  // unevenness would be apparent if show() were called at the end.
  strip.show();

  strip.setPixelColor(iMessage, rMessage, gMessage, bMessage);

}
