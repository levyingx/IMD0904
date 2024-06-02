#include <FS.h>
#include "SPIFFS.h"

#define LED_PIN 5
#define BTN_PIN 12

int estadoAnterior = HIGH;
bool estadoLed = false;
int num=0;
String str;
String s;


void writeFile(String state, String path) { //escreve conteúdo em um arquivo
  File rFile = SPIFFS.open(path, "a");//a para anexar
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    rFile.println(state);
    Serial.println("escrevendo no arquivo.........");
  }
  rFile.close();
}

String readFile(String path) {
  Serial.println("Read file");
  File rFile = SPIFFS.open(path, "r");//r+ leitura e escrita
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    Serial.print("----------Lendo arquivo ");
    Serial.print(path);
    Serial.println("  ---------");
    while (rFile.position() < rFile.size())
    {
      s = rFile.readStringUntil('\n');
      s.trim();
      Serial.println(s);
    }
    rFile.close();
    return s;
  }
}

void formatFile() {
  Serial.println("Formantando SPIFFS");
  SPIFFS.format();
  Serial.println("Formatou SPIFFS");
}

void openFS(void) {
  if (!SPIFFS.begin()) {
    Serial.println("\nErro ao abrir o sistema de arquivos");
  }
  else {
    Serial.println("\nSistema de arquivos aberto com sucesso!");
  }
}

void setup() {

  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  //formatFile();
  openFS(); 
  delay(5000);

  Serial.println("Verificando ultimo estado do led!");
  String lead_estado = readFile("/logsResults.txt");
  Serial.print("ESTADO ANTERIOR: ");
  Serial.println(lead_estado);
  delay(5000);
 
  if(lead_estado == "ligado" ){
    digitalWrite(LED_PIN, HIGH);
    estadoLed = true;
  }

  Serial.println("-----------iniciando programa-----------");

}

void loop() {
  int estadoBTN = digitalRead((BTN_PIN));

    if(estadoBTN == HIGH){
      //Serial.println("clicado");
      estadoLed = !estadoLed;
      delay(500);
   
      if(estadoLed == true){
        digitalWrite(LED_PIN, HIGH);
        writeFile("ligado","/logsResults.txt");
        delay(1000);
      }else{

        digitalWrite(LED_PIN, LOW);
        writeFile("desligado","/logsResults.txt");
        delay(1000);
      }

      Serial.println("-----------arquivo atualizado!-----------");
      readFile("/logsResults.txt");

    }
  
}