/*
 * Copyright 2020-2021 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package com.microej.example.partial.app;

import com.microej.example.partial.app.scroll.Scroll;
import com.microej.example.partial.app.scroll.ScrollableList;
import com.microej.example.partial.app.transition.HorizontalTransitionDisplayable;
import com.microej.example.partial.app.transition.VerticalTransitionDisplayable;
import com.microej.example.partial.support.PartialRenderPolicy;

import ej.microui.MicroUI;
import ej.microui.display.Display;
import ej.microui.display.Displayable;
import ej.mwt.Desktop;
import ej.mwt.Widget;
import ej.mwt.render.RenderPolicy;
import ej.mwt.stylesheet.Stylesheet;
import ej.widget.basic.Button;
import ej.widget.basic.Label;
import ej.widget.basic.OnClickListener;
import ej.widget.container.LayoutOrientation;
import ej.widget.container.List;
import ej.widget.container.SimpleDock;

/**
 * Contains the entry point of the application.
 */
public class Main {

	private static final int NUM_LIST_ITEMS = 18;

	private final Stylesheet stylesheet;

	private boolean currentScrollOrientation;
	private Desktop currentPageDesktop;

	/**
	 * Entry point of the application.
	 *
	 * @param args
	 *            application arguments (not used).
	 */
	public static void main(String[] args) {
		MicroUI.start();
		new Main();
	}

	private Main() {
		this.stylesheet = StylesheetBuilder.createStylesheet();

		Desktop desktop = createPageDesktop(LayoutOrientation.HORIZONTAL);
		desktop.requestShow();

		this.currentScrollOrientation = LayoutOrientation.HORIZONTAL;
		this.currentPageDesktop = desktop;
	}

	private Desktop createPageDesktop(boolean scrollOrientation) {
		Desktop desktop = new Desktop() {
			@Override
			protected RenderPolicy createRenderPolicy() {
				return new PartialRenderPolicy(this, false);
			}
		};

		desktop.setWidget(createPageWidget(scrollOrientation));
		desktop.setStylesheet(this.stylesheet);
		return desktop;
	}

	private Widget createPageWidget(boolean scrollOrientation) {
		ScrollableList list = new ScrollableList(scrollOrientation);

		for (int i = 0; i < NUM_LIST_ITEMS; i++) {
			Label item = new Label("Item " + i); //$NON-NLS-1$
			list.addChild(item);
		}

		Scroll scroll = new Scroll(scrollOrientation);
		scroll.showScrollbar(false);
		scroll.setChild(list);

		Button horizontalTransitionButton = new Button("Horizontal transition"); //$NON-NLS-1$
		horizontalTransitionButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick() {
				changePage(true);
			}
		});

		Button verticalTransitionButton = new Button("Vertical transition"); //$NON-NLS-1$
		verticalTransitionButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick() {
				changePage(false);
			}
		});

		List changePageList = new List(LayoutOrientation.HORIZONTAL);
		changePageList.addChild(horizontalTransitionButton);
		changePageList.addChild(verticalTransitionButton);

		ProgressBar progressBar = new ProgressBar();

		SimpleDock dock = new SimpleDock(LayoutOrientation.VERTICAL);
		dock.setFirstChild(progressBar);
		dock.setCenterChild(scroll);
		dock.setLastChild(changePageList);
		return dock;
	}

	private void changePage(boolean horizontalTransition) {
		boolean newScrollOrientation = !this.currentScrollOrientation;
		Desktop oldPageDesktop = this.currentPageDesktop;
		Desktop newPageDesktop = createPageDesktop(newScrollOrientation);

		Displayable displayable;
		if (horizontalTransition) {
			displayable = new HorizontalTransitionDisplayable(oldPageDesktop, newPageDesktop);
		} else {
			displayable = new VerticalTransitionDisplayable(oldPageDesktop, newPageDesktop);
		}
		Display.getDisplay().requestShow(displayable);

		this.currentScrollOrientation = newScrollOrientation;
		this.currentPageDesktop = newPageDesktop;
	}
}
