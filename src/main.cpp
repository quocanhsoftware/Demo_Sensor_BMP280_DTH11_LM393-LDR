#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

// ===== KHAI BAO CHAN =====
//Đường truyền dữ liệu
#define BMP_SDA    21
//Tạo xung đồng hồ
#define BMP_SCL    22


#define DHT_PIN    4
#define DHT_TYPE   DHT11

#define LIGHT_PIN  27

// ===== KHOI TAO CAM BIEN =====
//tạo đối tượng bmp để làm việc với cảm biến BMP280.
Adafruit_BMP280 bmp;
//tạo đối tượng DHT11 sử dụng GPIO 4.
DHT dht(DHT_PIN, DHT_TYPE);

//tạo biến kiểu Boolean, ban đầu là false, nghĩa là chưa xác nhận BMP280 hoạt động.
bool bmpReady = false;

void setup() {
  Serial.begin(115200);

  // Khoi tao I2C cho BMP280
  //Đường truyền dữ liệu SDA
  //Tạo xung đồng hồ SCL
  Wire.begin(BMP_SDA, BMP_SCL);

  // Thu dia chi I2C 0x76 va 0x77
  if (bmp.begin(0x76)) {
    bmpReady = true;
  } else if (bmp.begin(0x77)) {
    bmpReady = true;
  }

  // Khoi tao DHT11 va cam bien anh sang
  dht.begin();  
  //input vì dữ liệu cần lấy là DO chân 27
  pinMode(LIGHT_PIN, INPUT);

  Serial.println("\n=== ESP32 SENSOR TEST ===");

  //kiểm tra xem BMP280 có hoạt động không
  if (bmpReady) {
    Serial.println("BMP280: OK");
  } else {
    Serial.println("BMP280: KHONG TIM THAY");
    Serial.println("Kiem tra day SDA, SCL, VCC, GND");
  }
}

void loop() {
  Serial.println("\n-------- SENSOR DATA --------");

  // 1. Nhiet do tu BMP280
  if (bmpReady) {
    float temperature = bmp.readTemperature();

    Serial.print("Nhiet do BMP280: ");
    Serial.print(temperature, 2);
    Serial.println(" *C");
  } else {
    Serial.println("Nhiet do BMP280: ERROR");
  }

  // 2. Do am tu DHT11
  float humidity = dht.readHumidity();

  if (isnan(humidity)) {
    Serial.println("Do am DHT11: ERROR");
  } else {
    Serial.print("Do am DHT11: ");
    Serial.print(humidity, 1);
    Serial.println(" %");
  }

  // 3. Anh sang digital tu LM393
  //đọc trạng thái logic tại GPIO 27.
  int lightState = digitalRead(LIGHT_PIN);

  Serial.print("Anh sang DO: ");
  Serial.println(lightState);

  Serial.println("-----------------------------");

  delay(2000);
}
