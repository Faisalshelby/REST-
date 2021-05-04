#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include<ArduinoJson.h>
#include <FirebaseESP8266.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
/* for normal hardware wire use above */
#define FIREBASE_HOST "https://esp8266-ddf68-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "9h9AzzrtqPSy1lK7QFF17edWPsZjCVe5k6lfuCyu"
#define WIFI_SSID "oneplus"
#define WIFI_PASSWORD "12345678"
const int trigger = 14;
const int echo = 12;
const int OUTPUT1 = 2;
const int OUTPUT2 = 0;
long int lts = 0;
long int lt = 0;
int duration;
float distance;
char post_data[100];
String id = "sensor 1";
int prevstate = LOW;
String data;
const char* ssid = "oneplus";
const char* password = "12345678";
String host = "http://192.168.4.1/feed?";
FirebaseData firebaseData;
WiFiClient client;
HTTPClient http;
void setup ()
{
  Serial.begin(115200);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  ESP.eraseConfig();
  WiFi.persistent(false);
  Serial.println("SET ESP IN STA MODE");
  WiFi.mode(WIFI_STA);
  Serial.println("- connecting to wifi");
  WiFi.begin(ssid, password);
  Serial.println(".");
  Serial.println("- wifi connected");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Rtc.Begin();
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

  //
  Serial.println("Serial communication started\n\n");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);



  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }


  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);   // connect to firebase

  Firebase.reconnectWiFi(true);
  delay(1000);
}

void loop ()

{
  RtcDateTime now = Rtc.GetDateTime();
  Serial.print("Time: ");
  printDateTime(now);
  //Serial.println();

  RtcTemperature temp = Rtc.GetTemperature();
  ret_temperature(temp);
  Serial.print("\t");
  Serial.print("Temperature: ");
  //  temp.Print(Serial);
  //  Serial.print("C");

  readdata();
  Serial.print("\t"); Serial.print("Distance: "); Serial.println(distance);
  //functions for sensors and motor
  // pump();


  //  if (Firebase.setInt(firebaseData, "/distance", distance)) {
  //    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  //    delay(500);
  //    digitalWrite(LED_BUILTIN, HIGH);
  //    delay(1000);
  //
  //  }
  //
  //  else {
  //    Serial.println(firebaseData.errorReason());
  //  }
  //  if (Firebase.setInt(firebaseData, "/temp", ret_temperature(temp)) {
  //  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  //    delay(500);
  //    digitalWrite(LED_BUILTIN, HIGH);
  //    delay(1000);
  //
  //  }
  //
  //  else {
  //    Serial.println(firebaseData.errorReason());
  //  }
  //
  //  delay(5000);
  if (distance < 5) {
    digitalWrite(OUTPUT2, HIGH);
    digitalWrite(OUTPUT1, HIGH);

  }
  else {
    digitalWrite(OUTPUT1, LOW);
    digitalWrite(OUTPUT2, LOW);
  }

}


float ret_temperature(const RtcTemperature& temvips) {
  Serial.print(temvips.AsFloatDegC());


}
#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u:%02u:%02u"),
             //dt.Month(),
             //dt.Day(),
             //dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
}




//%02u/%02u/%04u


void readdata() {
  while (lts - millis() > 10000) {
    digitalWrite(trigger, HIGH);
    delayMicroseconds(2);
    digitalWrite(trigger, LOW);
    duration = pulseIn(echo, HIGH);
    distance = duration * 0.034 / 2;
    String Distance = String(distance);
    getPostJson();
    http.begin(host);
    http.addHeader("content-type", "application/x-www-form-urlencoded");
    http.POST(post_data);
    // http.POST(id);
    http.writeToStream(&Serial);
    http.end();

    lts = millis();
  }

}
