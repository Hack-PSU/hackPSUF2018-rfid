#include "esp8266.hpp"

namespace hackPSU{

    ESP8266_Device::ESP8266_Device(char* name) : Api(name) {
        WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
        OTA_enabled = false;
        String h = "hackpsu_scanner";
        h.toCharArray(hostname, 16);

    }

    void ESP8266_Device::enableUpdate(){
        if(OTA_enabled) { return; }

        ArduinoOTA.setPort(8266);
        ArduinoOTA.setHostname(hostname);
        ArduinoOTA.setPassword((char*)OTA_PASSWORD);
        // ArduinoOTA.setPasswordHash((char*)OTA_PASSWORD_HASH); // md5(OTA_PASSWORD)
        ArduinoOTA.onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) {
                type = "sketch";
            } else { // U_SPIFFS
                type = "filesystem";
            }

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
        });
        ArduinoOTA.onEnd([]() {
            Serial.println("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) {
                Serial.println("Auth Failed");
            } else if (error == OTA_BEGIN_ERROR) {
                Serial.println("Begin Failed");
            } else if (error == OTA_CONNECT_ERROR) {
                Serial.println("Connect Failed");
            } else if (error == OTA_RECEIVE_ERROR) {
                Serial.println("Receive Failed");
            } else if (error == OTA_END_ERROR) {
                Serial.println("End Failed");
            }
        });

        ArduinoOTA.begin();
        OTA_enabled = true;
    }

    void ESP8266_Device::handleUpdate(){
        ArduinoOTA.handle();
    }

    void ESP8266_Device::pre_send(Request* request){
        Api::pre_send(request);

        if(status() != WL_CONNECTED){
            connect();
            delay(2500);
        }
    }
    
    void ESP8266_Device::transmit(Request* req){
        // Begin HTTP request

        int status = WiFi.status();
        if(status != WL_CONNECTED){
            // make the code a negative number
            code = -1 * status;
            return;
        }
        
        #ifdef HTTPS_FINGERPRINT
            http.begin(req->generateURL(REDIS), FP);
        #else
            http.begin(req->generateURL(REDIS));
        #endif

        for(JsonPair& p: req->getHeader()){
            http.addHeader(p.key, p.value.as<char*>());
        }
        
        if(req->isMethod(API::GET)) {
            code = http.GET();
        } else if(req->isMethod(API::POST)) {
            String pld = "";
            req->getPayload().printTo(pld);
            code = http.POST(pld);
        }
    }

    Response* ESP8266_Device::scan() {
        if(code < 0) {
            return new Response("", code);
        }
        delay(250);
        Response* res =  new Response(http.getString(), code);
        http.end();
        return res;
    }

    void ESP8266_Device::post_send(Request* request, Response* response){
        for(uint8_t i = 0; i < RETRY_RATE; ++i) {
            Api::post_send(request, response);
            if(response->payload.length() == 0) {
                response->code = OUT_OF_MEMORY;
            } else if(response->code < 0 && status() != WL_CONNECTED){
                // Resend on a failed attempt
                connect();
                delete response;
                write(request);
                response = read();
            } else {
                break;
            }
        }
        code = 0; // reset this value for next use
    }


    int ESP8266_Device::status() {
        return WiFi.status();
    }

    int ESP8266_Device::connect() {
        WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
        yield();
        delay(2500);
        return status();
    }

    String ESP8266_Device::localIP() {
        return WiFi.localIP().toString();
    }

    String ESP8266_Device::mac() {
        return WiFi.macAddress();
    }
}