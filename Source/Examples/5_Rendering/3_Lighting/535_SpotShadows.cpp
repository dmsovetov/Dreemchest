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
    "cbuffer Shadow     shadow     : 4;                                         \n"
    "                                                                           \n"
    "attribute vec4 a_position;                                                 \n"
    "attribute vec3 a_normal;                                                   \n"
    "                                                                           \n"
    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"
    "varying vec4 v_lsVertex;                                                   \n"
    "                                                                           \n"
    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_wsNormal  = (instance.inverseTranspose                                \n"
    "               * vec4(a_normal, 1.0)).xyz                                  \n"
    "               ;                                                           \n"

    "   v_wsVertex  = (instance.transform * a_position).xyz;                    \n"
    "   v_lsVertex  = shadow.transform * instance.transform * a_position;       \n"

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
    "cbuffer Shadow     shadow   : 4;                                           \n"

    "varying vec3 v_wsVertex;                                                   \n"
    "varying vec3 v_wsNormal;                                                   \n"
    "varying vec4 v_lsVertex;                                                   \n"

    "uniform sampler2D Texture0;                                                \n"

    "float random(vec3 seed, int i)                                             \n"
    "{                                                                          \n"
    "    vec4 seed4 = vec4(seed,i);                                             \n"
    "    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));    \n"
    "    return fract(sin(dot_product) * 43758.5453);                           \n"
    "}                                                                          \n"

    "float sampleShadow(vec4 lightSpaceCoord)                                   \n"
    "{                                                                          \n"
    "   vec3 projected = lightSpaceCoord.xyz / lightSpaceCoord.w;               \n"
    "   projected = projected * 0.5 + 0.5;                                      \n"
    "   float result = 0.0;                                                     \n"
    "   for (int i = 0; i < shadow.size; i++)                                   \n"
    "   {                                                                       \n"
    "       float r       = float(shadow.size) * random(gl_FragCoord.xyy, i);   \n"
    "       int index     = int(mod(r, float(shadow.size)));                    \n"
    "       vec2  offset  = shadow.kernel[index];                               \n"
    "       float closest = texture2D(Texture0, projected.xy + offset).r;       \n"
    "       result += projected.z < closest ? 1.0 : 0.0;                        \n"
    "   }                                                                       \n"
    "   return result / float(shadow.size);                                     \n"
    "}                                                                          \n"

    "vec3 project(vec4 clipSpace)                                               \n"
    "{                                                                          \n"
    "   vec3 projected = clipSpace.xyz / clipSpace.w;                           \n"
    "   return projected * 0.5 + 0.5;                                           \n"
    "}                                                                          \n"

    "float attenuation(vec3 point, vec3 light, vec4 factors)                    \n"
    "{                                                                          \n"
    "   float d = length(point - light);                                        \n"
    "   return 1.0 / (factors.x + factors.y * d + factors.z * d * d);           \n"
    "}                                                                          \n"

    "float cutoff(vec3 direction, vec3 light, float angle)                      \n"
    "{                                                                          \n"
    "   float spot = max(dot(direction, light), 0.0);                           \n"
    "   return 1.0 - (1.0 - spot) * 1.0 / (1.0 - angle);                        \n"
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
    "   vec3 direction  = normalize(-light.position);                               \n"
    "   vec3 view       = normalize(v_wsVertex - camera.position);                  \n"
    "   vec3 L          = normalize(light.position - v_wsVertex);                   \n"

    "   float att     = attenuation(v_wsVertex, light.position, light.attenuation);             \n"
    "   float s       = sampleShadow(v_lsVertex);                                               \n"
    "   float falloff = cutoff(-L, light.direction, light.cutoff);                              \n"
    "   vec2  l       = phong(L, normal, view);                                                 \n"
    "   vec3  color   = s * att * falloff * light.intensity * light.color * (l.x + pow(l.y, 32.0)); \n"

    "   gl_FragColor = vec4(color, 1.0);                                          \n"
    "}                                                                              \n"
    ;

static String s_vertexShadow =
    "cbuffer Instance instance  : 2;                                            \n"
    "cbuffer Shadow   shadow    : 4;                                            \n"

    "attribute vec4 a_position;                                                 \n"

    "void main()                                                                \n"
    "{                                                                          \n"
    "   gl_Position = shadow.transform * instance.transform * a_position;       \n"
    "}                                                                          \n"
    ;

struct Light
{
    Vec3    position;
    Rgb     color;
    f32     intensity;
    Vec4    attenuation;
    Vec3    direction;
    f32     cutoff;
    static UniformElement Layout[];
} s_light;

UniformElement Light::Layout[] =
{
      { "position",    UniformElement::Vec3,  offsetof(Light, position)     }
    , { "color",       UniformElement::Vec3,  offsetof(Light, color)        }
    , { "intensity",   UniformElement::Float, offsetof(Light, intensity)    }
    , { "attenuation", UniformElement::Vec4,  offsetof(Light, attenuation)  }
    , { "direction",   UniformElement::Vec3,  offsetof(Light, direction)    }
    , { "cutoff",      UniformElement::Float, offsetof(Light, cutoff)       }
    , { NULL }
};

struct Shadow
{
    enum { MaxKernelSize = 16 };
    Matrix4 transform;
    s32     size;
    Vec2    kernel[MaxKernelSize];
    static UniformElement Layout[];
};

UniformElement Shadow::Layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Shadow, transform)                       }
    , { "size",      UniformElement::Integer, offsetof(Shadow, size)                            }
    , { "kernel",    UniformElement::Vec2,    offsetof(Shadow, kernel), Shadow::MaxKernelSize   }
    , { NULL }
};

