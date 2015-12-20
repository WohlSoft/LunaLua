#include <climits>
#include <tuple>
#include <algorithm>
#include "../Globals.h"
#include "Rendering.h"
#include "RenderUtils.h"
#include "../Defines.h"
#include "../SMBXInternal/Level.h"
#include "../Misc/MiscFuncs.h"
#include "BMPBox.h"
#include "RenderOps/RenderOp.h"
#include "RenderOps/RenderRectOp.h"
#include "RenderOps/RenderGLOp.h"
#include "RenderOps/RenderStringOp.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../GlobalFuncs.h"
#include "GLEngine.h"

using namespace std;

// CTOR
Renderer::Renderer() {
    m_hScreenDC = NULL;
}

// DTOR
Renderer::~Renderer() {
    if (m_hScreenDC != NULL)
        DeleteDC(m_hScreenDC);
}

// UPDATE HDC -- Try and get the main screen HDC from SMBX. Fails if SMBX in bad state or screen not ready
bool Renderer::ReloadScreenHDC() {
    if (GM_SCRN_HDC == 0) { // does SMBX itself have the DC yet?
        m_hScreenDC = 0;
        return false;
    }
    m_hScreenDC = (HDC)GM_SCRN_HDC;
    return true;
}



// LOAD BITMAP RESOURCE - Load an image resource with given resource code. If resource code exists, replaces old image
bool Renderer::LoadBitmapResource(std::wstring filename, int resource_code, int transparency_color) {
    if (Renderer::LoadBitmapResource(filename, resource_code)) {

        auto it = LoadedImages.find(resource_code);
        if (it != LoadedImages.end()) {
            it->second->MakeColorTransparent(transparency_color);
        }
        return true;
    }

    return false;
}

bool Renderer::LoadBitmapResource(std::wstring filename, int resource_code) {
    // If slot in use, delete old image
    DeleteImage(resource_code);

    // Concoct full filepath
    wstring full_path = L"";

    if (!isAbsolutePath(filename)){
        wstring world_dir = (wstring)GM_FULLDIR;
        full_path = (gIsOverworld ? world_dir : world_dir.append(Level::GetName()));
        if (!gIsOverworld){
            full_path = removeExtension(full_path);
            full_path = full_path.append(L"\\"); // < path into level folder
        }
        full_path = full_path + filename;
    }
    else
    {
        full_path = filename;
    }

    //MessageBoxW(NULL, full_path.c_str(), L"Dbg", NULL);
    // Create and store the image resource
    BMPBox* pNewbox = new BMPBox(full_path, m_hScreenDC);
    if (pNewbox->ImageLoaded() == false) {
        delete pNewbox;
        gLogger.Log(L"BMPBox image load failed", LOG_STD);
        return false;
    }

    StoreImage(pNewbox, resource_code);

    return true;
}


std::vector<int> Renderer::LoadAnimatedBitmapResource(std::wstring filename, int* frameTime)
{
    // Concoct full filepath
    wstring full_path = L"";

    if (!isAbsolutePath(filename)){
        wstring world_dir = (wstring)GM_FULLDIR;
        full_path = (gIsOverworld ? world_dir : world_dir.append(Level::GetName()));
        if (!gIsOverworld){
            full_path = removeExtension(full_path);
            full_path = full_path.append(L"\\"); // < path into level folder
        }
        full_path = full_path + filename;
    }
    else
    {
        full_path = filename;
    }

    std::tuple<std::vector<HBITMAP>, int> ret = LoadAnimatedGfx(filename);
    std::vector<HBITMAP>& bitmaps = std::get<0>(ret);
    if (frameTime) {
        double avgFrameTime = (double)std::get<1>(ret);
        *frameTime = (int)((avgFrameTime / 100) * 65);
    }
    
    std::vector<int> bitmapResCode;
    for (HBITMAP nextBitmap : bitmaps) {
        int nextResCode = GetAutoImageResourceCode();
        BMPBox* pNewbox = new BMPBox(nextBitmap, m_hScreenDC);
        if (pNewbox->ImageLoaded() == false) {
            delete pNewbox;
            gLogger.Log(L"BMPBox image load failed", LOG_STD);
            continue;
        }
        StoreImage(pNewbox, nextResCode);
        bitmapResCode.push_back(nextResCode);
    }
    return bitmapResCode;  
}


//STORE IMAGE
void Renderer::StoreImage(BMPBox* bmp, int resource_code) {
    LoadedImages[resource_code] = bmp;
}

// DELETE IMAGE - Deletes the image resource and returns true if it exists, otherwise does nothing and returns false
bool Renderer::DeleteImage(int resource_code) {
    auto it = LoadedImages.find(resource_code);
    if (it != LoadedImages.end()) {
        if (it->second) {
            delete it->second;
        }
        LoadedImages.erase(it);
        return true;
    }
    return false;
}

// Get an automatic resource code (always negative numbers, returns 0 if unable to allocate)
int Renderer::GetAutoImageResourceCode() const {
    for (int resource_code = INT_MIN; resource_code < 0; resource_code++) {
        // If the resource code is unused
        if (LoadedImages.count(resource_code) == 0) {
            return resource_code;
        }
    }
    return 0;
}

