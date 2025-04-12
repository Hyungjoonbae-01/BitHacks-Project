#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

// OLED display size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi credentials
const char* ssid = "BitHacks";
const char* password = "BitHacks2025!";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -7 * 3600, 60000); // California timezone

const int buttonPin1 = 18; // First button
bool lastButtonState1 = HIGH;

int timeState = 0;
void setup() {
  Serial.begin(115200);
  // Connect to WiFi
  pinMode(buttonPin1, INPUT_PULLUP);

  Wire.begin(23, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();
  // Connect to WiFi
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts++ < 40) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi failed.");
    ESP.restart();
  }

  Serial.println("\nWiFi connected.");
  timeClient.begin();
}

void loop() {
    timeClient.update();
    // get current date
    time_t rawTime = timeClient.getEpochTime();
    struct tm *timeInfo = localtime(&rawTime);

    // get current time
    int hour = timeInfo->tm_hour;
    String period = "AM";
    if (hour >= 12) {
      period = "PM";
      if (hour > 12) hour -= 12;
    } else if (hour == 0) hour = 12;
    char formattedTime[12]; // Buffer for "YYYY-MM-DD"
    snprintf(formattedTime, sizeof(formattedTime), "%02d:%02d:%02d%s", hour, timeInfo->tm_min, timeInfo->tm_sec, period.c_str());
    char formattedDate[11]; // Buffer for "HH-MM-SS period"
    snprintf(formattedDate, sizeof(formattedDate), "%04d-%02d-%02d", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);

    // Display date and time on OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Date:");
    display.setTextSize(2);
    display.println(formattedDate);
    display.display();
    
    bool buttonState1 = digitalRead(buttonPin1);

    // Edge detection: trigger when button 1 is pressed
    if (lastButtonState1 == HIGH && buttonState1 == LOW) {
      Serial.println("\nButton 1 pressed");
      timeState = 1;
      display.setTextSize(1);
      display.setCursor(0, 30);
      display.println("Current Time:");
      display.setTextSize(2);
      display.setCursor(0, 45);
      display.println(formattedTime);
      display.display();
      
    }
    buttonState1 = lastButtonState1;
    delay(1000);
}