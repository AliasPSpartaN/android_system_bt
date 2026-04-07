/*
 * Copyright 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "l2cap/fcs.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

namespace bluetooth {
namespace l2cap {

TEST(L2capFcsTest, initial_checksum_is_zero) {
  Fcs fcs;
  fcs.Initialize();
  ASSERT_EQ(0x0000, fcs.GetChecksum());
}

TEST(L2capFcsTest, checksum_after_zero_byte) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0x00);
  // CRC-16 for [0x00] with CRC-16/ARC algorithm is 0x0000
  ASSERT_EQ(0x0000, fcs.GetChecksum());
}

TEST(L2capFcsTest, checksum_single_byte_0x01) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0x01);
  ASSERT_EQ(0xc0c1, fcs.GetChecksum());
}

TEST(L2capFcsTest, checksum_single_byte_0xff) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0xFF);
  ASSERT_EQ(0x4040, fcs.GetChecksum());
}

TEST(L2capFcsTest, checksum_single_byte_0xaa) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0xAA);
  ASSERT_EQ(0x7f80, fcs.GetChecksum());
}

TEST(L2capFcsTest, checksum_two_bytes_0x01_0x02) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0x01);
  fcs.AddByte(0x02);
  ASSERT_EQ(0x5180, fcs.GetChecksum());
}

TEST(L2capFcsTest, checksum_two_bytes_0xff_0xff) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0xFF);
  fcs.AddByte(0xFF);
  ASSERT_EQ(0xb001, fcs.GetChecksum());
}

TEST(L2capFcsTest, checksum_three_bytes) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0x12);
  fcs.AddByte(0x34);
  fcs.AddByte(0x56);
  ASSERT_EQ(0xfb36, fcs.GetChecksum());
}

TEST(L2capFcsTest, multiple_zero_bytes_checksum_stays_zero) {
  Fcs fcs;
  fcs.Initialize();
  for (int i = 0; i < 10; i++) {
    fcs.AddByte(0x00);
  }
  ASSERT_EQ(0x0000, fcs.GetChecksum());
}

// Verify the FCS self-verification property: appending the FCS bytes
// (low byte first, then high byte) to the original data yields checksum 0.
TEST(L2capFcsTest, self_verification_property) {
  const std::vector<uint8_t> data = {0x01, 0x02};

  Fcs fcs;
  fcs.Initialize();
  for (uint8_t byte : data) {
    fcs.AddByte(byte);
  }
  uint16_t checksum = fcs.GetChecksum();
  uint8_t fcs_lo = checksum & 0xFF;
  uint8_t fcs_hi = (checksum >> 8) & 0xFF;

  // Adding the FCS bytes (LSB first) should yield checksum 0
  fcs.Initialize();
  for (uint8_t byte : data) {
    fcs.AddByte(byte);
  }
  fcs.AddByte(fcs_lo);
  fcs.AddByte(fcs_hi);
  ASSERT_EQ(0x0000, fcs.GetChecksum());
}

TEST(L2capFcsTest, self_verification_property_longer_data) {
  const std::vector<uint8_t> data = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};

  Fcs fcs;
  fcs.Initialize();
  for (uint8_t byte : data) {
    fcs.AddByte(byte);
  }
  uint16_t checksum = fcs.GetChecksum();
  uint8_t fcs_lo = checksum & 0xFF;
  uint8_t fcs_hi = (checksum >> 8) & 0xFF;

  fcs.Initialize();
  for (uint8_t byte : data) {
    fcs.AddByte(byte);
  }
  fcs.AddByte(fcs_lo);
  fcs.AddByte(fcs_hi);
  ASSERT_EQ(0x0000, fcs.GetChecksum());
}

TEST(L2capFcsTest, reinitialize_resets_checksum) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0x01);
  fcs.AddByte(0x02);
  ASSERT_NE(0x0000, fcs.GetChecksum());

  fcs.Initialize();
  ASSERT_EQ(0x0000, fcs.GetChecksum());
}

TEST(L2capFcsTest, reinitialize_and_recompute) {
  Fcs fcs;
  fcs.Initialize();
  fcs.AddByte(0x01);
  fcs.AddByte(0x02);
  uint16_t first_result = fcs.GetChecksum();

  fcs.Initialize();
  fcs.AddByte(0x01);
  fcs.AddByte(0x02);
  uint16_t second_result = fcs.GetChecksum();

  ASSERT_EQ(first_result, second_result);
}

TEST(L2capFcsTest, byte_order_matters) {
  Fcs fcs_ab;
  fcs_ab.Initialize();
  fcs_ab.AddByte(0x01);
  fcs_ab.AddByte(0x02);

  Fcs fcs_ba;
  fcs_ba.Initialize();
  fcs_ba.AddByte(0x02);
  fcs_ba.AddByte(0x01);

  ASSERT_NE(fcs_ab.GetChecksum(), fcs_ba.GetChecksum());
}

}  // namespace l2cap
}  // namespace bluetooth
