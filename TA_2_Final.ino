#define BLYNK_TEMPLATE_ID "TMPL6r-Eq4aVv"
#define BLYNK_TEMPLATE_NAME "Tugas Akhir"
#define BLYNK_AUTH_TOKEN "27E5ALPF7N8fn4K49S_sQ9WYZvmXj7QY"
#define BLYNK_PRINT Serial

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <PZEM004Tv30.h>


#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 17
#define PZEM_TX_PIN 16
#endif

#if !defined(PZEM_SERIAL)
#define PZEM_SERIAL Serial2
#endif


#if defined(ESP32)
/*************************
 *  ESP32 initialization
 * ---------------------
 * 
 * The ESP32 HW Serial interface can be routed to any GPIO pin 
 * Here we initialize the PZEM on Serial2 with RX/TX pins 16 and 17
 */
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);
#elif defined(ESP8266)
/*************************
 *  ESP8266 initialization
 * ---------------------
 * 
 * Not all Arduino boards come with multiple HW Serial ports.
 * Serial2 is for example available on the Arduino MEGA 2560 but not Arduino Uno!
 * The ESP32 HW Serial interface can be routed to any GPIO pin 
 * Here we initialize the PZEM on Serial2 with default pins
 */
//PZEM004Tv30 pzem(Serial1);
#else
/*************************
 *  Arduino initialization
 * ---------------------
 * 
 * Not all Arduino boards come with multiple HW Serial ports.
 * Serial2 is for example available on the Arduino MEGA 2560 but not Arduino Uno!
 * The ESP32 HW Serial interface can be routed to any GPIO pin 
 * Here we initialize the PZEM on Serial2 with default pins
 */
PZEM004Tv30 pzem(PZEM_SERIAL);
#endif

LiquidCrystal_I2C lcd(0x27,20,4);  // 0x3f set the LCD address to 0x27 for a 16 chars and 2 line display

//Sensor
const int PirPin = 35; // GPIO 35

//Declaration Relay
#define RELAY_PIN_1 32
#define RELAY_PIN_2 33
#define RELAY_PIN_3 25
#define RELAY_PIN_4 26
#define RELAY_PIN_5 27
//#define RELAY_PIN_6 14
#define RELAY_PIN_7 12
#define RELAY_PIN_8 13

//Declaraion switch
#define manualModePin 0
#define manualCahaya 15
#define SwitchPin1 23  
#define SwitchPin2 3  
#define SwitchPin3 19  
#define SwitchPin4 5  
#define SwitchPin5 4  

//Declaration Virtual button
#define VPIN_BUTTON_1    V10 
#define VPIN_BUTTON_2    V11
#define VPIN_BUTTON_3    V12 
#define VPIN_BUTTON_4    V13
#define VPIN_BUTTON_5    V14 
#define VPIN_BUTTON_6    V15
#define ModeCahaya       V16
#define ModePir          V17
#define Limiter_19       V19



// Relay State
bool toggleModeState = HIGH;
bool toggleModeCahaya = HIGH;
bool toggleState_1 = HIGH; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = HIGH; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = HIGH; //Define integer to remember the toggle state for relay 3
bool toggleState_4 = HIGH; //Define integer to remember the toggle state for relay 4
bool toggleState_5 = HIGH; //Define integer to remember the toggle state for relay 5
bool toggleState_6 = HIGH; //Define integer to remember the toggle state for relay 6
bool toggleState_7 = HIGH; //Define integer to remember the toggle state for relay 7
bool toggleState_8 = HIGH; //Define integer to remember the toggle state for relay 8

// Switch State
bool SwitchModeState = LOW;
bool SwitchModeCahaya = LOW;
bool SwitchState_1 = LOW;
bool SwitchState_2 = LOW;
bool SwitchState_3 = LOW;
bool SwitchState_4 = LOW;
bool SwitchState_5 = LOW;


