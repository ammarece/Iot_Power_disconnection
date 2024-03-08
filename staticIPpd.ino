#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "iMaK";
const char* password = "gate@024";
IPAddress staticIP(192, 168, 137, 100); // Define your static IP address within your local network
IPAddress gateway(192, 168, 137, 1);    // Define your gateway IP address
IPAddress subnet(255, 255, 255, 0);   // Define your subnet mask

int ledPin = D3; // Define the GPIO pin connected to the LED
const int externalLedPin = D5; // Define the GPIO pin for the external LED
bool wifiConnected = false;
ESP8266WebServer server(80);

// Define an EEPROM address to store the LED state
int eepromAddress = 0;

LiquidCrystal_I2C lcd(0x3F, 16, 2); // Set the I2C address and LCD size (16x2)

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(externalLedPin, OUTPUT);
  digitalWrite(externalLedPin, LOW); // Turn off external LED
  Serial.begin(115200);
  EEPROM.begin(20);

  // Print the static IP configuration to the serial monitor
  Serial.print("Setting static IP to ");
  Serial.println(staticIP);
  Serial.print("Gateway: ");
  Serial.println(gateway);
  Serial.print("Subnet: ");
  Serial.println(subnet);

  // Set static IP configuration
  WiFi.config(staticIP, gateway, subnet);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PD Power Ltd");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  // Print the assigned IP address to the serial monitor
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Server is Online");
  lcd.setCursor(0, 1);
  lcd.print("IP: " + WiFi.localIP().toString());

  // Read the last LED state from EEPROM
  int lastLedState = EEPROM.read(eepromAddress);
  Serial.print("lastLedState:");
  Serial.println(lastLedState);
  if (lastLedState == HIGH) {
    digitalWrite(ledPin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Service Active     ");
    Serial.println("Service Active");
  } else {
    digitalWrite(ledPin, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Renew Service     ");
    Serial.println("Renew Service");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);

  server.begin();
}

void loop() {
  server.handleClient();
  if (WiFi.status() == WL_CONNECTED && !wifiConnected) {
    // WiFi connected, turn on the external LED
    digitalWrite(externalLedPin, HIGH); // Turn on external LED

    Serial.println("WiFi Connected");
    wifiConnected = true;
  } else if (WiFi.status() != WL_CONNECTED && wifiConnected) {
    // WiFi disconnected, turn off the external LED
    digitalWrite(externalLedPin, LOW); // Turn off external LED

    Serial.println("WiFi Disconnected");
    wifiConnected = false;
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8' />";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0' />";
  html += "<title>Pandav Power Limited</title></head><style>";
  html += "*,*::before,*::after{margin:0;padding0;box-sizing:border-box;}";
  html += "html,body{height:100vh;display:flex;justify-content:center;";
  html += "align-items:center;text-align:center;color:aliceblue;}";
  html += "h1{font-size:4rem;margin-bottom:3rem;}";
  html += "button{border:none;outline:none;padding:1em 3em;cursor:pointer;";
  html += "border-radius:100px;box-shadow:1px 1px 10px rgba(0,0,0,0.5);";
  html += "font-weight:bold;font-size:1rem;color:rgba(0,0,0,0.7);";
  html += "transition:all 200ms ease-in-out;}";
  html += "button:hover{background-color:rgb(248,148,70);color:aliceblue;}";
  html += "body{background-image:url('https://images.unsplash.com/photo-1473341304170-971dccb5ac1e?";
  html += "ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D&auto=format&fit=crop&w=2070&q=80');";
  html += "background-size:cover;background-repeat:no-repeat;background-position:center;}";
  html += "main{width:100%;padding:2em;display:flex;flex-direction:column;gap:4rem;";
  html += "backdrop-filter:blur(7px);box-shadow:1px 1px 10px rgba(0,0,0,0.5);border-radius:10px;}";
  html += ".actions{display:flex;flex-direction:column;gap:1rem;}";
  html += ".actions div{display:flex;justify-content:center;gap:2rem;}</style>";
  html += "<body><main><header><h1>Priyadarshini Power Limited</h1><h2>Subscriber</h2></header>";
  html += "<section class='actions'><div class='subs-0'>";
  html += "<a href='/off'><button>Cut Service</button></a>";
  html += "<a href='/on'><button>Active Service</button></a>";
  html += "</div></section></main></body></html>";

  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(ledPin, HIGH);
  EEPROM.write(eepromAddress, 1);
  EEPROM.commit();
  int lastLedState = EEPROM.read(eepromAddress);
  Serial.print("lastLedState:");
  Serial.println(lastLedState);
  handleRoot();
  lcd.setCursor(0, 1);
  lcd.print("Service Active      ");
  Serial.println("Service Active");
}

void handleOff() {
  digitalWrite(ledPin, LOW);
  EEPROM.write(eepromAddress, 0);
  EEPROM.commit();
  int lastLedState = EEPROM.read(eepromAddress);
  Serial.print("lastLedState:");
  Serial.println(lastLedState);
  handleRoot();
  lcd.setCursor(0, 1);
  lcd.print("Renew Service     ");
  Serial.println("Renew Service");
}
