/*
 * Copyright 2020-2021 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package com.microej.example.partial.app.transition;

import com.microej.example.partial.support.PartialBufferNatives;

import ej.microui.display.Display;
import ej.microui.display.Displayable;
import ej.microui.display.GraphicsContext;
import ej.mwt.Desktop;
import ej.mwt.Widget;
import ej.mwt.animation.Animation;
import ej.mwt.animation.Animator;

/**
 * A transition displayable shows a transition between two desktops with a horizontal slide effect.
 */
public class HorizontalTransitionDisplayable extends Displayable implements Animation {

	private static final int DURATION = 250;

	private final Desktop oldDesktop;
	private final Desktop newDesktop;
	private final Animator animator;

	private long startTime;
	private int currentShift;

	/**
	 * Creates a horizontal transition displayable.
	 *
	 * @param oldDesktop
	 *            the old desktop.
	 * @param newDesktop
	 *            the new desktop.
	 */
	public HorizontalTransitionDisplayable(Desktop oldDesktop, Desktop newDesktop) {
		this.oldDesktop = oldDesktop;
		this.newDesktop = newDesktop;
		this.animator = new Animator();
	}

	@Override
	protected void onShown() {
		super.onShown();

		this.oldDesktop.setAttached();
		this.newDesktop.setAttached();

		this.startTime = System.currentTimeMillis();
		this.animator.startAnimation(this);
	}

	@Override
	protected void onHidden() {
		super.onHidden();

		this.animator.stopAnimation(this);
	}

	@Override
	protected void render(GraphicsContext g) {
		Desktop oldDesktop = this.oldDesktop;
		Desktop newDesktop = this.newDesktop;
		int currentShift = this.currentShift;

		Display display = Display.getDisplay();
		int displayWidth = display.getWidth();
		int displayHeight = display.getHeight();

		Widget oldWidget = oldDesktop.getWidget();
		Widget newWidget = newDesktop.getWidget();
		assert (oldWidget != null && newWidget != null);

		int currentY = 0;
		int remainingHeight = displayHeight;

		while (remainingHeight > 0) {
			int linesToRender = Math.min(PartialBufferNatives.getBufferHeight(), remainingHeight);

			g.setTranslation(currentShift, -currentY);
			g.setClip(0, currentY, displayWidth - currentShift, linesToRender);
			oldWidget.render(g);

			g.setTranslation(currentShift - displayWidth, -currentY);
			g.setClip(displayWidth - currentShift, currentY, currentShift, linesToRender);
			newWidget.render(g);

			PartialBufferNatives.setFlushLimits(0, currentY, displayWidth, linesToRender);
			display.flush();

			currentY += linesToRender;
			remainingHeight -= linesToRender;
		}
	}

	@Override
	public boolean tick(long currentTimeMillis) {
		long elapsedTime = currentTimeMillis - this.startTime;
		if (elapsedTime >= DURATION) {
			this.newDesktop.requestShow();
			return false;
		} else {
			this.currentShift = Display.getDisplay().getWidth() * (int) elapsedTime / DURATION;
			requestRender();
			return true;
		}
	}

	@Override
	public boolean handleEvent(int event) {
		return false;
	}
}