float voltage = 0;
float current = 0;
float power = 0;
float energy = 0;
float frequency = 0;
float pf = 0;
int LimitPwr = 1300;  // Nilai limit awal 

int sensorValue;
int manualMode;
int MCahaya;
int sensorState;             // Status sensor sekarang
int lastSensorState = LOW;   // Status sensor sebelumnya
int On_Off = 0;
char flag = 0;
char estate_on = 0; 
char estate_off = 0;
BlynkTimer timer;
bool limitChanged = false; // Tandai apakah limit telah diubah
unsigned long previousMillisLimit = 0;
const unsigned long interval = 5000; // Jeda 5 detik

// Definisi waktu terakhir pembacaan
unsigned long previousMillisPZEM = 0;  // Waktu sebelumnya untuk delay PZEM
const long intervalPZEM = 2000;  // Interval delay PZEM dalam milidetik (2 detik)

unsigned long wifiConnectionStartTime = 0;
bool offlineMode = false;
const long TimeConect = 30000;

const int ledPin = 2;
const int OfledPin = 14;

//blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Ransomeware_Alert!";
char pass[] = "Mackie1604";

void setup() {

  // Debug console
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(OfledPin, OUTPUT);
  
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(2,0);
 lcd.print("Tugas Akhir");
 lcd.setCursor(0,1);
 lcd.print("Dea Firmansyah");
  delay(4000);
 lcd.clear();

 wifiConnectionStartTime = millis();
// Koneksi ke jaringan WiFi
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.println("Connecting to WiFi...");
    lcd.setCursor(0,0);
    lcd.print("Connecting to WiFi...");
      lcd.setCursor(0,1);
      lcd.print(ssid);
      lcd.setCursor(0,3);
      lcd.print(pass);

    digitalWrite(ledPin, HIGH);
    delay(3000);
    digitalWrite(ledPin, LOW);
    lcd.clear();
    if (millis() - wifiConnectionStartTime > TimeConect ) { // 1 minute timeout
      offlineMode = true;
      Serial.println("Failed to connect to WiFi, entering offline mode.");
      lcd.setCursor(0,0);
      lcd.print("Offline Mode");
      digitalWrite(OfledPin, HIGH);
      delay(5000);
      lcd.clear();
      break;
    }
  }
  
  if (!offlineMode) {
    Serial.println("Connected to WiFi");
    digitalWrite(ledPin, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Connected to WiFi");
    delay(2000);
    lcd.clear();
    
    Blynk.config(auth);
    if (!Blynk.connect(30000)) { // Try to connect to Blynk for 1 minute
      offlineMode = true;
      Serial.println("Failed to connect to Blynk, entering offline mode.");
      lcd.setCursor(0,0);
      lcd.print("Offline Mode");
      digitalWrite(OfledPin, HIGH);
      delay(5000);
      lcd.clear();
    } else {
      Serial.println("Connected to Blynk");
      lcd.setCursor(0,0);
      lcd.print("Connected to Blynk");
      digitalWrite(ledPin, HIGH);
      delay(3000);
      lcd.clear();
      
  // Mematikan LED
  digitalWrite(ledPin, LOW);
  delay(200); // Menunggu selama 1 detik
  // Menghidupkan LED
  digitalWrite(ledPin, HIGH);
  delay(200); // Menunggu selama 1 detik

  // Mematikan LED
  digitalWrite(ledPin, LOW);
  delay(200); // Menunggu selama 1 detik
    // Menghidupkan LED
  digitalWrite(ledPin, HIGH);
  delay(200); // Menunggu selama 1 detik

  // Mematikan LED
  digitalWrite(ledPin, LOW);
  delay(200); // Menunggu selama 1 detik
    // Menghidupkan LED
  digitalWrite(ledPin, HIGH);
    }
  }
  // Inisialisasi pin relay sebagai output
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  pinMode(RELAY_PIN_4, OUTPUT);
  pinMode(RELAY_PIN_5, OUTPUT);
  //pinMode(RELAY_PIN_6, OUTPUT);
  pinMode(RELAY_PIN_7, OUTPUT);
  pinMode(RELAY_PIN_8, OUTPUT);
  
  // Matikan semua relay saat booting
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_2, LOW);
  digitalWrite(RELAY_PIN_3, LOW);
  digitalWrite(RELAY_PIN_4, LOW);
  digitalWrite(RELAY_PIN_5, LOW);
  //digitalWrite(RELAY_PIN_6, LOW);
  digitalWrite(RELAY_PIN_7, LOW);
  digitalWrite(RELAY_PIN_8, HIGH);


  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);
  pinMode(SwitchPin4, INPUT_PULLUP);
  pinMode(SwitchPin5, INPUT_PULLUP);
  
  pinMode(manualCahaya, INPUT_PULLUP);
  pinMode(manualModePin, INPUT_PULLUP); // Konfigurasi pin mode manual sebagai input dengan pull-up resistor
  pinMode(PirPin, INPUT);  // Konfigurasi pin sebagai input

  timer.setInterval(2000L, Send_Values_To_Blynk);
}

 
void loop() {
 
   if (!offlineMode) {
    Blynk.run();
    manual_control();
    Send_Values_To_Blynk(); // Kirim data ke cloud
  }
  else{
 Offline_control();
  }
  //manual_control();
  //Blynk.run();
  //Send_Values_To_Blynk(); // Kirim data ke cloud
 
  timer.run();
  setLimit(LimitPwr); // Set limit setiap kali loop berjalan
  Get_Values_From_PZEM(); // Baca data PZEM
  SendMonitor();
  unsigned long currentMillisLimit = millis();
  
  //manualMode = digitalRead(manualModePin); // Baca nilai dari pin input untuk mode manual
  // Jika mode manual aktif (pin IO 34 HIGH)
  if (manualMode == 1) {
    
    //digitalWrite(RELAY_PIN_1, LOW); // Matikan relay
    Serial.println("________________________________________________________");
    Serial.println("PIR OFF"); // Tampilkan status relay pada serial monitor
    Serial.println("________________________________________________________");
  } else {
    Serial.println("________________________________________________________");
    Serial.println("PIR Active");
    Serial.println("________________________________________________________");
    PirSensor();
  }

  // Jeda 5 detik jika power > LimitPwr
  if (power > LimitPwr) {
    lcd.setCursor(11,2); lcd.print("Overload");

    if (currentMillisLimit - previousMillisLimit >= interval) {
      previousMillisLimit = currentMillisLimit;
    } else {
      return; // Keluar dari loop jika belum mencapai interval
    }
  }



}

