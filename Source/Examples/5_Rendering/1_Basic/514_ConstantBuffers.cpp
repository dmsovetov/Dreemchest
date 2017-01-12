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
#include "../Examples.h"

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

// A fragment shader that takes data from a passed constant buffer
static String s_fragmentShader =
    "cbuffer Material material : 0;                                     \n" // This line declares a constant buffer of type 'Material' that is bound to unit 0
    "                                                                   \n"
    "void main()                                                        \n"
    "{                                                                  \n"
    "   gl_FragColor = vec4(material.color * material.brightness, 1.0); \n"
    "}                                                                  \n"
    ;

// Declare a C structure of our constant buffer
struct Material
{
    Vec3    color;
    f32     brightness;
} s_material;

// Every constant buffer should have an associated layout
static UniformElement s_bufferLayout[] =
{
      { "color",      UniformElement::Vec3,  offsetof(Material, color)       }
    , { "brightness", UniformElement::Float, offsetof(Material, brightness)  }
    , { NULL }
};

class ConstantBuffers : public RenderingApplicationDelegate
{
    StateBlock8 m_renderStates;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();
        
        if (!initialize(800, 600))
        {
            application->quit(-1);
        }

        InputLayout inputLayout  = m_renderingContext->requestInputLayout(VertexFormat::Position);
        
        // Initialize a vertex buffer from a preset data.
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(Examples::Triangle, sizeof(Examples::Triangle));
        
        // To not bother with vertex shader we are using an identity vertex shader
        // preset here, that just passes an input to an ouput.
        Program program = m_renderingContext->requestProgram(Examples::VertexIdentity, s_fragmentShader);
        
        // Initialize a material constant buffer
        s_material.color = Vec4(1.0f, 0.5f, 0.25f, 1.0f);
        s_material.brightness = 0.75f;
        
        // Construct the material uniform layout from an array of UniformElement items
        // and register it with an identifier 'Material'.
        UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Material", s_bufferLayout);
        
        // Create the constant buffer.
        ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&s_material, sizeof(s_material), uniformLayout);
        
        m_renderStates.bindVertexBuffer(vertexBuffer);
        m_renderStates.bindInputLayout(inputLayout);
        m_renderStates.bindProgram(program);
        
        // Bind a created constant buffer to a rendering states block
        m_renderStates.bindConstantBuffer(constantBuffer, 0);
    }

    virtual void handleRenderFrame(const Window::Update& e) NIMBLE_OVERRIDE
    {
        RenderFrame frame(m_renderingContext->defaultStateBlock());
        RenderCommandBuffer& commands = frame.entryPoint();
        
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        commands.drawPrimitives(0, PrimTriangles, 0, 3, m_renderStates);

        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new ConstantBuffers)
