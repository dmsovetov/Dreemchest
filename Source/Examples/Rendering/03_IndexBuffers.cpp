/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#include <Dreemchest.h>

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

static f32 s_vertices[] =
{
    -0.7f, -0.7f, 0.0f,
     0.7f, -0.7f, 0.0f,
     0.7f,  0.7f, 0.0f,
    -0.7f,  0.7f, 0.0f,
};

// This time we will also declare an index buffer to render a rectangle.
static u16 s_indices[] =
{
    0, 1, 2, // First triangle
    0, 2, 3, // Second triangle
};

class IndexBuffers : public RenderingApplicationDelegate
{
    StateBlock8 m_renderStates;
    RenderFrame m_renderFrame;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        Renderer::InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position);
        Renderer::VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(s_vertices, sizeof(s_vertices));
        
        // Now request an index buffer handle.
        Renderer::IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(s_indices, sizeof(s_indices));
        
        // As before configure a render state block...
        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
        
        // ...but also bind an index buffer
        m_renderStates.bindIndexBuffer(indexBuffer);
    }

    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        RenderCommandBuffer& commands = m_renderFrame.entryPoint();
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), Renderer::ClearAll);
        
        // This is a main difference - we have to invoke a drawIndexed method.
        commands.drawIndexed(0, Renderer::PrimTriangles, 0, 6, &m_renderStates);

        m_renderingContext->display(m_renderFrame);
    }
};

dcDeclareApplication(new IndexBuffers)
