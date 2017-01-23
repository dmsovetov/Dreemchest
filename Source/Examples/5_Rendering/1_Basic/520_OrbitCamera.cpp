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

    "varying vec3 v_color;                              \n"

    "void main()                                        \n"
    "{                                                  \n"
    "   v_color     = gl_Normal * 0.5 + 0.5;            \n"
    "   mat4 mvp    = projection.transform              \n"
    "               * camera.transform                  \n"
    "               ;                                   \n"

    "   gl_Position = mvp * gl_Vertex;                  \n"
    "}                                                  \n"
;

static String s_fragmentShader =
    "varying vec3 v_color;                              \n"

    "void main()                                        \n"
    "{                                                  \n"
    "   gl_FragColor = vec4(v_color, 1.0);              \n"
    "}                                                  \n"
    ;

class OrbitCamera : public RenderingApplicationDelegate
{
    StateBlock8 m_renderStates;
    Framework::Mesh mesh;
    ConstantBuffer_ m_cameraConstantBuffer;
    
    Vec2 m_polar;   //!< Camera polar coordinates.
    
    // Indicates that a mouse button is pressed and we should rotate the camera
    bool m_mouseButtonPressed;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First load a mesh from a file
        mesh = Framework::objFromFile("Assets/Meshes/bunny.obj");
        
        if (!mesh)
        {
            application->quit(-1);
        }
        
        // Initialize camera transform state
        m_polar              = Vec2(0.0f, 0.0f);
        m_mouseButtonPressed = false;
        
        // Now configure a mesh rendering states
        VertexFormat vertexFormat = mesh.vertexFormat;
        InputLayout inputLayout = m_renderingContext->requestInputLayout(vertexFormat);
        VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(&mesh.vertices[0], mesh.vertices.size());
        
        m_renderStates.bindInputLayout(inputLayout);
        m_renderStates.bindVertexBuffer(vertexBuffer);

        if (mesh.indices.size())
        {
            IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(&mesh.indices[0], sizeof(u16) * mesh.indices.size());
            m_renderStates.bindIndexBuffer(indexBuffer);
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
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Camera", Framework::Camera::Layout);
            m_cameraConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Framework::Camera), uniformLayout);
            m_renderStates.bindConstantBuffer(m_cameraConstantBuffer, 1);
        }

        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
    }
    
    // This method is called when mouse/touch is pressed.
    virtual void handleTouchBegan(const Window::TouchBegan& e) NIMBLE_OVERRIDE
    {
        m_mouseButtonPressed = true;
    }
    
    // This method is called when mouse/touch is released.
    virtual void handleTouchEnded(const Window::TouchEnded& e) NIMBLE_OVERRIDE
    {
        m_mouseButtonPressed = false;
    }
    
    // This method is called when mouse/touch is moved.
    virtual void handleTouchMoved(const Window::TouchMoved& e) NIMBLE_OVERRIDE
    {
        static s32 prevX = -1;
        static s32 prevY = -1;
        
        if (!m_mouseButtonPressed)
        {
            prevX = -1;
            prevY = -1;
            return;
        }
        
        if (prevX == -1 || prevY == -1)
        {
            prevX = e.x;
            prevY = e.y;
        }
        
        s32 dx = e.x - prevX;
        s32 dy = e.y - prevY;
        
        m_polar.x += dx * 0.25f;
        m_polar.y += dy * 0.25f;
        
        prevX = e.x;
        prevY = e.y;
    }
 
    virtual void handleRenderFrame(f32 dt) NIMBLE_OVERRIDE
    {
        RenderFrame& frame = m_renderingContext->allocateFrame();
        
        RenderCommandBuffer& commands = frame.entryPoint();
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Update the camera constant buffer
        Vec4 angles   = Vec4(cos(radians(m_polar.x)), sin(radians(m_polar.x)), cos(radians(m_polar.y)), sin(radians(m_polar.y)));
        Vec3 position = Vec3::axisX() * angles.x * angles.w + Vec3::axisZ() * angles.y * angles.w + Vec3::axisY() * angles.z;
        Vec3 target   = Vec3::zero();
        Framework::Camera camera = Framework::Camera::lookAt(position * 2.0f, target, Vec3::axisY());
        commands.uploadConstantBuffer(m_cameraConstantBuffer, &camera, sizeof(camera));

        // Render the mesh
        commands.drawPrimitives(0, mesh.primitives, 0, mesh.vertices.size(), m_renderStates);
    
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new OrbitCamera)
