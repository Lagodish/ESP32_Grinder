#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <string.h>
#include <addons.h>

TaskHandle_t Task1;
TaskHandle_t Task2;

const int L1 = 25;   //pwm
const int L2 = 26;   //pwm
const int H1 = 14;   //io
const int H2 = 27;   //io

const int pot = 35;

// setting PWM properties
const int freq = 20000;
const int ledChannel_0 = 0;
const int ledChannel_1 = 1;
const int resolution = 10;

int potValue = 0;
int PWM = 0;

void SPWM( void * parameter)
{
    Serial.println("SPWM");

    while(1){
      if((potValue < 2000)&&(potValue >= 0)){
        digitalWrite(H1, LOW);
        digitalWrite(H2, HIGH);
        PWM = map(potValue, 2000, 0, 0, 1023);
        ledcWrite(ledChannel_0, PWM); 
        ledcWrite(ledChannel_1, 0); 
      }
      if((potValue <= 4095)&&(potValue > 2100)){
        digitalWrite(H1, HIGH);
        digitalWrite(H2, LOW);
        PWM = map(potValue, 2100, 4095, 0, 1023);
        ledcWrite(ledChannel_1, PWM); 
        ledcWrite(ledChannel_0, 0); 
      }
      if((potValue>=2000)&&(potValue<=2100)){
        digitalWrite(H1, LOW);
        digitalWrite(H2, LOW);
        ledcWrite(ledChannel_0, 0); 
        ledcWrite(ledChannel_1, 0);
      }
      vTaskDelay(10);
    }

    Serial.println("Ending SPWM");
    vTaskDelete(Task1);
}

void Servises( void * parameter)
{
    Serial.println("Servises");
    while(1){
        //ArduinoOTA.handle();
        potValue = analogRead(pot);
        vTaskDelay(100);
    }

    Serial.println("Ending Servises");
    vTaskDelete(Task2);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Booting");
  //Serial.println("Frequency_2Ph_Gen");
  //wifi_set();
  //ota_start();

  ledcSetup(ledChannel_0, freq, resolution);
  ledcSetup(ledChannel_1, freq, resolution);

  
  ledcAttachPin(L1, ledChannel_0);
  ledcAttachPin(L2, ledChannel_1);

  pinMode (H1, OUTPUT);
  pinMode (H2, OUTPUT);

  digitalWrite(H1, LOW);
  digitalWrite(H2, LOW);
  ledcWrite(ledChannel_0, 0);
  ledcWrite(ledChannel_1, 0);

  xTaskCreatePinnedToCore(
    SPWM,             /* Task function. */
    "SPWM",           /* String with name of task. */
    20000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    2,                /* Priority of the task. */
    &Task1,           /* Task handle. */
    0);               /* Core 0 */ 

  delay(500); 

  xTaskCreatePinnedToCore(
    Servises,         /* Task function. */
    "Servises",       /* String with name of task. */
    8000,             /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    &Task2,           /* Task handle. */
    1);               /* Core 1 */

}

void loop() {
vTaskDelete(NULL); 
}