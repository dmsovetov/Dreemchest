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

#ifndef __DC_Renderer_PipelineFeatureLayout_H__
#define __DC_Renderer_PipelineFeatureLayout_H__

#include "Renderer.h"
#include "RenderState.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{    
    //! A single pipeline feature item is a string identifier and a corrensponding bit mask.
    struct PipelineFeature
    {
        FixedString             name;       //!< A feature name that is exposed to shader code.
        PipelineFeatures        bits;       //!< A corresponding pipeline feature mask.
        
        //! Generate a feature mask that corresponds to a specified vertex attribute.
        static PipelineFeatures vertexAttribute(VertexAttribute vertexAttribute);
        
        //! Generate a feature mask that corresponds to a constant buffer binding at specified slot.
        static PipelineFeatures constantBuffer(u8 index);
        
        //! Generate a feature mask that corresponds to a texture sampler binding at specified index.
        static PipelineFeatures sampler(u8 index);
        
        //! Generate a feature mask that corresponds to a user defined feature bits.
        static PipelineFeatures user(PipelineFeatures userDefined);
    };
    
    //! A pipeline state contains an active shader and a bitmask of activated pipeline features.
    class PipelineState
    {
    public:
        
                                        //! Constructs a PipelineState instance.
                                        PipelineState( void );
        
        //! Sets an active program.
        void                            setProgram(ResourceId value);
        
        //! Returns an active program.
        ResourceId                      program( void ) const;
        
        //! Sets an active feature layout.
        void                            setFeatureLayout(const PipelineFeatureLayout* value);
        
        //! Returns an active feature layout.
        const PipelineFeatureLayout*    featureLayout( void ) const;
        
        //! Sets an input layout.
        void                            setInputLayout(const VertexBufferLayout* value);
        
        //! Returns an active input layout.
        const VertexBufferLayout*       inputLayout() const;
        
        //! Sets a vertex buffer.
        void                            setVertexBuffer(ResourceId id);
        
        //! Returns an active vertex buffer.
        ResourceId                      vertexBuffer() const;
        
        //! Sets an index buffer.
        void                            setIndexBuffer(ResourceId id);
        
        //! Returns an active index buffer.
        ResourceId                      indexBuffer() const;
        
        //! Activates a corresponding sampler bit in a feature bitmask.
        void                            setTexture(ResourceId id, u8 index);
        
        //! Returns an texture id bound to a specified sampler.
        ResourceId                      texture(u8 index) const;
        
        //! Activates a corresponding constant buffer bit in a feature bitmask.
        void                            setConstantBuffer(ResourceId id, u8 index);
        
        //! Returns an constant buffer id bound to a specified slot.
        ResourceId                      constantBuffer(u8 index) const;
        
        //! Returns a supported features bitmask.
        PipelineFeatures                mask( void ) const;
        
        //! Resets a constant buffer binding for a specified id.
        void                            resetConstantBuffer(ResourceId id);
        
        //! Resets a program buffer binding for a specified id.
        void                            resetProgram(ResourceId id);
        
        //! Sets a depth state.
        void                            setDepthState(Compare function, bool depthWrite);
        
        //! Returns a depth test function.
        Compare                         depthTestFunction() const;
        
        //! Returns a depth write value.
        bool                            depthWrite() const;
        
        //! Sets a blend factor.
        void                            setBlending(BlendFactor src, BlendFactor dst);
        
        //! Returns a source blend factor.
        BlendFactor                     sourceBlendFactor() const;
        
        //! Returns a destination blend factor.
        BlendFactor                     destBlendFactor() const;
        
        //! Sets a color mask.
        void                            setColorMask(u8 value);
        
        //! Returns a color mask.
        u8                              colorMask() const;
        
        //! Sets a stencil test function.
        void                            setStencilFunction(Compare function, u8 ref, u8 mask);
        
        //! Returns a stencil test function.
        Compare                         stencilFunction() const;
        
        //! Returns a stencil test mask.
        u8                              stencilMask() const;
        
        //! Returns a stencil test reference value.
        u8                              stencilRef() const;
        
        //! Sets a stencil actions.
        void                            setStencilOp(StencilAction sfail, StencilAction dfail, StencilAction dspass);
        
        //! Returns a stencil fail stencil action.
        StencilAction                   stencilStencilFail() const;
        
        //! Returns a depth fail stencil action.
        StencilAction                   stencilDepthFail() const;
        
        //! Returns a depth stencil pass stencil action.
        StencilAction                   stencilPass() const;
        
        //! Sets a triangle side to be culled away.
        void                            setCullFace(TriangleFace value);
        
        //! Returns a triangle side to be culled away.
        TriangleFace                    cullFace() const;
        
        //! Sets a rasterization mode.
        void                            setRasterization(PolygonMode value);
        
        //! Returns a rasterization mode.
        PolygonMode                     rasterization() const;
        
        //! Sets an alpha test function.
        void                            setAlphaTest(Compare function, u8 ref);
        
        //! Returns an alpha test function.
        Compare                         alphaTestFunction() const;
        
        //! Returns an alpha test reference value.
        u8                              alphaTestRef() const;
        
        //! Sets a polygon offset.
        void                            setPolygonOffset(f32 factor, f32 units);
        
        //! Returns a polygon offset factor.
        f32                             polygonOffsetFactor() const;
        
        //! Returns a polygon offset units.
        f32                             polygonOffsetUnits() const;
        
    private:
        
        ResourceId                      m_program;                                      //!< An active program.
        const PipelineFeatureLayout*    m_featureLayout;                                //!< An active feature layout.
        const VertexBufferLayout*       m_inputLayout;                                  //!< A vertex buffer input layout.
        ResourceId                      m_vertexBuffer;                                 //!< A vertex buffer that should be set.
        ResourceId                      m_indexBuffer;                                  //!< An index buffer that should be set.
        ResourceId                      m_constantBuffer[State::MaxConstantBuffers];    //!< An array of bound constant buffers.
        ResourceId                      m_texture[State::MaxTextureSamplers];           //!< An array of bound textures.
        u8                              m_colorMask;                                    //!< A color mask.
        TriangleFace                    m_cullFace;
        PolygonMode                     m_rasterization;
        
        struct
        {
            Compare                     function;                                       //!< A depth test function.
            bool                        depthWrite;                                     //!< Is the depth write enabled?
        } m_depthState;
        
        struct
        {
            BlendFactor                 source;                                         //!< A source blending factor.
            BlendFactor                 dest;                                           //!< A destination blending factor.
        } m_blend;
        
        struct
        {
            f32                         factor;
            f32                         units;
        } m_polygonOffset;
        
        struct
        {
            Compare                     function;                                       //!< An alpha test function.
            u8                          ref;                                            //!< An alpha test reference value.
        } m_alphaTest;
        
        struct
        {
            Compare                     function;                                       //!< A stencil test function.
            u8                          ref;                                            //!< A stencil test reference value.
            u8                          mask;                                           //!< A stencil test mask.
        } m_stencil;
        
        struct
        {
            StencilAction               sfail;                                          //!< A stencil fail action.
            StencilAction               dfail;                                          //!< A depth fail action.
            StencilAction               dspass;                                         //!< A depth stencil pass action.
        } m_stencilOp;
    };
    
    //! Pipeline feature layout defines mappings from a pipeline feature mask to actual preprocessor definitions.
    class PipelineFeatureLayout
    {
    public:
        
        //! A single pipeline feature.
        struct Element
        {
            PipelineFeatures    mask;       //!< Pipeline feature mask that matches this element.
            String              name;       //!< Feature name.
            s32                 offset;     //!< A feature mask offset to extract actual element value.
        };
        
                                //! Construct PipelineFeatureLayout instance.
                                PipelineFeatureLayout( void );
        
        //! Returns a bitmask of supported pipeline features.
        PipelineFeatures        mask( void ) const;
        
        //! Returns a total number of shader feature elements.
        s32                     elementCount( void ) const;
        
        //! Returns an element at specified index.
        const Element&          elementAt(s32 index) const;
        
        //! Adds a new element to a layout.
        void                    addFeature(const String& name, PipelineFeatures mask);
        
    private:
        
        Array<Element>          m_elements; //!< An array of available pipeline features.
        PipelineFeatures        m_mask;     //!< A bitmask with supported pipeline features.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_PipelineFeatureLayout_H__   */