void PirSensor(){

sensorValue = digitalRead(PirPin);      // Baca nilai dari pin input

  // Memeriksa jika nilai sensor berubah
  if (sensorValue != lastSensorState) {
    // Jika nilai sensor berubah menjadi HIGH (1)
    if (sensorValue == HIGH) {
      // Jika relay dalam keadaan mati, aktifkan relay
      if (digitalRead(RELAY_PIN_1) == LOW) {
        digitalWrite(RELAY_PIN_1, HIGH); // Aktifkan relay
        digitalWrite(RELAY_PIN_2, HIGH);
        digitalWrite(RELAY_PIN_3, HIGH);
        Serial.println("Relay ON");   // Tampilkan status relay pada serial monitor
      } else {
        // Jika relay dalam keadaan hidup, matikan relay
        digitalWrite(RELAY_PIN_1, LOW); // Matikan relay
        digitalWrite(RELAY_PIN_2, LOW);
        digitalWrite(RELAY_PIN_3, LOW);
        Serial.println("Relay OFF");  // Tampilkan status relay pada serial monitor
      }
    }
    // Simpan status terakhir sensor
    lastSensorState = sensorValue;
  }
}


// This function get the values from PZEM004 via serial.
void Get_Values_From_PZEM() {
    // Periksa apakah sudah mencapai interval yang diinginkan sejak delay PZEM terakhir
    unsigned long currentMillisPZEM = millis();
    if (currentMillisPZEM - previousMillisPZEM >= intervalPZEM) {
        // Update waktu sebelumnya dengan waktu sekarang
        previousMillisPZEM = currentMillisPZEM;

        voltage = pzem.voltage();
        if (!isnan(voltage)) {
            Serial.print("Voltage: ");
            Serial.print(voltage);
            Serial.println("V");
        } else {
            Serial.println("Error reading voltage");
        }

        current = pzem.current();
        if (!isnan(current)) {
            Serial.print("Current: ");
            Serial.print(current);
            Serial.println("A");
        } else {
            Serial.println("Error reading current");
        }

        power = pzem.power();
        if (!isnan(power)) {
            Serial.print("Power: ");
            Serial.print(power);
            Serial.println("W");
        } else {
            Serial.println("Error reading power");
        }

        energy = pzem.energy();
        if (!isnan(energy)) {
            Serial.print("Energy: ");
            Serial.print(energy, 3);
            Serial.println("kWh");
        } else {
            Serial.println("Error reading energy");
        }

        frequency = pzem.frequency();
        if (!isnan(frequency)) {
            Serial.print("Frequency: ");
            Serial.print(frequency, 1);
            Serial.println("Hz");
        } else {
            Serial.println("Error reading frequency");
        }

        pf = pzem.pf();
        if (!isnan(pf)) {
            Serial.print("PF: ");
            Serial.println(pf);
            Serial.println("________________________________________________________");
        } else {
            Serial.println("Error reading power factor");
        }

        Serial.println();
    }
}


