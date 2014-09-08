#pragma once
#include <Windows.h>
#include <string>
#include <list>
#include "RenderString.h"
//#include "BMPBox.h"
#include <map>
//#include "RenderOp.h"

class RenderOp;
class BMPBox;

// Main renderer object
struct Renderer {

	Renderer();
	~Renderer();

	bool ReloadScreenHDC();

	bool LoadBitmapResource(std::wstring filename, int resource_code, int transparency_color); // don't give full path
	bool DeleteImage(int resource_code);	

	void AddOp(RenderOp* op);				// Add a drawing operation to the list
	void SafePrint(std::wstring, int font_type, float x, float y);
	void SafePrint(RenderString str);
	void DebugPrint(std::wstring message);				// Print a debug message on the screen
	void DebugPrint(std::wstring message, double val);	// Print a debug message on the screen and display a related value

	void RenderAll();

	void ClearExpired();
	void ClearAll();

protected:
	void DrawOp(RenderOp* render_operation);
	void StoreImage(BMPBox* bmp, int resource_code);

	// Members //
public:
	HDC m_hScreenDC;								// handle to the main screen DC

	std::list<RenderString> RenderStrings;			// strings to be rendered at the post draw phase
	std::list<RenderOp*> RenderOperations;			// render operations to be performed
	std::map<int, BMPBox*> LoadedImages;			// loaded image resources

	std::list<std::wstring> DebugMessages;				// Debug message to be printed
};

namespace Render{
	void CalcCameraPos(double* p_X, double* p_Y); // Tries to read smbx memory to return the camera coords in the 2 passed args

	void Print(std::wstring, int font_type, float x, float y); // only works during post-draw phase!

}