/******************************************************************************
MQTT_Switch_Example.ino
Example for controlling a light using an MQTT switch
by: Alex Wende, SparkFun Electronics

This sketch connects the ESP32 to a MQTT broker and subcribes to the topic
room/light. When the button is pressed, the client will toggle between
publishing "on" and "off".
******************************************************************************/

#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid =  "MIWIFI_2G_rsYk";   
const char *password =  "3q6pywysfkju"; 

const byte SWITCH_PIN = 0;         // Pin to control the light with
const char *ID = "myExampleSwitch";  // Name of our device, must be unique
const char *TOPIC = "room/light";  // Topic to subcribe to

//IPAddress broker(192,168,1,139); 
IPAddress broker(5,196,95,208); //test.mosquitto.org
WiFiClient wclient;

PubSubClient client(wclient); // Setup MQTT client
bool state=0;

// Connect to WiFi network
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to network

  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }

  //Serial.println();
  Serial.print("\nWiFi connected with IP addr: ");
  Serial.println(WiFi.localIP());
}

void brokerReconnect() {
  while (!client.connected()) {// Loop until we're reconnected
    Serial.print("Attempting MQTT connection to broker...");
    if (client.connect(ID)) {    // Attempt to connect
      Serial.println("connected!");
      Serial.print("Publishing to: ");
      Serial.println(TOPIC);
    } 
    else {
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  pinMode(SWITCH_PIN,INPUT);  // Configure SWITCH_Pin as an input
  digitalWrite(SWITCH_PIN,HIGH);  // enable pull-up resistor (active low)
  delay(100);
  setup_wifi(); 
  client.setServer("test.mosquitto.org", 1883);
}

void loop() {
  if (!client.connected()){  // Reconnect if connection is lost
    brokerReconnect();
  }
  client.loop();

  // if the switch is being pressed
  if(digitalRead(SWITCH_PIN) == 0) {
    state = !state; //toggle state
    if(state == 1){ // ON
      iint st=client.publish(TOPIC, "ON");
      if (st==true)
        Serial.println((String)TOPIC + " => ON");
      else
        Serial.println((String)TOPIC + " => ON ERROR");
    }
    else  {
      client.publish(TOPIC, "OFF");
      Serial.println((String)TOPIC + " => OFF");
    }

    while(digitalRead(SWITCH_PIN) == 0){ // Wait for switch to be released
      // Let the ESP handle some behind the scenes stuff if it needs to
      yield(); 
      delay(20);
    }
  }
}
