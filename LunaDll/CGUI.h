#ifndef CGUI_Hhh
#define CGUI_Hhh

#include <list>

using namespace std;

// Top class for all GUI elements
class CGUIElement {
protected:
	virtual void Process() =0;
	virtual void Draw() =0;

	int m_X;		// Screen X position
	int m_Y;		// Screen Y position
	int m_W;		// Element width
	int m_H;		// Element height

};

// Interface for element that can contain child elements
class IGUIContainer {
protected:
    inline void AddElement(CGUIElement* ele=0) {}
    list<CGUIElement*> m_ChildElements;
};

 // Forward declarations for anything #including this file
class CGUIContainer;

#endif
