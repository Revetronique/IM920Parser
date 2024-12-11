#include "IM920Parser.h"

IM920Parser::IM920Parser() : size_data{ 0 }, id_sender{ 0 }, rssi{ 0 }
{
  // initialization
  for(int i = 0; i < IM920Parser::max_len_command; i++) command[i] = '\0';
  for(int i = 0; i < IM920Parser::max_byte_data; i++) packet[i] = '\0';
}

IM920Parser::~IM920Parser()
{
  delete command;
  delete packet;
}

int IM920Parser::process(const char* buffer, size_t len, bool ecio)
{
  // EOL doesn't match to the correct rule (CR+LF)
  if(buffer[len - 2] != '\r' || buffer[len - 1] != '\n')  return -1;
  // another header or delimiter
  else if(buffer[0] != '0' || buffer[1] != '0' || buffer[IM920Parser::offset_byte_header - 1] != ':') return -2;
  // no content
  else if(len <= IM920Parser::offset_byte_header + 2) return -3;

  // communication info
  char temp[] = {0, 0, 0, 0};
  // node id
  for(int i = 0; i < 4; i++)  temp[i] = buffer[IM920Parser::num_header + 1 + i];
  id_sender = programming::helper::decodeHex<unsigned short>(temp, 4);
  // rssi
  for(int i = 0; i < 2; i++)  temp[i] = buffer[IM920Parser::num_header + IM920Parser::num_node_id + 2 + i];
  rssi = programming::helper::decodeHex<uint8_t>(temp, 2);
  
  // reset data size
  size_data = 0;
  // processing receiving data
  for(int i = IM920Parser::offset_byte_header; i < len - 1; i++){
    if(ecio){ // ascii
      packet[size_data] = (uint8_t)buffer[i];
    }
    else{
      // each element of the packet (2 bytes)
      // and skip the 3rd next indices
      temp[0] = buffer[i++]; temp[1] = buffer[i++];
      // decode hex characters to the value
      packet[size_data] = programming::helper::decodeHex<uint8_t>(temp, 2);
    }
    // update data size
    size_data++;
  }

  return 0;
}

uint8_t IM920Parser::getPacketData(unsigned short index)
{
  auto id = min(index, size_data);
  return packet[id];
}

const char* IM920Parser::setCommandBroadcast(uint8_t* data, size_t len, bool ecio)
{
  IM920Parser::generate_command(PSTR("TXDA"), data, len, ecio);
  return command;
}

const char* IM920Parser::setCommandBroadcastFix(uint8_t* data, size_t len, bool ecio)
{
  IM920Parser::generate_command(PSTR("TXDU"), data, len, ecio);
  return command;
}

const char* IM920Parser::setCommandUnicast(unsigned short id, uint8_t* data, size_t len, bool ecio)
{
  IM920Parser::generate_command(PSTR("TXDU"), data, len, ecio, id);
  return command;
}

const char* IM920Parser::setCommandSendback(unsigned short id, uint8_t* data, size_t len, bool ecio)
{
  IM920Parser::generate_command(PSTR("TXSB"), data, len, ecio, id);
  return command;
}

const char* IM920Parser::setCommandDelegate(unsigned short id, uint8_t* data, size_t len, bool ecio)
{
  IM920Parser::generate_command(PSTR("TXDG"), data, len, ecio, id);
  return command;
}

void IM920Parser::generate_command(const char* cmd, uint8_t* data, size_t len, bool ecio, unsigned short node)
{
  // once clear command message
  for(int i = 0; i < max_len_command; i++) command[i] = '\0';

  // whether the command uni-cast mode
  bool is_uni = node > 1 && node < 0xFFF0;
  // message format
  const char* format = IM920Parser::format_command(is_uni);

  // data size
  const unsigned short len_msg = ecio ? len + 1 : 2 * len + 1;
  // format size (with CR&LF)
  const unsigned short len_cmd = is_uni ? 
    IM920Parser::num_command + IM920Parser::num_node_id + 2 + len_msg + 4 : // unicast
    IM920Parser::num_command + len_msg + 3;  // broadcast

  // data packet
  char msg[len_msg];
  // convert to hex
  for(int i = 0; i < len; i++){
    if(ecio){ // ascii
      msg[i] = (char)data[i];
    }
    else{ // 2-digit hex
      msg[2 * i] = programming::helper::convertIntToHexChar((data[i] >> 4) & 0x0F);
      msg[2 * i + 1] = programming::helper::convertIntToHexChar(data[i] & 0x0F);
    }
  }
  msg[len_msg - 1] = '\0';

  // create command message
  if(is_uni){ // unicast
    snprintf_P(command, len_cmd, format, cmd, node, msg);
  }
  else{ // hex
    snprintf_P(command, len_cmd, format, cmd, msg);
  }
}

IM920Parser IM920sL;