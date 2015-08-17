#ifndef Rendering_hhhh
#define Rendering_hhhh
#include <Windows.h>
#include <string>
#include <list>
#include "RenderString.h"
//#include "BMPBox.h"
#include <map>
//#include "RenderOp.h"
#include "GLEngineCmds.h"
#include <vector>

class RenderOp;
class BMPBox;

// Main renderer object
struct Renderer {

    Renderer();
    ~Renderer();

    bool ReloadScreenHDC();

    bool LoadBitmapResource(std::wstring filename, int resource_code, int transparency_color); // don't give full path
    bool LoadBitmapResource(std::wstring filename, int resource_code);
    std::vector<int> LoadAnimatedBitmapResource(std::wstring filename);

    bool DeleteImage(int resource_code);
    int  GetAutoImageResourceCode() const;

    void AddOp(RenderOp* op);							// Add a drawing operation to the list
    void GLCmd(const GLEngineCmd &cmd);
    void SafePrint(std::wstring, int font_type, float x, float y);
    void SafePrint(RenderString str);

    void DebugPrint(std::wstring message);				// Print a debug message on the screen
    void DebugPrint(std::wstring message, double val);	// Print a debug message on the screen and display a related value
    void DebugRelativeRect(int x, int y, int w, int h, DWORD color);	// Draw a rectangle on screen somewhere (level coordinates)

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
    bool IsOnScreen(double x, double y, double w, double h);// Returns whether or not the digven rectangle is on screen this frame
    void CalcCameraPos(double* p_X, double* p_Y);			// Tries to read smbx memory to return the camera coords in the 2 passed args

    void Print(std::wstring, short font_type, float x, float y); // only works during post-draw phase!

}

#endif
