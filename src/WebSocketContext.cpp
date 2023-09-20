#include "WebSocketContext.h"

WebSocketContext::WebSocketContext(WiFiClient *client, RequestContext *request, int bufferLength)
{
    this->client = client;
    this->request = request;
    this->bufferLength = bufferLength;
}

WebSocketContext::~WebSocketContext()
{
}

void WebSocketContext::sendHeader()
{
    this->client->print("HTTP/1.1 101 Switching Protocols\r\n");
    this->client->print("Upgrade: websocket\r\n");
    this->client->print("Connection: Upgrade\r\n");
    this->client->print("Sec-WebSocket-Accept: ");
    this->client->print(generateAcceptKey(this->request->getHeader("Sec-WebSocket-Key").c_str()) + "\r\n");
    this->client->print("\r\n");
}

String WebSocketContext::generateAcceptKey(const char *clientKey)
{
    uint8_t sha1HashBin[20] = {0};
#ifdef ESP8266
    sha1(String(clientKey) + MASTER_KEY, &sha1HashBin[0]);
#elif defined(ESP32)
    String data = clientKey + MASTER_KEY;
    esp_sha(SHA1, (unsigned char *)data.c_str(), data.length(), &sha1HashBin[0]);
#else
    clientKey += MASTER_KEY;
    SHA1_CTX ctx;
    SHA1Init(&ctx);
    SHA1Update(&ctx, (const unsigned char *)clientKey.c_str(), clientKey.length());
    SHA1Final(&sha1HashBin[0], &ctx);
#endif
    if (rbase64.encode(sha1HashBin, 20) == RBASE64_STATUS_OK)
    {
        String key = rbase64.result();
        key.trim();
        return key;
    }
    return "";
}

bool WebSocketContext::handle()
{
    if (this->lastMessage != NULL)
    {
        delete[] this->lastMessage;
        this->lastMessage = NULL;
    }
    this->lastMessage = this->receive();
    return this->lastMessage != NULL;
}

void WebSocketContext::send(String message)
{
    this->send(message.c_str());
}

void WebSocketContext::send(const char *message)
{
    // mask
    uint8_t mask[4] = {0x00, 0x00, 0x00, 0x00};
    uint8_t headerSize = 0;

    // random mask
    for (int i = 0; i < 4; i++)
    {
        mask[i] = random(0, 255);
    }

    if (strlen(message) < 126)
    {
        headerSize = 2;
    }
    else if (strlen(message) < 65536)
    {
        headerSize = 4;
    }
    else
    {
        headerSize = 10;
    }

    uint8_t header[headerSize];
    header[0] = 0x81;
    if (strlen(message) < 126)
    {
        header[1] = strlen(message) | 0x80;
    }
    else if (strlen(message) < 65536)
    {
        header[1] = 126 | 0x80;
        header[2] = strlen(message) >> 8;
        header[3] = strlen(message) & 0xFF;
    }
    else
    {
        header[1] = 127 | 0x80;
        header[2] = strlen(message) >> 56;
        header[3] = strlen(message) >> 48;
        header[4] = strlen(message) >> 40;
        header[5] = strlen(message) >> 32;
        header[6] = strlen(message) >> 24;
        header[7] = strlen(message) >> 16;
        header[8] = strlen(message) >> 8;
        header[9] = strlen(message) & 0xFF;
    }

    this->client->write(header, headerSize);
    this->client->write(mask, 4);

    // mask data
    for (int i = 0; i < strlen(message); i++)
    {
        this->client->write(message[i] ^ mask[i % 4]);
    }

    this->client->flush();
}

char *WebSocketContext::receive()
{
    if (!this->available())
    {
        if (this->lastMessage != NULL)
        {
            delete[] this->lastMessage;
            this->lastMessage = NULL;
        }
        return NULL;
    }

    // read text frame header
    uint8_t header[2];
    this->client->readBytes(header, 2);

    // read length
    uint8_t length = header[1] & 0x7F;
    if (length == 126)
    {
        uint8_t lengthBytes[2];
        this->client->readBytes(lengthBytes, 2);
        length = lengthBytes[0] << 8 | lengthBytes[1];
    }
    else if (length == 127)
    {
        uint8_t lengthBytes[8];
        this->client->readBytes(lengthBytes, 8);
        length = lengthBytes[0] << 56 | lengthBytes[1] << 48 | lengthBytes[2] << 40 | lengthBytes[3] << 32 | lengthBytes[4] << 24 | lengthBytes[5] << 16 | lengthBytes[6] << 8 | lengthBytes[7];
    }

    // read mask
    uint8_t mask[4];
    this->client->readBytes(mask, 4);

    uint8_t opcode = header[0] & 0x0F;

    if (opcode == 0x08)
    {
        // close connection
        this->client->stop();
        return NULL;
    }

    if (opcode == 0x09)
    {
        // ping
        this->client->write(0x8A);
        this->client->write(0x00);
        this->client->flush();
        return NULL;
    }
    // read data
    char *data = new char[length + 1];
    this->client->readBytes(data, length);

    // unmask data
    for (int i = 0; i < length; i++)
    {
        data[i] = data[i] ^ mask[i % 4];
    }

    data[length] = '\0';
    return data;
}

char *WebSocketContext::getMessage()
{
    return this->lastMessage;
}

bool WebSocketContext::available()
{
    return this->client->available();
}

void WebSocketContext::setReadOnly(bool readOnly)
{
    this->readOnly = readOnly;
}

bool WebSocketContext::isReadOnly()
{
    return this->readOnly;
}