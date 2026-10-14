// Copyright 2026 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.
package com.lynx.markdown;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertTrue;

import android.content.Context;
import android.view.View;
import androidx.test.annotation.UiThreadTest;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;
import java.util.concurrent.CountDownLatch;
import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class ServalMarkdownViewTest {
  private Context getContext() {
    return InstrumentationRegistry.getInstrumentation().getTargetContext();
  }

  @Test
  @UiThreadTest
  public void destroysInternallyCreatedMeasurer() {
    ServalMarkdownView view = new ServalMarkdownView(getContext());
    MarkdownMeasurer measurer = view.getMarkdownMeasurer();
    assertNotEquals(0, measurer.getNativeInstance());

    view.destroy();
    view.destroy();

    assertEquals(0, measurer.getNativeInstance());
  }

  @Test
  @UiThreadTest
  public void preservesExternallyCreatedMeasurer() {
    MarkdownMeasurer measurer = new MarkdownMeasurer(getContext());
    ServalMarkdownView view = new ServalMarkdownView(getContext(), false);
    view.setMarkdownMeasurer(measurer);
    long instance = measurer.getNativeInstance();

    view.destroy();

    assertEquals(instance, measurer.getNativeInstance());
    measurer.setContent("measured after view destruction");
    int widthSpec =
        View.MeasureSpec.makeMeasureSpec(300, View.MeasureSpec.EXACTLY);
    long result = measurer.measure(widthSpec, 0);
    assertTrue(MarkdownValuePack.unpackMeasureResultHeight(result) > 0);
    measurer.destroy();
  }

  @Test
  @UiThreadTest
  public void allowsExternalMeasurerToBeDestroyedBeforeView() {
    MarkdownMeasurer measurer = new MarkdownMeasurer(getContext());
    ServalMarkdownView view = new ServalMarkdownView(getContext(), false);
    view.setMarkdownMeasurer(measurer);

    measurer.destroy();
    view.destroy();
    view.destroy();

    assertEquals(0, measurer.getNativeInstance());
  }

  @Test
  @UiThreadTest
  public void rebindsExternalMeasurerAfterViewDestruction() {
    MarkdownMeasurer measurer = new MarkdownMeasurer(getContext());
    measurer.setContent("rebound markdown");
    ServalMarkdownView first = new ServalMarkdownView(getContext(), false);
    first.setMarkdownMeasurer(measurer);
    first.destroy();

    ServalMarkdownView second = new ServalMarkdownView(getContext(), false);
    second.setMarkdownMeasurer(measurer);
    int widthSpec =
        View.MeasureSpec.makeMeasureSpec(300, View.MeasureSpec.EXACTLY);
    second.measure(widthSpec, 0);

    assertTrue(second.getMeasuredHeight() > 0);
    second.destroy();
    measurer.destroy();
  }

  @Test
  @UiThreadTest
  public void destroysExternalMeasurerAndViewOnDifferentThreads()
      throws InterruptedException {
    for (int i = 0; i < 100; ++i) {
      MarkdownMeasurer measurer = new MarkdownMeasurer(getContext());
      ServalMarkdownView view = new ServalMarkdownView(getContext(), false);
      view.setMarkdownMeasurer(measurer);
      CountDownLatch start = new CountDownLatch(1);
      Thread layoutThread = new Thread(() -> {
        try {
          start.await();
        } catch (InterruptedException exception) {
          throw new AssertionError(exception);
        }
        measurer.destroy();
      });
      layoutThread.start();

      start.countDown();
      view.destroy();
      layoutThread.join();

      assertEquals(0, measurer.getNativeInstance());
    }
  }
}
