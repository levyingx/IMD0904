#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

#define IO_USERNAME  "levying" //<----------Mudar para o seu
#define IO_KEY       "aio_xabq33QRzdoaJTCJsesw4XMdqAyL" //<----------Mudar para o seu
const char* ssid = "Wokwi-GUEST";
const char* password ="";

const char* mqttserver = "io.adafruit.com";
const int mqttport = 1883;
const char* mqttUser = IO_USERNAME;
const char* mqttPassword = IO_KEY;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// Saídas digitais
#define LED 26

//Entradas analogicas
#define TEMP 36
#define POT 32
#define LDR 34

//Variavéis
float pot = 0;
float ldr = 0;
float temp = 0;
float temperatura = 0;
const float BETA = 3950; //Deve corresponder ao coeficiente beta do termistor


void setup_wifi() {

  delay(10);
 
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Messagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    //messageTemp += (char)payload[i]; <----------Usar quando tiver uma mensagem na resposta do bloco
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("LED Ligado");    
  } else {
    digitalWrite(LED, LOW);  // Turn the LED off by making the voltage HIGH
    Serial.println("LED Desligado");    
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Create a random client ID
    String clientId = "ESP32 - Sensores";
    clientId += String(random(0xffff), HEX);
    // Se conectado
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Conectado.");
      // Depois de conectado, publique um anúncio ...
      client.publish("levying/temp", "Iniciando Comunicação");
      client.publish("levying/pot", "Iniciando Comunicação");
      client.publish("levying/ldr", "Iniciando Comunicação");
      //... e subscribe.
      client.subscribe("levying/led"); // <<<<----- mudar aqui
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  //analogSetAttenuation (ADC_0db);  // Atenuação do ADC 1,1V - 4095 
  //analogSetPinAttenuation(POT,ADC_11db); // Atenuação do ADC 3,3V - 4095 
      
  pinMode(LED, OUTPUT); 

  setup_wifi();
  client.setServer(mqttserver, 1883); // Publicar
  client.setCallback(callback); // Receber mensagem
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(2000); //Intervalo de 10s entre leituras (MODDED TO 2SEC)
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    temp = analogRead(TEMP);
    temperatura = temp*0.02686203;; //calculo da temperatura para o LM35           
 
    //float temperatura = 1/((log(1/(4095./(temp-1))))/(BETA+1.0)/298.15)-273.15;
    //float temperatura = ((1.1/4095)*temp)/0.02307692307;
    
    char s_temp[8];
    dtostrf(temperatura,1,2,s_temp);
    Serial.print("Temperatura: ");
    Serial.println(s_temp);
    Serial.println(temperatura);
    client.publish("levying/temp", s_temp);

    
    pot = analogRead(POT)/100;
    char s_pot[8];
    dtostrf(pot,1,2,s_pot);
    Serial.print("Potênciometro: ");
    Serial.println(s_pot);
    Serial.println(pot);
    client.publish("levying/pot", s_pot);


    ldr = analogRead(LDR)/100;
    char s_ldr[5];
    dtostrf(ldr,1,2,s_ldr);
    Serial.print("LDR: ");
    Serial.println(s_ldr);
    Serial.println(ldr);
    client.publish("levying/ldr", s_ldr);
  }
}
