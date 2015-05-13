#ifndef RenderGLOp_H
#define RenderGLOp_H

#include "RenderOp.h"
#include "../GLEngineCmds.h"

class RenderGLOp : public RenderOp {
public:
    RenderGLOp(const GLEngineCmd &cmd);
    void Draw(Renderer* renderer);
    GLEngineCmd m_Cmd;
};

#endif
