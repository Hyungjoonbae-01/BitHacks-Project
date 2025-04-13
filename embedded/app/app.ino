#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobotDFPlayerMini.h>
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

//HardwareSerial mySerial(1);  // Use UART1 on ESP32-S3
DFRobotDFPlayerMini myDFPlayer;

const int buttonPin1 = 36;
const int buttonPin2 = 37;
bool lastButtonState1 = HIGH;
bool lastButtonState2 = HIGH;

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  Wire.begin(12, 13);
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
  /*
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX (check your pins)
  if (!myDFPlayer.begin(Serial2)) {
    Serial.println("Unable to begin DFPlayer.");
    while (true);
  }
  myDFPlayer.volume(20);
  */
}

void loop() {
  timeClient.update();
  // get each button's state
  bool buttonState1 = digitalRead(buttonPin1);
  bool buttonState2 = digitalRead(buttonPin2);
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
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("Current Time:");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.println(formattedTime);
  display.display();
  // Say the current data and time
  if (lastButtonState1 == HIGH && buttonState1 == LOW) {
    Serial.println("Button 1 pressed: Speaking time");
    speakTime();
  }
  // Say Random Compliment
  /*
  if (lastButtonState2 == HIGH && buttonState2 == LOW) {
    Serial.println("Button 2 pressed: Playing compliment");
    playCompliment();
  }
  */
  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;
  delay(100);
}
void speakTime() {
  if (!timeClient.update()) {
    Serial.println("Failed to update time");
    return;
  }
  time_t rawTime = timeClient.getEpochTime();
  struct tm *timeInfo = localtime(&rawTime);

  int hour = timeInfo->tm_hour;
  int minute = timeInfo->tm_min;
  bool isPM = false;

  // Convert to 12-hour format
  if (hour >= 12) {
    isPM = true;
    if (hour > 12) hour -= 12;
  }
  if (hour == 0) hour = 12;
  
  // Say: "The time is"
  myDFPlayer.play(1); delay(1500);
  // Say hour (e.g., "Three")
  myDFPlayer.play(hour + 1); delay(1000);
  // Say minute (e.g., "Twenty-nine")
  myDFPlayer.play(minute + 21); delay(1000);
  // Say AM or PM
  myDFPlayer.play(isPM ? 81:80); delay(1000);
}
/*
void playCompliment() {
  int complimentTrack = random(0, complimentCount); // 0–4
  myDFPlayer.play(100 + complimentTrack); // e.g. 0100.mp3 to 0104.mp3
}
*/
