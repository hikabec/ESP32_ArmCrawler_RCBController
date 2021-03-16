/*  ESP32_ArmCrawler_RCBController.ino by hikabec    
   
    Based on ESP32_BLE_RCBController_Test.ino by robo8080
      https://gist.github.com/robo8080/df411237ac2aca3e5c6b91b842d157e9
    
    RCBController
      https://itunes.apple.com/us/app/rcbcontroller/id689724127
      http://rcbcontroller.micutil.com/
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

// Add
#define PIN_L1 25
#define PIN_L2 26
#define PIN_R1 32
#define PIN_R2 33

// Add
void motor_stop(int pin1, int pin2) {  
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

// Add
void motor_forward(int pin1, int pin2) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
}

// Add
void motor_back(int pin1, int pin2) {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
}

BLEUUID SERVICE_UUID        = BLEUUID((uint16_t)0xFFF0);  // RCBController Service UUID
BLEUUID CHARACTERISTIC_UUID = BLEUUID((uint16_t)0xFFF1);  // RCBController Characteristic UUID

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("connectionCallback");
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("disconnectionCallback");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("Received Value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.printf("%02x ", value[i]);

        // Add
        switch (value[1]) {
          case 1: // UP
            motor_forward(PIN_L1, PIN_L2);
            motor_forward(PIN_R1, PIN_R2);
            break;
          case 2: // DOWN
            motor_back(PIN_L1, PIN_L2);
            motor_back(PIN_R1, PIN_R2);
            break;
          case 4: // RIGHT
            motor_forward(PIN_L1, PIN_L2);
            motor_back(PIN_R1, PIN_R2);
            break;
          case 8: // LEFT
            motor_back(PIN_L1, PIN_L2);
            motor_forward(PIN_R1, PIN_R2);
            break;
          case 0: // STOP
            motor_stop(PIN_L1, PIN_L2);
            motor_stop(PIN_R1, PIN_R2);
            break;  
        }
          
        Serial.println("");
        
      }
    }
};

void setup() {
  Serial.begin(115200);

  // Add
  pinMode(PIN_L1, OUTPUT);
  pinMode(PIN_L2, OUTPUT);
  pinMode(PIN_R1, OUTPUT);
  pinMode(PIN_R2, OUTPUT);
  
  // Create the BLE Device
  BLEDevice::init("ESP32 ArmCrawler");
  
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_WRITE_NR
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}
