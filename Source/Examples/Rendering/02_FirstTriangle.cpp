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

// An array of vertex coordinates.
static f32 s_vertices[] =
{
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
};

// This time our application delegate will be a subclass of the Renderer::RenderingApplicationDelegate
// to reduce an amount of code to be written to initialize an application.
class FirstTriangle : public RenderingApplicationDelegate
{
    // Here we declare a rendering state block that will be used in
    // rendering process. Render state block is a composition of
    // rendering states required for rendering, like vertex buffer
    // binding or blend state changes.
    StateBlock m_renderStates;
    
    // A render frame instance records all data and commands required
    // to render a single frame.
    RenderFrame m_renderFrame;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();
        
        // We will use the initialization method declared in Renderer::RenderingApplicationDelegate class
        // to create a window instance and a rendering context.
        if (!initialize(800, 600))
        {
            application->quit(-1);
        }

        // Here we create an input layout from a flexible vertex format with just a position inside.
        InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position);
        
        // Now request a vertex buffer handle.
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(s_vertices, sizeof(s_vertices));
        
        // Finally configure a render state block that will be used during rendering
        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
    }

    // This method should be implemented in a subclass of RenderingApplicationDelegate to
    // actually render a frame each time the window is updated.
    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        // A process of rendering a frame consist from a command buffer generation
        // and then submitting it to a device.
        // Here we take an entry point command buffer from our rendering frame.
        RenderCommandBuffer& commands = m_renderFrame.entryPoint();

        // Emit a command that clears a viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Now emit a drawPrimitives command with a render state block configured upon initialization
        // as a last argument.
        commands.drawPrimitives(0, PrimTriangles, 0, 3, &m_renderStates);
        
        // Rendering frame is now ready, so pass it to a rendering context to display it on a screen.
        m_renderingContext->display(m_renderFrame);
    }
};

// Now declare an application entry point with FirstTriangle application delegate.
dcDeclareApplication(new FirstTriangle)