// This function sends the PZEM004 values to Virtual Pins.
void Send_Values_To_Blynk(){

    Blynk.virtualWrite(V0, voltage);
    Blynk.virtualWrite(V1, current);
    Blynk.virtualWrite(V2, power);
    Blynk.virtualWrite(V3, energy);
    Blynk.virtualWrite(V4, frequency);
    Blynk.virtualWrite(V5, pf);
}


void SendMonitor(){
 
        // Print the values to the Serial console
        Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");  
        Serial.print("Current: ");      Serial.print(current);      Serial.println("A");  
        Serial.print("Power: ");        Serial.print(power);        Serial.println("W");  
        Serial.print("Energy: ");       Serial.print(energy,3);     Serial.println("kWh");  
        Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");   
        Serial.print("PF: ");           Serial.println(pf);  
        //Print LCD 20x4
        lcd.setCursor(0,0); lcd.print("V:"); lcd.setCursor(3,0); lcd.print(voltage); lcd.print("V");
        lcd.setCursor(0,1); lcd.print("I:"); lcd.setCursor(3,1); lcd.print(current); lcd.print("A");
        lcd.setCursor(0,2); lcd.print("F:"); lcd.setCursor(3,2); lcd.print(frequency, 1); lcd.print("Hz");
        lcd.setCursor(0,3); lcd.print("P:"); lcd.setCursor(3,3); lcd.print(power); lcd.print("W");
        lcd.setCursor(11,3); lcd.print("Lim:"); lcd.setCursor(15,3); lcd.print(LimitPwr);

}


//Limiter Daya
void setLimit(int newLimitPwr) {
 if(power <= newLimitPwr){
        OPON();
    } else {
        OPOFF();
    }
}

//Seting relay
void OPON(){
  digitalWrite(RELAY_PIN_8, HIGH); //aktif high
}
void OPOFF(){
  digitalWrite(RELAY_PIN_8, LOW);
}

