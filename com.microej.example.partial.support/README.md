# Overview

This library provides an example of partial buffer support. It provides a render policy implementation.

# Usage

The partial render policy can be selected when the desktop is created. For example:

```java
	Desktop desktop = new Desktop() {
		@Override
		protected RenderPolicy createRenderPolicy() {
			return new PartialRenderPolicy(this);
		}
	};
```

# Requirements

This library requires the following Foundation Libraries:

    EDC-1.3, BON-1.4, MICROUI-3.0, DRAWING-1.0

# Dependencies

_All dependencies are retrieved transitively by MicroEJ Module Manager_.

# Source

N/A

# Restrictions

None.

---
_Markdown_  
_Copyright 2020 MicroEJ Corp. All rights reserved._  
_Use of this source code is governed by a BSD-style license that can be found with this software._  
