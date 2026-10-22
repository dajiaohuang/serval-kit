// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#ifndef MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_ANDROID_ANDROID_SERVAL_MARKDOWN_VIEW_H_
#define MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_ANDROID_ANDROID_SERVAL_MARKDOWN_VIEW_H_

#include <jni.h>

#include "markdown/platform/android/markdown_class_cache.h"

namespace serval::markdown {

class AndroidMarkdownMeasurer;
class MarkdownView;

class AndroidServalMarkdownView : public AndroidMainView {
 public:
  AndroidServalMarkdownView(JNIEnv* env, jobject view);
  ~AndroidServalMarkdownView() override;

  void SetMeasurer(AndroidMarkdownMeasurer* measurer);
  MarkdownView* GetMarkdownView();

  void OnRendererFrame(int64_t time);
  // UI-thread API. When enabled, frames retain the host-supplied render region.
  // Disabling restores automatic region calculation on the next frame.
  void SetExternalRenderRegion(bool external);
  // Supply the visible rectangle in view-local physical pixels before rendering
  // a frame with external render-region control enabled.
  void SetRenderRegion(RectF region);

 private:
  bool external_render_region_{false};
};

}  // namespace serval::markdown

#endif  // MARKDOWN_INCLUDE_MARKDOWN_PLATFORM_ANDROID_ANDROID_SERVAL_MARKDOWN_VIEW_H_
