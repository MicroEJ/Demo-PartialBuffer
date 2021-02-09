/*
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
package com.microej.example.partial.app;

import com.microej.example.partial.app.scroll.Scroll;
import com.microej.example.partial.app.scroll.ScrollableList;

import ej.mwt.Widget;
import ej.mwt.style.EditableStyle;
import ej.mwt.style.background.RectangularBackground;
import ej.mwt.style.dimension.FixedDimension;
import ej.mwt.style.dimension.RelativeDimension;
import ej.mwt.style.outline.FlexibleOutline;
import ej.mwt.style.outline.UniformOutline;
import ej.mwt.style.outline.border.RectangularBorder;
import ej.mwt.stylesheet.CachedStylesheet;
import ej.mwt.stylesheet.Stylesheet;
import ej.mwt.stylesheet.cascading.CascadingStylesheet;
import ej.mwt.stylesheet.selector.StateSelector;
import ej.mwt.stylesheet.selector.StrictTypeSelector;
import ej.mwt.stylesheet.selector.TypeSelector;
import ej.mwt.util.Alignment;
import ej.widget.basic.Button;
import ej.widget.basic.Label;

/**
 * Creates the stylesheet of the demo.
 */
public class StylesheetBuilder {

	private static final int CORAL = 0xee502e;
	private static final int POMEGRANATE = 0xcf4520;
	private static final int TURQUOISE = 0x00aec7;
	private static final int BONDI = 0x008eaa;
	private static final int ABSINTHE = 0x6cc24a;
	private static final int AVOCADO = 0x48a23f;
	private static final int CONCRETE_WHITE_75 = 0xe5e9eb;
	private static final int CONCRETE_BLACK_50 = 0x4b5357;

	private static final int MARGIN = 5;

	private StylesheetBuilder() {
		// private constructor
	}

	/**
	 * Creates the stylesheet of the demo.
	 *
	 * @return the stylesheet of the demo.
	 */
	public static Stylesheet createStylesheet() {
		CascadingStylesheet stylesheet = new CascadingStylesheet();

		// default
		EditableStyle defaultStyle = stylesheet.getDefaultStyle();
		defaultStyle.setBackground(new RectangularBackground(CONCRETE_WHITE_75));

		// progress bar
		EditableStyle progressBarStyle = stylesheet.getSelectorStyle(new TypeSelector(ProgressBar.class));
		progressBarStyle.setBackground(new RectangularBackground(POMEGRANATE));
		progressBarStyle.setBorder(new RectangularBorder(CONCRETE_BLACK_50, 1));
		progressBarStyle.setColor(CORAL);
		progressBarStyle.setDimension(new FixedDimension(Widget.NO_CONSTRAINT, 28));
		progressBarStyle.setMargin(new UniformOutline(MARGIN));

		// scroll
		EditableStyle scrollStyle = stylesheet.getSelectorStyle(new TypeSelector(Scroll.class));
		scrollStyle.setBackground(new RectangularBackground(BONDI));
		scrollStyle.setBorder(new RectangularBorder(CONCRETE_BLACK_50, 1));
		scrollStyle.setMargin(new FlexibleOutline(0, MARGIN, 0, MARGIN));

		// scrollable list
		EditableStyle scrollableListStyle = stylesheet.getSelectorStyle(new TypeSelector(ScrollableList.class));
		scrollableListStyle.setBackground(new RectangularBackground(BONDI));
		scrollableListStyle.setMargin(new UniformOutline(2));

		// label
		EditableStyle labelStyle = stylesheet.getSelectorStyle(new StrictTypeSelector(Label.class));
		labelStyle.setBackground(new RectangularBackground(TURQUOISE));
		labelStyle.setBorder(new RectangularBorder(CONCRETE_BLACK_50, 1));
		labelStyle.setVerticalAlignment(Alignment.VCENTER);
		labelStyle.setMargin(new UniformOutline(2));
		labelStyle.setPadding(new UniformOutline(5));

		// button
		EditableStyle buttonStyle = stylesheet.getSelectorStyle(new TypeSelector(Button.class));
		buttonStyle.setBackground(new RectangularBackground(ABSINTHE));
		buttonStyle.setBorder(new RectangularBorder(CONCRETE_BLACK_50, 1));
		buttonStyle.setHorizontalAlignment(Alignment.HCENTER);
		buttonStyle.setVerticalAlignment(Alignment.VCENTER);
		buttonStyle.setDimension(new FixedDimension(Widget.NO_CONSTRAINT, 28));
		buttonStyle.setMargin(new UniformOutline(MARGIN));

		// active button
		EditableStyle activeButtonStyle = stylesheet.getSelectorStyle(new StateSelector(Button.ACTIVE));
		activeButtonStyle.setBackground(new RectangularBackground(AVOCADO));
		activeButtonStyle.setDimension(new RelativeDimension(Widget.NO_CONSTRAINT, 0.10f));

		return new CachedStylesheet(stylesheet);
	}
}
