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

#include "ApplicationDelegate.h"
#include "Framework.h"

namespace Framework
{
    
    // ** Camera::Layout
    const UniformElement Camera::Layout[] =
    {
        { "transform", UniformElement::Matrix4, offsetof(Camera, transform) }
        , { "inverseTranspose", UniformElement::Matrix4, offsetof(Camera, inverseTranspose) }
        , { "rotation", UniformElement::Matrix4, offsetof(Camera, rotation) }
        , { "position",  UniformElement::Vec3,    offsetof(Camera, position)  }
        , { NULL }
    };
    
    // ** Instance::Layout
    const UniformElement Instance::Layout[] =
    {
        { "transform",        UniformElement::Matrix4, offsetof(Instance, transform)        }
        , { "inverseTranspose", UniformElement::Matrix4, offsetof(Instance, inverseTranspose) }
        , { "alpha",            UniformElement::Float,   offsetof(Instance, alpha)            }
        , { NULL }
    };
    
    // ** Projection::Layout
    const UniformElement Projection::Layout[] =
    {
        { "transform", UniformElement::Matrix4, offsetof(Projection, transform) }
        , { "viewport",  UniformElement::Vec4,    offsetof(Projection, viewport)  }
        , { NULL }
    };
    
void ApplicationDelegate::setCameraPosition(const Vec3& value)
{
    m_camera.position = value;
}

f32 ApplicationDelegate::time() const
{
    return m_time;
}
    
Matrix4 ApplicationDelegate::cameraTransform() const
{
    Quat rotation = Quat::rotateAroundAxis(m_camera.pitch, Vec3::axisX()) * Quat::rotateAroundAxis(m_camera.yaw, Vec3::axisY());
    return Camera::fromQuat(m_camera.position, rotation).transform;
}

bool ApplicationDelegate::initialize(RenderingBackend backend, s32 width, s32 height)
{
    if (!RenderingApplicationDelegate::initialize(backend, width, height))
    {
        return false;
    }
    
    memset(&m_camera, 0, sizeof(m_camera));
    m_camera.near = 0.1f;
    m_camera.far  = 10.0f;
    m_camera.fov  = 90.0f;
    
    StateBlock& defaultStates = m_renderingContext->defaultStateBlock();
    
    // Configure projection constant buffer
    {
        Projection projection = Projection::perspective(m_camera.fov, m_window->width(), m_window->height(), m_camera.near, m_camera.far);
        
        UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Projection", Projection::Layout);
        ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&projection, sizeof(projection), uniformLayout);
        defaultStates.bindConstantBuffer(constantBuffer, 0);
    }
    
    // Configure camera constant buffer
    {
        UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Camera", Camera::Layout);
        m_camera.constantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Camera), uniformLayout);
        defaultStates.bindConstantBuffer(m_camera.constantBuffer, 1);
    }
    
    // Configure instance constant buffer
    {
        UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Instance", Instance::Layout);
        m_instanceConstantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Instance), uniformLayout);
        defaultStates.bindConstantBuffer(m_instanceConstantBuffer, 2);
    }
    
    m_time = 0.0f;
    m_sphere = createMesh("Assets/Meshes/sphere.obj");
    m_programPink = m_renderingContext->requestProgram(VertexFixedTransform, FragmentPink);
    
    m_debugInputLayout  = m_renderingContext->requestInputLayout(VertexFormat::Position | VertexFormat::Color);
    m_debugVertexBuffer = m_renderingContext->requestVertexBuffer(NULL, 4096);
    m_debugProgram      = m_renderingContext->requestProgram(VertexDebug, FragmentDebug);
    
    return true;
}

void ApplicationDelegate::handleTouchBegan(const Platform::Window::TouchBegan& e)
{
    m_camera.active = true;
}

void ApplicationDelegate::handleTouchEnded(const Platform::Window::TouchEnded& e)
{
    m_camera.active = false;
}

void ApplicationDelegate::handleTouchMoved(const Platform::Window::TouchMoved& e)
{
    static s32 prevX = -1;
    static s32 prevY = -1;
    
    if (!m_camera.active)
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
    
    m_camera.yaw   += dx * 0.25f;
    m_camera.pitch += dy * 0.25f;
    
    prevX = e.x;
    prevY = e.y;
}

