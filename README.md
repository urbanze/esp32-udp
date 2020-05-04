# ESP32 IDF UDP library
All functions have comments and are inspired in the Arduino Core. WiFi Library used: [WiFi](https://github.com/urbanze/esp32-wifi)

# Simple example to listen and send echo
Attention: ESP32 will be listening port 19000 and send echo in port 5000.
```
WF wifi;
UDP udp;
wifi.ap_start("wifi", "1234567890"); //Create AP

udp.begin(19000); //Listening port 19000
while (1)
{
    uint16_t avl = udp.available();

    //If data available (received in port 19000), read and echo!
    if (avl)
    {
        char bff[128] = {0};
        udp.readBytes(bff, 128);

        ESP_LOGI(__func__, "Received %dB: [%s] from [%s:%d]", avl, bff, udp.remoteIP(), udp.remotePort());

        //Send echo to remoteIP in port 5000
        udp.beginPacket(udp.remoteIP(), 5000);
        udp.printf("ESP32: %s", bff);
    }

    vTaskDelay(pdMS_TO_TICKS(50));
}
```
