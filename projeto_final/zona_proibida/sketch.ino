#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  public:
    String rssiString;

    void onResult(BLEAdvertisedDevice advertisedDevice) {
      // Check if the found device's address matches the specific address
      if (strcmp(advertisedDevice.getAddress().toString().c_str(), "74:b0:59:8d:3b:b9") == 0) {
        // Convert the RSSI value to a string
        rssiString = String(advertisedDevice.getRSSI());

        // Print the device's address and RSSI if it matches
        Serial.print("Specific device found: ");
        Serial.print(advertisedDevice.getAddress().toString().c_str());
        Serial.print(" RSSI: ");
        Serial.println(rssiString);
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Bluetooth...");

  // init BLE
  BLEDevice::init("");

  // criar scanner
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

  // intervalo e window 
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true); 

  // iniciar scan
  pBLEScan->start(1, false);
}

void loop() {
  // escanear a cada 1s
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->start(1, false);
  delay(1000);
}