void ApplicationDelegate::handleRenderFrame(f32 dt)
{
    m_time += dt;
    m_camera.position += (m_camera.forward + m_camera.strafe) * dt;
    
    RenderFrame& frame = m_renderingContext->allocateFrame();
    RenderCommandBuffer& commands = frame.entryPoint();
    
    // Update the camera constant buffer
    Quat rotation = Quat::rotateAroundAxis(m_camera.pitch, Vec3::axisX()) * Quat::rotateAroundAxis(m_camera.yaw, Vec3::axisY());
    Camera camera = Camera::fromQuat(m_camera.position, rotation);
    commands.uploadConstantBuffer(m_camera.constantBuffer, &camera, sizeof(camera));
    
    handleRenderFrame(frame, frame.stateStack(), commands, dt);
    m_renderingContext->display(frame);
}
    
#if defined(DC_PLATFORM_KEYBOARD)
void ApplicationDelegate::handleKeyPressed(const Platform::Window::KeyPressed& e)
{
    Quat rotation /*= */;
    
    switch (e.key)
    {
        case Platform::Key::A:
            m_camera.strafe = Quat::rotateAroundAxis(m_camera.yaw, Vec3::axisY()).rotate(-Vec3::axisZ());
            break;
            
        case Platform::Key::D:
            m_camera.strafe = Quat::rotateAroundAxis(m_camera.yaw, Vec3::axisY()).rotate(Vec3::axisZ());
            break;
            
        case Platform::Key::W:
            m_camera.forward = rotation.rotate(-Vec3::axisZ());
            break;
            
        case Platform::Key::S:
            m_camera.forward = rotation.rotate(Vec3::axisZ());
            break;
    }
}

void ApplicationDelegate::handleKeyReleased(const Platform::Window::KeyReleased& e)
{
    switch (e.key)
    {
        case Platform::Key::A:
        case Platform::Key::D:
            m_camera.strafe = Vec3::zero();
            break;
            
        case Platform::Key::W:
        case Platform::Key::S:
            m_camera.forward = Vec3::zero();
            break;
    }
}
#endif  //  #if defined(DC_PLATFORM_KEYBOARD)

RenderItem ApplicationDelegate::createSkyBox(Texture_ texture)
{
    RenderItem renderItem = createUnitCube();
    Program    program    = m_renderingContext->requestProgram(VertexSkyBox, FragmentSkyBox);

    renderItem.states.bindTexture(texture, 0);
    renderItem.states.bindProgram(program);
    renderItem.states.setDepthState(LessEqual, false);
    
    return renderItem;
}
    
RenderItem ApplicationDelegate::createUnitCube()
{
    RenderItem renderItem;
    
    InputLayout   inputLayout  = m_renderingContext->requestInputLayout(VertexFormat::Position);
    VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(UnitCube, sizeof(UnitCube));
    
    renderItem.primitives = PrimTriangles;
    renderItem.first      = 0;
    renderItem.count      = 36;
    renderItem.indexed    = false;
    renderItem.states.bindVertexBuffer(vertexBuffer);
    renderItem.states.bindInputLayout(inputLayout);
    
    return renderItem;
}

RenderItem ApplicationDelegate::createMesh(const String& fileName)
{
    Mesh mesh = objFromFile(fileName);
    NIMBLE_ABORT_IF(!mesh, "failed to load mesh");
    
    VertexFormat vertexFormat  = mesh.vertexFormat;
    InputLayout inputLayout    = m_renderingContext->requestInputLayout(vertexFormat);
    VertexBuffer_ vertexBuffer = m_renderingContext->requestVertexBuffer(&mesh.vertexBuffer[0], mesh.vertexBuffer.size());
    
    RenderItem renderItem;
    renderItem.primitives = mesh.primitives;
    renderItem.first      = 0;
    renderItem.count      = mesh.vertexCount;
    renderItem.states.bindInputLayout(inputLayout);
    renderItem.states.bindVertexBuffer(vertexBuffer);
    
    if (mesh.indices.size())
    {
        IndexBuffer_ indexBuffer = m_renderingContext->requestIndexBuffer(&mesh.indices[0], sizeof(u16) * mesh.indices.size());
        renderItem.states.bindIndexBuffer(indexBuffer);
        renderItem.indexed = true;
    }
    else
    {
        renderItem.indexed = false;
    }
    
    return renderItem;
}

