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
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
};

static String s_vertexShader =
    "void main()                                    \n"
    "{                                              \n"
    "   gl_Position = gl_Vertex;                    \n"
    "}                                              \n"
    ;

static String s_fragmentShader =
    "struct Material { vec4 color; float brightness; };         \n"
    "uniform Material material;                                 \n"
    "void main()                                                \n"
    "{                                                          \n"
    "   gl_FragColor = material.color * material.brightness;    \n"
    "}                                                          \n"
    ;

// Declare a C structure of our constant buffer
struct Material
{
    Vec4    color;
    f32     brightness;
} s_material;

// Every constant buffer should have an associated layout
static ConstantBufferElement s_bufferLayout[] =
{
      { "material.color",      ConstantBufferElement::Vec4,  offsetof(Material, color)       }
    , { "material.brightness", ConstantBufferElement::Float, offsetof(Material, brightness)  }
    , { NULL }
};

class ConstantBuffers : public RenderingApplicationDelegate
{
    StateBlock m_renderStates;
    RenderFrame m_renderFrame;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();
        
        if (!initialize(800, 600))
        {
            application->quit(-1);
        }

        InputLayout inputLayout = m_renderingContext->requestInputLayout(VertexFormat::Position);
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(s_vertices, sizeof(s_vertices));
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        
        // Initialize a material constant buffer
        s_material.color = Vec4(1.0f, 0.5f, 0.25f, 1.0f);
        s_material.brightness = 0.75f;
        
        // Create a constant buffer
        ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&s_material, sizeof(s_material), s_bufferLayout);
        
        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
        m_renderStates.bindProgram(program);
        
        // Bind a created constant buffer to a rendering states block
        m_renderStates.bindConstantBuffer(constantBuffer, 0);
    }

    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        CommandBuffer& commands = m_renderFrame.entryPoint();
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        commands.drawPrimitives(0, PrimTriangles, 0, 3, &m_renderStates);

        m_renderingContext->display(m_renderFrame);
    }
};

dcDeclareApplication(new ConstantBuffers)
