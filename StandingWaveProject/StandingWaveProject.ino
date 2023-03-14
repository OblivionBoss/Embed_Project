//How its work 
//  When start it will be immoble, but if you press FSR it will change value and 
//  display appropiate vertical standing wave
//  On MQTT, we display the difference between the graph of horizontal standing wave
//  and vertical standing wave (graph of vertical standing wave at 3.3V)

//Presentation script
//  Background (Horizontal Standing wave is common but not vertical) & Objective (Simulated vertical standing wave)
//  Idea (Cowboy hoop & certain youtube video) & Scientific information
//  Description of project
//    - Overview (This thing designed to display vertical standing wave of the string/rope of N wave ; N = 1,2,3,4,5 )
//               <At this point demo the project to the instructor of how it work also>
//    - Hardware (3-6V DC Motor, L293D H-bridge IC, DF9-40 FSR [Force sensing resistor], 12V 2A DC adapter)
//    - Software (Use PWM to control output of DC Motor, MQTT to send value which shown differnce between vertical and horizontal standing wave)
//      **Note : Used to have LCD display and more wave apply (See more details down below)
//             : The graph of Actual Wave and Actual Frequency is based on the condition which DC Motor have Voltage input at 5.21V
//  Problem
//    - L293D Overheat -> Current Overload
//    - Only 2 Standing Wave available -> DC Motor dysfunction -> Insuffient Voltage to DC Motor -> Hypothize to be power loss between line
//    - LCD malfunction -> Hypothize to be Libary error
#include <WiFi.h>
#include <ThingsBoard.h>
#include <math.h>
//#include <LCD_I2C.h>
//LCD_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules, change according

// the number of the pin
#define FirstPin 26  // 26 corresponds to GPIO26
#define SecondPin 25 // 25 corresponds to GPIO25
#define EN 33        // 33 corresponds to GPIO33
#define fsrPin 34    // the FSR and 3.3K pulldown are connected to 34

// setting PWM properties
const int freq = 1000;
const int ledChannel = 0;
const int resolution = 8;
const double tension = 0.051975245; // N
const double linearDensity = 0.007; // fg/m
const double Const = sqrt(tension/linearDensity);

//const char ssid[] = "@JumboPlusIot"; 
//const char password[] = "g10embed";
const char ssid[] = "vivo13055";
const char password[] = "Boss26052546";
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

//Initial value
int NWave = 0;
int fsrReading = 0;     // the analog reading from the FSR resistor divider
bool lastRead = false;
int WavePWM[] = {0, 160, 210, 255}; // f_n = 0.1435n^3 − 0.6465n^2 + 1.3650n + 4.2940
double sum = 0;
double count = 0;
//double avgA;
//double Force; //Maximum force per maximum analog value
//double PWM;
//double MotorFreq;   // Send to MQTT 
//double Wave;        // Send to MQTT
//double ActWave;
//double ActMotorFreq;        // Send to MQTT
//String ArrLine[] = {"", "", "", ""}; 

void setup(){
  Serial.begin(115200);
  pinMode(fsrPin, INPUT);
  
  pinMode(FirstPin, OUTPUT);
  pinMode(SecondPin, OUTPUT);
  ledcAttachPin(EN, ledChannel);
  ledcSetup(ledChannel, freq, resolution);

  //attachInterrupt(fsrPin, SENSOR, CHANGE); // interrupt on rising edge
  // If you are using more I2C devices using the Wire library use lcd.begin(false)
  // this stop the library(LCD_I2C) from calling Wire.begin()
  //lcd.begin();    
  //lcd.backlight();
  connect();
  
  //ledcWrite(ledChannel, 176); //EN with PWM
  // n | 1   2   3   4 
  // r |        176 192
} // 15|144 156 180 240 

