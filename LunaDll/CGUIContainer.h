#pragma once

#include "CGUI.h"

// Basic GUI element which contains others
class CGUIContainer : CGUIElement, IGUIContainer {
	list<CGUIElement*> m_ChildElements;
};