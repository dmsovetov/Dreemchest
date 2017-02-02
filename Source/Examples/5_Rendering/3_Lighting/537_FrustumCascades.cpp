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
    "cbuffer Cascades   cascades : 4;                                           \n"

    "attribute vec4 a_position;                                                 \n"
    "attribute vec3 a_normal;                                                   \n"

    "varying vec3  v_wsNormal;                                                  \n"
    "varying float v_depth;                                                     \n"
    "varying vec4  v_lsVertex[4];                                               \n"

    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_wsNormal  = (instance.inverseTranspose                                \n"
    "               * vec4(a_normal, 1.0)).xyz                                  \n"
    "               ;                                                           \n"

    "   vec4 wsVertex = instance.transform * a_position;                        \n"
    "   v_depth       = -(camera.transform * wsVertex).z;                       \n"

    "   for (int i = 0; i < cascades.count; i++)                                \n"
    "   {                                                                       \n"
    "       vec4 lsVertex = cascades.transform[i] * wsVertex;                   \n"
    "       v_lsVertex[i] = lsVertex / lsVertex.w * 0.5 + 0.5;                  \n"
    "   }                                                                       \n"

    "   gl_Position = projection.transform                                      \n"
    "               * camera.transform                                          \n"
    "               * wsVertex                                                  \n"
    "               ;                                                           \n"
    "}                                                                          \n"
;

static String s_fragmentShader =
    "cbuffer Camera     camera   : 1;                                           \n"
    "cbuffer Instance   instance : 2;                                           \n"
    "cbuffer Cascades   cascades : 4;                                           \n"

    "varying vec3  v_wsNormal;                                                  \n"
    "varying float v_depth;                                                     \n"
    "varying vec4  v_lsVertex[4];                                               \n"

    "int selectCascadeInterval(float depth)                                     \n"
    "{                                                                          \n"
    "   for (int i = 0; i < cascades.count - 1; i++)                            \n"
    "   {                                                                       \n"
    "       if (depth < cascades.distances[i])                                  \n"
    "       {                                                                   \n"
    "           return i;                                                       \n"
    "       }                                                                   \n"
    "   }                                                                       \n"
    "   return cascades.count - 1;                                              \n"
    "}                                                                          \n"

    "int selectCascadeTexture()                                                 \n"
    "{                                                                          \n"
    "   for (int i = 0; i < cascades.count; i++)                                \n"
    "   {                                                                       \n"
    "       vec4 bounds = cascades.bounds[i];                                   \n"
    "       vec2 uv     = v_lsVertex[i].xy;                                     \n"
    "       if (uv.x >= bounds.x && uv.x <= bounds.z &&                         \n"
    "           uv.y >= bounds.y && uv.y <= bounds.w)                           \n"
    "       {                                                                   \n"
    "           return i;                                                       \n"
    "       }                                                                   \n"
    "   }                                                                       \n"
    "   return cascades.count - 1;                                              \n"
    "}                                                                          \n"

    "void main()                                                                \n"
    "{                                                                          \n"
    "   vec3  normal = normalize(v_wsNormal);                                   \n"
    "   vec3  dir    = normalize(vec3(1.0, 1.0, 1.0));                          \n"
    "   float dp     = max(dot(normal, dir), 0.0);                              \n"

    "#if defined(F_IntervalSelection)                                           \n"
    "   int cascade = selectCascadeInterval(v_depth);                           \n"
    "#else                                                                      \n"
    "   int cascade = selectCascadeTexture();                                   \n"
    "   vec4 unused = vec4(v_depth);                                            \n"
    "#endif //  F_IntervalSelection                                             \n"

    "   gl_FragColor = dp * cascades.colors[cascade];                           \n"
    "}                                                                          \n"
    ;

struct Cascades
{
    enum { MaxCascades = 4 };
    Matrix4 transform[MaxCascades];
    f32     distances[MaxCascades];
    Rgba    colors[MaxCascades];
    Vec4    bounds[MaxCascades];
    s32     count;
    static UniformElement Layout[];
};

UniformElement Cascades::Layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Cascades, transform), Cascades::MaxCascades }
    , { "distances", UniformElement::Float,   offsetof(Cascades, distances), Cascades::MaxCascades }
    , { "colors",    UniformElement::Vec4,    offsetof(Cascades, colors),    Cascades::MaxCascades }
    , { "bounds",    UniformElement::Vec4,    offsetof(Cascades, bounds),    Cascades::MaxCascades }
    , { "count",     UniformElement::Integer, offsetof(Cascades, count)                            }
    , { NULL }
};

