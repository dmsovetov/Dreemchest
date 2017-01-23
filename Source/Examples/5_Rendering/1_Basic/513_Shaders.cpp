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

// Declare a vertex shader code that will output a received vertex.
static String s_vertexShader =
    "void main()                                    \n"
    "{                                              \n"
    "   gl_Position = gl_Vertex;                    \n"
    "}                                              \n"
    ;

// Declare a fragment shader that will output fragments with a constant color.
static String s_fragmentShader =
    "void main()                                    \n"
    "{                                              \n"
    "   gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);    \n"
    "}                                              \n"
    ;

// Triangle vertex buffer.
const f32 Triangle[9] =
{
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
};

class Shaders : public RenderingApplicationDelegate
{
    StateBlock8 m_renderStates;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }

        Renderer::InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position);
        
        // Create
        Renderer::VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(Triangle, sizeof(Triangle));
        
        // Create a program that consists from a vertex and fragment shaders.
        Renderer::Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        
        // As always, configure the rendering states block
        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
        
        // And make sure to bind a previously created shader program
        m_renderStates.bindProgram(program);
    }

    virtual void handleRenderFrame(f32 dt) NIMBLE_OVERRIDE
    {
        RenderFrame& frame = m_renderingContext->allocateFrame();
        
        // Note, that a shader program was bound to a rendering states block
        // upon initialization, so this method stays same as in a
        // 'First Triangle' exmple.
        
        RenderCommandBuffer& commands = frame.entryPoint();
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        commands.drawPrimitives(0, PrimTriangles, 0, 3, m_renderStates);
        
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new Shaders)
