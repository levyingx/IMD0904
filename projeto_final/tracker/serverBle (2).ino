#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include<WiFi.h>
#include <PubSubClient.h>
#include <math.h>

// Defina os UUIDs do serviço e da característica
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

//broker connect
#define IO_USERNAME  "" //<----------Mudar para o seu
#define IO_KEY       "" //<----------Mudar para o seu

//buzzer gpio
#define BUZZER 25



const char* ssid = "";
const char* password ="";

const char* mqttserver = "maqiatto.com";
const int mqttport = 1883;
const char* mqttUser = IO_USERNAME;
const char* mqttPassword = IO_KEY;

WiFiClient espClient;
PubSubClient client(espClient);

BLEScan* pBLEScan;
bool deviceConnected = false;

std::vector<String> allowedDeviceNames; 

// Função para verificar nomes dos dispositivos
bool isDeviceNameAllowed(String deviceName) {
    for (String name : allowedDeviceNames) {
        if (deviceName.equals(name)) {
            return true;
        }
    }
    return false;
}

// função para registrar identificadores dos dispositivos
void saveDeviceNameAllowed(String str){
  char payloadCharArray[str.length() + 1];
  str.toCharArray(payloadCharArray, str.length() + 1);
  // Delimitador
  const char delimiter = ',';
  // Divide a string e adiciona os tokens ao vetor allowedDeviceNames
  char* token = strtok(payloadCharArray, &delimiter);
  
  while (token != NULL) {
    allowedDeviceNames.push_back(String(token));
    token = strtok(NULL, &delimiter);
  }

  Serial.println("Dispositivos permitido ");
  for (size_t i = 0; i < allowedDeviceNames.size(); i++) {
    Serial.print("Dispositivos permitido ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(allowedDeviceNames[i]);
  }  
}

// função para atualizar todas as zonas cadastradas automaticamente
void clear_zones(){
  for (const auto& deviceName : allowedDeviceNames) {
        String topic = String(mqttUser) + "/notify/" + deviceName;
        Serial.println(topic);
        client.publish(topic.c_str(), "0");
    }
}

// função calcular distancia
float calculateDistance(int rssi) {
    int txPower = -59; // Nível de potência do transmissor (valor típico para BLE)
    if (rssi == 0) {
        return -1.0; // Não foi possível calcular a distância
    }

    float ratio = rssi * 1.0 / txPower;
    if (ratio < 1.0) {
        return pow(ratio, 10);
    } else {
        float distance = (0.89976) * pow(ratio, 7.7095) + 0.111;
        return distance;
    }
}


void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}


void connect_broker() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Create a random client ID
    String clientId = "Cat - Zone";
    clientId += String(random(0xffff), HEX);
    // Se conectado
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("conectado");
      //... e subscribe.
      String user = String(mqttUser);
      client.subscribe(String(user + "/connect").c_str());
      client.subscribe(String(user + "/disconnect").c_str()); 
      client.subscribe(String(user + "/buzzer").c_str());
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.print("topico recebida [");
  Serial.print(topic);
  Serial.print("] ");
  
   if (String(topic) == String(mqttUser) + "/buzzer") {
      
      if ((char)payload[0] == '1') {
        Serial.println("buzzzer Ligado"); 
        digitalWrite(BUZZER, HIGH);   
      } else {
        Serial.println("buzzer Desligado");    
        digitalWrite(BUZZER, LOW);
        Serial.println("LIMPANDO ZONAS ATIVAS");
        clear_zones();
      }
      return;

   }else if(String(topic) == String(mqttUser) + "/disconnect"){
    Serial.println("LIMPANDO ZONAS");
    deviceConnected = false;
    clear_zones();

   }else{
      
      String str = "";
      for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        str += (char)payload[i];
      }
      
      if(str!=""){
        allowedDeviceNames.clear();
        Serial.println("registrar"); 
        saveDeviceNameAllowed(str);
        deviceConnected = true;
      }
   }

}


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        String macAddress = advertisedDevice.getAddress().toString();
        String name =  advertisedDevice.getName().c_str();

        Serial.print("Dispositivo BLE encontrado: ");
        Serial.println(advertisedDevice.toString().c_str());
        Serial.println(advertisedDevice.getName().c_str());


       for (const auto& deviceName : allowedDeviceNames) {
        int rssi = advertisedDevice.getRSSI();
       
        float distance = calculateDistance(rssi);
        
        Serial.print("Distancia: ");
        Serial.println(distance);
        if ((deviceName == macAddress)) {
          Serial.println("DISPOSITIVO BEACON");
          char* val = "0";
          if(distance<100){val = "1";}
          String topic = String(mqttUser) +"/notify/" + deviceName;
          client.publish(topic.c_str(), val);
        }
      }
  }
};



void setup(){
  Serial.begin(115200);

  setup_wifi();
  
  BLEDevice::init("ESP32");
  
  // Cria o servidor BLE
  BLEServer *server = BLEDevice::createServer();

  // Cria o serviço BLE
  BLEService *service = server->createService(SERVICE_UUID);

  // Cria o serviço scan
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  //pBLEScan->start(30, false);

  // Inicia o serviço
  service->start();
  
  // Inicia a publicação do serviço
  BLEAdvertising *pAdvertising = server->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();  

  client.setServer(mqttserver, 1883); // Publicar
  client.setCallback(callback); // Receber mensagem
  Serial.println("Servidor BLE iniciado");

  pinMode(BUZZER, OUTPUT);
}

void loop() {

  if (!client.connected()) {
    connect_broker();
  }

  if(deviceConnected){ 
    pBLEScan->start(30, false);
  }
  client.loop();
  delay(1000); 
  
}