//Sinkronisasi dg blynk
BLYNK_CONNECTED() {
//  // Request the latest state from the server
  Blynk.syncVirtual(Limiter_19);
  Blynk.syncVirtual(ModePir);
  Blynk.syncVirtual(ModeCahaya);
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
  Blynk.syncVirtual(VPIN_BUTTON_3);
  Blynk.syncVirtual(VPIN_BUTTON_4);
  Blynk.syncVirtual(VPIN_BUTTON_5);
  Blynk.syncVirtual(VPIN_BUTTON_6);

}


BLYNK_WRITE(Limiter_19) {
lcd.clear();
	 LimitPwr = param.asInt();
    setLimit(LimitPwr);
    Serial.print("Limit : ");
    Serial.println(LimitPwr);
}

BLYNK_WRITE(ModePir) {
 toggleModeState= param.asInt();
  if(toggleModeState == 1){
    manualMode = 0 ;
    Serial.println("Mode Pir AUTO ");  
  }
  else { 
    manualMode = 1 ;
    Serial.println("Mode pir Manual Blynk");
  }
}

BLYNK_WRITE(ModeCahaya) {
  toggleModeCahaya = param.asInt();
  if(toggleModeCahaya == 1){
    digitalWrite(RELAY_PIN_7, HIGH);

  }
  else { 
    digitalWrite(RELAY_PIN_7, LOW);
  }
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  if(toggleState_1 == 1){
    digitalWrite(RELAY_PIN_1, HIGH);
  }
  else { 
    digitalWrite(RELAY_PIN_1, LOW);
  }
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  if(toggleState_2 == 1){
    digitalWrite(RELAY_PIN_2, HIGH);
  }
  else { 
    digitalWrite(RELAY_PIN_2, LOW);
  }
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  if(toggleState_3 == 1){
    digitalWrite(RELAY_PIN_3, HIGH);
  }
  else { 
    digitalWrite(RELAY_PIN_3, LOW);
  }
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleState_4 = param.asInt();
  if(toggleState_4 == 1){
    digitalWrite(RELAY_PIN_4, HIGH);
  }
  else { 
    digitalWrite(RELAY_PIN_4, LOW);
  }
}

BLYNK_WRITE(VPIN_BUTTON_5) {
  toggleState_5 = param.asInt();
  if(toggleState_5 == 1){
    digitalWrite(RELAY_PIN_5, HIGH);
  }
  else { 
    digitalWrite(RELAY_PIN_5, LOW);
  }
}



BLYNK_WRITE(VPIN_BUTTON_7) {
  toggleState_7 = param.asInt();
  if(toggleState_7 == 1){
    digitalWrite(RELAY_PIN_7, HIGH);
  }
  else { 
    digitalWrite(RELAY_PIN_7, LOW);
  }
}


