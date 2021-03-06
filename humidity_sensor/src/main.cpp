#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

#include <wifi-ap.h>
#include <ota-updater.h>
#include <rfchannel.h>


#define PIN_RF_CE D2
#define PIN_RF_CSN D8

#define DHTTYPE DHT11   // DHT 11
#define DHTPIN D3     // Digital pin connected to the DHT sensor

#define SEND_INTERVAL_MSEC (5 * 1000)

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL }; 
const uint64_t myPipe = 0xabababab01LL; 
const char * testMessage = "test message";

#ifndef DFLT_WiFI_SSID
    //#define DFLT_WiFI_SSID nullptr
    #define DFLT_WiFI_SSID "test"
#endif

#ifndef DFLT_WiFI_PSWD
    //#define DFLT_WiFI_PSWD nullptr
    #define DFLT_WiFI_PSWD "test"
#endif

const char * wifiSsid = DFLT_WiFI_SSID;
const char * wifiPswd = DFLT_WiFI_PSWD;

uint8_t values[6];

RF24 radio(PIN_RF_CE, PIN_RF_CSN);
RFChannel channel(&radio, 200, 10*1000);

DHT dht(DHTPIN, DHTTYPE);

#define SENSOR_NAME "humsensor-01"

WiFiAP wifiAp(SENSOR_NAME);
OtaUpdater ota;
ESP8266WebServer server(80);


void setup() {

    // init serial
    Serial.begin(115200);
    Serial.println("Initializing...");

    // init radio
    bool radioInit = radio.begin();
    Serial.print("Radio init ");
    Serial.print(radioInit ? "success" : "failure");
    Serial.println();
    
    radio.setChannel(60);
    radio.setPALevel(RF24_PA_HIGH);           // If you want to save power use "RF24_PA_MIN" but keep in mind that reduces the module's range
    radio.setDataRate(RF24_250KBPS);
    radio.setAutoAck(1);                     // Ensure autoACK is enabled
    //radio.setRetries(2,15);                  // Optionally, increase the delay between retries & # of retries
    radio.enableDynamicPayloads();
    //radio.setPayloadSize(12);

    //radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance

    // Use pipes[1] - write pipe
    // Use myPipe - read pipe
    //radio.openWritingPipe(pipes[1]);
    //radio.openReadingPipe(1, myPipe);
    channel.begin();
    channel.openRedingPipe(1, myPipe);

    radio.printDetails();

    htonl()

    dht.begin();

    wifiAp.begin();
    wifiAp.setState(true);
    WiFi.mode(WIFI_AP_STA);
    WiFi.enableSTA(true);

    ota.begin();
    ota.enableOta();

    server.on("/", [](){
        String webPage;
        webPage += "<h1>ESP8266 Web Server</h1>";
        webPage += "<p><a href=\"/info\">Info</a></p>";
        webPage += "<p><a href=\"/rf24\">RF24</a></p>";
        server.send(200, "text/html", webPage);
    });
    server.on("/rf24", [](){
        char buff[40];
        String webPage;
        webPage += "<h1>ESP8266 Web Server</h1>";
        webPage += "<p><a href=\"/\">Home</a></p>";
        webPage += "<p>";
        // RF channel
        webPage += "Channel: ";
        sprintf(buff, "%i", (int)radio.getChannel());
        webPage += buff;
        webPage += "<br/>";
        // RF Data Rate
        webPage += "Data Rate: ";
        auto dataRate = radio.getDataRate();
        auto dataRateStr = dataRate == RF24_250KBPS ? "250KBPS" : dataRate == RF24_1MBPS ? "1MBPS" : dataRate == RF24_2MBPS ? "2MBPS" : "Unknown";
        webPage += dataRateStr;
        webPage += "<br/>";
        // RF isChipConnected
        webPage += "Chip Connected: ";
        webPage += (radio.isChipConnected() ? "YES" : "NO");
        webPage += "<br/>";
        // RF isChipConnected
        webPage += "IsFailure: ";
        webPage += (radio.failureDetected ? "YES" : "NO");
        webPage += "<br/>";

        webPage += "</p>";
        server.send(200, "text/html", webPage);
    });
    server.on("/info", [](){
        String webPage;
        webPage += "<h1>ESP8266 Web Server</h1>";
        webPage += "<p><a href=\"/\">Home</a></p>";
        server.send(200, "text/html", webPage);
    });
    server.begin();
    MDNS.begin(SENSOR_NAME);
}

bool checkWiFiConnected(){
    bool result = false;
    if (wifiSsid != nullptr) {
        if (WiFi.isConnected()) {
            result = true;
        } else {

            int cnt = 0;
            auto status = WiFi.begin(wifiSsid, wifiPswd);
            WiFi.waitForConnectResult();
            status = WiFi.status();
            // while (status != WL_CONNECTED && cnt < 5)
            // {
            //     delay(50);
            //     status = WiFi.status();
            //     cnt++;
            // }
            
            result = status == WL_CONNECTED;
        }
    }
    Serial.printf("WiFi State: %i\n", (int)result);
    return result;
}

unsigned long lastSent = 0;

void loop() {
//  bool written = radio.write(testMessage, 12);
    //bool written = radio.write(testMessage, 12);

    // Serial.print("Write ");
    // Serial.print(written ? "successful" : "failed");
    // Serial.println();



    if (millis() - lastSent > SEND_INTERVAL_MSEC) {

        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        float h = dht.readHumidity();
        // Read temperature as Celsius (the default)
        float t = dht.readTemperature();
        // Read temperature as Fahrenheit (isFahrenheit = true)
        float f = dht.readTemperature(true);

        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t) || isnan(f)) {
            Serial.println(F("Failed to read from DHT sensor!"));
            return;
        }

        // Compute heat index in Fahrenheit (the default)
        float hif = dht.computeHeatIndex(f, h);
        // Compute heat index in Celsius (isFahreheit = false)
        float hic = dht.computeHeatIndex(t, h, false);


        values[0] = (uint8_t)h;
        values[1] = (uint8_t)((uint16_t)(h * 100) % 100);
        values[2] = (uint8_t)t;
        values[3] = (uint8_t)((uint16_t)(t * 100) % 100);
        values[4] = (uint8_t)hic;
        values[5] = (uint8_t)((uint16_t)(hic * 100) % 100);

        //bool written = radio.write(values, sizeof(uint8_t)*6);
        bool written = channel.sendData(myPipe, pipes[1], 0, values, sizeof(uint8_t)*6, true);

        Serial.print("Write ");
        Serial.print(written ? "successful" : "failed");
        Serial.println();

        if (written) {
            lastSent = millis();
        }

        Serial.print(F("Humidity: "));
        Serial.print(h);
        Serial.print(F("%  Temperature: "));
        Serial.print(t);
        Serial.print(F("°C "));
        Serial.print(f);
        Serial.print(F("°F  Heat index: "));
        Serial.print(hic);
        Serial.print(F("°C "));
        Serial.print(hif);
        Serial.println(F("°F"));

    }

    ota.handle();
    server.handleClient();

    checkWiFiConnected();

    MDNS.update();

    delay(200);
}