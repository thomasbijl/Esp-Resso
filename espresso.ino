#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

const char * ssid = "ssid";
const char * password = "password";

const int preheatingTime = 30000;
const int espressoPrepTime = 30000;
const int lungoPrepTime = 47500;
const int lungoServing = 2;
const int espressoServing = 1;
const int totalServingCapacity = 8;

int remainingServes = 0;

#define espressoRelay D3
#define lungoRelay D4

String htmlPage;

void setup() {

  Serial.begin(9600);

  pinMode(espressoRelay, OUTPUT);
  pinMode(lungoRelay, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.print("Verbinding maken met de server");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Verbonden met netwerk.");
  Serial.print("IP-Adres: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/SERVE-ESPRESSO", prepareEspresso);
  server.on("/SERVE-LUNGO", prepareLungo);
  server.on("/MANUAL-PREHEAT", preheatMachine);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("iKoffie is opgestart");
  preheatMachine();
  Serial.println("Zorg dat het waterreservoir volledig is gevuld");
}

void loop(void) {
  server.handleClient();

  if (!remainingServes) {
    switch (remainingServes) {
    case 4:
      Serial.println("Waterreservoir is 50%");

    case 2:
      // statements
      Serial.println("Waterreservoir is 20%");
    }
  }

}

void handleRoot() {
  buildHtmlPage();
  server.send(200, "text/html", htmlPage);
}

void preheatMachine() {
  digitalWrite(espressoRelay, !digitalRead(espressoRelay));
  digitalWrite(lungoRelay, !digitalRead(lungoRelay));
  Serial.print("Je machine wordt opgewarmd...");
  delay(preheatingTime);
  Serial.print("Je machine is opgewarmd!");
}

void prepareEspresso() {
  digitalWrite(espressoRelay, !digitalRead(espressoRelay));
  server.sendHeader("Location", "/");
  server.send(303);
  Serial.print("Je espresso wordt bereid...");
  delay(espressoPrepTime);
  Serial.print("Je espresso is klaar!");
  remainingServes++;
}

void prepareLungo() {
  digitalWrite(lungoRelay, !digitalRead(lungoRelay));
  server.sendHeader("Location", "/");
  server.send(303);
  Serial.print("Je lungo wordt bereid...");
  delay(lungoPrepTime);
  Serial.print("Je lungo is klaar!");
  remainingServes = remainingServes + 2
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Niet gevonden");
}

void buildHtmlPage() {
  String espressoState = ((digitalRead(espressoRelay)) ? "Actief" : "inactief");
  String lungoState = ((digitalRead(lungoRelay)) ? "Actief" : "inactief");

  htmlPage = "<!DOCTYPE html>";
  htmlPage += "<html>";
  htmlPage += "<head>";
  htmlPage += "<title>ESP(8266)resso</title>";
  htmlPage += "</head>";

  htmlPage += "<body bgcolor='#E0E0D0'>";
  htmlPage += "<br>Espresso: " + espressoState;
  htmlPage += "<br>Lungo: " + lungoState;
  htmlPage += "<form action=\"/SERVE-ESPRESSO\" method=\"POST\"><input type=\"submit\" value=\"Espresso\"></form>";
  htmlPage += "<form action=\"/SERVE-LUNGO\" method=\"POST\"><input type=\"submit\" value=\"Lungo\"></form>";
  htmlPage += "</body>";
  htmlPage += "</html>";
}
