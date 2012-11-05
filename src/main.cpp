#include "serialutil.h"
#include "usbutil.h"
#include "listener.h"
#include "signals.h"
#include "log.h"
#include <stdlib.h>

#define VERSION_CONTROL_COMMAND 0x80
#define RESET_CONTROL_COMMAND 0x81

// USB
#define DATA_IN_ENDPOINT 1
#define DATA_OUT_ENDPOINT 2

extern void reset();
extern void setup();
extern void loop();

const char* VERSION = "2.0";

SerialDevice SERIAL_DEVICE;

UsbDevice USB_DEVICE = {
    DATA_IN_ENDPOINT,
    MAX_USB_PACKET_SIZE_BYTES,
    DATA_OUT_ENDPOINT,
    MAX_USB_PACKET_SIZE_BYTES};

Listener listener = {&USB_DEVICE, &SERIAL_DEVICE};

int main(void) {
#ifdef __PIC32__
    init();
#endif // __PIC32__
    setup();

    for (;;)
        loop();

    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

bool handleControlRequest(uint8_t request) {
    switch(request) {
    case VERSION_CONTROL_COMMAND:
    {
        char* combinedVersion = (char*)malloc(strlen(VERSION) +
                strlen(getMessageSet()) + 4);
        sprintf(combinedVersion, "%s (%s)", VERSION, getMessageSet());
        debug("Version: %s\r\n", combinedVersion);

        sendControlMessage((uint8_t*)combinedVersion, strlen(combinedVersion));
        free(combinedVersion);
        return true;
    }
    case RESET_CONTROL_COMMAND:
        debug("Resetting...\r\n");
        reset();
        return true;
    default:
        return false;
    }
}

#ifdef __cplusplus
}
#endif