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
    "cbuffer Cascades   cascades   : 4;                                         \n"

    "varying vec4  v_wsVertex;                                                  \n"
    "varying vec3  v_wsNormal;                                                  \n"
    "varying float v_depth;                                                     \n"
    "varying vec4  v_lsVertex[4];                                               \n"

    "void main()                                                                \n"
    "{                                                                          \n"
    "   v_wsNormal  = (instance.inverseTranspose                                \n"
    "               * vec4(gl_Normal, 1.0)).xyz                                 \n"
    "               ;                                                           \n"

    "   v_wsVertex = instance.transform * gl_Vertex;                            \n"
    "   v_depth    = -(camera.transform * v_wsVertex).z;                        \n"

    "   for (int i = 0; i < cascades.count; i++)                                \n"
    "   {                                                                       \n"
    "       vec4 lsVertex = cascades.normalize[i] * cascades.transform[i] * v_wsVertex;   \n"
    "       v_lsVertex[i] = lsVertex / lsVertex.w;                              \n"
    "   }                                                                       \n"

    "   gl_Position = projection.transform                                      \n"
    "               * camera.transform                                          \n"
    "               * v_wsVertex                                                \n"
    "               ;                                                           \n"
    "}                                                                          \n"
    ;

static String s_fragmentShader =
    "cbuffer Camera     camera   : 1;                                           \n"
    "cbuffer Instance   instance : 2;                                           \n"
    "cbuffer Light      light    : 3;                                           \n"
    "cbuffer Cascades   cascades : 4;                                           \n"

    "uniform sampler2D Texture0;                                                \n"

    "varying vec4  v_wsVertex;                                                  \n"
    "varying vec3  v_wsNormal;                                                  \n"
    "varying float v_depth;                                                     \n"
    "varying vec4  v_lsVertex[4];                                               \n"

    "float sampleShadow(vec4 lightSpaceCoord)                                   \n"
    "{                                                                          \n"
    "   float closest = texture2D(Texture0, lightSpaceCoord.xy).r;              \n"
    "   return lightSpaceCoord.z < closest ? 1.0 : 0.0;                         \n"
    "}                                                                          \n"

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
    "   vec3  dir    = normalize(-light.direction);                             \n"
    "   float dp     = max(dot(normal, dir), 0.0);                              \n"

    "//#define F_IntervalSelection\n"
    "#if defined(F_IntervalSelection)                                           \n"
    "   int cascade = selectCascadeInterval(v_depth);                           \n"
    "#else                                                                      \n"
    "   int   cascade = selectCascadeTexture();                                 \n"
    "   vec4 unused = vec4(v_depth);                                            \n"
    "#endif //  F_IntervalSelection                                             \n"

    "   float s = sampleShadow(v_lsVertex[cascade]);                            \n"

    "   gl_FragColor = vec4(s * dp) /** cascades.colors[cascade]*/;                 \n"
    "}                                                                          \n"
    ;

static String s_vertexShadow =
    "cbuffer Instance instance  : 2;                                            \n"
    "cbuffer Shadow   filter    : 4;                                            \n"

    "void main()                                                                \n"
    "{                                                                          \n"
    "   gl_Position = filter.transform * instance.transform * gl_Vertex;        \n"
    "}                                                                          \n"
    ;

const f32 k_lambda  = 0.7f;
const s32 k_texture = 2048;

struct Cascades
{
    enum { MaxCascades = 4 };
    Matrix4 transform[MaxCascades];
    Matrix4 normalize[MaxCascades];
    f32     distances[MaxCascades];
    Rgba    colors[MaxCascades];
    Vec4    bounds[MaxCascades];
    s32     count;
    static UniformElement Layout[];
};

UniformElement Cascades::Layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Cascades, transform), Cascades::MaxCascades }
    , { "normalize", UniformElement::Matrix4, offsetof(Cascades, normalize), Cascades::MaxCascades }
    , { "distances", UniformElement::Float,   offsetof(Cascades, distances), Cascades::MaxCascades }
    , { "colors",    UniformElement::Vec4,    offsetof(Cascades, colors),    Cascades::MaxCascades }
    , { "bounds",    UniformElement::Vec4,    offsetof(Cascades, bounds),    Cascades::MaxCascades }
    , { "count",     UniformElement::Integer, offsetof(Cascades, count)                            }
    , { NULL }
};

