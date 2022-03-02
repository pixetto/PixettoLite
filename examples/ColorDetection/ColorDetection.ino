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
#include <SoftwareSerial.h>
#include <PixettoLite.h>

SoftwareSerial pxtSerial(10, 11); // RX, TX

char pxtbuf[PXT_BUF_SIZE];
struct pxt_data* pxtdata = (struct pxt_data*)pxtbuf;

void setup()
{
  Serial.begin(38400);
  pxtSerial.begin(38400);
  Serial.println(pxtGetVersion(pxtSerial), HEX);
  pxtSetFunc(pxtSerial, FUNC_COLOR_DETECTION);
}

void loop()
{
  char s[32];
  int n = pxtAvailable(pxtSerial);
  while (n--) {
    if (pxtGetData(pxtSerial, pxtbuf, PXT_BUF_SIZE) > 0) {
      sprintf(s, "%02d %d %d %d %d", pxtdata->class_id,
              pxtdata->x, pxtdata->y, pxtdata->w, pxtdata->h);
      Serial.println(s);
    } 
  }
}
