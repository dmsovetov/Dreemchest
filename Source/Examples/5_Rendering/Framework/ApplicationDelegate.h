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
    #if defined(DC_PLATFORM_KEYBOARD)
        virtual void        handleKeyPressed(const Platform::Window::KeyPressed& e) NIMBLE_OVERRIDE;
        virtual void        handleKeyReleased(const Platform::Window::KeyReleased& e) NIMBLE_OVERRIDE;
    #endif  //  #if defined(DC_PLATFORM_KEYBOARD)
        virtual void        handleRenderFrame(f32 dt) NIMBLE_OVERRIDE;
        
        void                renderSimpleScene(RenderCommandBuffer& commands, f32 x = 0.0f, f32 z = 0.0f);
        void                renderColumnsScene(RenderCommandBuffer& commands, f32 x = 0.0f, f32 z = 0.0f);
        void                renderPinkItem(RenderCommandBuffer& commands, StateStack& stateStack, const RenderItem& item, const Matrix4& transform = Matrix4(), f32 alpha = 1.0f);
        void                renderItem(RenderCommandBuffer& commands, const RenderItem& item, const Matrix4& transform = Matrix4(), f32 alpha = 1.0f);
        
        void                renderWireBounds(RenderCommandBuffer& commands, StateStack& stateStack, const Bounds& bounds, const Rgba& color = Rgba(1.0f, 1.0f, 1.0f)) const;
        void                renderWireBounds(RenderCommandBuffer& commands, StateStack& stateStack, const Vec3 vertices[8], const Rgba& color = Rgba(1.0f, 1.0f, 1.0f)) const;
        void                renderFrustum(RenderCommandBuffer& commands, StateStack& stateStack, const Frustum& frustum, const Matrix4& transform, const Rgba& color = Rgba(1.0f, 1.0f, 1.0f)) const;
        void                renderWireSphere(RenderCommandBuffer& commands, StateStack& stateStack, const Sphere& sphere, const Rgba& color = Rgba(1.0f, 1.0f, 1.0f)) const;
        void                renderWireCircle(RenderCommandBuffer& commands, StateStack& stateStack, const Vec3& tangent, const Vec3& bitangent, const Vec3& center, f32 radius, const Rgba& color = Rgba(1.0f, 1.0f, 1.0f), s32 segments = 16) const;
        
        RenderItem          createSkyBox(Texture_ texture);
        RenderItem          createUnitCube();
        RenderItem          createMesh(const String& fileName);
        RenderItem          createFullscreenQuad();
        Texture_            createCubeMap(const String& path);
        Matrix4             cameraTransform() const;
        
    protected:
        
        RenderItem          m_sphere;
        RenderItem          m_box;
        Program             m_programPink;
        ConstantBuffer_     m_instanceConstantBuffer;
        VertexBuffer_       m_debugVertexBuffer;
        InputLayout         m_debugInputLayout;
        Program             m_debugProgram;
        
        struct ColoredVertex
        {
            Vec3            position;
            u32             color;
            
                            ColoredVertex(const Vec3& position)
                                : position(position) {}
                            ColoredVertex()
                                {}
        };
        
        struct
        {
            f32             yaw;
            f32             pitch;
            bool            active;
            Vec3            position;
            Vec3            forward;
            Vec3            strafe;
            f32             near;
            f32             far;
            f32             fov;
            ConstantBuffer_ constantBuffer;
        } m_camera;
        
    private:
        
        f32                 m_time;
        RenderItem          m_platform;
        RenderItem          m_object;
        RenderItem          m_column;
    };
    
} // namespace Framework

#endif  //  /*  !__Framework_ApplicationDelegate_H__    */
