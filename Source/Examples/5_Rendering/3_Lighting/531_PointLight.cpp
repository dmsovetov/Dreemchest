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
    "cbuffer Projection projection : 0;                                         \n"
    "cbuffer Camera     camera     : 1;                                         \n"
    "cbuffer Instance   instance   : 2;                                         \n"
    "                                                                           \n"
    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"
    "                                                                           \n"
    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_wsNormal  = (instance.inverseTranspose                                 \n"
    "               * vec4(gl_Normal, 1.0)).xyz                                 \n"
    "               ;                                                           \n"

    "   v_wsVertex  = (instance.transform * gl_Vertex).xyz;                     \n"

    "   gl_Position = projection.transform                                      \n"
    "               * camera.transform                                          \n"
    "               * vec4(v_wsVertex, 1.0)                                     \n"
    "               ;                                                           \n"
    "}                                                                          \n"
;

static String s_fragmentShader =
    "cbuffer Camera     camera   : 1;                                           \n"
    "cbuffer Instance   instance : 2;                                           \n"
    "cbuffer Light      light    : 3;                                           \n"

    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"

    "float attenuation(vec3 point, vec3 light, vec4 factors)                    \n"
    "{                                                                          \n"
    "   float d = length(point - light);                                        \n"
    "   return 1.0 / (factors.x + factors.y * d + factors.z * d * d);           \n"
    "}                                                                          \n"

    "vec2 phong(vec3 L, vec3 N, vec3 V)                                         \n"
    "{                                                                          \n"
    "   float diffuse  = max(dot(L, N), 0.0);                                   \n"
    "   vec3  R        = reflect(L, N);                                         \n"
    "   float specular = max(dot(V, R), 0.0);                                   \n"
    "   return vec2(diffuse, specular);                                         \n"
    "}                                                                          \n"

    "vec3 lightColor(vec3 L, vec3 N, vec3 V)                                    \n"
    "{                                                                          \n"
    "   vec2 l = phong(L, N, V);                                                \n"
    "   return light.color * (l.x + pow(l.y, 32.0));                            \n"
    "}                                                                          \n"

    "void main()                                                                    \n"
    "{                                                                              \n"
    "   vec3 normal     = normalize(v_wsNormal);                                    \n"
    "   vec3 view       = normalize(v_wsVertex - camera.position);                  \n"
    "   vec3 L          = normalize(light.position - v_wsVertex);                   \n"

    "   float att   = attenuation(v_wsVertex, light.position, light.attenuation);   \n"
    "   vec2  l     = phong(L, normal, view);                                       \n"
    "   vec3  color = att * light.intensity * light.color * (l.x + pow(l.y, 32.0)); \n"

    "   gl_FragColor = vec4(color, 1.0);                                            \n"
    "}                                                                              \n"
    ;

static String s_vertexConstantColor =
    "cbuffer Projection projection : 0;                                         \n"
    "cbuffer Camera     camera     : 1;                                         \n"
    "cbuffer Instance   instance   : 2;                                         \n"

    "void main()                                                                \n"
    "{                                                                          \n"
    "   gl_Position = projection.transform                                      \n"
    "               * camera.transform                                          \n"
    "               * instance.transform                                        \n"
    "               * gl_Vertex                                                 \n"
    "               ;                                                           \n"
    "}                                                                          \n"
    ;

static String s_fragmentConstantColor =
    "void main()                                                                \n"
    "{                                                                          \n"
    "   gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);                                \n"
    "}                                                                          \n"
    ;

struct Light
{
    Vec3    position;
    Rgb     color;
    f32     intensity;
    Vec4    attenuation;
    static UniformElement Layout[];
} s_light;

UniformElement Light::Layout[] =
{
      { "position",    UniformElement::Vec3,  offsetof(Light, position)     }
    , { "color",       UniformElement::Vec3,  offsetof(Light, color)        }
    , { "intensity",   UniformElement::Float, offsetof(Light, intensity)    }
    , { "attenuation", UniformElement::Vec4,  offsetof(Light, attenuation)  }
    , { NULL }
};

class PointLights : public Framework::ApplicationDelegate
{
    StateBlock8     m_renderStates;
    RenderItem      m_platform;
    RenderItem      m_object;
    RenderItem      m_column;
    RenderItem      m_sphere;
    ConstantBuffer_ m_instanceConstantBuffer;
    ConstantBuffer_ m_lightConstantBuffer;
    Program         m_programConstantColor;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First initialize objects that will be rendered
        m_platform = Framework::createRenderItemFromMesh(m_renderingContext, "Assets/Meshes/platform.obj");
        m_object   = Framework::createRenderItemFromMesh(m_renderingContext, "Assets/Meshes/bunny_decimated.obj");
        m_column   = Framework::createRenderItemFromMesh(m_renderingContext, "Assets/Meshes/column.obj");
        m_sphere   = Framework::createRenderItemFromMesh(m_renderingContext, "Assets/Meshes/sphere.obj");

        setCameraPosition(Vec3(0.0f, 2.0f, -2.0f));
        
        // Configure instance constant buffer
        {
            UniformLayout layout = m_renderingContext->requestUniformLayout("Instance", Framework::Instance::Layout);
            m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Framework::Instance), layout);
            m_renderStates.bindConstantBuffer(m_instanceConstantBuffer, 2);
        }
        
        // Configure a light constant buffer
        {
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Light", Light::Layout);
            m_lightConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Light), uniformLayout);
            m_renderStates.bindConstantBuffer(m_lightConstantBuffer, 3);
        }
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);
        
        m_programConstantColor = m_renderingContext->requestProgram(s_vertexConstantColor, s_fragmentConstantColor);
    }
 
    virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_OVERRIDE
    {
        // Push the default state
        StateScope defaultScope = stateStack.push(&m_renderStates);
        
        static f32 s_time = 0.0f;
        s_time += dt;
        
        // Update light parameters
        {
            f32 st              = sinf(s_time);
            f32 ct              = cosf(s_time);
            s_light.position    = Vec3(st, 1.5f, ct) * 0.7f;
            s_light.attenuation = Vec4(0.0f, 0.0f, 1.0f);
            s_light.color       = Rgb(1.0f, 1.0f, 1.0f);
            s_light.intensity   = 0.5f;
            commands.uploadConstantBuffer(m_lightConstantBuffer, &s_light, sizeof(s_light));
        }
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);

        // Render the platform
        renderItem(commands, m_platform, Matrix4::scale(1.5f, 1.0f, 1.5f));
        
        // Now the stanford bunny
        renderItem(commands, m_object, Matrix4::scale(0.5f, 0.5f, 0.5f));

        // And finally columns
        for (s32 x = -1; x <= 1; x++)
        {
            for (s32 z = -1; z <= 1; z++)
            {
                renderItem(commands, m_column, Matrix4::scale(0.1f, 0.1f, 0.1f) * Matrix4::translation(x * 10.0f, 1.0f, z * 10.0f));
            }
        }
        
        // Render light sources for debugging
        StateScope constantColorScope = stateStack.newScope();
        constantColorScope->bindProgram(m_programConstantColor);
        renderItem(commands, m_sphere, Matrix4::translation(s_light.position) * Matrix4::scale(0.05f, 0.05f, 0.05f));
        
        m_renderingContext->display(frame);
    }
    
    void renderItem(RenderCommandBuffer& commands, const RenderItem& item, const Matrix4& transform = Matrix4())
    {
        Framework::Instance instance = Framework::Instance::fromTransform(transform);
        commands.uploadConstantBuffer(m_instanceConstantBuffer, &instance, sizeof(instance));
        commands.drawItem(0, item);
    }
};

dcDeclareApplication(new PointLights)
