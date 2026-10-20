// Copyright 2026 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#ifndef MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_HARMONY_HARMONY_MARKDOWN_MEASURER_H_
#define MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_HARMONY_HARMONY_MARKDOWN_MEASURER_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "markdown/parser/markdown_resource_loader.h"
#include "markdown/platform/harmony/harmony_resource_loader.h"
#include "markdown/platform/harmony/internal/harmony_animation_clock.h"
#include "markdown/utils/markdown_value.h"
#include "markdown/view/markdown_view.h"

namespace serval::markdown {

class NativeServalMarkdownView;

class L_EXPORT NativeMarkdownMeasurer : public MarkdownResourceLoader,
                                        public MarkdownViewMeasureHost {
 public:
  NativeMarkdownMeasurer();
  ~NativeMarkdownMeasurer() override;

  MarkdownView* GetMarkdownView() const { return view_.get(); }
  void SetContent(const std::string& content) const;
  void SetStyle(const ValueMap& style) const;
  void MarkDirty() const;
  void SetConfig(const ValueMap& config) const;
  MeasureResult Measure(MeasureSpec spec);
  void Align(float left, float top);

  // Layout-thread API. Frame timestamps are in nanoseconds, as on Android/iOS.
  void OnLayoutFrame(int64_t frame_time_nanos);
  void PauseAnimation();
  void ResumeAnimation(int32_t animation_step = -1);
  int32_t GetAnimationStep() const;
  void SetAnimationStep(int32_t animation_step) const;
  void OnImageLoaded(const std::string& url);
  void OnFontLoaded(const std::string& family, int32_t weight, int32_t style);
  void SetEventListener(MarkdownEventListener* listener) const;
  void SetExposureListener(MarkdownExposureListener* listener) const;
  void SetNumberProp(MarkdownProps prop, double value) const;
  void SetStringProp(MarkdownProps prop, const std::string& value) const;
  void SetArrayProp(MarkdownProps prop, ValueArray& value) const;
  void SetMapProp(MarkdownProps prop, ValueMap& value) const;

  bool BindView(NativeServalMarkdownView* view);
  void DetachView(NativeServalMarkdownView* view);
  void SetResourceLoader(IHarmonyResourceLoader* loader);
  void SetRequestMeasureCallback(std::function<void()> callback);
  void RequestMeasure() override;

  void* LoadFont(const char* family, MarkdownFontWeight weight) override;
  std::shared_ptr<MarkdownDrawable> LoadInlineView(const char* id_selector,
                                                   float max_width,
                                                   float max_height) override;
  std::shared_ptr<MarkdownDrawable> LoadImage(const char* src,
                                              float desire_width,
                                              float desire_height,
                                              float max_width, float max_height,
                                              float border_radius) override;
  MarkdownReplacementView LoadReplacementView(void* ud, int32_t id,
                                              float max_width,
                                              float max_height) override;

 private:
  std::shared_ptr<MarkdownPlatformView> WrapEtsView(ArkUI_NodeHandle handle);

  std::shared_ptr<MarkdownView> view_;
  NativeServalMarkdownView* bound_view_{nullptr};
  IHarmonyResourceLoader* loader_{nullptr};
  std::function<void()> request_measure_callback_;
  HarmonyAnimationClock animation_clock_;
  std::unordered_map<ArkUI_NodeHandle, std::shared_ptr<MarkdownPlatformView>>
      view_cache_;
};

}  // namespace serval::markdown

#endif  // MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_HARMONY_HARMONY_MARKDOWN_MEASURER_H_