void loop(void){
  delay(16);
  
  //if (WiFi.status() != WL_CONNECTED) {
  //  reconnected();
  //  return;
  //}

  //if(!tb.connected()){
  //  if(!tb.connect(mqtt_broker,mqtt_Token,1883)){
  //    Serial.println("Connect mqtt...");
  //    return;
  //  }
  //}
  fsrReading = analogRead(fsrPin);
  //Serial.println(fsrReading);
  //while(fsrReading == 0) GSW(WavePWM[NWave]); 
  if(fsrReading != 0){
    
    if(!lastRead){
      Serial.println("===========================================");
      Serial.println("Touch detected!");
    } 
    //DetForce(fsrReading);
    lastRead = true;
    sum += fsrReading;
    count++;    
  }else{
    if(lastRead){
    double avgF = sum/count;
    tb.sendTelemetryFloat("FreqCal", 55*(avgF/16)/1536); //<- send value to thingboard
    tb.sendTelemetryFloat("WaveCal", 55*(avgF/16)/1536/Const); //<- send value to thingboard
    tb.sendTelemetryFloat("FreqAct", (8.3076*pow(10, -6)*pow(avgF/16, 3)) - (0.0051*pow(avgF/16, 2))+(1.05*avgF/16) - 69.2423); //<- send value to thingboard
    tb.sendTelemetryFloat("WaveAct", ((8.3076*pow(10, -6)*pow(avgF/16, 3)) - (0.0051*pow(avgF/16, 2))+(1.05*avgF/16) - 69.2423)*Const); //<- send value to thingboard
    Serial.println("===========================================");
    Serial.print("Average ");
    NWave = DetForce(avgF);
    Serial.print("Display ");
    Serial.print(NWave);
    Serial.println(" Wave");
    Serial.println("===========================================");
    sum = 0;
    count = 0;
    lastRead = false;
    }
    else{
      //Serial.println(NWave);
      GSW(WavePWM[NWave]);
    } 
  }
  //if(fsrReading != 0) GSW(WavePWM[NWave]);
  delay(16);
  
  
  //tb.loop();
}

//Generate Standing Wave
void GSW(int dutyCycle){
  hbridge(true, dutyCycle);
  delay(5000);
  hbridge(true, 0);

}

//Determine the input force and return number of wave
int DetForce(double fsrReading){//, String Arr[]){
  //String str[sizeof(Arr) / sizeof(Arr[0])] = Arr;
  Serial.print("Reading = ");
  Serial.print(fsrReading);     // the raw analog reading
  // We'll have a few threshholds, qualitatively determined
  if (fsrReading <= 819) {
    Serial.println(" - Light touch");
    //GSW(WavePWM[1]);
  //  str[3] = "  Light Touch   ";
  //  str[4] = " Display 1 wave ";
    //LCDdisplay("  Light Touch   "," Display 1 wave ");
    return 1;
  } else if (fsrReading <= 1638) {
    Serial.println(" - Light press");
    //GSW(WavePWM[2]);
  //  str[3] = "  Light Press   ";
  //  str[4] = " Display 2 wave ";
    //LCDdisplay("  Light Press   "," Display 2 wave ");
    return 2;
  } else if (fsrReading <= 2457) {
    Serial.println(" - Medium press");
    //GSW(WavePWM[3]);
  //  str[3] = "  Medium Press  ";
  //  str[4] = " Display 3 wave ";
    //LCDdisplay("  Medium Press  "," Display 3 wave ");
    return 3;
  } else {
    Serial.println(" - Heavy press");
    //GSW(WavePWM[4]);
  //  str[3] = "  Heavy Press   ";
  //  str[4] = " Display 4 wave ";
    //LCDdisplay("  Heavy Press   "," Display 4 wave ");
    return 4;
  }
  //Multidisplay(str);
}

//H-bridge control with L239D
void hbridge(bool direction, int dutyCycle){
  ledcWrite(ledChannel, dutyCycle); //EN with PWM
  if(dutyCycle > 0){
    if(direction){
      digitalWrite(FirstPin, HIGH);
      digitalWrite(SecondPin, LOW);
    }else{
      digitalWrite(FirstPin, LOW);
      digitalWrite(SecondPin, HIGH);
    }    
  }else{
    digitalWrite(FirstPin, LOW);
    digitalWrite(SecondPin, LOW);    
  }
}

//Display message on LCD screen
//void Multidisplay(String Arr[]){
//  String arr[sizeof(Arr) / sizeof(Arr[0])] = Arr;
//  int size = sizeof(arr) / sizeof(arr[0]);
//  if(size % 2 != 0){    
//    arr[size+1] = "                ";
//    size++;
//  }
//  for(int i = 0; i < size; i += 2){
//    if(i == size) i = 0;    
//    LCDdisplay(arr[i], arr[i+1]);
//    delay(3000);
//  }   
//}

//Display message on LCD screen
//void LCDdisplay(String FirstLine, String SecondLine){
//  lcd.clear();
//  lcd.setCursor(0, 0); 
//  lcd.print(FirstLine); // You can make spaces using well... spaces
//  lcd.setCursor(0, 1);  // Or setting the cursor in the desired position.
//  lcd.print(SecondLine);
//}

//String NumToString(double Number, int DecimalPlace) {
//  char buffer[20];
//  dtostrf(Number, 0, DecimalPlace, buffer);
//  return String(buffer);
//}

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
  Serial.print("\n");
  Serial.println("Connected");
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