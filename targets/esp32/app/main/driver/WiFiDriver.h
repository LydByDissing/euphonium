#ifndef EUPH_WIFI_DRIVER_H
#define EUPH_WIFI_DRIVER_H

#include "BerryBind.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "BellUtils.h"

#include "EventBus.h"
#include <Core.h>
#include <vector>
#include "mdns.h"
extern "C" {
#include "dns_server.h"
}
#include <mutex>

#define MAX_RECONNECT_COUNT 3
#define DEFAULT_SCAN_LIST_SIZE 10

struct WiFiState {
    int reconnectCount = 0;
    bool connected = false;
    bool isConnecting = false;
    bool fromAp = false;

    std::mutex stateMutex;
};

class WiFiStateChangedEvent: public Event {
    public:
    std::string stateString;
    std::string ipAddress;
    berry::map networks;
    WiFiStateChangedEvent(std::string stateString, berry::map networks, std::string ipAddress) {
        this->stateString = stateString;
        this->ipAddress = ipAddress;
        this->subType = "wifiStateChanged";
        this->eventType = EventType::LUA_MAIN_EVENT;
        this->networks = networks;
    };

    berry::map toBerry() {
        berry::map result;
        result["state"] = this->stateString;
        result["networks"] = this->networks;
        result["ipAddress"] = this->ipAddress;
        return result;
    }
};

extern WiFiState globalWiFiState;

void setWiFiEventBus(std::shared_ptr<EventBus> _luaEventBus);
void startFastScan();
void initializeWiFiStack();
void tryToConnect(std::string ssid, std::string password, bool fromAp);
void setupAP(std::string ssid, std::string password);
void exportWiFiDriver(std::shared_ptr<berry::VmState> berry);
#endif
