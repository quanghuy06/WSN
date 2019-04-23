//*******************************************************
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <string.h>
#include <ArduinoJson.h>

#define DHTPIN 5            // Chân dữ liệu của DHT11 kết nối với GPIO4 của ESP8266
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE, 15);
const char* ssid = "Quang Huy";      // Tên mạng Wifi được chỉ định sẽ kết nối (SSID)
const char* password = "quanghuy061296";  // Password của mạng Wifi được chỉ định sẽ kết nối
const char* mqtt_server = "172.20.10.13"; 

const char* topic1 = "Button";  
const char* topic2 = "DHT11"; 
const char* topic3 = "Range";
//******************************************************

WiFiClient espClient;
PubSubClient client(espClient);

//******************************************************
byte outPin = 2;
int led = 14;

void setup() {
  pinMode(outPin,OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(outPin,LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);    // clien.setServer(mqttServer, port);
  client.setCallback(callback);           // callback
  reconnect();
}

//*****************************************************

void setup_wifi(){

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//******************************************************

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String value;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    value += (char)payload[i];
  }

  /*--------------------------Control-------------------------------------*/
  if(strcmp(topic, topic1)==0)
  {
    if((char)payload[0] == 'O' && (char)payload[1] == 'N') //on
    {
      digitalWrite(outPin,LOW);
      Serial.print("Subed ON");
    }
    else if((char)payload[0] == 'O' && (char)payload[1] == 'F' && (char)payload[2] == 'F') //off
    {
      digitalWrite(outPin,HIGH);
      Serial.print("Subed OFF");
    }
  }
  if (String(topic) == "Range") {
      Serial.print("setup led1 pwm ");
      Serial.println(value.toInt());
      analogWrite(led,value.toInt());
  }
  Serial.println();
  /*-----------------------------------------------------------------*/
}

//**************************************************************************//

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Huy","huytq","Quanghuy@123")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("ESP8266/connection status", "Connected!");
      // ... and resubscribe1
      client.subscribe(topic1);
      client.subscribe(topic3);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void loop() {
  char message_DHT[100];
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("Temperature : ");
  Serial.print(temp);
  Serial.print("Humidity : ");
  Serial.println(humi);
  message_DHT[0]= '\0';
  String tmp = "{\"temp\": " + String(temp) + ",\"hum\": " + String(humi) + "}";
  strcat(message_DHT, String(tmp).c_str());
  Serial.println(message_DHT);
  if(String(temp) != "nan"){
    client.publish( topic2, message_DHT);
    Serial.println("published");
  }
  delay(1000);
  if (!client.connected()) {  //nếu chưa kết nối đến sever mqtt
    reconnect();     // gọi hàm reconnect() để thực hiện kết nối lại với server khi bị mất kết nối
  }
  client.loop();
}