class FrustumCascades : public Framework::ApplicationDelegate
{
    StateBlock8     m_renderStates;
    ConstantBuffer_ m_cascadesConstantBuffer;
    Program         m_programLight;
    RenderItem      m_unitCube;
    
    virtual void handleLaunched(Application* application) NIMBLE_OVERRIDE
    {
        Logger::setStandardLogger();

        if (!initialize(800, 600))
        {
            application->quit(-1);
        }
        
        setCameraPosition(Vec3(1.0f, 0.6f, 0.0f));
        
        // Configure cascades constant buffer
        {
            UniformLayout layout  = m_renderingContext->requestUniformLayout("Cascades", Cascades::Layout);
            m_cascadesConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Cascades), layout);
            m_renderStates.bindConstantBuffer(m_cascadesConstantBuffer, 4);
        }

        m_renderStates.bindProgram(m_programPink);
        
        // Create a light shader program
        m_programLight = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);

        m_unitCube = createUnitCube();
    }
    
    Matrix4 calculateCascadeTransform(s32 textureSize, const Sphere& worldSpaceBounds, const Matrix4& lightTransform) const
    {
        // Compute an inverse of a light matrix
        //Matrix4 inverseLight = lightTransform.inversed();
        
        // Transform a world space bounds to a light space
        Bounds lightSpaceBounds = Bounds::fromSphere(lightTransform * worldSpaceBounds.center(), worldSpaceBounds.radius());
        
        // Calculate a projection matrix based on a light-space cascade bounding box
        const Vec3& min = lightSpaceBounds.min();
        const Vec3& max = lightSpaceBounds.max();
        
        // IMPORTANT: minZ and maxZ are swapped!
        Matrix4 projection = Matrix4::ortho( min.x, max.x, min.y, max.y, max.z + 50.0f, min.z );
        
        // Return a final view-projection matrix for a cascade
        return projection * lightTransform;
    }
 
    virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_OVERRIDE
    {
        // Push the default state
        StateScope defaultScope = stateStack.push(&m_renderStates);
        
        Vec3 lightDirection = Vec3(sinf(time() * 0.5f), cosf(time() * 0.5f), 2.0f);
        
        // Update cascades
        {
            Frustum        frustum  = Frustum(m_camera.fov, m_window->aspectRatio(), m_camera.near, m_camera.far);
            Array<Frustum> splits   = frustum.split(4, 0.9f);
            Matrix4 cameraTransform = this->cameraTransform().inversed();
            Matrix4 lightTransform  = Matrix4::lookAt(lightDirection, Vec3::zero(), Vec3::axisY());
            
            Cascades cascades;
            cascades.count = splits.size();
            
            const Rgba colors[] =
            {
                  Rgba(1.0f, 0.0f, 0.0f)
                , Rgba(0.0f, 1.0f, 0.0f)
                , Rgba(0.0f, 0.0f, 1.0f)
                , Rgba(1.0f, 1.0f, 0.0f)
            };
            
            for (size_t i = 0, n = splits.size(); i < n; i++)
            {
                const Frustum& frustum = splits[i];
                Sphere         bounds  = frustum.vertices().sphere() * cameraTransform;
                cascades.colors[i]     = colors[i];
                cascades.transform[i]  = calculateCascadeTransform(512, bounds, lightTransform);
                cascades.distances[i]  = frustum.far();
                cascades.bounds[i]     = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
            }
            
            commands.uploadConstantBuffer(m_cascadesConstantBuffer, &cascades, sizeof(cascades));
        }
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Render the scene
        {
            StateScope lightScope = stateStack.newScope();
            lightScope->bindProgram(m_programLight);
            for (s32 x = -1; x <= 1; x++)
            {
                for (s32 y = -1; y <= 1; y++)
                {
                    renderSimpleScene(commands, x * 3, y * 3);
                }
            }
        }
        
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new FrustumCascades)
