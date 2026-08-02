#include "DEV_Config.h"
#include "EPD_3in7g.h"
#include "GUI_Paint.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ---- FILL THESE IN ----
const char* WIFI_SSID     = "Airtel_gaur_6860";
const char* WIFI_PASSWORD = "air97769";
const char* IMAGE_URL     = "https://github.com/swansh-shandilya/ai-eink-backpack-tag/raw/refs/heads/main/Images/myimg.bin";
// ------------------------

#define IMAGE_WIDTH   EPD_3IN7G_WIDTH
#define IMAGE_HEIGHT  EPD_3IN7G_HEIGHT
UDOUBLE Imagesize = ((IMAGE_WIDTH % 4 == 0) ? (IMAGE_WIDTH / 4) : (IMAGE_WIDTH / 4 + 1)) * IMAGE_HEIGHT;

void setup() {
    Serial.begin(115200);
    delay(500);

    // ---- Step A: connect to WiFi ----
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        Serial.print(" [status: ");
        Serial.print(WiFi.status());
        Serial.print("]");
    }
    Serial.println("");
    Serial.println("WiFi connected!");

    // ---- Step B: prepare a buffer to hold the downloaded image ----
    UBYTE *ImageBuffer = (UBYTE *)malloc(Imagesize);
    if (ImageBuffer == NULL) {
        Serial.println("Failed to allocate buffer!");
        while (1);
    }

    // ---- Step C: download the image bytes from GitHub ----
    WiFiClientSecure client;
    client.setInsecure();   // skip certificate check (fine for hobby use)
    client.setTimeout(15000);  // wait up to 15 seconds for data to arrive

    HTTPClient http;
    http.begin(client, IMAGE_URL);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);  // GitHub raw links can redirect to a different domain
    int httpCode = http.GET();

    if (httpCode == 200) {
        Serial.println("Download started...");
        WiFiClient *stream = http.getStreamPtr();

        size_t bytesRead = 0;
        unsigned long lastDataTime = millis();

        // Keep reading until we have every byte we expect, or 15s pass with no new data at all.
        while (bytesRead < Imagesize) {
            size_t avail = stream->available();
            if (avail > 0) {
                size_t toRead = avail;
                if (toRead > (Imagesize - bytesRead)) {
                    toRead = Imagesize - bytesRead;  // don't read past the buffer's end
                }
                size_t justRead = stream->readBytes(ImageBuffer + bytesRead, toRead);
                bytesRead += justRead;
                lastDataTime = millis();
            } else {
                if (millis() - lastDataTime > 15000) {
                    Serial.println("Timed out waiting for more data.");
                    break;
                }
                delay(10);
            }
        }

        Serial.print("Bytes downloaded: ");
        Serial.println(bytesRead);

        if (bytesRead == Imagesize) {
            // ---- Step D: send it to the display, same as Phase 2 ----
            DEV_Module_Init();
            EPD_3IN7G_Init();
            Serial.println("Pushing image to display...");
            EPD_3IN7G_Display(ImageBuffer);
            Serial.println("Display updated!");
            EPD_3IN7G_Sleep();
        } else {
            Serial.println("Downloaded size doesn't match expected size, skipping display.");
        }
    } else {
        Serial.print("HTTP GET failed, error code: ");
        Serial.println(httpCode);
    }

    http.end();
    free(ImageBuffer);
}

void loop() {
}
