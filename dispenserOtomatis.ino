#include <NewPing.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Konfigurasi Wi-Fi
const char* ssid = "FKOMINFO-KARYAWAN";
const char* password = "Un1ga@2024";

// Konfigurasi Ultrasonik
#define TRIG_GALON D2
#define ECHO_GALON D3
#define TRIG_GELAS D5
#define ECHO_GELAS D6

#define MAX_DISTANCE 200  // Jarak maksimum sensor (cm)
NewPing galon(TRIG_GALON, ECHO_GALON, MAX_DISTANCE);
NewPing gelas(TRIG_GELAS, ECHO_GELAS, MAX_DISTANCE);

// Konfigurasi Relay dan Pump
#define RELAY_PIN D1

// Variabel Status
String statusGalon = "Penuh";
String statusAir = "Berhenti";

// Web Server
ESP8266WebServer server(80);

void setup() {
  // Inisialisasi Serial Monitor
  Serial.begin(115200);

  // Inisialisasi Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke Wi-Fi...");
  }
  Serial.println("Wi-Fi terhubung!");

  // Tampilkan alamat IP
  Serial.println(WiFi.localIP());

  // Inisialisasi Relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Konfigurasi Web Server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web server dimulai!");
}

void loop() {
  // Pantau Volume Galon
  int jarakGalon = galon.ping_cm();
  if (jarakGalon == 0 || jarakGalon > 40) {
    statusGalon = "Kosong";
    digitalWrite(RELAY_PIN, LOW);  // Matikan pump
    statusAir = "Berhenti";
  } else {
    statusGalon = "Penuh";
  }

  // Pantau Keberadaan Gelas
  int jarakGelas = gelas.ping_cm();
  if (jarakGelas > 0 && jarakGelas < 10 && statusGalon == "Penuh") {
    digitalWrite(RELAY_PIN, HIGH); // Nyalakan pump
    statusAir = "Mengalir";
  } else {
    digitalWrite(RELAY_PIN, LOW); // Matikan pump
    statusAir = "Berhenti";
  }

  // Tampilkan Web
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Status Dispenser</h1>";
  html += "<p>Status Galon: " + statusGalon + "</p>";
  html += "<p>Status Air: " + statusAir + "</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}