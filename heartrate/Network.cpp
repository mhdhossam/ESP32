#include "Network.h"
#include "addons/TokenHelper.h"

#define WIFI_SSID "MHD"
#define WIFI_PASSWORD "12345678"

#define API_KEY "AIzaSyCS2cS4XDvZEZhPhFZuhKwXlpjXtjP6mqc"
#define FIREBASE_PROJECT_ID "esp32-47534"
#define USER_EMAIL "mhd123@gmail.com"
#define USER_PASSWORD "123456"

static Network *instance = NULL;

void printHeapSize(const char* message) {
    Serial.printf("[%s] Current free heap: %u bytes\n", message, xPortGetFreeHeapSize());
    Serial.printf("[%s] Minimum free heap: %u bytes\n", message, xPortGetMinimumEverFreeHeapSize());
}

void printStackUsage(TaskHandle_t taskHandle, const char* taskName) {
    UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(taskHandle);
    Serial.printf("Task [%s] high water mark: %u words\n", taskName, stackHighWaterMark);
}

Network::Network() {
    instance = this;
}

void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("WIFI CONNECTED! BUT WAIT FOR THE LOCAL IP ADDR");
}

void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.print("LOCAL IP ADDRESS: ");
    Serial.println(WiFi.localIP());
    printHeapSize("WiFi Got IP");
    instance->firebaseInit();
}

void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("WIFI DISCONNECTED!");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void FirestoreTokenStatusCallback(TokenInfo info) {
    Serial.printf("Token Info: type = %s, status = %s\n", getTokenType(info), getTokenStatus(info));
}

void Network::initWiFi() {
    WiFi.disconnect();
    WiFi.onEvent(WiFiEventConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(WiFiEventGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(WiFiEventDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    printHeapSize("Init WiFi");
}

void Network::firebaseInit() {
    Serial.println("Initializing Firebase...");
    printHeapSize("Before Firebase Init");

    config.api_key = API_KEY;

    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    config.token_status_callback = FirestoreTokenStatusCallback;
    printHeapSize("Before Firebase Begin");

    Firebase.begin(&config, &auth);
    printHeapSize("After Firebase Begin");

    Serial.println("Firebase initialized.");
    printHeapSize("Firebase Init Complete");
}

void Network::firestoreDataUpdate(float beatAvg, float beatsPerMinute) {
    if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
        String documentPath = "House/Room_1";

        FirebaseJson content;

        content.set("fields/beatAvg/floatValue", String(beatAvg).c_str());
        content.set("fields/beatsPerMinute/floatValue", String(beatsPerMinute).c_str());

        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "beatAvg,beatsPerMinute")) {
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            return;
        } else {
            Serial.println(fbdo.errorReason());
        }

        if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw())) {
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            return;
        } else {
            Serial.println(fbdo.errorReason());
        }
    }
}

void networkTask(void *pvParameters) {
    Network* network = static_cast<Network*>(pvParameters);
    network->initWiFi();
    // Monitor heap and stack usage
    printHeapSize("networkTask");
    printStackUsage(NULL, "networkTask");
    vTaskDelete(NULL);  // Delete task after WiFi initialization
}
