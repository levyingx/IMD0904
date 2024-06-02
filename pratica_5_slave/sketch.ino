#include <SoftwareSerial.h>

#define LED_PIN 2

SoftwareSerial master(10,11);
String msg = "";
String old_msg = "";
int value = 0;
int old_value = 0;

void connectLed(int value){
  Serial.println("atualizar led: ");
  if(value < 25){
    Serial.println("Baixa luminosidade!");
  }else{
    Serial.println(value);
    analogWrite(LED_PIN,convertToRead(value));
    //liga led
  }
  delay(1000);
}

void readMasterMsg(){
  while(master.available()){
    delay(10);
    if(master.available()>0){
      char c = master.read();
      msg+=c;
    }
  }
  master.flush();
}

void convertMsgToValue(String msg){
  char msg_arr[6];
  msg.toCharArray(msg_arr, sizeof(msg_arr));
  value = atoi(msg_arr);
}


void setup() {
  master.begin(9600);
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  // put your setup code here, to run once:
}

void loop() {
  
  readMasterMsg();

  if(old_msg != msg && msg !=""){
    old_msg = msg;
    convertMsgToValue(msg);
    Serial.print("dado enviado pelo mestre:");
    Serial.println(msg);
    delay(1000);
    connectLed(value);
    msg = "";
  }

}

int convertToRead(int x){
  int value = 255 * x;
  return x;
}