class PointLights : public Framework::ApplicationDelegate
{
    StateBlock8     m_renderStates;
    ConstantBuffer_ m_lightConstantBuffer;
    ConstantBuffer_ m_shadowConstantBuffer;
    Program         m_programShadow;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        // First initialize objects that will be rendered
        setCameraPosition(Vec3(0.0f, 1.0f, -1.0f));

        // Configure a light constant buffer
        {
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Light", Light::Layout);
            m_lightConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Light), uniformLayout);
            m_renderStates.bindConstantBuffer(m_lightConstantBuffer, 3);
        }
        
        // Configure a shadow constant buffer
        {
            UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Shadow", Shadow::Layout);
            m_shadowConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Shadow), uniformLayout);
            m_renderStates.bindConstantBuffer(m_shadowConstantBuffer, 4);
        }
        
        // Create a simple shader program
        Program program = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        m_renderStates.bindProgram(program);

        m_programShadow = m_renderingContext->requestProgram(s_vertexShadow);
    }
 
    virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_OVERRIDE
    {
        // Push the default state
        StateScope defaultScope = stateStack.push(&m_renderStates);
        
        // Update light parameters
        {
            f32 st           = sinf(time());
            f32 ct           = cosf(time());
            s_light.intensity   = 1.0f;
            s_light.color       = Rgb(1.0f, 1.0f, 1.0f);
            s_light.position    = Vec3(-0.5f, 1.0f, -0.5f);
            s_light.direction   = Vec3(st, -1.0f, ct);
            s_light.attenuation = Vec4(0.0f, 0.0f, 1.0f);
            s_light.cutoff      = acosf(radians(35.0f));
            s_light.direction.normalize();
            commands.uploadConstantBuffer(m_lightConstantBuffer, &s_light, sizeof(s_light));
            
            Shadow shadow;
            shadow.transform = Matrix4::perspective(70.0f, 1.0f, 0.01f, 100.0f) * Matrix4::lookAt(s_light.position, s_light.position + s_light.direction, Vec3::axisY());
            shadow.size      = 16;
            shadow.kernel[ 0] = Vec2( -0.942016240f, -0.39906216f ) * (1.0f / 512.0f);
            shadow.kernel[ 1] = Vec2(  0.945586090f, -0.76890725f ) * (1.0f / 512.0f);
            shadow.kernel[ 2] = Vec2( -0.094184101f, -0.92938870f ) * (1.0f / 512.0f);
            shadow.kernel[ 3] = Vec2(  0.344959380f,  0.29387760f ) * (1.0f / 512.0f);
            shadow.kernel[ 4] = Vec2( -0.915885810f,  0.45771432f ) * (1.0f / 512.0f);
            shadow.kernel[ 5] = Vec2( -0.815442320f, -0.87912464f ) * (1.0f / 512.0f);
            shadow.kernel[ 6] = Vec2( -0.382775430f,  0.27676845f ) * (1.0f / 512.0f);
            shadow.kernel[ 7] = Vec2(  0.974843980f,  0.75648379f ) * (1.0f / 512.0f);
            shadow.kernel[ 8] = Vec2(  0.443233250f, -0.97511554f ) * (1.0f / 512.0f);
            shadow.kernel[ 9] = Vec2(  0.537429810f, -0.47373420f ) * (1.0f / 512.0f);
            shadow.kernel[10] = Vec2( -0.264969110f, -0.41893023f ) * (1.0f / 512.0f);
            shadow.kernel[11] = Vec2(  0.791975140f,  0.19090188f ) * (1.0f / 512.0f);
            shadow.kernel[12] = Vec2( -0.241888400f,  0.99706507f ) * (1.0f / 512.0f);
            shadow.kernel[13] = Vec2( -0.814099550f,  0.91437590f ) * (1.0f / 512.0f);
            shadow.kernel[14] = Vec2(  0.199841260f,  0.78641367f ) * (1.0f / 512.0f);
            shadow.kernel[15] = Vec2(  0.143831610f, -0.14100790f ) * (1.0f / 512.0f);
            commands.uploadConstantBuffer(m_shadowConstantBuffer, &shadow, sizeof(shadow));
        }
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Render the shadow map
        TransientTexture shadow = commands.acquireTexture2D(512, 512, TextureD16 | TextureLinear);
        
        {
            RenderCommandBuffer& shadows = commands.renderToTexture(shadow);
            shadows.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
            StateScope shadowScope = stateStack.newScope();
            shadowScope->bindProgram(m_programShadow);
            shadowScope->setCullFace(TriangleFaceFront);
            
            renderColumnsScene(shadows);
        }
        
        // Render lit scene
        StateScope shadowScope = stateStack.newScope();
        shadowScope->bindTexture(shadow, 0);
        renderColumnsScene(commands);
        
        commands.releaseTexture(shadow);
        
        // Render light sources for debugging
        renderPinkItem(commands, stateStack, m_sphere, Matrix4::translation(s_light.position) * Matrix4::scale(0.05f, 0.05f, 0.05f));
        
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new PointLights)
