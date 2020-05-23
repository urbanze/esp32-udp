#include "udp.h"



//===========UDP===========
/**
 * @brief Start UDP Server to listen packets in specific port.
 * 
 * Called only one time per port.
 * If you want to listen another port, recall this function.
 * 
 * @param [port]: Port.
 */
void UDP::begin(uint16_t port)
{
    struct sockaddr_in addr;
    memset(&sar, 0, sizeof(sar));
    memset(&addr, 0, sizeof(addr));

    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (!sr)
    {
        sr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sr < 0)
        {
            ESP_LOGE(tag, "Fail to create socket [%d]", errno);
            return;
        }
    }

    if (bind(sr, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        ESP_LOGE(tag, "Fail to bind socket [%d]", errno);
        close(sr);
        return;
    }
}

/**
 * @brief Start packet to be sent over UDP.
 * 
 * This can be called only one time per IP/port.
 * If you want to send to another IP/port, recall this function.
 * 
 * @param [*ip]: IP String. Eg: "192.168.4.2"
 * @param [port]: Port.
 */
void UDP::beginPacket(const char *ip, uint16_t port)
{
    memset(&sas, 0, sizeof(sas));

    sas.sin_addr.s_addr = inet_addr(ip);
    sas.sin_family = AF_INET;
    sas.sin_port = htons(port);

    if (!ss)
    {
        ss = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (ss < 0)
        {
            ESP_LOGE(tag, "Fail to create socket [%d]", errno);
            return;
        }
    }
}

/**
 * @brief Close socket if object.
 */
void UDP::stop()
{
    close(sr);
    close(ss);
}

/**
 * @brief Flush (erase) all data available in RX queue.
 */
void UDP::flush()
{
    uint8_t bff = 0;
    int16_t avl = available();
    for (int16_t i = 0; i < avl; i++)
    {
        recvfrom(sr, &bff, 1, MSG_DONTWAIT, (struct sockaddr*)&sar, &fromlen);
    }
}

/**
 * @brief Get how many Bytes are available to read.
 * 
 * @return Bytes available to read.
 */
int16_t UDP::available()
{
    int16_t avl = 0;
    ioctl(sr, FIONREAD, &avl);
    return avl;
}

/**
 * @brief Get source IP from last received packet.
 */
char *UDP::remoteIP()
{
    memset(rmt_ip, 0, sizeof(rmt_ip));
    inet_ntop(AF_INET, &sar.sin_addr, rmt_ip, sizeof(rmt_ip));
    return rmt_ip;
}

/**
 * @brief Get source Port from last received packet.
 */
uint16_t UDP::remotePort()
{
    return ntohs(sar.sin_port);
}


/**
 * @brief Send data over UDP connection.
 * 
 * @param [*text]: Data do send.
 * 
 * @return Data wrote.
 */
int16_t UDP::write(uint8_t *data, uint16_t size)
{
    if (sr)
    {
        if (sendto(sr, data, size, 0, (struct sockaddr*)&sas, sizeof(sas)) < 0)
        {
            ESP_LOGE(tag, "Fail to send [%d]", errno);
            close(sr);
            return -1;
        }
    }
    else
    {
        if (sendto(ss, data, size, 0, (struct sockaddr*)&sas, sizeof(sas)) < 0)
        {
            ESP_LOGE(tag, "Fail to send [%d]", errno);
            close(ss);
            return -1;
        }
    }
    
    

    return size;
}

/**
 * @brief Send data over UDP connection.
 * 
 * printf() alias.
 *  
 * @return Data wrote.
 */
int16_t UDP::printf(const char *format, ...)
{
    va_list vl;

    va_start(vl, format);
    int16_t size = vsnprintf(NULL, 0, format, vl)+1;
    va_end(vl);

    char bff[size] = {0};
    vsprintf(bff, format, vl);

    return write((uint8_t*)bff, size);
}

/**
 * @brief Read only one Byte of data available.
 * 
 * @return Data of Byte value readed.
 */
uint8_t UDP::read()
{
    uint8_t bff = 0;
    recvfrom(sr, &bff, 1, MSG_DONTWAIT, (struct sockaddr*)&sar, &fromlen);
    return bff;
}

/**
 * @brief Read [size] Bytes of data available.
 */
void UDP::readBytes(uint8_t *bff, uint16_t size)
{
    //memset(bff, 0, sizeof(size));
    recvfrom(sr, bff, size-1, MSG_DONTWAIT, (struct sockaddr*)&sar, &fromlen);
}

/**
 * @brief Read [size] Bytes of data available.
 */
void UDP::readBytes(char *bff, uint16_t size)
{
    //memset(bff, 0, sizeof(size));
    recvfrom(sr, bff, size-1, MSG_DONTWAIT, (struct sockaddr*)&sar, &fromlen);
}
