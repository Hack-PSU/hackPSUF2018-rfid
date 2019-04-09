#pragma once

#include <Arduino.h>

#include <api.hpp>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// OTA libraries
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


#ifndef NETWORK_PASSWORD
    #error Macro, NETWORK_PASSWORD, not set
    #define NETWORK_PASSWORD "" // Here so intellisense is happy
#endif

#ifndef NETWORK_SSID
    #error Macro, NETWORK_SSID, not set
    #define NETWORK_SSID "" // Here so intellisense is happy
#endif

#if !defined(OTA_PASSWORD)
    #warning OTA uploads not enabled, define OTA_PASSWORD and OTA_PASSWORD_HASH to allow OTA uplaoding.
#endif

namespace hackPSU {
    class ESP8266_Device : public Api{
    public:
        ESP8266_Device(char* name);
        ~ESP8266_Device() = default;

        int status() override;
        int connect() override;
        String localIP() override;
        String mac() override;

        void enableUpdate() override;
        void handleUpdate() override;

    protected:
        void pre_send(Request* req) override;
        void post_send(Request* req, Response* res) override;

    private:
        HTTPClient http;
        bool OTA_enabled;
        char hostname[16];
        int code;

        Response* scan() override;
        void transmit(Request*) override;        
    };
}