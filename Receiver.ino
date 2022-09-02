/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
//#define BLYNK_TEMPLATE_ID   "YourTemplateID"
#define BLYNK_TEMPLATE_ID "TMPLdngwx7UL"
#define BLYNK_DEVICE_NAME "mesh"
#define BLYNK_AUTH_TOKEN "vZnoNH0qyCdUKWhoXleTP1Cs4w56vcRt"


#include <SPI.h>              // include libraries
#include <LoRa.h>


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define ss 15  //GPIO 15
#define rst 16  //GPIO 16
#define dio0 2  //GPIO 2

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "iPhone x";
char pass[] = "nopassword";


byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;
byte Node3 = 0xDD;

String SenderNode = "";
String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
String incoming = "";

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 1 ; // updated every 1 second
int Secs = 0;
String t1, t2, t3, h1, h2, h3;

void setup() {
  Serial.begin(115200);                   // initialize serial
  Blynk.begin(auth, ssid, pass);
  while (!Serial);

  Serial.println("LoRa Master Node");

  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {

  Blynk.run();
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    Secs = Secs + 1;
    //Serial.println(Secs);
    if ( Secs >= 16 )
    {
      Secs = 0;
    }
    if ( (Secs >= 1) && (Secs <= 5) )
    {

      String message = "10";
      sendMessage(message, MasterNode, Node1);
    }

    if ( (Secs >= 6 ) && (Secs <= 10))
    {

      String message = "20";
      sendMessage(message, MasterNode, Node2);
    }
    if ((Secs >=10) && (Secs <= 15))
    {
      String message = "30";
      sendMessage(message, MasterNode, Node3);
    }

    previoussecs = currentsecs;
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());

}


void sendMessage(String outgoing, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(otherNode);              // add destination address
  LoRa.write(MasterNode);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  if ( sender == 0XBB )
    SenderNode = "Node1:";
  if ( sender == 0XCC )
    SenderNode = "Node2:";
  if( sender == 0xDD)
  SenderNode = "Node3:";
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length


  while (LoRa.available()) {
    incoming = LoRa.readStringUntil('\n');
    Serial.print("Incoming Data: ");
    Serial.println(incoming);
  }

  if (incomingLength != incoming.length()) {   // check length for error
    //Serial.println("error: message length does not match length");
    ;
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    // Serial.println("This message is not for me.");
    ;
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  //Serial.println("Received from: 0x" + String(sender, HEX));
  //Serial.println("Sent to: 0x" + String(recipient, HEX));
  //Serial.println("Message ID: " + String(incomingMsgId));
  // Serial.println("Message length: " + String(incomingLength));
  // Serial.println("Message: " + incoming);
  //Serial.println("RSSI: " + String(LoRa.packetRssi()));
  // Serial.println("Snr: " + String(LoRa.packetSnr()));
  // Serial.println();

  if ( sender == 0XBB)
  {
    String t1 = getValue(incoming, ',', 0); // Temperature
    String h1 = getValue(incoming, ',', 1); // Humidity
    Blynk.virtualWrite(V0, t1);
    Blynk.virtualWrite(V1, h1);
    //    t1 = t.toInt();
    //    h1 = h.toInt();
    Serial.println("Node 1 :");
    Serial.print(t1 + "\t");
    Serial.println(h1 + "\t");
    incoming = "";

  }

  if ( sender == 0XCC )
  {


    String t2 = getValue(incoming, ',', 0); // Temperature
    String h2 = getValue(incoming, ',', 1); // Humidity
    Blynk.virtualWrite(V2, t2);
    Blynk.virtualWrite(V3, h2);
    //    t2 = t.toInt();
    //    h2 = h.toInt();
    Serial.println("Node 2 :");
    Serial.print(t2 + "\t");
    Serial.println(h2 + "\t");

    incoming = "";

  }
  
  if ( sender == 0XDD )
  {
    String t3 = getValue(incoming, ',', 0); // Temperature
    String h3 = getValue(incoming, ',', 1); // Humidity
    Blynk.virtualWrite(V4, t3);
    Blynk.virtualWrite(V5, h3);
    //    t1 = t.toInt();
    //    h1 = h.toInt();
    Serial.println("Node 3 :");
    Serial.print(t3 + "\t");
    Serial.println(h3 + "\t");
    incoming = "";

  }
}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
