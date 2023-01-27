#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <TM1637Display.h>

// Define the connections pins
#define CLK 14  // 14 - D5
#define DIO 12  // 12 - D6

// Create a display object of type TM1637Display
TM1637Display display = TM1637Display(CLK, DIO);

const char* ssid = "LIVE TIM_B884_2G";
const char* password = "mxu64cj6cf";
const int button = 4;  //D2(gpio4)

//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.1.19:8000/ping";
int people_counted = 0;

void setup() {
  display.setBrightness(7);
  Serial.begin(9600);

  pinMode(button, INPUT_PULLUP);

  people_counted = 0;

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  display.showNumberDec(people_counted);
}

void send_http_signal() {
  WiFiClient client;
  HTTPClient http;

  String serverPath = serverName + "?id=" + WiFi.macAddress() + "&qty=" + people_counted;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverPath.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");
    return;
  }

  Serial.println(digitalRead(button));

  if (digitalRead(button) == LOW) {
    int y = 0, z = 0;
    while (true) {

      if (z > 200) {
        people_counted = -1;
        for (int i = 0; i < 6; i++) {
          display.showNumberDec(0, i % 2 == 0);
          delay(100);
        }
        break;
      } else if (digitalRead(button) == LOW) {
        y = 0;
        z++;
        delay(5);
        continue;
      } else if (y > 10) {
        break;
      }

      y++;
      delay(5);
    }
    people_counted += 1;
    display.showNumberDec(people_counted);
    send_http_signal();
  }

  delay(5);
}
