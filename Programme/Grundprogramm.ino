#include <WiFi.h>
#include <WebServer.h>

// =====================
// WLAN Access Point Daten
// =====================
// Der ESP32 erstellt ein eigenes WLAN, mit dem sich der Nutzer verbindet
const char* ssid = "ESP32_Website_KIT";
const char* password = "12345678";

// Webserver läuft auf Port 80 (Standard für HTTP)
WebServer server(80);

// =====================
// Pinbelegung (GPIO)
// =====================
// Hier sind die LEDs angeschlossen
#define LED_RED   4
#define LED_GREEN 16
#define LED_BLUE  17

// =====================
// Zustände der LEDs
// =====================
// Merkt sich, ob eine LED aktuell AN oder AUS ist
bool redState = false;
bool greenState = false;
bool blueState = false;

// =====================
// HTML Webseite
// =====================
// Diese Seite wird im Browser angezeigt
// Enthält 3 Buttons zum Steuern der LEDs
String webpage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 LED Toggle</title>
<style>
  body { font-family: Arial; text-align: center; margin-top: 50px; }
  button {
    width: 200px;
    height: 80px;
    font-size: 20px;
    margin: 15px;
    border: none;
    border-radius: 10px;
    color: white;
    opacity: 0.5;
  }
  .active { opacity: 1; }
  .red { background-color: red; }
  .green { background-color: green; }
  .blue { background-color: blue; }
</style>
</head>
<body>

<h2>LED Toggle Steuerung</h2>

<button id="redBtn" class="red" onclick="toggle('red')">Rot</button><br>
<button id="greenBtn" class="green" onclick="toggle('green')">Grün</button><br>
<button id="blueBtn" class="blue" onclick="toggle('blue')">Blau</button>

<script>
// Sendet einen Befehl an den ESP32
function toggle(color) {
  fetch("/" + color)
    .then(res => res.text())
    .then(state => updateButton(color, state));
}

// Aktualisiert die Darstellung des Buttons (hell = an, dunkel = aus)
function updateButton(color, state) {
  let btn = document.getElementById(color + "Btn");
  if (state === "1") {
    btn.classList.add("active");
  } else {
    btn.classList.remove("active");
  }
}
</script>

</body>
</html>
)rawliteral";

// =====================
// SERVER HANDLER
// =====================

// Startseite ausliefern (Webseite anzeigen)
void handleRoot() {
  server.send(200, "text/html", webpage);
}

// LED ROT toggeln (an/aus wechseln)
void handleRed() {
  redState = !redState;                 // Zustand umkehren
  digitalWrite(LED_RED, redState);     // LED setzen
  server.send(200, "text/plain", redState ? "1" : "0"); // Status zurückgeben
}

// LED GRÜN toggeln
void handleGreen() {
  greenState = !greenState;
  digitalWrite(LED_GREEN, greenState);
  server.send(200, "text/plain", greenState ? "1" : "0");
}

// LED BLAU toggeln
void handleBlue() {
  blueState = !blueState;
  digitalWrite(LED_BLUE, blueState);
  server.send(200, "text/plain", blueState ? "1" : "0");
}

// =====================
// SETUP (läuft einmal beim Start)
// =====================
void setup() {
  Serial.begin(115200); // Serielle Ausgabe für Debug

  // Pins als Ausgang setzen (für LEDs)
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // Alle LEDs beim Start ausschalten
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);

  // Access Point starten
  WiFi.softAP(ssid, password);
  Serial.println("AP gestartet");
  Serial.println(WiFi.softAPIP()); // zeigt die IP (meist 192.168.4.1)

  // URL-Routen festlegen
  server.on("/", handleRoot);     // Webseite
  server.on("/red", handleRed);   // Rot Button
  server.on("/green", handleGreen);
  server.on("/blue", handleBlue);

  // Webserver starten
  server.begin();
}

// =====================
// LOOP (läuft dauerhaft)
// =====================
void loop() {
  // Bearbeitet eingehende Anfragen vom Browser
  server.handleClient();
}