struct Shadow
{
    Matrix4 transform;
    static UniformElement Layout[];
};

UniformElement Shadow::Layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Shadow, transform) }
    , { NULL }
};

struct Light
{
    Vec3   direction;
    static UniformElement Layout[];
};

UniformElement Light::Layout[] =
{
      { "direction", UniformElement::Vec3, offsetof(Light, direction) }
    , { NULL }
};

class CascadedShadows : public Framework::ApplicationDelegate
{
    StateBlock8     m_renderStates;
    ConstantBuffer_ m_cascadesConstantBuffer;
    ConstantBuffer_ m_shadowConstantBuffer;
    ConstantBuffer_ m_lightConstantBuffer;
    Program         m_programLight;
    Program         m_programShadow;
    Program         m_programShowDepth;
    
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
        
        // Configure filter constant buffer
        m_shadowConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Shadow), "Shadow", Shadow::Layout);
        
        // Configure light constant buffer.
        {
            m_lightConstantBuffer  = m_renderingContext->requestConstantBuffer(NULL, sizeof(Light), "Light", Light::Layout);
            m_renderStates.bindConstantBuffer(m_lightConstantBuffer, 3);
        }
        
        m_renderStates.bindProgram(m_programPink);
        
        // Create a light shader program
        m_programLight = m_renderingContext->requestProgram(s_vertexShader, s_fragmentShader);
        
        // Create a filter rendering program
        m_programShadow = m_renderingContext->requestProgram(s_vertexShadow);
    }
    
    Matrix4 fixSubTexel(s32 textureSize, const Matrix4& viewProjection, const Matrix4& projection) const
    {
        // Transform an origin to a light projection space
        Vec3 zero = viewProjection * Vec3::zero();
        
        f32 sh = (textureSize * 0.5f) * 0.5f;
        f32 tx = zero.x * sh;
        f32 ty = zero.y * sh;
        
        f32 dx = (floor(tx) - tx) / sh;
        f32 dy = (floor(ty) - ty) / sh;
        
        // Compute the final projection matrix
        return Matrix4::translation(dx, dy, 0.0f) * projection;
    }

    Matrix4 calculateCascadeTransform(s32 textureSize, const Sphere& worldSpaceBounds, const Matrix4& lightTransform, const Matrix4& normalization = Matrix4()) const
    {
        // Transform a world space bounds to a light space
        Bounds lightSpaceBounds = Bounds::fromSphere(lightTransform * worldSpaceBounds.center(), worldSpaceBounds.radius());
        
        // Calculate a projection matrix based on a light-space cascade bounding box
        const Vec3& min = lightSpaceBounds.min();
        const Vec3& max = lightSpaceBounds.max();
        
        // IMPORTANT: minZ and maxZ are swapped!
        Matrix4 projection = Matrix4::ortho( min.x, max.x, min.y, max.y, max.z + 50.0f, min.z );
        
        // Fix the sub-texel jittering
        projection = fixSubTexel(textureSize, projection * lightTransform, projection);
        
        // Return a final view-projection matrix for a cascade
        return projection * lightTransform;
    }
    
    Matrix4 calculateNormalizationMatrix(const Vec4& bounds) const
    {
        Matrix4 projection;
        
        // Map from [-1, 1] to [0, 1]
        projection = Matrix4::translation(0.5f, 0.5f, 0.5f)
                   * Matrix4::scale(0.5f, 0.5f, 0.5f)
                   * projection
                   ;
        
        // Map from [0, 1] to bounds
        projection = Matrix4::translation(bounds.x, bounds.y, 0.0f)
                   * Matrix4::scale(bounds.z - bounds.x, bounds.w - bounds.y, 1.0f)
                   * projection
                   ;

        return projection;
    }
    
    Cascades calculateCascades(const Frustum& frustum, const Matrix4& cameraTransform, const Matrix4& lightTransform, int textureSize, f32 lambda)
    {
        static const Rgba s_colors[] =
        {
              Rgba(1.0f, 0.0f, 0.0f)
            , Rgba(0.0f, 1.0f, 0.0f)
            , Rgba(0.0f, 0.0f, 1.0f)
            , Rgba(1.0f, 1.0f, 0.0f)
        };
        
        Array<Frustum> splits = frustum.split(4, lambda);
        
        Cascades cascades;
        cascades.count = splits.size();
        for (size_t i = 0, n = splits.size(); i < n; i++)
        {
            s32 x = i % 2;
            s32 y = i / 2;
            
            const Frustum& frustum = splits[i];
            Sphere         bounds  = frustum.vertices().sphere() * cameraTransform;
            cascades.colors[i]     = s_colors[i];
            cascades.bounds[i]     = Vec4(x * 0.5f, y * 0.5f, x * 0.5f + 0.5f, y * 0.5f + 0.5f);
            cascades.transform[i]  = calculateCascadeTransform(textureSize, bounds, lightTransform);
            cascades.normalize[i]  = calculateNormalizationMatrix(cascades.bounds[i]);
            cascades.distances[i]  = frustum.far();
        }
        
        return cascades;
    }
 
    virtual void handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_OVERRIDE
    {
        // Push the default state
        StateScope defaultScope = stateStack.push(&m_renderStates);
        
        Vec3 lightPosition  = Vec3(sinf(time() * 0.1f), 1.0f, cosf(time() * 0.1f)) * 2.0f;
        Vec3 lightDirection = -lightPosition; lightDirection.normalize();
        
        Cascades cascades;
        
        // Update cascades
        {
            Frustum frustum         = Frustum(m_camera.fov, m_window->aspectRatio(), m_camera.near, m_camera.far);
            Matrix4 cameraTransform = this->cameraTransform().inversed();
            Matrix4 lightTransform  = Matrix4::lookAt(lightPosition, Vec3::zero(), Vec3::axisY());
            
            cascades = calculateCascades(frustum, cameraTransform, lightTransform, k_texture, k_lambda);
            commands.uploadConstantBuffer(m_cascadesConstantBuffer, &cascades, sizeof(cascades));
        }
        
        // Update light
        {
            Light light;
            light.direction = lightDirection;
            commands.uploadConstantBuffer(m_lightConstantBuffer, &light, sizeof(light));
        }
        
        // Clear the viewport
        commands.clear(Rgba(0.3f, 0.3f, 0.3f), ClearAll);
        
        // Acquire filter texture
        TransientTexture filter = commands.acquireTexture2D(k_texture, k_texture, TextureD32 | TextureLinear);
        
        // Render cascaded filter map
        {
            StateScope filterScope = stateStack.newScope();
            filterScope->bindProgram(m_programShadow);
            filterScope->bindConstantBuffer(m_shadowConstantBuffer, 4);
            filterScope->setCullFace(TriangleFaceFront);

            for (s32 i = 0; i < cascades.count; i++)
            {
                const Vec4           bounds         = cascades.bounds[i];
                RenderCommandBuffer& commandsShadow = commands.renderToTexture(filter, 0, Rect(bounds.x, bounds.y, bounds.z, bounds.w));
                
                if (i == 0)
                {
                    commandsShadow.clear(Rgba(), ClearAll);
                }

                Shadow shadow;
                shadow.transform = cascades.transform[i];
                commandsShadow.uploadConstantBuffer(m_shadowConstantBuffer, &shadow, sizeof(shadow));
                
                for (s32 x = -1; x <= 1; x++)
                {
                    for (s32 y = -1; y <= 1; y++)
                    {
                        renderSimpleScene(commandsShadow, x * 3, y * 3);
                    }
                }
            }
        }
        
        // Render the scene
        {
            StateScope lightScope = stateStack.newScope();
            lightScope->bindProgram(m_programLight);
            lightScope->bindTexture(filter, 0);
            for (s32 x = -1; x <= 1; x++)
            {
                for (s32 y = -1; y <= 1; y++)
                {
                    renderSimpleScene(commands, x * 3, y * 3);
                }
            }
        }
        
        // Render light source for debugging
        renderPinkItem(commands, stateStack, m_sphere, Matrix4::translation(lightPosition) * Matrix4::scale(0.1f, 0.1f, 0.1f));
        
        // Release filter texture
        commands.releaseTexture(filter);
        
        m_renderingContext->display(frame);
    }
};

dcDeclareApplication(new CascadedShadows)
