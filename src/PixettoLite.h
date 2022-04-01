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

#include <Arduino.h>

#define PXT_PACKET_START        0xFD
#define PXT_PACKET_END          0xFE
#define PXT_CMD_GET_VERSION     0xD0
#define PXT_CMD_SET_FUNC        0xD1
#define PXT_CMD_GET_OBJNUM      0xD2
#define PXT_CMD_GET_DATA        0xD4

#define PXT_BUF_SIZE            64

enum {
  FUNC_COLOR_DETECTION                = 1,
  FUNC_COLOR_CODE_DETECTION           = 2,
  FUNC_SHAPE_DETECTION                = 3,
  FUNC_SPHERE_DETECTION               = 4,
  FUNC_TEMPLATE_MATCHING              = 6,
  FUNC_KEYPOINTS                      = 8,
  FUNC_NEURAL_NETWORK                 = 9,

  FUNC_APRILTAG                       = 10,

  FUNC_FACE_DETECTION                 = 11,
  FUNC_TRAFFIC_SIGN_DETECTION         = 12,
  FUNC_HANDWRITTEN_DIGITS_DETECTION   = 13,
  FUNC_HANDWRITTEN_LETTERS_DETECTION  = 14,
  FUNC_CLOUD_DETECTION                = 15,  // not supported by harp-cam

  FUNC_LANES_DETECTION                = 16,
  FUNC_EQUATION_DETECTION             = 17,
  FUNC_SIMPLE_CLASSIFIER              = 18,  // not supported by harp-cam
  FUNC_VOICE_COMMAND                  = 19,  // not supported by harp-cam
  FUNC_AUTONOMOUS_DRIVING             = 20   // not supported by harp-cam
};

struct pxt_data {
  // head +4
  uint8_t head;
  uint8_t len;
  uint8_t cmd;
  uint8_t retcode;

  // version +4
  uint32_t version; // 0x00010602

  // object id and bounding box +8
  uint8_t  func_id;
  uint8_t  reserved1;
  uint16_t class_id;
  uint8_t x;
  uint8_t y;
  uint8_t w;
  uint8_t h;

  // extra +32
  union {
    struct {
      float pos_x;
      float pos_y;
      float pos_z;

      float rot_x;
      float rot_y;
      float rot_z;

      float center_x;
      float center_y;
    } apltag; // +32

    struct {
      uint8_t left_x1;
      uint8_t left_y1;
      uint8_t left_x2;
      uint8_t left_y2;

      uint8_t right_x1;
      uint8_t right_y1;
      uint8_t right_x2;
      uint8_t right_y2;

      uint8_t sign_x;
      uint8_t sign_y;
      uint8_t sign_w;
      uint8_t sign_h;
    } traffic; // +12

    struct {
      float result;
      char equation[16];
    } math; // +20

    uint8_t bytes[32];
  } extra;

  // End +4
  uint8_t reserved[2];
  uint8_t cksum;
  uint8_t tail;
};

unsigned long pxtGetVersion(Stream& serial);
void pxtSetFunc(Stream& serial, int id);
int pxtAvailable(Stream& serial);
int pxtGetData(Stream& serial, byte* buf, int buflen);
