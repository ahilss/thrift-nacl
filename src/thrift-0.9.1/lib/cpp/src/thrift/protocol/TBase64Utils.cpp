/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <thrift/protocol/TBase64Utils.h>

#include <boost/static_assert.hpp>

using std::string;

namespace apache { namespace thrift { namespace protocol {


static const uint8_t *kBase64EncodeTable = (const uint8_t *)
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void  base64_encode(const uint8_t *in, uint32_t len, uint8_t *buf) {
  buf[0] = kBase64EncodeTable[(in[0] >> 2) & 0x3f];
  if (len == 3) {
    buf[1] = kBase64EncodeTable[((in[0] << 4) & 0x30) | ((in[1] >> 4) & 0x0f)];
    buf[2] = kBase64EncodeTable[((in[1] << 2) & 0x3c) | ((in[2] >> 6) & 0x03)];
    buf[3] = kBase64EncodeTable[in[2] & 0x3f];
  } else if (len == 2) {
    buf[1] = kBase64EncodeTable[((in[0] << 4) & 0x30) | ((in[1] >> 4) & 0x0f)];
    buf[2] = kBase64EncodeTable[(in[1] << 2) & 0x3c];
  } else  { // len == 1
    buf[1] = kBase64EncodeTable[(in[0] << 4) & 0x30];
  }
}

static const uint8_t kBase64DecodeTable[256] ={
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3e,0xff,0xff,0xff,0x3f,
0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xff,0xff,0xff,0xff,0xff,
0xff,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};

void base64_decode(uint8_t *buf, uint32_t len) {
  buf[0] = (kBase64DecodeTable[buf[0]] << 2) |
           (kBase64DecodeTable[buf[1]] >> 4);
  if (len > 2) {
    buf[1] = ((kBase64DecodeTable[buf[1]] << 4) & 0xf0) |
              (kBase64DecodeTable[buf[2]] >> 2);
    if (len > 3) {
      buf[2] = ((kBase64DecodeTable[buf[2]] << 6) & 0xc0) |
                (kBase64DecodeTable[buf[3]]);
    }
  }
}

void base64_decode(const uint8_t *in, uint32_t len, uint8_t *out) {
  out[0] = (kBase64DecodeTable[in[0]] << 2) |
           (kBase64DecodeTable[in[1]] >> 4);
  if (len > 2) {
    out[1] = ((kBase64DecodeTable[in[1]] << 4) & 0xf0) |
              (kBase64DecodeTable[in[2]] >> 2);
    if (len > 3) {
      out[2] = ((kBase64DecodeTable[in[2]] << 6) & 0xc0) |
                (kBase64DecodeTable[in[3]]);
    }
  }
}

void base64EncodeString(const std::string& in, std::string* out) {
  static const size_t kEncodeRemainderSize[3] = {0, 2, 3};

  if (in.empty()) {
    out->clear();
    return;
  }

  size_t in_size = in.size();
  const uint8_t *in_data = (const uint8_t*)in.data();

  size_t remainder = in_size % 3;
  size_t out_size = 4 * (in_size / 3) + kEncodeRemainderSize[remainder];
  uint8_t* out_data = new uint8_t[out_size];

  const uint8_t* in_ptr = in_data;
  uint8_t* out_ptr = out_data;

  const uint8_t* end_ptr = in_data + in_size - remainder;

  while (in_ptr != end_ptr) {
    // Encode 3 bytes at a time
    base64_encode(in_ptr, 3, out_ptr);
    in_ptr += 3;
    out_ptr += 4;
  }

  if (remainder > 0) {
    // Handle remainder
    base64_encode(in_ptr, remainder, out_ptr);
  }

  out->assign((char*)out_data, out_size);
  delete [] out_data;
}

bool base64DecodeString(const std::string& in, std::string* out) {
  static const size_t kDecodeRemainderSize[4] = {0, 0, 1, 2};

  if (in.empty()) {
    out->clear();
    return true;
  }

  size_t in_size = in.size();
  const uint8_t *in_data = (const uint8_t*)in.data();

  size_t remainder = in_size % 4;
  // A single leftover byte is not decodable
  if (remainder == 1) {
    return false;
  }

  size_t out_size = 3 * (in_size / 4) + kDecodeRemainderSize[remainder];
  uint8_t* out_data = new uint8_t[out_size];

  const uint8_t* in_ptr = in_data;
  uint8_t* out_ptr = out_data;

  const uint8_t* end_ptr = in_data + in_size - remainder;

  while (in_ptr != end_ptr) {
    // Decode 4 bytes at a time
    base64_decode(in_ptr, 4, out_ptr);
    in_ptr += 4;
    out_ptr += 3;
  }

  if (remainder > 0) {
    // Handle remainder
    base64_decode(in_ptr, remainder, out_ptr);
  }

  out->assign((char*)out_data, out_size);
  delete [] out_data;

  return true;
}


}}} // apache::thrift::protocol
