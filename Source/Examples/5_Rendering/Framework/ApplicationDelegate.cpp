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

namespace Framework
{
    
void ApplicationDelegate::setCameraPosition(const Vec3& value)
{
    m_camera.position = value;
}

f32 ApplicationDelegate::time() const
{
    return m_time;
}

bool ApplicationDelegate::initialize(s32 width, s32 height)
{
    if (!RenderingApplicationDelegate::initialize(width, height))
    {
        return false;
    }
    
    memset(&m_camera, 0, sizeof(m_camera));
    
    StateBlock& defaultStates = m_renderingContext->defaultStateBlock();
    
    // Configure projection constant buffer
    {
        Examples::Projection projection = Examples::Projection::perspective(90.0f, m_window->width(), m_window->height(), 0.1f, 100.0f);
        
        UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Projection", Examples::Projection::Layout);
        ConstantBuffer_ constantBuffer = m_renderingContext->requestConstantBuffer(&projection, sizeof(projection), uniformLayout);
        defaultStates.bindConstantBuffer(constantBuffer, 0);
    }
    
    // Configure camera constant buffer
    {
        UniformLayout uniformLayout = m_renderingContext->requestUniformLayout("Camera", Examples::Camera::Layout);
        m_camera.constantBuffer = m_renderingContext->requestConstantBuffer(NULL, sizeof(Examples::Camera), uniformLayout);
        defaultStates.bindConstantBuffer(m_camera.constantBuffer, 1);
    }
    
    m_time = 0.0f;
    
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
    
    RenderFrame& frame = m_renderingContext->allocateFrame();
    RenderCommandBuffer& commands = frame.entryPoint();
    
    // Update the camera constant buffer
    Quat rotation = Quat::rotateAroundAxis(m_camera.pitch, Vec3::axisX()) * Quat::rotateAroundAxis(m_camera.yaw, Vec3::axisY());
    Examples::Camera camera = Examples::Camera::fromQuat(m_camera.position, rotation);
    commands.uploadConstantBuffer(m_camera.constantBuffer, &camera, sizeof(camera));
    
    handleRenderFrame(frame, frame.stateStack(), commands, dt);
    m_renderingContext->display(frame);
}

} // namespace Framework
