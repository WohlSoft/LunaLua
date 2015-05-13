#include "RenderGLOp.h"
#include "../GLEngineProxy.h"

RenderGLOp::RenderGLOp(const GLEngineCmd &cmd) {
    m_PerCycleOnly = false;
    m_LastRenderedOn = 0;
    m_FramesLeft = 1;
    m_Cmd = cmd;
}

void RenderGLOp::Draw(Renderer* renderer) {
    if (g_GLEngine.IsEnabled())
    {
        g_GLEngine.QueueCmd(m_Cmd);
    }
}