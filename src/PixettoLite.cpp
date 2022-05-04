/*
 * Copyright 2022 VIA Technologies, Inc. All Rights Reserved.
 *
 * This PROPRIETARY SOFTWARE is the property of VIA Technologies, Inc.
 * and may contain trade secrets and/or other confidential information of
 * VIA Technologies, Inc. This file shall not be disclosed to any third
 * party, in whole or in part, without prior written consent of VIA.
 *
 * THIS PROPRIETARY SOFTWARE AND ANY RELATED DOCUMENTATION ARE PROVIDED AS IS,
 * WITH ALL FAULTS, AND WITHOUT WARRANTY OF ANY KIND EITHER EXPRESS OR IMPLIED,
 * AND VIA TECHNOLOGIES, INC. DISCLAIMS ALL EXPRESS OR IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
 * ENJOYMENT OR NON-INFRINGEMENT.
*/

#include "PixettoLite.h"

static bool debug = false;
static int s_func_id = 0;

/*
static bool is_valid(unsigned char* buf, int len)
{
  return (buf[0] == PXT_PACKET_START &&
          buf[len - 1] == PXT_PACKET_END &&
          buf[1] == len);
}
*/

static void dbg_dump(unsigned char* buf, int len)
{
  if (debug == false)
    return;

  if (len > 0)
    Serial.write("{ ");

  for (int i = 0; i < len; i++) {
    Serial.print(buf[i], HEX);
    if (buf[i] != PXT_PACKET_END && i != (len - 1))
      Serial.write(", ");
    else
      Serial.println(" }");
  }
}

static void cksum(uint8_t *buf, int len)
{
  uint8_t sum = 0;
  for (int i = 1; i < len - 2; i++)
    sum += buf[i];
  sum %= 256;
  buf[len - 2] = sum;
}

static int getdata(Stream& serial, uint8_t* buf, int buflen)
{
  buf[0] = 0;
  memset(buf, 0, buflen);

  while (buf[0] != PXT_PACKET_START) {
    int n = serial.readBytes(&buf[0], 1);
    if (n <= 0)
      break;
  }
  if (buf[0] == PXT_PACKET_START) {
    serial.readBytes(&buf[1], 1);
    int n = buf[1];
    if (n <= PXT_BUF_SIZE) {
      serial.readBytes(&buf[2], n - 2);
      if (buf[n - 1] == PXT_PACKET_END) {
        // dbg_dump(buf, buflen);
        return buf[1];
      }
    }
  }
  // invalid data
  dbg_dump(buf, buflen);

  return 0;
}

uint16_t pxt_le16(uint8_t a, uint8_t b)
{
  uint16_t val = a;
  val <<= 8;
  val += b;
  return val;
}

uint32_t pxt_le32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  uint32_t val = a;
  val <<= 8;
  val += b;
  val <<= 8;
  val += c;
  val <<= 8;
  val += d;
  return val;
}

static void pxtWait(Stream& serial)
{
  static bool pxt_is_ready = false;
  while (!pxt_is_ready) {
    if (pxtGetVersion(serial) > 0)
      pxt_is_ready = true;
    else
      delay(100);
  }
}

unsigned long pxtGetVersion(Stream& serial)
{
  unsigned long vers = 0;

  while (vers == 0) {
    // Send command
    uint8_t cmd[] = { PXT_PACKET_START, 0x05, PXT_CMD_GET_VERSION, 0, PXT_PACKET_END };
    serial.write(cmd, sizeof(cmd));
    serial.flush();

    // Get result
    // ex: { FD, 9, E3, 1, 1, 6, 1, F5, FE }

    uint8_t buf[PXT_BUF_SIZE];
    int len = 0;
    if ((len = getdata(serial, buf, PXT_BUF_SIZE)) > 0) {
      if (buf[2] == PXT_CMD_GET_VERSION || buf[2] == 0xE3) {
        struct pxt_data* p = (struct pxt_data*) buf;
        vers = p->version;
        // Serial.println(vers, HEX);
      }
    }
  }
  return vers;
}

void pxtSetFunc(Stream& serial, int id)
{
  pxtWait(serial);

  if (s_func_id == id || id < 0)
    return;

  uint8_t cmd[] = { PXT_PACKET_START, 0x06, PXT_CMD_SET_FUNC, uint8_t(id), 0, PXT_PACKET_END };
  cksum(cmd, sizeof(cmd));
  serial.write(cmd, sizeof(cmd));
  serial.flush();

  s_func_id = id;
  delay(20);
}

int pxtGetData(Stream& serial, byte* buf, int buflen)
{
  uint8_t cmd[] = { PXT_PACKET_START, 0x05, PXT_CMD_GET_DATA, 0, PXT_PACKET_END };
  cksum(cmd, sizeof(cmd));
  serial.write(cmd, sizeof(cmd));
  serial.flush();
  int len;

  struct pxt_data* p = (struct pxt_data*) buf;

  while ((len = getdata(serial, buf, buflen)) > 0) {
    if (p->cmd == PXT_CMD_GET_DATA && len != 5)
      break;
  }

  return len;
}

int pxtAvailable(Stream& serial)
{
  pxtWait(serial);

  uint8_t cmd[] = { PXT_PACKET_START, 0x05, PXT_CMD_GET_OBJNUM, 0, PXT_PACKET_END };
  cksum(cmd, sizeof(cmd));
  serial.write(cmd, sizeof(cmd));
  serial.flush();

  uint8_t buf[PXT_BUF_SIZE];
  memset(buf, 0, PXT_BUF_SIZE);

  struct pxt_data* p = (struct pxt_data*) buf;
  int len = 0;

  while ((len = getdata(serial, buf, PXT_BUF_SIZE)) > 0) {
    if (p->cmd == PXT_CMD_GET_OBJNUM || p->cmd == 0x46) {
      int n = p->retcode;
      // Serial.print("pxtAvailable: ");
      // Serial.println(n);
      return n;
    }
  }
  return 0;
}