Texture_ ApplicationDelegate::createCubeMap(const String& path)
{
    Image images[6];
    String fileNames[] =
    {
          path + "/posx.tga"
        , path + "/negx.tga"
        , path + "/posy.tga"
        , path + "/negy.tga"
        , path + "/posz.tga"
        , path + "/negz.tga"
    };
    
    Surface pixels;
    for (s32 i = 0; i < 6; i++)
    {
        images[i] = tgaFromFile(fileNames[i]);
        pixels.insert(pixels.end(), images[i].pixels.begin(), images[i].pixels.end());
    }
    
    return m_renderingContext->requestTextureCube(&pixels[0], images[0].width, 1, images[0].format | TextureTrilinear);
}

RenderItem ApplicationDelegate::createFullscreenQuad()
{
    RenderItem renderItem;
    InputLayout   il = m_renderingContext->requestInputLayout(0);
    VertexBuffer_ vb = m_renderingContext->requestVertexBuffer(FullscreenQuad, sizeof(FullscreenQuad));
    renderItem.primitives = PrimQuads;
    renderItem.indexed = false;
    renderItem.count = 4;
    renderItem.first = 0;
    renderItem.states.bindInputLayout(il);
    renderItem.states.bindVertexBuffer(vb);
    renderItem.states.setDepthState(LessEqual, false);
    return renderItem;
}
    
void ApplicationDelegate::renderColumnsScene(RenderCommandBuffer& commands, f32 x, f32 z)
{
    if (!m_platform) m_platform = createMesh("Assets/Meshes/platform.obj");
    if (!m_object) m_object = createMesh("Assets/Meshes/bunny_decimated.obj");
    if (!m_column) m_column = createMesh("Assets/Meshes/column.obj");
    
    // Render the platform
    renderItem(commands, m_platform, Matrix4::translation(x, 0.0f, z) * Matrix4::scale(1.5f, 1.0f, 1.5f));
    
    // Now the stanford bunny
    renderItem(commands, m_object, Matrix4::translation(x, 0.0f, z) * Matrix4::scale(0.5f, 0.5f, 0.5f));
    
    // And finally columns
    for (s32 xx = -1; xx <= 1; xx++)
    {
        for (s32 zz = -1; zz <= 1; zz++)
        {
            renderItem(commands, m_column, Matrix4::translation(xx + x, 0.15f, zz + z) * Matrix4::scale(0.1f, 0.1f, 0.1f));
        }
    }
}
    
void ApplicationDelegate::renderSimpleScene(RenderCommandBuffer& commands, f32 x, f32 z)
{
    if (!m_platform) m_platform = createMesh("Assets/Meshes/platform.obj");
    if (!m_sphere) m_sphere = createMesh("Assets/Meshes/sphere.obj");
    if (!m_box) m_box = createMesh("Assets/Meshes/cube.obj");
    
    // Render the platform
    renderItem(commands, m_platform, Matrix4::translation(x, 0.0f, z) * Matrix4::scale(1.5f, 1.0f, 1.5f));
    
    // Now the sphere
    renderItem(commands, m_sphere, Matrix4::translation(x, 0.25f, z) * Matrix4::scale(0.5f, 0.5f, 0.5f));
    
    // And finally columns
    for (s32 xx = -1; xx <= 1; xx++)
    {
        for (s32 zz = -1; zz <= 1; zz++)
        {
            renderItem(commands, m_box, Matrix4::translation(xx + x, 1.0f, zz + z) * Matrix4::scale(0.1f, 0.1f, 0.1f));
        }
    }
}
    
void ApplicationDelegate::renderPinkItem(RenderCommandBuffer& commands, StateStack& stateStack, const RenderItem& item, const Matrix4& transform, f32 alpha)
{
    StateScope constantColorScope = stateStack.newScope();
    constantColorScope->bindProgram(m_programPink);
    renderItem(commands, item, transform, alpha);
}

void ApplicationDelegate::renderItem(RenderCommandBuffer& commands, const RenderItem& item, const Matrix4& transform, f32 alpha)
{
    Framework::Instance instance = Framework::Instance::fromTransform(transform, alpha);
    commands.uploadConstantBuffer(m_instanceConstantBuffer, &instance, sizeof(instance));
    commands.drawItem(0, item);
}
    
void ApplicationDelegate::renderWireBounds(RenderCommandBuffer& commands, StateStack& stateStack, const Bounds& bounds, const Rgba& color) const
{
    const Vec3& min = bounds.min();
    Vec3        w   = Vec3(bounds.width(), 0.0f, 0.0f);
    Vec3        h   = Vec3(0.0f, bounds.height(), 0.0f);
    Vec3        d   = Vec3(0.0f, 0.0f, bounds.depth());
    
    Vec3 vertices[8] =
    {
          min
        , min + w
        , min + w + d
        , min     + d
        
        , min         + h
        , min + w     + h
        , min + w + d + h
        , min     + d + h
    };
    
    renderWireBounds(commands, stateStack, vertices, color);
}

