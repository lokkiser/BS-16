
#include "modbus_slave_util.h"
#include <stdlib.h>
//#include "stm_util.h"
//#include "main.h"





void modbus_slave_send_packet(uint8_t * buf, uint16_t len, uint16_t buf_size)
{
	uint16_t crc;
	uint16_t packet_len;

	packet_len = len + sizeof(uint16_t);
	if (packet_len > buf_size) return;

	crc = modbus_crc(buf, len);

	buf[len] = crc & 0xFF;
	buf[len + 1] = crc >> 8;

	modbus_slave_send(buf, packet_len);
}


void modbus_slave_send_error(uint8_t address, uint8_t function_code, uint8_t error_code, uint8_t * buf, uint16_t buf_size)
{
	buf[0] = address;
	buf[1] = 0x80 | function_code;
	buf[2] = error_code;

	modbus_slave_send_packet(buf, 3, buf_size);
}


void modbus_slave_recv(uint8_t * packet, uint16_t packet_len, uint8_t * buf, uint16_t buf_size)
{

  uint8_t address;
  uint8_t function_code;
  uint16_t reference_number;
  uint16_t cnt;
  uint8_t res;
  uint16_t i, j, k;
  uint16_t value;
  uint8_t on;
  uint8_t byte_num, bit_num;




  if ((packet_len < 8) || !check_modbus_crc(packet, packet_len)) return;
  address = packet[0];
  function_code = packet[1];
  reference_number = (packet[2] << 8) | packet[3];
  cnt = (packet[4] << 8) |  packet[5];


  switch (function_code) {

    case MQF_READ_COIL_STATUS:
    case MQF_READ_DISCRETE_INPUTS:

    	if (!cnt || (cnt > 128)) {
    		modbus_slave_send_error(address, function_code, MBE_DATA_NOT_AVIALIBLE, buf, buf_size);
    		return;
    	}

    	buf[0] = address;
    	buf[1] = function_code;
    	buf[2] = 1 + cnt / 8; //byte_count

    	if (2 + buf[2] + sizeof(uint16_t) > buf_size) {
    		modbus_slave_send_error(address, function_code, MBE_DATA_NOT_AVIALIBLE, buf, buf_size);
    		return;
    	}

    	for (i = 0; i < cnt; ++i) {
    		res = modbus_slave_read_coil(address, function_code, reference_number + i, &on);
    		if (res) {
    			modbus_slave_send_error(address, function_code, res, buf, buf_size);
    			return;
    		}
    		byte_num = 3 + (i / 8);
    		bit_num = i % 8;
    		if (on) SETBIT(buf[byte_num], bit_num);
    		else CLRBIT(buf[byte_num], bit_num);
    	}

    	modbus_slave_send_packet(buf, 3 + buf[2], buf_size);
    break;


    case MQF_READ_INPUT_REGISTERS:
    case MQF_READ_HOLDING_REGISTERS:

  	  if (!cnt || (cnt > 128)) {
  		  modbus_slave_send_error(address, function_code, MBE_DATA_NOT_AVIALIBLE, buf, buf_size);
  		  return;
  	  }

  	  buf[0] = address;
  	  buf[1] = function_code;
  	  buf[2] = cnt * 2 ; ///byte_count

  	  if (2 + buf[2] + sizeof(uint16_t) > buf_size) {
  		  modbus_slave_send_error(address, function_code, MBE_DATA_NOT_AVIALIBLE, buf, buf_size);
  		  return;
  	  }


  	  j = 3;
  	  for (i = 0; i < cnt; ++i) {
  		  res = modbus_slave_read_register(address, function_code, reference_number + i, &value);
  		  if (res) {
  			  modbus_slave_send_error(address, function_code, res, buf, buf_size);
  			  return;
  		  }

  	   	  buf[j] = value >> 8;
  	   	  buf[j + 1] = value & 0xFF;
  	   	  j += 2;
  	  }
  	  modbus_slave_send_packet(buf, 3 + buf[2],  buf_size);
    break;

    case MQF_WRITE_FORCE_SINGLE_COIL:
    case MQF_WRITE_SINGLE_REGISTER:

        if (function_code == MQF_WRITE_FORCE_SINGLE_COIL) {
        	res = modbus_slave_write_coil(address, function_code, reference_number, cnt ? 1 : 0);
        } else {
            res = modbus_slave_write_register(address, function_code, reference_number, cnt);
        }

        if (res) {
            modbus_slave_send_error(address, function_code, res, buf, buf_size);
        	return;
        }

        modbus_slave_send(packet, packet_len); //ответ тот же
    break;


   case MQF_WRITE_FORCE_MULTIPLE_COILS:



	   if (!cnt || (cnt > 128) || (buf_size < 8)) {
		  modbus_slave_send_error(address, function_code, MBE_DATA_NOT_AVIALIBLE, buf, buf_size);
		  return;
	   }



       j = 0;
       res = 0;
       for (i = 0; i < cnt; ++i) {
         byte_num = 7 + i / 8;
         bit_num = i % 8;
         res = modbus_slave_write_coil(address, function_code, reference_number + i, GETBIT(packet[byte_num], bit_num));
         if (res) continue;
         j++;
       }

       if (!j) {
           modbus_slave_send_error(address, function_code, res, buf, buf_size);
           return;
       }


	   //
       //modbus_read_t
       buf[0] = address;
       buf[1] = function_code;
       buf[2] = reference_number << 8;
       buf[3] = reference_number & 0xFF;
       buf[4] = j << 8;
       buf[5] = j & 0xFF;

       modbus_slave_send_packet(buf, 6,  buf_size);
  break;



  case MQF_WRITE_PRESET_MULTIPLI_REGISTERS:

	  //modbus_write_header_t
	  if (!cnt || (cnt > 128) || (buf_size < 8)) {
		  modbus_slave_send_error(address, function_code, MBE_DATA_NOT_AVIALIBLE, buf, buf_size);
		  return;
	  }


	  res = 0;
	  j = 0;
	  k = 7;
	  for (i = 0; i < cnt; ++i) {
		  value = (packet[k] << 8) | packet[k + 1];
		  k += 2;
		  res = modbus_slave_write_register(address, function_code, reference_number + i, value);
		  if (res) continue;
		  j++;
	  }

	  if (!j) {
		  modbus_slave_send_error(address, function_code, res, buf, buf_size);
		  return;
	  }


	  buf[0] = address;
      buf[1] = function_code;
      buf[2] = reference_number << 8;
	  buf[3] = reference_number & 0xFF;
	  buf[4] = j << 8;
	  buf[5] = j & 0xFF;


      modbus_slave_send_packet(buf, 6,  buf_size);

  break;

  default :
	  modbus_slave_send_error(address, function_code, MBE_FUNCTION_NOT_AVIALIBLE, buf, buf_size);
  }


}
















