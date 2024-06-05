#ifndef Network_H_
#define Network_H_

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

class Network {
private:
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    friend void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    friend void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
    friend void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
    friend void FirestoreTokenStatusCallback(TokenInfo info);

public:
    Network();
     void  firestoreDataUpdate(float beatAvg, float beatsPerMinute);
    void firebaseInit();
    void initWiFi();
};

void printStackUsage(TaskHandle_t taskHandle, const char* taskName);
void printHeapSize();
void networkTask(void *pvParameters);

#endif // Network_H_
