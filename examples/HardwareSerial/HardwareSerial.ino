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
#include <PixettoLite.h>

HardwareSerial& pxtSerial = Serial; // RX, TX = { 0, 1 }

byte pxtbuf[PXT_BUF_SIZE];
struct pxt_data* pxtdata = (struct pxt_data*)pxtbuf;

int ledPin = 13;

void setup()
{
  pxtSerial.begin(38400);
  pxtSetFunc(pxtSerial, FUNC_COLOR_DETECTION);
}

void loop()
{
  int n = pxtAvailable(pxtSerial);
  if (n == 0)
    digitalWrite(ledPin, LOW);
  while (n--) {
    if (pxtGetData(pxtSerial, pxtbuf, PXT_BUF_SIZE) > 0) {
      digitalWrite(ledPin, HIGH);
    }
  }
}