//Kontrol via switch hardware
void manual_control()
{
lcd.setCursor(13,0); lcd.print("Online");
  if (digitalRead(manualCahaya) == LOW && SwitchModeCahaya == LOW) {
    digitalWrite(RELAY_PIN_7, HIGH);
    Blynk.virtualWrite(ModeCahaya, HIGH);
    toggleModeCahaya = HIGH;
    SwitchModeCahaya = HIGH;

  }
  if (digitalRead(manualCahaya) == HIGH && SwitchModeCahaya == HIGH) {
    digitalWrite(RELAY_PIN_7, LOW);
    Blynk.virtualWrite(ModeCahaya, LOW);
    toggleModeCahaya = LOW;
    SwitchModeCahaya = LOW;
    Serial.println("Sensor Cahaya OFF");

  }
  if (digitalRead(manualModePin) == LOW && SwitchModeState == LOW) {
    manualMode = 0 ; // Kondisi switch mode pir
    Blynk.virtualWrite(ModePir, HIGH);
    toggleModeState = HIGH;
    SwitchModeState = HIGH;
    Serial.println("Switch PIR ON");

  }
  if (digitalRead(manualModePin) == HIGH && SwitchModeState == HIGH) {
    manualMode = 1 ; // Kondisi switch mode pir
    Blynk.virtualWrite(ModePir, LOW);
    toggleModeState = LOW;
    SwitchModeState = LOW;
    Serial.println("Switch PIR OFF");
  }
  if (digitalRead(SwitchPin1) == LOW && SwitchState_1 == LOW) {
    digitalWrite(RELAY_PIN_1, HIGH);
    Blynk.virtualWrite(VPIN_BUTTON_1, HIGH);
    toggleState_1 = HIGH;
    SwitchState_1 = HIGH;
    Serial.println("Switch-1 on");
  }
  if (digitalRead(SwitchPin1) == HIGH && SwitchState_1 == HIGH) {
    digitalWrite(RELAY_PIN_1, LOW);
    Blynk.virtualWrite(VPIN_BUTTON_1, LOW);
    toggleState_1 = LOW;
    SwitchState_1 = LOW;
    Serial.println("Switch-1 off");
  }
  if (digitalRead(SwitchPin2) == LOW && SwitchState_2 == LOW) {
    digitalWrite(RELAY_PIN_2, HIGH);
    Blynk.virtualWrite(VPIN_BUTTON_2, HIGH);
    toggleState_2 = HIGH;
    SwitchState_2 = HIGH;
    Serial.println("Switch-2 on");
  }
  if (digitalRead(SwitchPin2) == HIGH && SwitchState_2 == HIGH) {
    digitalWrite(RELAY_PIN_2, LOW);
    Blynk.virtualWrite(VPIN_BUTTON_2, LOW);
    toggleState_2 = LOW;
    SwitchState_2 = LOW;
    Serial.println("Switch-2 off");
  }
  if (digitalRead(SwitchPin3) == LOW && SwitchState_3 == LOW) {
    digitalWrite(RELAY_PIN_3, HIGH);
    Blynk.virtualWrite(VPIN_BUTTON_3, HIGH);
    toggleState_3 = HIGH;
    SwitchState_3 = HIGH;
    Serial.println("Switch-3 on");
  }
  if (digitalRead(SwitchPin3) == HIGH && SwitchState_3 == HIGH) {
    digitalWrite(RELAY_PIN_3, LOW);
    Blynk.virtualWrite(VPIN_BUTTON_3, LOW);
    toggleState_3 = LOW;
    SwitchState_3 = LOW;
    Serial.println("Switch-3 off");
  }
  if (digitalRead(SwitchPin4) == LOW && SwitchState_4 == LOW) {
    digitalWrite(RELAY_PIN_4, HIGH);
    Blynk.virtualWrite(VPIN_BUTTON_4, HIGH);
    toggleState_4 = HIGH;
    SwitchState_4 = HIGH;
    Serial.println("Switch-4 on");
  }
  if (digitalRead(SwitchPin4) == HIGH && SwitchState_4 == HIGH) {
    digitalWrite(RELAY_PIN_4, LOW);
    Blynk.virtualWrite(VPIN_BUTTON_4, LOW);
    toggleState_4 = LOW;
    SwitchState_4 = LOW;
    Serial.println("Switch-4 off");
  }
  if (digitalRead(SwitchPin5) == LOW && SwitchState_5 == LOW) {
    digitalWrite(RELAY_PIN_5, HIGH);
    Blynk.virtualWrite(VPIN_BUTTON_5, HIGH);
    toggleState_5 = HIGH;
    SwitchState_5 = HIGH;
    Serial.println("Switch-5 on");
  }
  if (digitalRead(SwitchPin5) == HIGH && SwitchState_5 == HIGH) {
    digitalWrite(RELAY_PIN_5, LOW);
    Blynk.virtualWrite(VPIN_BUTTON_5, LOW);
    toggleState_5 = LOW;
    SwitchState_5 = LOW;
    Serial.println("Switch-5 off");
  }
}



