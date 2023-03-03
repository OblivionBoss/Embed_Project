#define INPUT_PIN 23
#define OUTPUT_PIN 24

#include <WiFi.h>
#include <ThingsBoard.h>

const char ssid[] = "@JumboPlusIot"; 
const char password[] = "g10embed";
//const char ssid[] = "vivo13055";
//const char password[] = "Boss26052546";

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
      Serial.println("Connect mqtt...");
      return;
    }
  }
  double pulse = 2.00;
  tb.sendTelemetryFloat("pulse", pulse); //<- send value to thingboard

  tb.loop();
}

void initWiFi(){
  Serial.println("Wifi connecting ...");
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n MQTT connecting ...");
  tb.connect(mqtt_broker, mqtt_Token,MQTT_PORT);
  while(!tb.connected()){
    delay(500);
    Serial.print(".");
  }

}

void reconnected(){
    status = WiFi.status();
    if(status != WL_CONNECTED){
      WiFi.begin(ssid,password);
      while(WiFi.status() != WL_CONNECTED){
        Serial.println("Reconnect...");
        delay(500);
      }
    }
}


