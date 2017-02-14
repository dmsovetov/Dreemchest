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
#include <Framework.h>

DC_USE_DREEMCHEST

using namespace Platform;
using namespace Renderer;

static String s_vertexShader =
    "cbuffer Projection projection : 0;                 \n"
    "cbuffer Camera     camera     : 1;                 \n"
    "cbuffer Instance   instance   : 2;                 \n"

    "attribute vec4 a_position;                         \n"
    "attribute vec3 a_normal;                           \n"

    "varying lowp vec3 v_color;                         \n"

    "void main()                                        \n"
    "{                                                  \n"
    "   v_color     = a_normal * 0.5 + 0.5;             \n"
    "   mat4 mvp    = projection.transform              \n"
    "               * camera.transform                  \n"
    "               * instance.transform                \n"
    "               ;                                   \n"

    "   gl_Position = mvp * a_position;                 \n"
    "}                                                  \n"
;

static String s_fragmentShader =
    "varying lowp vec3 v_color;                         \n"

    "void main()                                        \n"
    "{                                                  \n"
    "   gl_FragColor = vec4(v_color, 1.0);              \n"
    "}                                                  \n"
    ;

class Meshes : public RenderingApplicationDelegate
{
    StateBlock8 m_renderStates;
    RenderItem m_renderItem;
    ConstantBuffer_ m_instanceConstantBuffer;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First load a mesh from a file
        Framework::Mesh mesh = Framework::objFromFile("Assets/Meshes/bunny.obj");
        
        if (!mesh)
        {
            application->quit(-1);
        }
        
        // Now configure a render item from a loaded mesh
        VertexFormat vertexFormat  = mesh.vertexFormat;
        InputLayout inputLayout    = m_renderingContext->requestInputLayout(vertexFormat);
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(&mesh.vertexBuffer[0], mesh.vertexBuffer.size());
        
        m_renderItem.primitives = mesh.primitives;
        m_renderItem.first      = 0;
        m_renderItem.count      = mesh.vertexCount;
        m_renderItem.states.bindInputLayout(inputLayout);
        m_renderItem.states.bindVertexBuffer(vertexBuffer);

        if (mesh.indices.size())
        {
            IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(&mesh.indices[0], sizeof(u16) * mesh.indices.size());
            m_renderItem.states.bindIndexBuffer(indexBuffer);
            m_renderItem.indexed = true;
        }
        else
        {
            m_renderItem.indexed = false;
        }
        
        // Configure projection constant buffer
        {
            Framework::Projection projection = Framework::Projection::perspective(60.0f, m_window->width(), m_window->height(), 0.1f, 100.0f);
            
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Projection", Framework::Projection::Layout);
            ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&projection, sizeof(projection), uniformLayout);
            m_renderStates.bindConstantBuffer(constantBuffer, 0);
        }
        
        // Configure camera constant buffer
        {
            Framework::Camera camera = Framework::Camera::lookAt(Vec3(0.0f, 2.0f, -2.0f), Vec3(0.0f, 0.6f, 0.0f));

            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Camera", Framework::Camera::Layout);
            ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&camera, sizeof(camera), uniformLayout);
            m_renderStates.bindConstantBuffer(constantBuffer, 1);
        }
        
        // Configure instance constant buffer
        {
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Instance", Framework::Instance::Layout);
            m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Framework::Instance), uniformLayout);
            m_renderStates.bindConstantBuffer(m_instanceConstantBuffer, 2);
        }
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
    }
 
    virtual void handleRenderFrame(f32 dt) NIMBLE_OVERRIDE
    {
        RenderFrame& frame = m_renderingContext->allocateFrame();
        
        RenderCommandBuffer& commands = frame.entryPoint();
        
        static f32 s_rotation = 0.0f;
        s_rotation += dt;

        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Update an instance constant buffer
        Framework::Instance instance = Framework::Instance::fromTransform(Matrix4::rotateXY(0.0f, s_rotation));
        commands.uploadConstantBuffer(m_instanceConstantBuffer, &instance, sizeof(instance));
        
        // Push root rendering state block
        StateScope scope = frame.stateStack().push(&m_renderStates);
        
        // Render the item
        commands.drawItem(0, m_renderItem);
    
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new Meshes)
