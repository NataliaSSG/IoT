#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// LIBRERIAS DE WIFI
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <Arduino.h>
#if defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#error Wrong platform
#endif 
#include <WifiLocation.h>
#if __has_include("wificonfig.h")
#include "wificonfig.h"
#else
const char* googleApiKey = "AIzaSyDNnb1xl_PKV29x_URCCIqSD6JyXlzZCCM";
const char* ssid = "Tec-IoT";
const char* password = "spotless.magnetic.bridge";
#endif

WifiLocation location (googleApiKey);

// LIBRERIA DEL SENSOR DE TEMP
#include "DHT.h"
#define DHTTYPE DHT11
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE);

// BRAZALETE
int idBrazalete = 1;

// MEDICIONES
float temperatura;
float temperaturas[10];
int i = 0;
int pulso;
int mediaPulso;
String ubicacion = "";


// Set time via NTP, as required for x.509 validation
void setClock () {
    configTime (0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print ("Waiting for NTP time sync: ");
    time_t now = time (nullptr);
    while (now < 8 * 3600 * 2) {
        delay (500);
        Serial.print (".");
        now = time (nullptr);
    }
    struct tm timeinfo;
    gmtime_r (&now, &timeinfo);
    Serial.print ("\n");
    Serial.print ("Current time: ");
    Serial.print (asctime (&timeinfo));
}


void setup(void) {
  Serial.begin(115200); 
  WiFi.begin(ssid, password);
  dht.begin();
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  setClock ();
  srand(time(NULL));
  mediaPulso = rand_normal(80,20);
  delay(700);
}

void loop() {
  if (i == 9){
    i = 0;
    float sumaTemperatura = 0;
    for (int j = 0; j < 10; j++){
      sumaTemperatura += temperaturas[j];
    }
    temperatura = sumaTemperatura / 10;
    if (temperatura < 20) {
      temperatura = temperatura + 16;
    }
    if (temperatura > 20 && temperatura < 25) {
      temperatura = temperatura + 14;
    }
    if (temperatura > 25 && temperatura < 30) {
      temperatura = temperatura + 10;
    }
    registrarTemperatura(temperatura);

    pulso = rand_normal(mediaPulso, 2); 
    registrarPulso(pulso);

    location_t loc = location.getGeoFromWiFi();
    ubicacion = String (loc.lat, 7) + "," + String (loc.lon, 7);
    
    Serial.print("Ubicacion: ");
    Serial.println(ubicacion);
    
    Serial.println();
    Serial.print("URL: ");
    enviarDatos(temperatura,pulso,ubicacion);

    
  }

  float t = dht.readTemperature();
  temperaturas[i] = t;
  i += 1;

  delay(1000);
}

// ENVIO DE DATOS
void enviarDatos(float temperatura, int pulso, String ubicacion) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String datos_a_enviar = "id=" + String(idBrazalete) + "&temperatura=" + String(temperatura) + "&pulso=" + String(pulso) + "&ubicacion=" + ubicacion;
    //String datos_a_enviar;
    String stringURL;
    stringURL = "http://iotequipo3.000webhostapp.com/mediband.php?id="; 
    stringURL += idBrazalete;
    stringURL += "&temperatura=";
    stringURL += temperatura;
    stringURL += "&pulso=";
    stringURL += pulso;
    stringURL += "&ubicacion=";
    stringURL += ubicacion;

    Serial.println(stringURL);

    WiFiClient client;
    http.begin(client, stringURL);
    //http.addHeader("Content-Type","application/x-www-form-urlencoded");

    int codigo_respuesta = http.POST(datos_a_enviar);
    if (codigo_respuesta > 0) {
      Serial.println("Codigo HTTP: " + String(codigo_respuesta));
      if (codigo_respuesta == 200) {
        String cuerpo_respuesta = http.getString();
        Serial.println("El servidor respondio: ");
        Serial.println(cuerpo_respuesta);
      }
    }
    else {
      Serial.println("Error enviado POST, codigo: ");
      Serial.println(codigo_respuesta);
    }
    http.end();
  }
  else {
    Serial.println("Error en la conexion WiFi");
  }
  delay(9000);
}


double rand_normal(double mean, double stddev) {
    static double n2 = 0.0;
    static int n2_cached = 0;

    if (!n2_cached) {
        double x, y, r;
        do {
            x = 2.0 * rand() / RAND_MAX - 1;
            y = 2.0 * rand() / RAND_MAX - 1;

            r = x * x + y * y;
        } while (r == 0.0 || r > 1.0);

        double d = sqrt(-2.0 * log(r) / r);
        double n1 = x * d;
        n2 = y * d;

        double result = n1 * stddev + mean;
        n2_cached = 1;
        return result;
    } else {
        n2_cached = 0;
        return n2 * stddev + mean;
    }
}


// IMPRIMIR
void registrarPulso(int pulso) {
    Serial.print("Pulso = ");
    Serial.print(pulso);
    Serial.println(" ppm");
}

void registrarTemperatura(float temperatura){
  Serial.print("Temperatura = ");
  Serial.print(temperatura);
  Serial.println("°C  ");
}
