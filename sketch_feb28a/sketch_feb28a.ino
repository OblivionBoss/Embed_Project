#define INPUT_PIN 89
#define OUTPUT_PIN 88

#include <WiFi.h>
#include <ThingsBoard.h>

const char ssid[] = ""; 
const char password[] = "";

const char mqtt_broker[] = "thingsboard.cloud";
const char mqtt_Token[] = "7aKzt3N0249c1ZVtlQfs";
const char mqtt_topic[] = "v1/devices/me/telemetry";
// const char mqtt_client_id = "";
int MQTT_PORT = 1883;
int status = WL_IDLE_STATUS;

WiFiClient net;
// MQTTClient client;
ThingsBoard tb(net);

void connect() {
  Serial.print("checking wifi...");
  WiFi.begin(ssid,password);
  initWiFi();
  // client.unsubscribe("/hello");
}

void IRAM_ATTR Mount(){

}

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);

  connect();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);  // <- fixes some issues with WiFi stability

  if (WiFi.status() != WL_CONNECTED) {
    reconnected();
    return;
  }

  if(!tb.connected()){
    if(!tb.connect(mqtt_broker,mqtt_Token,1883)){
      return;
    }
  }

  tb.sendTelemetryFloat("pulse", 2); //<- send value to thingboard

  tb.loop();
}

void initWiFi(){
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
}

void reconnected(){
    status = WiFi.status();
    if(status != WL_CONNECTED){
      WiFi.begin(ssid,password);
      while(WiFi.status() != WL_CONNECTED){
        delay(500);
      }
    }
}


