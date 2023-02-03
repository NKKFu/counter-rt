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

const char *ssid = "ICBEU_CLIENT";
const char *password = "";
const int button = 4;  // D2(gpio4)

// Your Domain name with URL path or IP address with path
String serverName = "http://192.168.124.187:8000/ping";
int people_counted = 0;

void setup() {
  display.setBrightness(7);
  Serial.begin(9600);

  pinMode(button, INPUT_PULLUP);

  people_counted = 0;
  WiFi.begin(ssid, password);
  display.showNumberDec(people_counted);
}

bool cannot_connect = false;
void send_http_signal() {
  WiFiClient client;
  HTTPClient http;

  // HTTP Timeout
  http.setTimeout(1000);

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
    // If it was timeout
    if (httpResponseCode == HTTPC_ERROR_CONNECTION_REFUSED) {
      cannot_connect = true;
    } else {
      cannot_connect = false;
    }

    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

bool dots = false;
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    display.showNumberDec(people_counted, 0b01000000);
    delay(200);
    display.showNumberDec(people_counted, 0);
    delay(200);
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
    display.showNumberDec(0, true);
    send_http_signal();
    if (cannot_connect) {
      display.showNumberDecEx(
        people_counted, 0b01000000, false, 4, 0);
    } else {
      display.showNumberDec(people_counted);
    }
  }

  delay(5);
}
