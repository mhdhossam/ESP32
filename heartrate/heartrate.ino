#include "Display.h"
#include "Network.h"
#include "MAX30105.h"
#include "heartRate.h"

#define I2C_COMMUNICATION
#define I2C_ADDRESS

MAX30105 particleSensor;

Network *network;
Display *display;

const int RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
int rates[RATE_SIZE]; //Array of heart rates
int rateSpot = 0;
int lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
float beatAvg;

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    initDisplay();
    initNetwork();
    // Initialize sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { //Use default I2C port, 400kHz speed
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
       
    }
    Serial.println("Place your index finger on the sensor with steady pressure.");
    particleSensor.setup(); //Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

void loop() {
    unsigned long currentMillis = millis();
    // Your existing code for heart rate sensing and data updating
    long irValue = particleSensor.getIR();
    if (checkForBeat(irValue) == true) {
        //We sensed a beat!
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute < 255 && beatsPerMinute > 20) {
            rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
            rateSpot %= RATE_SIZE; //Wrap variable

            //Take average of readings
            beatAvg = 0;
            for (byte x = 0 ; x < RATE_SIZE ; x++)
                beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
        }
    }

    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);
    display->healthUpdates("BPM " + String(beatsPerMinute, 1) + "",
                           "Avg BPM " + String(beatAvg, 0) + "%",
                           "ok");
    network->firestoreDataUpdate(beatsPerMinute, beatAvg);

    if (irValue < 50000)
        Serial.print(" No finger?");

    Serial.println();
}

void initDisplay() {
    display = new Display();
    display->initTFT();
    display->centerMsg("System Init...");
}

void initNetwork() {
    network = new Network();
    network->initWiFi();
    xTaskCreate(networkTask, "networkTask", 4096, network, 1, NULL);
}
