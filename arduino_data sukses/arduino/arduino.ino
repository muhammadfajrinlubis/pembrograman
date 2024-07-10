#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>
#include <NewPing.h>

// Inisialisasi LCD dengan alamat I2C 0x27 dan ukuran 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inisialisasi HX711
HX711 scale;

// Pin untuk HX711
const int DOUT_PIN = 2;  // Pin DOUT HX711 ke pin 2 ESP32
const int SCK_PIN = 3;   // Pin SCK HX711 ke pin 3 ESP32

// Pin untuk sensor ultrasonik
#define TRIGGER_PIN_1 4
#define ECHO_PIN_1 5
#define TRIGGER_PIN_2 6
#define ECHO_PIN_2 7

// Jarak maksimum yang dapat diukur (1 meter = 100 cm)
#define MAX_DISTANCE 100

NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE);

void setup() {
  // Inisialisasi Serial
  Serial.begin(115200);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); // Kolom 0, Baris 0
  lcd.print("Projek Pengukur");
  lcd.setCursor(0, 1); // Kolom 0, Baris 1
  lcd.print("Berat & Tinggi");
  delay(5000); // Tunda 5 detik
  lcd.clear();

  // Inisialisasi HX711
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(104.0); // Faktor kalibrasi; sesuaikan dengan kebutuhan
  scale.tare(); // Reset skala ke 0
}

void loop() {
  // Mengukur berat
  if (scale.is_ready()) {
    float kg = scale.get_units(5); // Dapatkan rata-rata dari 5 pembacaan
    kg = kg / 1000.0; // Konversi dari gram ke kilogram

    // Mengukur tinggi
    unsigned int distance1 = sonar1.ping_cm();
    unsigned int distance2 = sonar2.ping_cm();
    // Jarak total antara dua sensor (1 meter = 100 cm)
    unsigned int distance_between_sensors = 100;
    // Menghitung tinggi dengan menggunakan perbedaan dari jarak total
    unsigned int cm = distance_between_sensors - (distance1 + distance2);

    // Tampilkan hasil di LCD
    lcd.clear();
    lcd.setCursor(0, 0); // Kolom 0, Baris 0
    lcd.print("Berat: ");
    lcd.print(kg, 2);
    lcd.print(" kg");

    lcd.setCursor(0, 1); // Kolom 0, Baris 1
    lcd.print("Tinggi: ");
    lcd.print(cm);
    lcd.print(" cm");

    // Kirim data ke Serial Monitor dan ESP32
    String dataBerat = "Berat:" + String(kg, 2) + "kg";
    String dataTinggi = "Tinggi:" + String(cm) + "cm";
    Serial.print(dataBerat);
    Serial.print(" ");
    Serial.println(dataTinggi);

    delay(1000); // Tunda 1 detik sebelum pembacaan berikutnya
  } else {
    lcd.setCursor(0, 0); // Kolom 0, Baris 0
    lcd.print("HX711 tidak siap");
    delay(1000);
  }

  // Menerima data dari Serial dan menampilkan di LCD
  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Data Received:");
    lcd.setCursor(0, 1);
    lcd.print(receivedData);
    delay(3000);
  }
}