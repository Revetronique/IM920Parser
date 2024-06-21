#ifndef IM920Parser_h
#define IM920Parser_h

#include <Arduino.h>
#include "helper.h"

class IM920Parser
{
  public:
    IM920Parser();
    ~IM920Parser();

    // receive data
    int process(const char*, size_t, bool ecio = false);
    uint8_t getPacketData(unsigned short);

    inline unsigned short getSenderID() { return id_sender; }
    inline uint8_t getRssi() { return rssi; }

    // command
    const char* setCommandBroadcast(uint8_t*, size_t, bool);  //"TXDA %02x%02x...%02x\r\n"
    const char* setCommandBroadcastFix(uint8_t*, size_t, bool);
    const char* setCommandUnicast(unsigned short, uint8_t*, size_t, bool);
    const char* setCommandSendback(unsigned short, uint8_t*, size_t, bool);
    const char* setCommandDelegate(unsigned short, uint8_t*, size_t, bool);

  private:
    // number of bytes
    static constexpr uint8_t num_command = 4;
    // header of receiving packets
    static constexpr uint8_t num_header = 2;
    static constexpr uint8_t num_node_id = 4;
    static constexpr uint8_t num_rssi = 2;
    // total header length (0x00,node[4],rssi[2]:)
    static constexpr unsigned short offset_byte_header = num_header + num_node_id + num_rssi + 3; // separated with different characters
    // capacity of data packet (32 bytes)
    static constexpr unsigned short max_byte_data = 32;

    // total number of bytes 
    static constexpr unsigned short max_len_command = num_command + 1 + num_node_id + 1 + max_byte_data * 2 + 2;
    
    char* command = new char[max_len_command + 1];
    uint8_t* packet = new uint8_t[max_byte_data];

    // receiving data size
    unsigned short size_data = 0;
    // sender id of which node send the data
    unsigned short id_sender = 0;
    // RSSI (wave strength)
    uint8_t rssi = 0;

    // format string (ex. TXDU 0002,01A38E...\r\n)
    inline const char* format_command(bool is_uni){
      return is_uni ? PSTR("%s %04X,%s\r\n") : PSTR("%s %s\r\n");
    }

    // general method for creating command message
    void generate_command(const char*, uint8_t*, size_t, bool ecio = false, unsigned short node = 0);
};

extern IM920Parser IM920sL;

#endif