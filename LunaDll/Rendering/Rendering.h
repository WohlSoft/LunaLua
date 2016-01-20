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
#include "GL/GLEngineCmds.h"

class RenderOp;
class BMPBox;

// Main renderer object
struct Renderer {

    Renderer();
    ~Renderer();

    bool LoadBitmapResource(std::wstring filename, int resource_code, int transparency_color); // don't give full path
    bool LoadBitmapResource(std::wstring filename, int resource_code);
    void StoreImage(const std::shared_ptr<BMPBox>& bmp, int resource_code);
    bool DeleteImage(int resource_code);
    std::shared_ptr<BMPBox> GetImageForResourceCode(int resource_code);

    std::vector<std::shared_ptr<BMPBox>> LoadAnimatedBitmapResource(std::wstring filename, int* frameTime = 0);

    void AddOp(RenderOp* op);							// Add a drawing operation to the list
    void GLCmd(const std::shared_ptr<GLEngineCmd>& cmd, double renderPriority = 1.0);
    
    void DebugPrint(std::wstring message);				// Print a debug message on the screen
    void DebugPrint(std::wstring message, double val);	// Print a debug message on the screen and display a related value
    void DebugRelativeRect(int x, int y, int w, int h, DWORD color);	// Draw a rectangle on screen somewhere (level coordinates)

    void RenderUpTo(double maxPriority);

    void ClearAllDebugMessages();

    // Calls from hooks
    void StartCameraRender(int idx);
    void StartFrameRender();
    void EndFrameRender();

private:
    void DrawOp(RenderOp& render_operation);
    

    // Members //
private:
    bool m_InFrameRender;
    int m_curCamIdx; // Camera state
    
    std::size_t m_renderOpsSortedCount;
    std::size_t m_renderOpsProcessedCount;
    std::vector<RenderOp*> m_currentRenderOps;  // render operations to be performed

    std::map<int, std::shared_ptr<BMPBox>> m_legacyResourceCodeImages;  // loaded image resources

    std::list<std::wstring> m_debugMessages;                // Debug message to be printed

    // Simple getters //
public:
    int GetCameraIdx() { return m_curCamIdx; }
    HDC GetScreenDC() { return (HDC)GM_SCRN_HDC; }
};

namespace Render{
    bool IsOnScreen(double x, double y, double w, double h); // Returns whether or not the digven rectangle is on screen this frame
    void CalcCameraPos(double* p_X, double* p_Y);            // Tries to read smbx memory to return the camera coords in the 2 passed args

    
}

#endif