void Offline_control(){

  lcd.setCursor(13,0); lcd.print("Offline");

  if (digitalRead(manualCahaya) == LOW && SwitchModeCahaya == LOW) {
    digitalWrite(RELAY_PIN_7, HIGH);
    toggleModeCahaya = HIGH;
    SwitchModeCahaya = HIGH;
    Serial.println("Sensor Cahaya ON");

  }
  if (digitalRead(manualCahaya) == HIGH && SwitchModeCahaya == HIGH) {
    digitalWrite(RELAY_PIN_7, LOW);
    toggleModeCahaya = LOW;
    SwitchModeCahaya = LOW;
    Serial.println("Sensor Cahaya OFF");

  }
  if (digitalRead(manualModePin) == LOW) {
    manualMode = 0 ; // Kondisi switch mode pir
    toggleModeState = HIGH;
    SwitchModeState = HIGH;
    Serial.println("Switch PIR ON");
 

  }
  if (digitalRead(manualModePin) == HIGH) {
    manualMode = 1 ; // Kondisi switch mode pir
    toggleModeState = LOW;
    SwitchModeState = LOW;
    Serial.println("Switch PIR OFF");

  }
  if (digitalRead(SwitchPin1) == LOW && SwitchState_1 == LOW) {
    digitalWrite(RELAY_PIN_1, HIGH);
    toggleState_1 = HIGH;
    SwitchState_1 = HIGH;
    Serial.println("Switch-1 on");
  }
  if (digitalRead(SwitchPin1) == HIGH && SwitchState_1 == HIGH) {
    digitalWrite(RELAY_PIN_1, LOW);
    toggleState_1 = LOW;
    SwitchState_1 = LOW;
    Serial.println("Switch-1 off");
  }
  if (digitalRead(SwitchPin2) == LOW && SwitchState_2 == LOW) {
    digitalWrite(RELAY_PIN_2, HIGH);
    toggleState_2 = HIGH;
    SwitchState_2 = HIGH;
    Serial.println("Switch-2 on");
  }
  if (digitalRead(SwitchPin2) == HIGH && SwitchState_2 == HIGH) {
    digitalWrite(RELAY_PIN_2, LOW);
    toggleState_2 = LOW;
    SwitchState_2 = LOW;
    Serial.println("Switch-2 off");
  }
  if (digitalRead(SwitchPin3) == LOW && SwitchState_3 == LOW) {
    digitalWrite(RELAY_PIN_3, HIGH);
    toggleState_3 = HIGH;
    SwitchState_3 = HIGH;
    Serial.println("Switch-3 on");
  }
  if (digitalRead(SwitchPin3) == HIGH && SwitchState_3 == HIGH) {
    digitalWrite(RELAY_PIN_3, LOW);
    toggleState_3 = LOW;
    SwitchState_3 = LOW;
    Serial.println("Switch-3 off");
  }
  if (digitalRead(SwitchPin4) == LOW && SwitchState_4 == LOW) {
    digitalWrite(RELAY_PIN_4, HIGH);
    toggleState_4 = HIGH;
    SwitchState_4 = HIGH;
    Serial.println("Switch-4 on");
  }
  if (digitalRead(SwitchPin4) == HIGH && SwitchState_4 == HIGH) {
    digitalWrite(RELAY_PIN_4, LOW);
    toggleState_4 = LOW;
    SwitchState_4 = LOW;
    Serial.println("Switch-4 off");
  }
  if (digitalRead(SwitchPin5) == LOW && SwitchState_5 == LOW) {
    digitalWrite(RELAY_PIN_5, HIGH);
    toggleState_5 = HIGH;
    SwitchState_5 = HIGH;
    Serial.println("Switch-5 on");
  }
  if (digitalRead(SwitchPin5) == HIGH && SwitchState_5 == HIGH) {
    digitalWrite(RELAY_PIN_5, LOW);
    toggleState_5 = LOW;
    SwitchState_5 = LOW;
    Serial.println("Switch-5 off");
  }

}