/*
void modbus_slave_send_error(uint8_t modbus_slave_address, uint8_t function_code, uint8_t error_code)
{
  modbus_error_t mbe;

  create_modbus_error(&mbe, modbus_slave_address, 0x80 | function_code, error_code);
  modbus_slave_send((uint8_t *)&mbe, sizeof(mbe));
}


//static uint8_t buf[260];


void modbus_slave_recv(uint8_t * data, uint16_t data_len)
{

  modbus_read_t * mbr;
  modbus_write_header_t * mwh;
  modbus_response_write_t mw;
  uint16_t reference_number;
  uint16_t cnt; 
  uint8_t res;
  uint16_t i, j;
  uint16_t * regs;
  uint8_t * pbyte;
  uint16_t value, byte_num, bit_num;
  uint8_t on;
  uint8_t buf[260];

  //if (!check_modbus_crc(data, data_len)) {
  //   modbus_slave_send_error(data[0], data[1], MBE_CRC);
  //    return;
  //}


  switch (data[1]) {

    case MQF_READ_COIL_STATUS:
    case MQF_READ_DISCRETE_INPUTS:
    
      mbr = parse_modbus_read(data, data_len);
      if (!mbr) {
          modbus_slave_send_error(data[0], mbr->function_code, MBE_CRC);
    	  return;
      }
    
      reference_number = HTONS(mbr->reference_number);
      cnt = HTONS(mbr->data);

      if (!cnt) { 
         modbus_slave_send_error(data[0], mbr->function_code, MBE_DATA_NOT_AVIALIBLE);
         return;
      }

      pbyte = buf + sizeof(modbus_response_read_header_t);
    
      for (byte_num = 0; byte_num < 1 + ((cnt - 1) / 8); ++byte_num) pbyte[byte_num] = 0;

      res = 0;
      j = 0;
      for (i = 0; i < cnt; ++i) {
        res = modbus_slave_read_coil(data[0], data[1], reference_number + i, &on);
        if (res) continue;
        byte_num = i / 8;
        bit_num = i - byte_num * 8;
        if (on) SETBIT(pbyte[byte_num], bit_num);
        j++;
      }


      if (!j) {
           modbus_slave_send_error(data[0], mbr->function_code, res);
           return;
      }
    
      cnt = create_modbus_response_read(buf, sizeof(buf), data[0], mbr->function_code, pbyte,  1 + ((j - 1) / 8));
      modbus_slave_send(buf, cnt);    
      break;
      
    case MQF_WRITE_FORCE_SINGLE_COIL:
      mbr = parse_modbus_read(data, data_len);
      if (!mbr) {
          modbus_slave_send_error(data[0], mbr->function_code, MBE_CRC);
    	  return;
      }

      reference_number = HTONS(mbr->reference_number);
      cnt = HTONS(mbr->data);

      res = modbus_slave_write_coil(data[0], data[1], reference_number, cnt ? 1 : 0);

      if (res) {
         modbus_slave_send_error(data[0], mbr->function_code, res);
         return;
      }
    
      modbus_slave_send(data, data_len); //ответ тот же    
    break;

   case MQF_WRITE_FORCE_MULTIPLE_COILS:   
     mwh = parse_modbus_write_header(data, data_len);
     if (!mwh) return;
   
     reference_number = HTONS(mwh->reference_number);
     cnt = HTONS(mwh->count);

     pbyte = (uint8_t *)(data + sizeof(modbus_write_header_t));

     res = 0;
     j = 0;
     for (i = 0; i < cnt; ++i) {
       byte_num = i / 8;
       bit_num = i - byte_num * 8;      
       res = modbus_slave_write_coil(data[0], data[1], reference_number + i, GETBIT(pbyte[i], bit_num));
       if (res) continue;
       j++;
     }    

     if (!j) {
        modbus_slave_send_error(data[0], mwh->function_code, res);
        return;
     }
     
     create_modbus_response_write(&mw, data[0], mwh->function_code, reference_number, j);
     modbus_slave_send((uint8_t *)&mw, sizeof(mw));
  break;  
    
  case MQF_WRITE_SINGLE_REGISTER:
    mbr = parse_modbus_read(data, data_len);
    if (!mbr) {
        modbus_slave_send_error(data[0], mbr->function_code, MBE_CRC);
        return;
    }
    
    reference_number = HTONS(mbr->reference_number);
    cnt = HTONS(mbr->data);
    
    res = modbus_slave_write_register(data[0], data[1], reference_number, cnt);
    if (res) {
       modbus_slave_send_error(data[0], mbr->function_code, res);
       return;
    }

    modbus_slave_send(data, data_len); //ответ тот же    
  break;

  case MQF_READ_INPUT_REGISTERS:
  case MQF_READ_HOLDING_REGISTERS:
    mbr = parse_modbus_read(data, data_len);
    if (!mbr) {
        modbus_slave_send_error(data[0], mbr->function_code, MBE_CRC);
        return;
    }
    
    reference_number = HTONS(mbr->reference_number);
    cnt = HTONS(mbr->data);

    if (cnt > 128) {
        modbus_slave_send_error(data[0], mbr->function_code, MBE_DATA_NOT_AVIALIBLE);
        return;
    }


    regs = (uint16_t *)(buf + sizeof(modbus_response_read_header_t));

    for (i = 0; i < cnt; ++i) {
      res = modbus_slave_read_register(data[0], data[1], reference_number + i, &value);
      if (res) {
         modbus_slave_send_error(data[0], mbr->function_code, res);
         return;
      }
      regs[i] = HTONS(value);
    }
    

    cnt = create_modbus_response_read(buf, sizeof(buf), data[0], mbr->function_code, (uint8_t *)regs, cnt * 2);
    modbus_slave_send(buf, cnt);
  break;

  case MQF_WRITE_PRESET_MULTIPLI_REGISTERS:
    mwh = parse_modbus_write_header(data, data_len);
    if (!mwh) {
        modbus_slave_send_error(data[0], mwh->function_code, MBE_CRC);
        return;
    }
   
    reference_number = HTONS(mwh->reference_number);
    cnt = HTONS(mwh->count);

    if (!cnt) { 
       modbus_slave_send_error(data[0], mwh->function_code, MBE_DATA_NOT_AVIALIBLE);
       return;
    }

    regs = (uint16_t *)(data + sizeof(modbus_write_header_t));

    res = 0;
    j = 0;
    for (i = 0; i < cnt; ++i) {
      res = modbus_slave_write_register(data[0], data[1], reference_number + i, HTONS(regs[i]));
      if (res) continue;
      j++;
    }

    if (!j) {
         modbus_slave_send_error(data[0], mwh->function_code, res);
         return;
    }
    
    create_modbus_response_write(&mw, data[0], mwh->function_code, reference_number, j);
    modbus_slave_send((uint8_t *)&mw, sizeof(mw));
  break;
  
  default :
      modbus_slave_send_error(data[0], data[1], MBE_FUNCTION_NOT_AVIALIBLE);
  }


}
*/
