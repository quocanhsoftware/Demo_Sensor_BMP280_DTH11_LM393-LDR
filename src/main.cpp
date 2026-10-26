#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

// ================= WIFI =================
const char* ssid = "P530";
const char* password = "1234567890";

// ================= JAVA TCP SERVER =================
const char* serverIP = "192.168.1.14";
const uint16_t serverPort = 5000;

WiFiClient client;

// ================= SENSOR PINS =================
#define BMP_SDA 21
#define BMP_SCL 22
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LIGHT_PIN 27

Adafruit_BMP280 bmp;
DHT dht(DHT_PIN, DHT_TYPE);

bool bmpReady = false;

// ================= WIFI CONNECTION =================
void connectWiFi() {
    Serial.print("Dang ket noi Wi-Fi");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi da ket noi!");
    Serial.print("IP ESP32: ");
    Serial.println(WiFi.localIP());
}

// ================= TCP CONNECTION =================
bool connectTCP() {
    if (client.connected()) {
        return true;
    }

    client.stop();

    Serial.print("Dang ket noi Java Server: ");
    Serial.print(serverIP);
    Serial.print(":");
    Serial.println(serverPort);

    if (client.connect(serverIP, serverPort)) {
        Serial.println("TCP ket noi thanh cong!");
        return true;
    }

    Serial.println("TCP ket noi that bai!");
    return false;
}

// ================= SETUP =================
void setup() {
    Serial.begin(115200);

    Wire.begin(BMP_SDA, BMP_SCL);

    if (bmp.begin(0x76)) {
        bmpReady = true;
    } else if (bmp.begin(0x77)) {
        bmpReady = true;
    }

    dht.begin();
    pinMode(LIGHT_PIN, INPUT);

    Serial.println("\n=== AI SMART SPACE ===");

    if (bmpReady) {
        Serial.println("BMP280: OK");
    } else {
        Serial.println("BMP280: KHONG TIM THAY");
    }

    connectWiFi();
    connectTCP();
}

// ================= MAIN LOOP =================
void loop() {
    // Neu Wi-Fi mat ket noi, thu ket noi lai
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi mat ket noi, dang thu lai...");
        WiFi.disconnect();
        WiFi.begin(ssid, password);

        unsigned long startTime = millis();

        while (WiFi.status() != WL_CONNECTED &&
               millis() - startTime < 10000) {
            delay(500);
            Serial.print(".");
        }

        Serial.println();
    }

    // Doc nhiet do BMP280
    float temperature = NAN;

    if (bmpReady) {
        temperature = bmp.readTemperature();
    }

    // Doc do am DHT11
    float humidity = dht.readHumidity();

    // Doc trang thai anh sang
    int lightState = digitalRead(LIGHT_PIN);

    // In du lieu len Serial Monitor
    Serial.println("\n------ SENSOR DATA ------");

    Serial.print("Nhiet do: ");
    if (isnan(temperature)) {
        Serial.println("ERROR");
    } else {
        Serial.print(temperature, 2);
        Serial.println(" C");
    }

    Serial.print("Do am: ");
    if (isnan(humidity)) {
        Serial.println("ERROR");
    } else {
        Serial.print(humidity, 1);
        Serial.println(" %");
    }

    Serial.print("Anh sang: ");
    Serial.println(lightState);

    // Chi gui du lieu khi TCP da ket noi
    if (WiFi.status() == WL_CONNECTED && connectTCP()) {
        // Tao JSON hop le; gia tri loi se duoc gui thanh null
        String json = "{";

        json += "\"temperature\":";
        json += isnan(temperature) ? "null" : String(temperature, 2);

        json += ",\"humidity\":";
        json += isnan(humidity) ? "null" : String(humidity, 1);

        json += ",\"light\":";
        json += String(lightState);

        json += "}";

        // println tu them ky tu xuong dong de Java readLine() doc duoc
        client.println(json);

        Serial.print("Da gui TCP: ");
        Serial.println(json);
    } else {
        Serial.println("Chua gui duoc du lieu: TCP chua ket noi.");
    }

    delay(3000);
}