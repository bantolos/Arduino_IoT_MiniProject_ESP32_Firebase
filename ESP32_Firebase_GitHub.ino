
/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2021 mobizt
 *
*/

/** This example will show how to authenticate using 
 * the legacy token or database secret with the new APIs (using config and auth data).
*/
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#define lightSensor 36

#define relay 4

int kondisiLampu;


//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "rahasia"
#define WIFI_PASSWORD "rahasia"

/* 2. If work with RTDB, define the RTDB URL and database secret */
#define DATABASE_URL "sayahapusya" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define DATABASE_SECRET "rahasia"

/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

unsigned long dataMillis = 0;
int count = 0;

void lampuOn(){
  digitalWrite(relay,LOW);
  kondisiLampu = 1;
  Serial.println("Lampu menyala!");
  if (Firebase.setFloat(fbdo, "/Relay/Lampu", kondisiLampu)) {
    Serial.println("Data Path : " + fbdo.dataPath());
    Serial.println("Data Type : " + fbdo.dataType());
  } else {
    Serial.println("Error : " + fbdo.errorReason());
  }
}

void lampuOff(){
  digitalWrite(relay,HIGH);
  kondisiLampu = 0;
  Serial.println("Lampu mati!");
  if (Firebase.setFloat(fbdo, "/Relay/Lampu", kondisiLampu)) {
    Serial.println("Data Path : " + fbdo.dataPath());
    Serial.println("Data Type : " + fbdo.dataType());
  } else {
    Serial.println("Error : " + fbdo.errorReason());
  }
}

void setup()
{

    Serial.begin(115200);
    
    pinMode(relay, OUTPUT);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the certificate file (optional) */
    //config.cert.file = "/cert.cer";
    //config.cert.file_storage = StorageType::FLASH;

    /* Assign the database URL and database secret(required) */
    config.database_url = DATABASE_URL;
    config.signer.tokens.legacy_token = DATABASE_SECRET;

    Firebase.reconnectWiFi(true);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);

    //Or use legacy authenticate method
    //Firebase.begin(DATABASE_URL, DATABASE_SECRET);

}

void loop()
{
    if (millis() - dataMillis > 5000)
    {
        dataMillis = millis();
        Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, "/test/int", count++) ? "ok" : fbdo.errorReason().c_str());
    }
    sensorCahaya();
    int vLight = getIntensitasCahaya();
    int setByIntensitasCahaya = getRelaySetting();
    int lampu = getLampu();
    
    if(setByIntensitasCahaya == 1) {
      if(vLight > 2000) {
        lampuOn();
      } else {
        lampuOff();
      }
    } else {
      if(lampu == 1) {
        lampuOn();
      } else {
        lampuOff();
      }
    }
   
    delay(1000);
}

void sensorCahaya() {
  int vLight = analogRead(lightSensor); 
  Serial.print("Intensitas Cahaya: ");
  Serial.println(vLight); 
  if (Firebase.setFloat(fbdo, "/SensorCahaya/IntensitasCahaya", vLight)) {
    Serial.println("Data Path : " + fbdo.dataPath());
    Serial.println("Data Type : " + fbdo.dataType());
  } else {
    Serial.println("Error : " + fbdo.errorReason());
  }
}

int getIntensitasCahaya() {
  int vLight;
  Firebase.getInt(fbdo, "/SensorCahaya/IntensitasCahaya", &vLight);
  return vLight;
}

int getRelaySetting() {
  int setByIntensitasCahaya;
  Firebase.getInt(fbdo, "/Relay/setByIntensitasCahaya", &setByIntensitasCahaya);
  return setByIntensitasCahaya;
}

int getLampu() {
  int lampu;
  Firebase.getInt(fbdo, "/Relay/Lampu", &lampu);
  return lampu;
}
