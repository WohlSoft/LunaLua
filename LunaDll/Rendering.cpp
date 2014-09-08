#include "Rendering.h"
#include "Defines.h"
#include "Level.h"
#include "MiscFuncs.h"
#include "BMPBox.h"
#include "RenderOp.h"
#include "PlayerMOB.h"

using namespace std;

// CTOR
Renderer::Renderer() {
	m_hScreenDC = NULL;
}

// DTOR
Renderer::~Renderer() {
	if(m_hScreenDC != NULL)
		DeleteDC(m_hScreenDC);	
}

// UPDATE HDC -- Try and get the main screen HDC from SMBX. Fails if SMBX in bad state or screen not ready
bool Renderer::ReloadScreenHDC() {
	if(GM_SCRN_HDC == 0) { // does SMBX itself have the DC yet?
		m_hScreenDC = 0;
		return false;	
	}
	m_hScreenDC = (HDC)GM_SCRN_HDC;
	return true;
}

// LOAD BITMAP RESOURCE - Load an image resource with given resource code. If resource code exists, replaces old image
bool Renderer::LoadBitmapResource(std::wstring filename, int resource_code, int transparency_color) {
	// If slot in use, delete old image
	DeleteImage(resource_code);

	// Concoct full filepath
	wstring world_dir = wstring((wchar_t*)GM_FULLDIR);
	wstring full_path = world_dir.append(Level::GetName());	
	full_path = removeExtension(full_path);
	full_path = full_path.append(L"\\"); // < path into level folder
	full_path = full_path + filename;
	
	// Create and store the image resource
	BMPBox* pNewbox = new BMPBox(full_path, m_hScreenDC);

	if(pNewbox->ImageLoaded() == false) {
		delete pNewbox;
		gLogger.Log(L"BMPBox image load failed", LOG_STD);
		return false;
	}

	pNewbox->m_TransColor = transparency_color;
	StoreImage(pNewbox, resource_code);

	return true;
}

//STORE IMAGE
void Renderer::StoreImage(BMPBox* bmp, int resource_code) {
	LoadedImages[resource_code] = bmp;
}

// DELETE IMAGE - Deletes the image resource and returns true if it exists, otherwise does nothing and returns false
bool Renderer::DeleteImage(int resource_code) {
	if(LoadedImages.find(resource_code) != LoadedImages.end()) {		
		delete LoadedImages[resource_code];
		LoadedImages.erase(resource_code);
		return true;
	}
	return false;
}

// CALC CAMERA POS
void Render::CalcCameraPos(double* ret_x, double* ret_y) {

	// Camera func, calculated from player position and boundaries
	PlayerMOB* demo = Player::Get(1);
	if(false) {
		gLunaRender.SafePrint(to_wstring((long long) demo->CurXPos),3,300,300);
		gLunaRender.SafePrint(to_wstring((long long) demo->CurYPos),3,300,350);
		
		double cx = demo->CurXPos - (400 - (demo->Width / 2));
		double cy = demo->CurYPos - (300 - (demo->Height / 2));

		// Force camera to fit in level bounds
		RECT rect;
		Level::GetBoundary(&rect, demo->CurrentSection);
		if(cx < rect.left)
			cx = rect.left;
		if(cx + 800 > rect.right)
			cx = rect.right - 800;
		if(cy < rect.top)
			cy = rect.top;
		if(cy + 600 > rect.bottom)
			cy = rect.bottom - 600;
		
		*ret_x = cx;
		*ret_y = cy;
	}

	// Old camera func, using "camera" memory
	if(GM_CAMERA_Y != 0 && GM_CAMERA_X != 0) {
		double* pCameraX = (double*)(GM_CAMERA_X);
		double* pCameraY = (double*)(GM_CAMERA_Y);
		double val;		
		if(ret_x != NULL) {
			val = pCameraX[1];
			*ret_x = val - val - val; // Fix backwards smbx camera
		}
		if(ret_y != NULL) {
			val = pCameraY[1];
			*ret_y = val - val - val; // Fix backwards smbx camera
		}
	}
}

