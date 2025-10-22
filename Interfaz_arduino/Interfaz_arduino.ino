#include <WiFi.h>
#include <SPI.h>
#include <RF24.h>
#include <ESPAsyncWebServer.h>

// --- Configura tu red WiFi ---
const char* ssid = "red";
const char* password = "contrasenia";

// --- NRF24 ---
// RF24 radio(4, 5); // CE, CSN
// const byte address[6] = "00001";

// --- Servidor HTTP asíncrono ---
AsyncWebServer server(80);

// --- Variables simuladas ---
float temp = 25.0;
float humedad = 50.0;
unsigned long lastUpdate = 0;
const unsigned long updateInterval = 3000; // cada 3 segundos

void setup() {
  Serial.begin(115200);

  // --- Conexión WiFi ---
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("✅ Conectado a WiFi");
  Serial.print("🌐 IP local: ");
  Serial.println(WiFi.localIP());

  // --- NRF24 (descomenta cuando lo conectes) ---
  // radio.begin();
  // radio.openWritingPipe(address);
  // radio.setPALevel(RF24_PA_LOW);
  // radio.stopListening();

  // --- RUTA PRINCIPAL ---
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Servidor ESP32 con RF24 y simulación de sensores listo!");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  // --- RUTA PARA CONTROL DE MOVIMIENTO ---
  server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("cmd")) {
      String cmd = request->getParam("cmd")->value();
      Serial.println("Comando recibido: " + cmd);

      // Simula envío por RF
      // radio.write(&cmd, sizeof(cmd));

      // Simula acción local
      Serial.println("📡 Simulando envío RF del comando: " + cmd);
    }

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{\"status\":\"ok\"}");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  // --- Manejo de preflight OPTIONS (CORS) ---
  server.on("/move", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(204);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  // --- NUEVA RUTA: datos simulados ---
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    char json[128];
    snprintf(json, sizeof(json),
             "{\"temperatura\": %.2f, \"humedad\": %.2f}",
             temp, humedad);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  // --- Iniciar servidor ---
  server.begin();
  Serial.println("🌍 Servidor iniciado correctamente");
}

void loop() {
  // --- Simulación de datos cada 3 segundos ---
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate > updateInterval) {
    lastUpdate = currentMillis;

    // Simula variaciones
    temp += random(-5, 5) * 0.1;
    humedad += random(-3, 3) * 0.2;

    // Asegura rangos válidos
    if (temp < 15) temp = 15;
    if (temp > 35) temp = 35;
    if (humedad < 30) humedad = 30;
    if (humedad > 90) humedad = 90;

    // Simula recepción RF
    Serial.print("📡 Datos simulados -> Temp: ");
    Serial.print(temp);
    Serial.print(" °C | Humedad: ");
    Serial.print(humedad);
    Serial.println(" %");
  }
}
