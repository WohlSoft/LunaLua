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
#include "GL/GLEngine.h"

using namespace std;

// CTOR
Renderer::Renderer() :
    m_curCamIdx(1)
{
}

// DTOR
Renderer::~Renderer() {
}

// LOAD BITMAP RESOURCE - Load an image resource with given resource code. If resource code exists, replaces old image
bool Renderer::LoadBitmapResource(std::wstring filename, int resource_code, int transparency_color) {
    if (Renderer::LoadBitmapResource(filename, resource_code)) {

        auto it = m_legacyResourceCodeImages.find(resource_code);
        if (it != m_legacyResourceCodeImages.end()) {
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
    std::shared_ptr<BMPBox> pNewbox = std::make_shared<BMPBox>(full_path, GetScreenDC());
    if (pNewbox->ImageLoaded() == false) {
        gLogger.Log(L"BMPBox image load failed", LOG_STD);
        return false;
    }

    StoreImage(pNewbox, resource_code);

    return true;
}

//STORE IMAGE
void Renderer::StoreImage(const std::shared_ptr<BMPBox>& bmp, int resource_code) {
    m_legacyResourceCodeImages[resource_code] = bmp;
}

// DELETE IMAGE - Deletes the image resource and returns true if it exists, otherwise does nothing and returns false
bool Renderer::DeleteImage(int resource_code) {
    auto it = m_legacyResourceCodeImages.find(resource_code);
    if (it != m_legacyResourceCodeImages.end()) {
        m_legacyResourceCodeImages.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<BMPBox> Renderer::GetImageForResourceCode(int resource_code)
{
    auto it = m_legacyResourceCodeImages.find(resource_code);
    if (it != m_legacyResourceCodeImages.end())
    {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<BMPBox>> Renderer::LoadAnimatedBitmapResource(std::wstring filename, int* frameTime)
{
    // Concoct full filepath
    wstring full_path = L"";

    if (!isAbsolutePath(filename)) {
        wstring world_dir = (wstring)GM_FULLDIR;
        full_path = (gIsOverworld ? world_dir : world_dir.append(Level::GetName()));
        if (!gIsOverworld) {
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

    std::vector<std::shared_ptr<BMPBox>> bitmapList;
    for (HBITMAP nextBitmap : bitmaps) {
        std::shared_ptr<BMPBox> pNewbox = std::make_shared<BMPBox>(nextBitmap, GetScreenDC());
        pNewbox->m_Filename = filename;
        if (pNewbox->ImageLoaded() == false) {
            gLogger.Log(L"BMPBox image load failed", LOG_STD);
            continue;
        }
        bitmapList.push_back(pNewbox);
    }
    return bitmapList;
}

// ADD OP
void Renderer::AddOp(RenderOp* op) {
    if (op->m_selectedCamera == 0)
    {
        // If the rendering operation was created in the middle of handling a
        // camera's rendering, lock the rendering operation to that camera.
        op->m_selectedCamera = m_curCamIdx;
    }
    this->m_currentRenderOps.push_back(op);
}

// GL Engine OP
void Renderer::GLCmd(const std::shared_ptr<GLEngineCmd>& cmd, double renderPriority) {
    RenderGLOp* op = new RenderGLOp(cmd);
    op->m_renderPriority = renderPriority;
    AddOp(op);
}

// DRAW OP -- Process a render operation, draw
void Renderer::DrawOp(RenderOp* op) {
    if (op->m_FramesLeft < 1)
        return;
    op->Draw(this);
}

// RENDER ALL
void Renderer::RenderAll() {

    std::stable_sort(m_currentRenderOps.begin(), m_currentRenderOps.end(),
        [](RenderOp* lhs, RenderOp* rhs) {return lhs->m_renderPriority < rhs->m_renderPriority; } );
    

    // Do render ops
    for (auto iter = m_currentRenderOps.begin(), end = m_currentRenderOps.end(); iter != end; ++iter) {
        RenderOp* pOp = *iter;
        if (pOp->m_selectedCamera == 0 || pOp->m_selectedCamera == m_curCamIdx)
        {
            DrawOp(pOp);
        }
    }

    // Format debug messages and enter them into renderstring list
    int dbg_x = 325;
    int dbg_y = 160;
    for (auto it = m_debugMessages.begin(); it != m_debugMessages.end(); it++)
    {
        std::wstring dbg = *it;
        RenderStringOp(dbg, 4, (float)dbg_x, (float)dbg_y).Draw(this);
        dbg_y += 20;
        if (dbg_y > 560) {
            dbg_y = 160;
            dbg_x += 190;
        }
    }
    this->m_debugMessages.clear();
}

// CLEAR ALL
void Renderer::ClearAllDebugMessages() {
    this->m_debugMessages.clear();
}

// DEBUG PRINT
void Renderer::DebugPrint(std::wstring message) {
    this->m_debugMessages.push_back(message);
}

void Renderer::DebugPrint(std::wstring message, double val) {
    this->m_debugMessages.push_back(message + L" " + to_wstring((long long)val));
}

// DEBUG RELATIVE RECT
void Renderer::DebugRelativeRect(int x, int y, int w, int h, DWORD color) {
    RenderRectOp* p_Op = new RenderRectOp;
    double camtop;
    double camleft;
    Render::CalcCameraPos(&camleft, &camtop);
    p_Op->color = color;
    p_Op->m_FramesLeft = 1;
    p_Op->x1 = x - camleft;
    p_Op->y1 = y - camtop;
    p_Op->x2 = (x + w) - camleft;
    p_Op->y2 = (y + h) - camtop;
    AddOp(p_Op);
}

void Renderer::StartFrameRender()
{
    m_curCamIdx = 0;
}

void Renderer::StartCameraRender(int idx)
{
    m_curCamIdx = idx;

    // TODO: Forward camera coordinates to GL renderer to let it do transforms
    //
}

void Renderer::EndFrameRender()
{
    if (m_curCamIdx == 0) return;

    m_curCamIdx = 0;

    // Remove cleared operations
    std::vector<RenderOp*> nonExpiredOps;
    for (auto iter = m_currentRenderOps.begin(), end = m_currentRenderOps.end(); iter != end; ++iter) {
        RenderOp* pOp = *iter;
        pOp->m_FramesLeft--;
        if (pOp->m_FramesLeft <= 0)
        {
            *iter = nullptr;
            delete pOp;
        }
        else
        {
            nonExpiredOps.push_back(pOp);
        }
    }
    m_currentRenderOps.swap(nonExpiredOps);
}



// IS ON SCREEN
bool Render::IsOnScreen(double x, double y, double w, double h) {
    double cam_x; double cam_y;
    CalcCameraPos(&cam_x, &cam_y);

    return FastTestCollision((int)cam_x, (int)cam_y, (int)cam_x + 800, (int)cam_y + 600, (int)x, (int)y, (int)x + (int)w, (int)y + (int)h);
}

// CALC CAMERA POS
void Render::CalcCameraPos(double* ret_x, double* ret_y) {
    // Old camera func, using "camera" memory
    if (GM_CAMERA_Y != 0 && GM_CAMERA_X != 0) {
        double* pCameraX = (GM_CAMERA_X);
        double* pCameraY = (GM_CAMERA_Y);
        double val;
        if (ret_x != nullptr) {
            val = pCameraX[1];
            *ret_x = val - val - val; // Fix backwards smbx camera
        }
        if (ret_y != nullptr) {
            val = pCameraY[1];
            *ret_y = val - val - val; // Fix backwards smbx camera
        }
    }
}