// ADD OP
void Renderer::AddOp(RenderOp* op) {
	this->RenderOperations.push_back(op);
}

// PRINT -- Calls SMBX's own print function. Only works during the HUD hook
void Render::Print(std::wstring str, int font_type, float x, float y) {
	typedef int printfunc(wchar_t**, int*, float*, float*);
	int font_ptr = font_type;
	float x_ptr = x;
	float y_ptr = y;
	wchar_t wbuf[1000];
	ZeroMemory(wbuf, 2000);
	wchar_t* wbufptr = wbuf+2;

	// Place the length of the string in bytes first
	int strlen = str.length() * 2;
	wbuf[0] = strlen;

	// Copy the string's bytes 4 bytes ahead of the len
	memcpy((void*)&wbuf[2], str.data(), strlen);

	printfunc* f = (printfunc*)GF_PRINT;
	int ret = f((&wbufptr), &font_ptr, &x_ptr, &y_ptr);
}

// SAFE PRINT
void Renderer::SafePrint(std::wstring str, int font_type, float x, float y) {
	RenderStrings.push_back(RenderString(str, font_type, x, y));
}

void Renderer::SafePrint(RenderString str) {
	RenderStrings.push_back(str);
}

// DRAW OP -- Process a render operation, draw, and decrement active timer remaining
void Renderer::DrawOp(RenderOp* op) {
	if(op->m_FramesLeft < 1) 
		return;
	op->Draw(this);
	op->m_FramesLeft--;
}

// RENDER ALL
void Renderer::RenderAll() {

	ClearExpired();

	// Do render ops
	for (std::list<RenderOp*>::iterator iter = RenderOperations.begin(), end = RenderOperations.end();	iter != end; ++iter) {
		DrawOp((*iter));
	}

	// Format debug messages and enter them into renderstring list
	int dbg_x = 350;
	int dbg_y = 160;
	for each(std::wstring dbg in DebugMessages) {
		SafePrint(RenderString(dbg, 3, (float)dbg_x, (float)dbg_y));
		dbg_y += 20;
		if(dbg_y > 560) {
			dbg_y = 160;
			dbg_x += 190;
		}
	}

	for (std::list<RenderString>::iterator iter = RenderStrings.begin(), end = RenderStrings.end();	iter != end; ++iter) {				
		Render::Print((*iter).mString, (*iter).mFontType, (*iter).x, (*iter).y);
	}

	// old version
	//while(RenderStrings.empty() == false) {
		//RenderString cur_string = RenderStrings.back();
		//Render::Print(cur_string.mString, cur_string.mFontType, cur_string.x, cur_string.y);
		//RenderStrings.pop_back();
	//}
}

// CLEAR EXPIRED -- Delete render ops with no display frames left
//				 -- Don't call this while iterating over renderops
void Renderer::ClearExpired() {
	std::list<RenderOp*>::iterator iter = RenderOperations.begin();
	std::list<RenderOp*>::iterator end = RenderOperations.end();

	while(iter != RenderOperations.end()) {
		RenderOp* pOp = *iter;
		if((*iter)->m_FramesLeft <= 0) {
			delete (*iter);
			iter = RenderOperations.erase(iter);
		} else {
			++iter;
		}
	}
}

// CLEAR ALL
void Renderer::ClearAll() {
	this->DebugMessages.clear();
	this->RenderStrings.clear();

	//while(CustomBitmaps.empty() == false) {
	//	DeleteObject(CustomBitmaps.back());
	//	CustomBitmaps.pop_back();
	//}
};

// DEBUG PRINT
void Renderer::DebugPrint(std::wstring message) {
	this->DebugMessages.push_back(message);
}

void Renderer::DebugPrint(std::wstring message, double val) {
	this->DebugMessages.push_back(message + L" " + to_wstring((long long) val));
}