#include "PerformanceTimer.h"
#include "Globals.h"

PerformanceTimer::PerformanceTimer() {
    ZeroMemory(&SMBXStartTime, sizeof(SMBXStartTime));
    ZeroMemory(&SMBXEndTime, sizeof(SMBXEndTime));
    ZeroMemory(&SMBXCurTime, sizeof(SMBXCurTime));
    ZeroMemory(&SMBXDisplayTime, sizeof(SMBXDisplayTime));
    ZeroMemory(&LunaStartTime, sizeof(LunaStartTime));
    ZeroMemory(&LunaEndTime, sizeof(LunaEndTime));
    ZeroMemory(&LunaCurTime, sizeof(LunaCurTime));
    ZeroMemory(&LunaDisplayTime, sizeof(LunaDisplayTime));
}

void PerformanceTimer::StartSMBX() {
    QueryPerformanceCounter(&SMBXStartTime);	
}

void PerformanceTimer::StopSMBX() {
    QueryPerformanceCounter(&SMBXEndTime);
    SMBXCurTime.QuadPart = ((SMBXCurTime.QuadPart + (SMBXEndTime.QuadPart - SMBXStartTime.QuadPart)) + SMBXCurTime.QuadPart) / 2;
    if((gFrames & 0x0000000F) == 0x0000000F) {
        SMBXDisplayTime.QuadPart = SMBXCurTime.QuadPart;
    }
}

void PerformanceTimer::ResetSMBX() {
    ZeroMemory(&SMBXCurTime, sizeof(SMBXCurTime));
    ZeroMemory(&SMBXDisplayTime, sizeof(SMBXDisplayTime));
}

LONGLONG PerformanceTimer::GetSMBXPerformance() {
    return SMBXCurTime.QuadPart;
}



void PerformanceTimer::StartLuna() {
    QueryPerformanceCounter(&LunaStartTime);	
}

void PerformanceTimer::StopLuna() {
    QueryPerformanceCounter(&LunaEndTime);
    LunaCurTime.QuadPart = ((LunaCurTime.QuadPart + (LunaEndTime.QuadPart - LunaStartTime.QuadPart)) + LunaCurTime.QuadPart) / 2;
    if((gFrames & 0x0000000F) == 0x0000000F) {
        LunaDisplayTime.QuadPart = LunaCurTime.QuadPart;
    }
}

void PerformanceTimer::ResetLuna() {
    ZeroMemory(&LunaCurTime, sizeof(LunaCurTime));
    ZeroMemory(&LunaDisplayTime, sizeof(LunaDisplayTime));
}

LONGLONG PerformanceTimer::GetLunaPerformance() {
    return LunaCurTime.QuadPart;
}