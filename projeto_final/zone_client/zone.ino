#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

// Broker connect
#define IO_USERNAME  "" // Mudar para o seu
#define IO_KEY       "" // Mudar para o seu

// WiFi connect
const char* ssid = ""; // Mudar para o seu
const char* password = ""; // Mudar para o seu

const char* mqttserver = "maqiatto.com";
const int mqttport = 1883;
const char* mqttUser = IO_USERNAME;
const char* mqttPassword = IO_KEY;

bool cat_zone = false;
String dynamicField = "";

WiFiClient espClient;
PubSubClient client(espClient);

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
BLEScan* pBLEScan;
int scanTime = 5; // Tempo de escaneamento em segundos

void setup_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

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

void publishBroker(const char* message ) {
    while (!client.connected()) {
        Serial.println("Tentando conexão MQTT...");
        String clientId = "989798t76";
        clientId += String(random(0xffff), HEX);

        if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
            Serial.println("Conectado ao broker MQTT!");
        } else {
            Serial.print("Falhou, rc=");
            Serial.print(client.state());
            Serial.println(" tentando novamente em 5 segundos");
            delay(5000);
        }
    }

    String topic = String(mqttUser) + "/alerta/" + dynamicField;
    client.publish(topic.c_str(), message);
}

void catControll(BLEAdvertisedDevice advertisedDevice){
  if (advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
        int rssi = advertisedDevice.getRSSI();
        Serial.print("RSSI: ");
        Serial.println(rssi);

        float distance = calculateDistance(rssi);
        Serial.print("Distância estimada: ");
        Serial.print(distance);
        Serial.println(" m");

        if (distance <= 50) {
            Serial.println("Gato está próximo!");
            cat_zone = true;
            publishBroker("1");
        } else {
            Serial.println("Gato está distante!");
            cat_zone = false; // Atualiza o estado do gato
            publishBroker("0");
        }
    } else {
        Serial.println("Dispositivo não está anunciando o serviço.");
        cat_zone = false;
    }
}


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.print("Dispositivo BLE encontrado: ");
        Serial.println(advertisedDevice.toString().c_str());

        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
            Serial.print("Serviço encontrado! UUID: ");
            Serial.println(advertisedDevice.getServiceUUID().toString().c_str());
            catControll(advertisedDevice);
            
        }else if(!cat_zone){
            Serial.println("Gato não está no perímetro");
            publishBroker("0");
        }

    }

    void onScanComplete(BLEScanResults scanResults) {
       bool cat_scan = false;
       int count = scanResults.getCount();
       for (int i = 0; i < count; i++) {
            BLEAdvertisedDevice device = scanResults.getDevice(i);
            if (device.haveServiceUUID() && device.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
                Serial.print("Dispositivo com serviço do gato encontrado!!");
                Serial.println(SERVICE_UUID);
                cat_scan = true;
                break;
            }
        }

        Serial.println("Scan completo!");
        if (!cat_scan) {
            cat_zone = false;
            publishBroker("0");
        }
   }
};


void setup() {
    Serial.begin(115200);
  
    Serial.println("Informe a zona proíbida:");
    while (dynamicField == "") {
        if (Serial.available() > 0) {
            dynamicField = Serial.readStringUntil('\n');
            dynamicField.trim(); // Remove espaços em branco no início e fim
        }
    }

    
    // Imprime o valor lido
    Serial.print("Valor lido do terminal: ");
    Serial.println(dynamicField);
    
    setup_wifi();
    
    client.setServer(mqttserver, mqttport);
    
    Serial.println("Starting BLE client...");
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); // Active scan uses more power, but gets results faster
    //pBLEScan->start(scanTime, false);
}

void loop() {
  
    if (!client.connected()) {
        cat_zone = false;
        publishBroker("0"); // Ensure connection to MQTT broker
    }
  
    client.loop(); // Ensure MQTT client stays connected
    pBLEScan->start(scanTime, false); // Restart the BLE scan
    delay(5000); // Wait before starting a new scan
}
