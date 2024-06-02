
#include <SoftwareSerial.h>


SoftwareSerial slave(10,11);

#define LDR_PIN 2
#define BTN 6

// LDR Characteristics
const float GAMMA = 0.7;
const float RL10 = 20;

int msg = "";
int old_msg = "";
float lux = 0;
float old_lux = 0;
int init_s = 0;

float readLuxSensor(int analogValue){
  float voltage = analogValue / 1024. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float value = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
  return analogValue;
}

void setup() {
  slave.begin(9600);

  Serial.begin(9600);
  Serial.println("iniciando programa!");

  pinMode(LDR_PIN, INPUT);
  pinMode(BTN, INPUT);

}

void loop() {
  int analogValue = analogRead(A0);
  int digitalValue = digitalRead(BTN);

  if (digitalValue == HIGH) {
    if(init_s==0){
      msg = 1023;
      init_s = 1;
    }else{
      lux = readLuxSensor(analogValue);
      msg = lux;
    }
  }

  if(old_msg!= msg){
    Serial.print("enviando valor para escravo");
    Serial.println(msg);
    slave.print(msg);
    slave.flush();
    old_msg= msg;
    delay(1000);
  }

}
