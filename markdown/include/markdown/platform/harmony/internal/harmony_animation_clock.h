// Copyright 2026 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#ifndef MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_HARMONY_INTERNAL_HARMONY_ANIMATION_CLOCK_H_
#define MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_HARMONY_INTERNAL_HARMONY_ANIMATION_CLOCK_H_

#include <cstdint>

namespace serval::markdown {

// Keeps layout animation time continuous across pauses while the host's frame
// clock continues to advance. Access on the layout thread only.
class HarmonyAnimationClock {
 public:
  void OnFrame(int64_t frame_time_nanos) {
    current_time_ms_ = frame_time_nanos / 1000000;
  }

  void Pause() {
    if (!paused_) {
      paused_ = true;
      pause_start_time_ms_ = current_time_ms_;
    }
  }

  void Resume() {
    if (paused_) {
      paused_duration_ms_ += current_time_ms_ - pause_start_time_ms_;
      paused_ = false;
    }
  }

  bool IsPaused() const { return paused_; }
  int64_t GetAnimationTimeMs() const {
    return current_time_ms_ - paused_duration_ms_;
  }

 private:
  int64_t current_time_ms_{0};
  int64_t pause_start_time_ms_{0};
  int64_t paused_duration_ms_{0};
  bool paused_{false};
};

}  // namespace serval::markdown

#endif  // MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_HARMONY_INTERNAL_HARMONY_ANIMATION_CLOCK_H_
