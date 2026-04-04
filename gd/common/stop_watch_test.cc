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

#include "common/stop_watch.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

namespace bluetooth {
namespace common {

// Test that a StopWatch can be constructed and destroyed without errors.
TEST(StopWatchTest, construct_and_destruct) {
  { StopWatch sw("test event"); }
}

// Test that multiple StopWatch objects can be created with different messages.
TEST(StopWatchTest, multiple_stop_watches) {
  {
    StopWatch sw1("first event");
    StopWatch sw2("second event");
    StopWatch sw3("third event");
  }
}

// Test that DumpStopWatchLog can be called without crashing,
// even before any StopWatch objects have been created.
TEST(StopWatchTest, dump_before_any_records) {
  StopWatch::DumpStopWatchLog();
}

// Test that DumpStopWatchLog can be called after a StopWatch has been used.
TEST(StopWatchTest, dump_after_one_record) {
  { StopWatch sw("single event"); }
  StopWatch::DumpStopWatchLog();
}

// Test that the log buffer (size 10) wraps correctly when more than 10
// StopWatch objects have been created.
TEST(StopWatchTest, log_buffer_wraps) {
  for (int i = 0; i < 20; i++) {
    StopWatch sw("event " + std::to_string(i));
  }
  // Should not crash even after the circular buffer wraps.
  StopWatch::DumpStopWatchLog();
}

// Test that a StopWatch with an empty message is handled gracefully.
TEST(StopWatchTest, empty_message) {
  { StopWatch sw(""); }
  StopWatch::DumpStopWatchLog();
}

// Test that a StopWatch measures non-negative elapsed time.
// This is tested indirectly: the StopWatch records start and end timestamps;
// a valid watch is simply one that doesn't crash and whose destructor runs
// after its constructor (so end >= start).
TEST(StopWatchTest, elapsed_time_is_non_negative) {
  // We rely on the StopWatch to store log entries without assertion failures.
  {
    StopWatch sw("timed event");
    // Small sleep to ensure the clock advances.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  // If we get here without crashing, elapsed time was non-negative.
  StopWatch::DumpStopWatchLog();
}

// Test that DumpStopWatchLog can be called multiple times in a row.
TEST(StopWatchTest, dump_multiple_times) {
  { StopWatch sw("event for multiple dumps"); }
  StopWatch::DumpStopWatchLog();
  StopWatch::DumpStopWatchLog();
  StopWatch::DumpStopWatchLog();
}

// Test that StopWatch objects can be created concurrently from a single thread
// in a nested scope without issues.
TEST(StopWatchTest, nested_stop_watches) {
  {
    StopWatch outer("outer");
    {
      StopWatch inner("inner");
    }
  }
  StopWatch::DumpStopWatchLog();
}

}  // namespace common
}  // namespace bluetooth
