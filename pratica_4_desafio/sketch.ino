#include <NTPClient.h>

#include <FS.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define LED_PIN 5
#define BTN_PIN 12

int estadoAnterior = HIGH;
bool estadoLed = false;
String str;
String s;

const char* ssid = "mich";
const char* password = "michelle2001";


//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Defina seu fuso horário aqui
const long gmtOffset_sec = -3 * 3600; // GMT -3 (Brasília)
const int daylightOffset_sec = 0;      // Nenhum deslocamento de horário de verão

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", gmtOffset_sec, daylightOffset_sec);

String obterData(){
  // Atualizar o cliente NTP
  timeClient.update();

  // Obter a data atual
  unsigned long epochTime = timeClient.getEpochTime();

  struct tm *ptm = gmtime((time_t *)&epochTime);
  char buf[30];
  sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  String currentTime = String(buf);

  Serial.println("Data e hora atual: " + currentTime);
  delay(1000);
  return currentTime;
}

void tryConnectWifi(){
  WiFi.begin(ssid, password);

  Serial.print("Conectando à rede ");
  Serial.print(ssid);
  // Esperar pela conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  // Conexão estabelecida
  Serial.println("Conectado com sucesso!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}


void writeFile(String state, String path) { //escreve conteúdo em um arquivo
  File rFile = SPIFFS.open(path, "a");//a para anexar
  if (!rFile) {
    Serial.println("Erro ao abrir arquivo!");
  }
  else {
    // Atualizar o cliente NTP
    String time = obterData();

    rFile.print(state);
    rFile.print(" | Data e hora de registro: ");
    rFile.println(time);

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

  tryConnectWifi();

  // Inicializar o cliente NTP
  timeClient.begin();
  timeClient.setTimeOffset(gmtOffset_sec);
 
  
  //formatFile();
  openFS(); 
  delay(5000);

  Serial.println("Verificando ultimo estado do led!");
  String lead_estado = readFile("/logsResults_2.txt");
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
        writeFile("ligado","/logsResults_2.txt");
        delay(1000);
      }else{

        digitalWrite(LED_PIN, LOW);
        writeFile("desligado","/logsResults_2.txt");
        delay(1000);
      }

      Serial.println("-----------arquivo atualizado!-----------");
      readFile("/logsResults_2.txt");

    }
  
}
