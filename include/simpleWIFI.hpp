#ifndef MARS_SIMPLE_WIFI
#define MARS_SIMPLE_WIFI

#include <WiFi.h>
#include <WebServer.h>

// curl -X POST http://<your-esp-ip>/update -H "Content-Type: text/plain" -d "Hello, World!"

#define IP_NAME         "MARS_MINI" //Enter the router name 
#define IP_PASSWORD     "marsmini"  //Enter the router password 

#define IP_LOCAL        192,168,1,100
#define IP_GATEWAY      192,168,1,10
#define IP_SUBNET_MASK  255,255,255,0

#define IP_START_DELAY  200

IPAddress local_IP(IP_LOCAL);
IPAddress gateway(IP_GATEWAY);
IPAddress subnet(IP_SUBNET_MASK);

WebServer server(80);

const char *ssid_AP     = IP_NAME; 
const char *password_AP = IP_PASSWORD;

void MARS_WIFI_simple_handle(){
    if (server.hasArg("plain")) {
        String payload = server.arg("plain");
        char str[100];
        snprintf(str, sizeof(str), "Successful trigger of function: %s\n", payload);
        Serial.print(str);
        server.send(200, "text/plain", str);
    }
}

void MARS_WIFI_init(void (*handle)(void)){
    delay(IP_START_DELAY);

    Serial.println("Disconnecting and setting soft-AP configuration.");
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);

    bool softAP = WiFi.softAPConfig(local_IP, gateway, subnet);
    // Try to start the WiFi
    bool exists = WiFi.softAP(ssid_AP, password_AP);
    
    if(softAP && exists){
        Serial.println("Ready: Soft-AP");
        Serial.printf("IP address : %s\nMAC address: %s\n",
                WiFi.softAPIP().toString(),
                WiFi.softAPmacAddress().c_str());
        // Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed to create ESP-Router in AP MODE");
        return;
    }

    server.on("/update", HTTP_POST, handle);

    server.begin();

    Serial.println("WIFI setup complete");
}


#endif