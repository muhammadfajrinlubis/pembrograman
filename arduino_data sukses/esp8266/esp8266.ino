#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// Ganti dengan SSID dan Password WiFi Anda
const char* ssid = "404 Not Found";
const char* password = "hardcore2023";

// URL server untuk menerima data
const char* serverName = "https://codeminds.my.id/backend/save_berat.php";

// Initialize WiFi client
WiFiClientSecure wifiClient;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Check server connection during setup
  if (checkServerConnection()) {
    Serial.println("Connected to server");
  } else {
    Serial.println("Cannot connect to server");
  }x
}

void loop() {
  // Receive data from Arduino Uno via Serial
  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    Serial.println("Data received from Arduino: " + receivedData);
    
    // Pecahkan data menjadi berat dan tinggi
    int separatorIndex = receivedData.indexOf(' ');
    if (separatorIndex != -1) { // Pastikan data valid
      String dataBerat = receivedData.substring(0, separatorIndex);
      String dataTinggi = receivedData.substring(separatorIndex + 1);

      // Tampilkan data di Serial Monitor
      Serial.println("Data Diterima:");
      Serial.println(dataBerat);
      Serial.println(dataTinggi);

      // Kirim data ke database
      sendDataToServer(dataBerat, dataTinggi);
    } else {
      Serial.println("Invalid data received");
    }

    delay(1000); // Tunda 1 detik sebelum pembacaan berikutnya
  }
}

bool checkServerConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    wifiClient.setInsecure(); // Mengabaikan verifikasi sertifikat SSL, hanya untuk pengujian
    http.begin(wifiClient, serverName);
    int httpResponseCode = http.GET();  // Use GET to check connection

    if (httpResponseCode > 0) {
      http.end();
      return true;
    } else {
      Serial.print("HTTP GET failed, error: ");
      Serial.println(httpResponseCode);
      http.end();
      return false;
    }
  }
  return false;
}

void sendDataToServer(String dataBerat, String dataTinggi) {
  if (WiFi.status() == WL_CONNECTED) {x
    HTTPClient http;
    wifiClient.setInsecure(); // Mengabaikan verifikasi sertifikat SSL, hanya untuk pengujian
    http.begin(wifiClient, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "berat=" + dataBerat + "&tinggi=" + dataTinggi;
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
