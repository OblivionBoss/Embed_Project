#define INPUT_PIN 89
#define OUTPUT_PIN 88

#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "";
const char password[] = "";

const char mqtt_broker[] = "thingsboard.cloud";
const char mqtt_topic[] = "v1/devices/me/telemetry";
const char mqtt_client_id = "";
int MQTT_PORT = 1883;

WiFiClient net;
MQTTClient client;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(mqtt_client_id)) {  
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe(mqtt_topic);
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void IRAM_ATTR Mount(){

}

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);


  client.begin(mqtt_broker, MQTT_PORT, net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

}

void sendMassege(){
  client.publish(mqtt_topic, "{"); //<= form it's like JSON body
}
