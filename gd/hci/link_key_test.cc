/*
 * Copyright 2020 The Android Open Source Project
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

#include "hci/link_key.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace bluetooth {
namespace hci {

// The expected bytes of kExampleLinkKey from the Bluetooth spec example.
static const uint8_t kExampleLinkKeyBytes[16] = {
    0x4C, 0x68, 0x38, 0x41, 0x39, 0xf5, 0x74, 0xd8,
    0x36, 0xbc, 0xf3, 0x4e, 0x9d, 0xfb, 0x01, 0xbf};

TEST(LinkKeyTest, example_link_key_has_correct_length) {
  ASSERT_EQ(16u, LinkKey::kLength);
}

TEST(LinkKeyTest, example_link_key_bytes_match_spec) {
  for (size_t i = 0; i < LinkKey::kLength; i++) {
    ASSERT_EQ(kExampleLinkKeyBytes[i], kExampleLinkKey.bytes[i])
        << "Mismatch at byte index " << i;
  }
}

TEST(LinkKeyTest, default_constructor_produces_zero_key) {
  LinkKey key = {};
  for (size_t i = 0; i < LinkKey::kLength; i++) {
    ASSERT_EQ(0x00, key.bytes[i]) << "Non-zero byte at index " << i;
  }
}

TEST(LinkKeyTest, equality_operator) {
  LinkKey key1 = kExampleLinkKey;
  LinkKey key2 = kExampleLinkKey;
  ASSERT_EQ(key1, key2);
}

TEST(LinkKeyTest, inequality_operator) {
  LinkKey key1 = kExampleLinkKey;
  LinkKey key2 = {};
  ASSERT_NE(key1, key2);
}

TEST(LinkKeyTest, comparison_operators) {
  LinkKey key_zero = {};
  LinkKey key_example = kExampleLinkKey;
  // key_zero.bytes[0] == 0x00, key_example.bytes[0] == 0x4C
  // std::array comparison is lexicographic
  ASSERT_TRUE(key_zero < key_example);
  ASSERT_FALSE(key_example < key_zero);
  ASSERT_TRUE(key_zero <= key_example);
  ASSERT_TRUE(key_example >= key_zero);
}

TEST(LinkKeyTest, to_string_returns_hex) {
  std::string s = kExampleLinkKey.ToString();
  // Should be 32 hex chars for 16 bytes
  ASSERT_EQ(32u, s.length());
  ASSERT_EQ("4c68384139f574d836bcf34e9dfb01bf", s);
}

TEST(LinkKeyTest, from_string_roundtrip) {
  std::string hex = kExampleLinkKey.ToString();
  auto parsed = LinkKey::FromString(hex);
  ASSERT_TRUE(parsed.has_value());
  ASSERT_EQ(kExampleLinkKey, *parsed);
}

TEST(LinkKeyTest, from_string_invalid_length) {
  // Too short
  auto result = LinkKey::FromString("4c6838");
  ASSERT_FALSE(result.has_value());
}

TEST(LinkKeyTest, from_string_invalid_chars) {
  // Non-hex characters
  auto result = LinkKey::FromString("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
  ASSERT_FALSE(result.has_value());
}

TEST(LinkKeyTest, from_array_constructor) {
  LinkKey key(kExampleLinkKeyBytes);
  ASSERT_EQ(kExampleLinkKey, key);
}

TEST(LinkKeyTest, copy_constructor) {
  LinkKey original = kExampleLinkKey;
  LinkKey copy = original;
  ASSERT_EQ(original, copy);
}

TEST(LinkKeyTest, assignment_operator) {
  LinkKey key = {};
  key = kExampleLinkKey;
  ASSERT_EQ(kExampleLinkKey, key);
}

}  // namespace hci
}  // namespace bluetooth