// IS ON SCREEN
bool Render::IsOnScreen(double x, double y, double w, double h) {
    double cam_x; double cam_y;
    CalcCameraPos(&cam_x, &cam_y);

    return FastTestCollision((int)cam_x, (int)cam_y, (int)cam_x + 800, (int)cam_y + 600, (int)x, (int)y, (int)x + (int)w, (int)y + (int)h);
}

// CALC CAMERA POS
void Render::CalcCameraPos(double* ret_x, double* ret_y) {

    // Camera func, calculated from player position and boundaries
    PlayerMOB* demo = Player::Get(1);
    if (false) {
        gLunaRender.AddOp(new RenderStringOp(to_wstring((long long)demo->momentum.x), 3, 300, 300));
        gLunaRender.AddOp(new RenderStringOp(to_wstring((long long)demo->momentum.y), 3, 300, 350));

        double cx = demo->momentum.x - (400 - (demo->momentum.width / 2));
        double cy = demo->momentum.y - (300 - (demo->momentum.height / 2));

        // Force camera to fit in level bounds
        RECT rect;
        Level::GetBoundary(&rect, demo->CurrentSection);
        if (cx < rect.left)
            cx = rect.left;
        if (cx + 800 > rect.right)
            cx = rect.right - 800;
        if (cy < rect.top)
            cy = rect.top;
        if (cy + 600 > rect.bottom)
            cy = rect.bottom - 600;

        *ret_x = cx;
        *ret_y = cy;
    }

    // Old camera func, using "camera" memory
    if (GM_CAMERA_Y != 0 && GM_CAMERA_X != 0) {
        double* pCameraX = (GM_CAMERA_X);
        double* pCameraY = (GM_CAMERA_Y);
        double val;
        if (ret_x != NULL) {
            val = pCameraX[1];
            *ret_x = val - val - val; // Fix backwards smbx camera
        }
        if (ret_y != NULL) {
            val = pCameraY[1];
            *ret_y = val - val - val; // Fix backwards smbx camera
        }
    }
}

// ADD OP
void Renderer::AddOp(RenderOp* op) {
    this->RenderOperations.push_back(op);
}

// GL Engine OP
void Renderer::GLCmd(const std::shared_ptr<GLEngineCmd>& cmd) {
    RenderGLOp* op = new RenderGLOp(cmd);
    AddOp(op);
}

// DRAW OP -- Process a render operation, draw, and decrement active timer remaining
void Renderer::DrawOp(RenderOp* op) {
    if (op->m_FramesLeft < 1)
        return;
    op->m_LastRenderedOn = gFrames;
    op->Draw(this);
    op->m_FramesLeft--;
}

// RENDER ALL
void Renderer::RenderAll() {

    ClearExpired();

    std::stable_sort(RenderOperations.begin(), RenderOperations.end(), 
        [](RenderOp* lhs, RenderOp* rhs) {return lhs->m_renderPriority < rhs->m_renderPriority; } );
    

    // Do render ops
    for (std::list<RenderOp*>::iterator iter = RenderOperations.begin(), end = RenderOperations.end(); iter != end; ++iter) {
        if ((*iter)->m_LastRenderedOn == gFrames && (*iter)->m_PerCycleOnly)
            continue;
        DrawOp((*iter));
    }

    // Format debug messages and enter them into renderstring list
    int dbg_x = 325;
    int dbg_y = 160;
    for (std::list<std::wstring >::const_iterator it = DebugMessages.begin(); it != DebugMessages.end(); it++)
    {
        std::wstring dbg = *it;
        RenderStringOp(dbg, 4, (float)dbg_x, (float)dbg_y).Draw(this);
        dbg_y += 20;
        if (dbg_y > 560) {
            dbg_y = 160;
            dbg_x += 190;
        }
    }
    this->DebugMessages.clear();
}

// CLEAR EXPIRED -- Delete render ops & strings with no display frames left
//				 -- Don't call this while iterating over renderops
void Renderer::ClearExpired() {

    // Clear expired render operations
    std::list<RenderOp*>::iterator iter = RenderOperations.begin();
    std::list<RenderOp*>::iterator end = RenderOperations.end();

    while (iter != RenderOperations.end()) {
        RenderOp* pOp = *iter;
        if ((*iter)->m_FramesLeft <= 0) {
            delete pOp;
            iter = RenderOperations.erase(iter);
        }
        else {
            ++iter;
        }
    }

}

// CLEAR ALL
void Renderer::ClearAll() {
    this->DebugMessages.clear();
}

// DEBUG PRINT
void Renderer::DebugPrint(std::wstring message) {
    this->DebugMessages.push_back(message);
}

void Renderer::DebugPrint(std::wstring message, double val) {
    this->DebugMessages.push_back(message + L" " + to_wstring((long long)val));
}

// DEBUG RELATIVE RECT
void Renderer::DebugRelativeRect(int x, int y, int w, int h, DWORD color) {
    RenderRectOp* p_Op = new RenderRectOp;
    double camtop;
    double camleft;
    Render::CalcCameraPos(&camleft, &camtop);
    p_Op->color = color;
    p_Op->m_FramesLeft = 1;
    p_Op->m_PerCycleOnly = false;
    p_Op->x1 = x - camleft;
    p_Op->y1 = y - camtop;
    p_Op->x2 = (x + w) - camleft;
    p_Op->y2 = (y + h) - camtop;
    AddOp(p_Op);
}
