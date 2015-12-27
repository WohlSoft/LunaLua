#ifndef Rendering_hhhh
#define Rendering_hhhh
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <memory>

//#include "BMPBox.h"
//#include "RenderOp.h"
#include "GLEngineCmds.h"

class RenderOp;
class BMPBox;

// Main renderer object
struct Renderer {

    Renderer();
    ~Renderer();

    bool ReloadScreenHDC();

    bool LoadBitmapResource(std::wstring filename, int resource_code, int transparency_color); // don't give full path
    bool LoadBitmapResource(std::wstring filename, int resource_code);
    std::vector<std::shared_ptr<BMPBox>> LoadAnimatedBitmapResource(std::wstring filename, int* frameTime = 0);

    bool DeleteImage(int resource_code);

    void AddOp(RenderOp* op);							// Add a drawing operation to the list
    void GLCmd(const std::shared_ptr<GLEngineCmd>& cmd, double renderPriority = 1.0);
    
    void DebugPrint(std::wstring message);				// Print a debug message on the screen
    void DebugPrint(std::wstring message, double val);	// Print a debug message on the screen and display a related value
    void DebugRelativeRect(int x, int y, int w, int h, DWORD color);	// Draw a rectangle on screen somewhere (level coordinates)

    void RenderAll();

    void ClearExpired();
    void ClearAll();

protected:
    void DrawOp(RenderOp* render_operation);
    void StoreImage(const std::shared_ptr<BMPBox>& bmp, int resource_code);

    // Members //
public:
    HDC m_hScreenDC;								// handle to the main screen DC

    std::list<RenderOp*> RenderOperations;			// render operations to be performed
    std::map<int, std::shared_ptr<BMPBox>> LoadedImages;			// loaded image resources

    std::list<std::wstring> DebugMessages;				// Debug message to be printed
};

namespace Render{
    bool IsOnScreen(double x, double y, double w, double h);// Returns whether or not the digven rectangle is on screen this frame
    void CalcCameraPos(double* p_X, double* p_Y);			// Tries to read smbx memory to return the camera coords in the 2 passed args

    
}

#endif
