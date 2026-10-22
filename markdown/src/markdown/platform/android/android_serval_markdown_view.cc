// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include "markdown/platform/android/android_serval_markdown_view.h"

#include "markdown/platform/android/android_markdown_measurer.h"
#include "markdown/view/markdown_view.h"

namespace serval::markdown {

AndroidServalMarkdownView::AndroidServalMarkdownView(JNIEnv* env, jobject view)
    : AndroidMainView(env, view) {}

AndroidServalMarkdownView::~AndroidServalMarkdownView() {
  if (auto* markdown_view = GetMarkdownView(); markdown_view != nullptr) {
    markdown_view->DetachView();
  }
}

void AndroidServalMarkdownView::SetMeasurer(AndroidMarkdownMeasurer* measurer) {
  measurer->BindView(this);
}

MarkdownView* AndroidServalMarkdownView::GetMarkdownView() {
  return static_cast<MarkdownView*>(drawable_.get());
}

void AndroidServalMarkdownView::OnRendererFrame(int64_t time) {
  if (!external_render_region_) {
    UpdateCachedViewRectInScreen();
  }
  if (auto* markdown_view = GetMarkdownView(); markdown_view != nullptr) {
    markdown_view->OnRendererFrame(time);
  }
}

void AndroidServalMarkdownView::SetExternalRenderRegion(bool external) {
  external_render_region_ = external;
}

void AndroidServalMarkdownView::SetRenderRegion(RectF region) {
  cached_view_rect_in_screen_ = region;
}

}  // namespace serval::markdown
