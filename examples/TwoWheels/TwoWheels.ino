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

/* Control two wheels to follow the lane with Pixetto and L298P shield. */

#include <SoftwareSerial.h>
#include <PixettoLite.h>

SoftwareSerial pxtSerial(A0, A1); // RX, TX

byte pxtbuf[PXT_BUF_SIZE];
struct pxt_data* pxtdata = (struct pxt_data*)pxtbuf;

// -----------------------------------------------------------------------------

const unsigned int M1 = 12; /* right */
const unsigned int M2 = 13; /* left */
const unsigned int E1 = 10;
const unsigned int E2 = 11;
const unsigned int min_speed = 128;
const unsigned int max_speed = 255;

void stop_2()
{
  analogWrite(E1, 0);
  analogWrite(E2, 0);
}

void forward(unsigned int duration)
{
  const int s1 = max(min_speed, max_speed);
  const int s2 = max(min_speed, max_speed);

  digitalWrite(M1, 1);
  digitalWrite(M2, 1);
  analogWrite(E1, s1);
  analogWrite(E2, s2);

  if (duration) {
    delay(duration);
    stop_2();
  }
}

void backward(unsigned int duration)
{
  const int s1 = max(min_speed, 128);
  const int s2 = max(min_speed, 128);

  digitalWrite(M1, 0);
  digitalWrite(M2, 0);
  analogWrite(E1, s1);
  analogWrite(E2, s2);

  if (duration) {
    delay(duration);
    stop_2();
  }
}

void turn_left(unsigned int duration, bool hold)
{
  const int s1 = max(min_speed, 128);
  const int s2 = max(min_speed, 128);

  if (hold) {
    digitalWrite(M1, 1);
    digitalWrite(M2, 0);
    analogWrite(E1, s1);
    analogWrite(E2, s2);
  } else {
    digitalWrite(M1, 1);
    digitalWrite(M2, 1);
    analogWrite(E1, max_speed);
    analogWrite(E2, s2);
  }
  if (duration) {
    delay(duration);
    stop_2();
  }
}

void turn_right(unsigned int duration, bool hold)
{
  const int s1 = max(min_speed, 128);
  const int s2 = max(min_speed, 128);
  if (hold) {
    digitalWrite(M1, 0);
    digitalWrite(M2, 1);
    analogWrite(E1, s1);
    analogWrite(E2, s2);
  } else {
    digitalWrite(M1, 1);
    digitalWrite(M2, 1);
    analogWrite(E1, s1);
    analogWrite(E2, max_speed);
  }
  if (duration) {
    delay(duration);
    stop_2();
  }
}

// -----------------------------------------------------------------------------

void setup()
{
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);

  pxtSerial.begin(38400);
  pxtSetFunc(pxtSerial, FUNC_LANES_DETECTION);
}

void loop()
{
  static unsigned long t1 = 0;

  int n = pxtAvailable(pxtSerial);

  if (n) {
    while (n--) {
      if (pxtGetData(pxtSerial, pxtbuf, PXT_BUF_SIZE) > 0) {
        bool hold = pxtdata->y > 50 && (pxtdata->x < 20 || pxtdata->x > 80);
        int duration = hold ? 100 : 0;
        if (pxtdata->x < 47) {
          turn_left(duration, hold);
        } else if (pxtdata->x > 53) {
          turn_right(duration, hold);
        } else {
          forward(0);
        }
      }
    }
    t1 = millis();
  }
  else {
    unsigned long t2 = millis() - t1;

    if (t2 > 15000) {
      stop_2();   // stop moving
    } else if (t2 > 5000) {
      forward(0); // stop turning
    }
  }
}
