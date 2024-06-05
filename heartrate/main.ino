#include "Display.h"
#include "Network.h"
#include "MAX30105.h"
#include "heartRate.h"
#include <Ticker.h>

#define I2C_COMMUNICATION
#define I2C_ADDRESS

MAX30105 particleSensor;
Network *network;
Display *display;

const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred

float beatsPerMinute;
float beatAvg;

TaskHandle_t heartRateTaskHandle = NULL;
Ticker heartRateTicker;
bool tasksEnabled = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    initDisplay();
    initNetwork();
    initHeartRate();
   tasksEnabled = true;
}

void loop() {
    if (!tasksEnabled) {
        delay(2000); // Wait 2 seconds to let system settle down
        tasksEnabled = true;
        if (heartRateTaskHandle != NULL) {
            vTaskResume(heartRateTaskHandle);
        }
    }
    yield();
}

bool initHeartRate() {
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Use default I2C port, 400kHz speed
        Serial.println("MAX30105 was not found. Please check wiring/power.");
        return false;
    }

    particleSensor.setup(); // Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0); // Turn off Green LED
    Serial.println("Place your index finger on the sensor with steady pressure.");

    xTaskCreatePinnedToCore(
        heartRateTask,                // Function to implement the task
        "heartRateTask",              // Name of the task
        1000000,                        // Stack size in words
        NULL,                         // Task input parameter
        5,                            // Priority of the task
        &heartRateTaskHandle,         // Task handle
        1                             // Core where the task should run
    );

    if (heartRateTaskHandle == NULL) {
        Serial.println("Failed to start task for heart rate update");
        return false;
    } else {
        heartRateTicker.attach(20, triggerHeartRateUpdate); // Update every 20 seconds
    }
    return true;
}

void triggerHeartRateUpdate() {
    if (heartRateTaskHandle != NULL) {
        xTaskResumeFromISR(heartRateTaskHandle);
    }
}

void heartRateTask(void *pvParameters) {
    Serial.println("heartRateTask loop started");
    while (1) {
        if (tasksEnabled) {
            readHeartRate();
        }
        vTaskSuspend(NULL);
    }
}

bool readHeartRate() {
    long irValue = particleSensor.getIR();
    if (checkForBeat(irValue) == true) {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute < 255 && beatsPerMinute > 20) {
            rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
            rateSpot %= RATE_SIZE; // Wrap variable

            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++) {
                beatAvg += rates[x];
            }
            beatAvg /= RATE_SIZE;
        }

        if (beatsPerMinute != 0) {
            display->healthUpdates("BPM " + String(beatsPerMinute, 1) + "",
                                   "Avg BPM " + String(beatAvg, 0) + "%",
                                   "ok");
            network->firestoreDataUpdate(beatsPerMinute, beatAvg);

            Serial.print("IR=");
            Serial.print(irValue);
            Serial.print(", BPM=");
            Serial.print(beatsPerMinute);
            Serial.print(", Avg BPM=");
            Serial.print(beatAvg);
            if (irValue < 50000) {
                Serial.print(" No finger?");
            }
            Serial.println();
        }
    }
    return true;
}

void initDisplay() {
    display = new Display();
    display->initTFT();
    display->centerMsg("System Init...");
}

void initNetwork() {
    network = new Network();
    network->initWiFi();
}
