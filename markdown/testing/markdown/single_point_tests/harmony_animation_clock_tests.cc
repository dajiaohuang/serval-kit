// Copyright 2026 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include "gtest/gtest.h"

#include "../mock_platform/markdown_tests_platform.h"
#include "../mock_platform/mock_markdown_platform_view.h"
#include "markdown/platform/harmony/internal/harmony_animation_clock.h"

namespace serval::markdown {
namespace {

class ClockDrivenDocument {
 public:
  ClockDrivenDocument()
      : main_view_(testing::CreateTestMarkdownSharedContext()) {
    auto* view = main_view_.GetMarkdownView();
    view->SetContent("abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz");
    view->SetAnimationType(MarkdownAnimationType::kTypewriter);
    view->SetAnimationVelocity(10);
    view->SetTypewriterDynamicHeight(false);
    main_view_.Measure(Spec());
  }

  void Frame(int64_t time_ms) {
    clock_.OnFrame(time_ms * 1000000);
    auto* view = main_view_.GetMarkdownView();
    if (!clock_.IsPaused()) {
      view->OnLayoutFrame(clock_.GetAnimationTimeMs());
    }
    main_view_.Measure(Spec());
    view->OnRendererFrame(time_ms);
  }

  int32_t RenderedStep() const {
    return main_view_.GetMarkdownView()->GetRenderedAnimationStep();
  }

  HarmonyAnimationClock clock_;

 private:
  static MeasureSpec Spec() {
    return {.width_ = 200,
            .width_mode_ = tttext::LayoutMode::kDefinite,
            .height_ = MeasureSpec::LAYOUT_MAX_SIZE,
            .height_mode_ = tttext::LayoutMode::kIndefinite};
  }

  testing::MockMarkdownMainView main_view_;
};

}  // namespace

TEST(HarmonyAnimationClockTest, TypewriterResumesWithoutJumpingOverPausedTime) {
  ClockDrivenDocument continuous;
  ClockDrivenDocument paused;
  continuous.Frame(1000);
  paused.Frame(1000);
  continuous.Frame(1200);
  paused.Frame(1200);
  const auto before_pause = paused.RenderedStep();
  ASSERT_GT(before_pause, 0);

  paused.clock_.Pause();
  paused.Frame(30000);
  paused.Frame(40000);
  EXPECT_EQ(paused.RenderedStep(), before_pause);

  paused.clock_.Resume();
  paused.Frame(40200);
  continuous.Frame(1400);
  EXPECT_EQ(paused.RenderedStep(), continuous.RenderedStep());
  EXPECT_GT(paused.RenderedStep(), before_pause);
}

TEST(HarmonyAnimationClockTest,
     SupportsPausingBeforeFirstFrameAndRepeatedPause) {
  ClockDrivenDocument continuous;
  ClockDrivenDocument paused;
  paused.clock_.Pause();
  paused.Frame(30000);
  EXPECT_EQ(paused.RenderedStep(), 0);
  paused.clock_.Resume();
  paused.Frame(31000);
  continuous.Frame(1000);
  EXPECT_EQ(paused.RenderedStep(), continuous.RenderedStep());

  paused.clock_.Pause();
  paused.Frame(35000);
  paused.clock_.Pause();
  paused.Frame(50000);
  paused.clock_.Resume();
  paused.clock_.Resume();
  paused.Frame(50200);
  continuous.Frame(1200);
  EXPECT_EQ(paused.RenderedStep(), continuous.RenderedStep());
}

}  // namespace serval::markdown
