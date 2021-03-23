/*
 * Copyright 2020-2021 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package com.microej.example.partial.app;

import ej.microui.display.GraphicsContext;
import ej.microui.display.Painter;
import ej.mwt.Widget;
import ej.mwt.animation.Animation;
import ej.mwt.style.Style;
import ej.mwt.util.Size;

/**
 * Shows a progress bar with an infinite animation loop.
 */
public class ProgressBar extends Widget implements Animation {

	private static final int ANIM_DURATION = 2_000;

	private long animStartTime;
	private int currentAnimStep;

	@Override
	protected void onShown() {
		this.animStartTime = System.currentTimeMillis();
		getDesktop().getAnimator().startAnimation(this);
	}

	@Override
	protected void onHidden() {
		getDesktop().getAnimator().stopAnimation(this);
	}

	@Override
	protected void renderContent(GraphicsContext g, int contentWidth, int contentHeight) {
		Style style = getStyle();
		int filledWidth = contentWidth * this.currentAnimStep / ANIM_DURATION;

		g.setColor(style.getColor());
		Painter.fillRectangle(g, 0, 0, filledWidth, contentHeight);
	}

	@Override
	protected void computeContentOptimalSize(Size size) {
		size.setSize(0, 0);
	}

	@Override
	public boolean tick(long currentTimeMillis) {
		long elapsedTime = currentTimeMillis - this.animStartTime;
		this.currentAnimStep = (int) (elapsedTime % ANIM_DURATION);
		requestRender();
		return true;
	}
}
