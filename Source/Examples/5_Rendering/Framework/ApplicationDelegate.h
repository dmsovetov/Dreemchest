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

#ifndef __Framework_ApplicationDelegate_H__
#define __Framework_ApplicationDelegate_H__

#include <Dreemchest.h>

namespace Framework
{
    DC_USE_DREEMCHEST
    using namespace Renderer;
    
    //! A rendering application delegate with camera and arc ball rotations.
    class ApplicationDelegate : public RenderingApplicationDelegate
    {
    public:
        
        virtual void        handleRenderFrame(RenderFrame& frame, StateStack& stateStack, RenderCommandBuffer& commands, f32 dt) NIMBLE_ABSTRACT;
        
    protected:
        
        void                setCameraPosition(const Vec3& value);
        f32                 time() const;
        
        virtual bool        initialize(s32 width, s32 height) NIMBLE_OVERRIDE;
        virtual void        handleTouchBegan(const Platform::Window::TouchBegan& e) NIMBLE_OVERRIDE;
        virtual void        handleTouchEnded(const Platform::Window::TouchEnded& e) NIMBLE_OVERRIDE;
        virtual void        handleTouchMoved(const Platform::Window::TouchMoved& e) NIMBLE_OVERRIDE;
        virtual void        handleRenderFrame(f32 dt) NIMBLE_OVERRIDE;
        
        void                renderSimpleScene(RenderCommandBuffer& commands, f32 x = 0.0f, f32 z = 0.0f);
        void                renderColumnsScene(RenderCommandBuffer& commands, f32 x = 0.0f, f32 z = 0.0f);
        void                renderPinkItem(RenderCommandBuffer& commands, StateStack& stateStack, const RenderItem& item, const Matrix4& transform = Matrix4(), f32 alpha = 1.0f);
        void                renderItem(RenderCommandBuffer& commands, const RenderItem& item, const Matrix4& transform = Matrix4(), f32 alpha = 1.0f);
        
        RenderItem          createSkyBox(Texture_ texture);
        RenderItem          createMesh(const String& fileName);
        RenderItem          createFullscreenQuad();
        Texture_            createCubeMap(const String& path);
        
    protected:
        
        RenderItem          m_sphere;
        RenderItem          m_box;
        
    private:
        
        f32                 m_time;
        RenderItem          m_platform;
        RenderItem          m_object;
        RenderItem          m_column;
        ConstantBuffer_     m_instanceConstantBuffer;
        Program             m_programPink;
        
        struct
        {
            f32             yaw;
            f32             pitch;
            bool            active;
            Vec3            position;
            ConstantBuffer_ constantBuffer;
        } m_camera;
    };
    
} // namespace Framework

#endif  //  /*  !__Framework_ApplicationDelegate_H__    */
