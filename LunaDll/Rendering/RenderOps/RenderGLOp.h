#ifndef RenderGLOp_H
#define RenderGLOp_H

#include <memory>
#include "RenderOp.h"
#include "../GL/GLEngineCmds.h"

class RenderGLOp : public RenderOp {
public:
    RenderGLOp(const std::shared_ptr<GLEngineCmd> &cmd);
    void Draw(Renderer* renderer);
    std::shared_ptr<GLEngineCmd> m_Cmd;
};

#endif