void ApplicationDelegate::renderWireSphere(RenderCommandBuffer& commands, StateStack& stateStack, const Sphere& sphere, const Rgba& color) const
{
    renderWireCircle(commands, stateStack, Vec3::axisX(), Vec3::axisZ(), sphere.center(), sphere.radius(), color, 32);
    renderWireCircle(commands, stateStack, Vec3::axisY(), Vec3::axisZ(), sphere.center(), sphere.radius(), color, 32);
    renderWireCircle(commands, stateStack, Vec3::axisX(), Vec3::axisY(), sphere.center(), sphere.radius(), color, 32);
}
    
void ApplicationDelegate::renderWireCircle(RenderCommandBuffer& commands, StateStack& stateStack, const Vec3& tangent, const Vec3& bitangent, const Vec3& center, f32 radius, const Rgba& color, s32 segments) const
{
    Array<ColoredVertex> vertexBuffer;
    f32 step = radians(360.0f / segments);
    
    for (s32 i = 0; i < segments; i++)
    {
        Vec3 vertex = (tangent * cosf(step * i) + bitangent * sinf(step * i)) * radius + center;
        vertexBuffer.push_back(vertex);
    }
    vertexBuffer.push_back(vertexBuffer[0]);
    
    for (size_t i = 0, n = vertexBuffer.size(); i < n; i++)
    {
        vertexBuffer[i].color = color.toInteger();
    }
    
    StateScope scope = stateStack.newScope();
    scope->bindProgram(m_debugProgram);
    scope->bindVertexBuffer(m_debugVertexBuffer);
    scope->bindInputLayout(m_debugInputLayout);
    
    commands.uploadVertexBuffer(m_debugVertexBuffer, &vertexBuffer[0], sizeof(ColoredVertex) * vertexBuffer.size());
    commands.drawPrimitives(0, PrimLineStrip, 0, vertexBuffer.size());
}

void ApplicationDelegate::renderWireBounds(RenderCommandBuffer& commands, StateStack& stateStack, const Vec3 vertices[8], const Rgba& color) const
{
    ColoredVertex  vertexBuffer[24];
    ColoredVertex* stream = vertexBuffer;
    
    // Bottom
    *(stream++) = vertices[0], *(stream++) = vertices[1];
    *(stream++) = vertices[1], *(stream++) = vertices[2];
    *(stream++) = vertices[2], *(stream++) = vertices[3];
    *(stream++) = vertices[3], *(stream++) = vertices[0];
    
    // Top
    *(stream++) = vertices[4], *(stream++) = vertices[5];
    *(stream++) = vertices[5], *(stream++) = vertices[6];
    *(stream++) = vertices[6], *(stream++) = vertices[7];
    *(stream++) = vertices[7], *(stream++) = vertices[4];
    
    // Vertical
    *(stream++) = vertices[0], *(stream++) = vertices[4];
    *(stream++) = vertices[1], *(stream++) = vertices[5];
    *(stream++) = vertices[2], *(stream++) = vertices[6];
    *(stream++) = vertices[3], *(stream++) = vertices[7];
    
    s32 count = stream - vertexBuffer;
    
    for (s32 i = 0; i < count; i++)
    {
        vertexBuffer[i].color = color.toInteger();
    }
    
    StateScope scope = stateStack.newScope();
    scope->bindProgram(m_debugProgram);
    scope->bindVertexBuffer(m_debugVertexBuffer);
    scope->bindInputLayout(m_debugInputLayout);
    
    commands.uploadVertexBuffer(m_debugVertexBuffer, vertexBuffer, sizeof(ColoredVertex) * count);
    commands.drawPrimitives(0, PrimLines, 0, count);
}
    
void ApplicationDelegate::renderFrustum(RenderCommandBuffer& commands, StateStack& stateStack, const Frustum& frustum, const Matrix4& transform, const Rgba& color) const
{
    FrustumVertices vertices = frustum.vertices();

    for (s32 i = 0; i < 8; i++)
    {
        vertices[i] = transform * vertices[i];
    }
    
    renderWireBounds(commands, stateStack, vertices.points(), color);
}

} // namespace Framework
