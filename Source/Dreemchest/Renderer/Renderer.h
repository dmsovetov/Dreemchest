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

#ifndef __DC_Renderer_H__
#define __DC_Renderer_H__

#include "../Dreemchest.h"

#include <Platform/Application.h>

DC_BEGIN_DREEMCHEST

namespace Renderer {

    NIMBLE_LOGGER_TAG( Renderer )

    class RenderView;
    
    dcDeclarePtrs(RenderingContext)
    dcDeclarePtrs(RenderView);
    
    //! A pipeline feature mask type.
    typedef u64 PipelineFeatures;
    
    //! A rendering state bit mask type.
    typedef u32 StateMask;
    
    //! A transient resource identifier type alias.
    typedef u8 TransientResourceId;
    
    //! A persistent resource identifer type alias.
    typedef u16 ResourceId;
    
    //! A container type to manage persistent resource identifiers.
    typedef IndexManager<ResourceId>            ResourceIdentifiers;
    
    //! A unique pointer type for a vertex buffer layout instance.
    typedef UPtr<class VertexBufferLayout>      VertexBufferLayoutUPtr;
    
    //! A unique pointer type for a pipeline feature layout instance.
    typedef UPtr<class PipelineFeatureLayout>   PipelineFeatureLayoutUPtr;
    
    //! A wrapper type that indicates that a pointer stored inside is persistent.
    struct PersistentPointer
    {
        const void* value;
    };
    
    //! Creates a persistent pointer
    NIMBLE_INLINE PersistentPointer persistentPointer(const void* value)
    {
        PersistentPointer pointer = { value };
        return pointer;
    }
    
    //! Vertex attributes that are passed to a shader.
    enum VertexAttribute
    {
          VertexPosition
        , VertexNormal
        , VertexColor
        , VertexTexCoord0
        , VertexTexCoord1
        , VertexTexCoord2
        , VertexTexCoord3
        , VertexTexCoord4
        , VertexTexCoord5
        , VertexTexCoord6
        , VertexTexCoord7
        , VertexTangent
        , VertexBitangent
        , VertexPointSize
        , MaxVertexAttributes
    };
    
    //! Available texture types.
    enum TextureType
    {
          TextureType1D         //!< A one-dimensional texture.
        , TextureType2D         //!< A two-dimensional texture.
        , TextureType3D         //!< A three-dimensional texture.
        , TextureTypeCube       //!< A cube texture.
        , TotalTextureTypes //!< A total number of texture types.
    };
    
    //! Available resource type tags used to distinguish handles that point to different types of resources.
    struct RenderResourceType
    {
        enum Enum
        {
              InputLayout = 1
            , VertexBuffer
            , IndexBuffer
            , ConstantBuffer
            , UniformLayout
            , Program
            , Texture
            , FeatureLayout
            , VertexShader
            , GeometryShader
            , FragmentShader
            , ComputeShader
            , TotalTypes
        };
    };
    
    //! A render resource identifier type.
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    class ResourceIdentifier
    {
    friend class RenderingContext;
    friend class ShaderLibrary;
    friend class RenderingContextHal;
    public:
        
        enum { ResourceType = TResource };
        
                                    //! Constructs an invalid render resource identifier.
                                    ResourceIdentifier( void );
        
                                    //! Type casts to a stored identifier type.
                                    operator TIdentifier( void ) const;
        
                                    //! Type casts to an integer to make it possible to index arrays with a resource identifier.
                                    operator s32( void ) const;
        
                                    //! Returns true if this resource identifier is valid.
                                    operator bool( void ) const;
        
        //! Compares two resoruce identifiers.
        bool                        operator < (const ResourceIdentifier& other) const;
        
        //! Compares two resource identifiers.
        bool                        operator == (const ResourceIdentifier& other) const;
        
        //! Compares two resource identifiers.
        bool                        operator != (const ResourceIdentifier& other) const;
        
        //! Sets a resource identifier from a value.
        void                        set(TIdentifier value);
        
        //! Create a resource identifier from a specified value.
        static ResourceIdentifier   create(TIdentifier value);
        
    private:
        
                                    //! Constructs a resource identifier with a specified value.
                                    ResourceIdentifier( TIdentifier value );
        
    private:
        
        TIdentifier                 m_id;   //!< Actual resource identifier.
    };
    
    // ** ResourceIdentifier::ResourceIdentifier
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE ResourceIdentifier<TResource, TIdentifier>::ResourceIdentifier( void )
        : m_id(0)
    {
    }
    
    // ** ResourceIdentifier::ResourceIdentifier
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE ResourceIdentifier<TResource, TIdentifier>::ResourceIdentifier( TIdentifier value )
        : m_id(value)
    {
        
    }
    
    // ** ResourceIdentifier::operator TIdentifier
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE ResourceIdentifier<TResource, TIdentifier>::operator TIdentifier( void ) const
    {
        return m_id;
    }
    
    // ** ResourceIdentifier::operator s32
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE ResourceIdentifier<TResource, TIdentifier>::operator s32( void ) const
    {
        return static_cast<s32>(m_id);
    }
    
    // ** ResourceIdentifier::operator TIdentifier
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE ResourceIdentifier<TResource, TIdentifier>::operator bool( void ) const
    {
        return m_id != 0;
    }
    
    // ** ResourceIdentifier::operator <
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE bool ResourceIdentifier<TResource, TIdentifier>::operator < (const ResourceIdentifier& other) const
    {
        return m_id < other.m_id;
    }
    
    // ** ResourceIdentifier::operator ==
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE bool ResourceIdentifier<TResource, TIdentifier>::operator == (const ResourceIdentifier& other) const
    {
        return m_id == other.m_id;
    }
    
    // ** ResourceIdentifier::operator !=
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE bool ResourceIdentifier<TResource, TIdentifier>::operator != (const ResourceIdentifier& other) const
    {
        return m_id != other.m_id;
    }
    
    // ** ResourceIdentifier::set
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    NIMBLE_INLINE void ResourceIdentifier<TResource, TIdentifier>::set(TIdentifier value)
    {
        m_id = value;
    }
    
    // ** ResourceIdentifier::operator TIdentifier
    template<RenderResourceType::Enum TResource, typename TIdentifier>
    ResourceIdentifier<TResource, TIdentifier> ResourceIdentifier<TResource, TIdentifier>::create(TIdentifier value)
    {
        return ResourceIdentifier(value);
    }

    //! Declare all render resource types.
    typedef ResourceIdentifier<RenderResourceType::InputLayout, ResourceId> InputLayout;
    typedef ResourceIdentifier<RenderResourceType::VertexBuffer, ResourceId> VertexBuffer_;
    typedef ResourceIdentifier<RenderResourceType::IndexBuffer, ResourceId> IndexBuffer_;
    typedef ResourceIdentifier<RenderResourceType::ConstantBuffer, ResourceId> ConstantBuffer_;
    typedef ResourceIdentifier<RenderResourceType::UniformLayout, ResourceId> UniformLayout;
    typedef ResourceIdentifier<RenderResourceType::Program, ResourceId> Program;
    typedef ResourceIdentifier<RenderResourceType::Texture, ResourceId> Texture_;
    typedef ResourceIdentifier<RenderResourceType::FeatureLayout, ResourceId> FeatureLayout;
    typedef ResourceIdentifier<RenderResourceType::VertexShader, ResourceId> VertexShader;
    typedef ResourceIdentifier<RenderResourceType::GeometryShader, ResourceId> GeometryShader;
    typedef ResourceIdentifier<RenderResourceType::FragmentShader, ResourceId> FragmentShader;
    typedef ResourceIdentifier<RenderResourceType::ComputeShader, ResourceId> ComputeShader;

    typedef ResourceIdentifier<RenderResourceType::Texture, TransientResourceId> TransientTexture;
    
    //! Available shader types.
    enum ShaderType
    {
          VertexShaderType      //!< A vertex shader type.
        , GeometryShaderType    //!< A geometry shader type.
        , FragmentShaderType    //!< A fragment shader type.
        , ComputeShaderType     //!< A compute shader type.
        , TotalShaderTypes
    };
    
    //! A shader program descriptor that is used by rendering context to create programs.
    struct ShaderProgramDescriptor
    {
        VertexShader        vertexShader;   //!< A vertex shader to be attached to a program.
        GeometryShader      geometryShader; //!< A geometry shader to be attached to a program.
        FragmentShader      fragmentShader; //!< A fragment shader to be attached to a program.
        ComputeShader       computeShader;  //!< A compute shader to be attached to a program.
    };
    
    //! A cube map side enumeration.
    enum CubeMapSide
    {
          CubeMapPositiveX
        , CubeMapNegativeX
        , CubeMapPositiveY
        , CubeMapNegativeY
        , CubeMapPositiveZ
        , CubeMapNegativeZ
    };
    
    //! An array of uniform elements defines a uniform buffer layout.
    struct UniformElement
    {
        //! Constant type.
        enum Type
        {
              Integer   //!< Integer constant value.
            , Float     //!< Float constant value.
            , Vec2      //!< Vec2 constant value.
            , Vec3      //!< Vec3 constant value.
            , Vec4      //!< Vec4 constant value.
            , Matrix4   //!< 4x4 matrix constant value.
        };

        FixedString                 name;       //!< Uniform name.
        Type                        type;       //!< Uniform type.
        u32                         offset;     //!< Uniform offset.
        InitializedValue<u16, 0>    size;       //!< An array size.
    };
    
    typedef Array<UniformElement> UniformBufferLayout;
    
    //! Rendering viewport interface that is exposed to a rendering context
    class RenderView : public InjectEventEmitter<RefCounted>
    {
    public:
        
        virtual                     ~RenderView( void ) {}
        
        //! Returns a view width.
        virtual s32                 width() const NIMBLE_ABSTRACT;
        
        //! Returns a view height.
        virtual s32                 height() const NIMBLE_ABSTRACT;
        
        //! Returns a view aspect ratio.
        virtual f32                 aspectRatio() const { return static_cast<f32>(width()) / height(); }
        
        //! Activates the view for rendering.
        virtual bool                makeCurrent( void ) { return false; }
        
        //! Begins a frame rendering.
        virtual bool                beginFrame( void ) { return false; }
        
        //! Ends a frame rendering.
        virtual void                endFrame( bool wait ) {}
        
        //! Notifies view about a frame update.
        void                        notifyUpdate( void )
        {
            notify<Update>(Update(this));
        }
        
        //! This event is emitted each frame.
        struct Update
        {
            //! Constructs Update instance.
            Update(RenderViewWPtr view) : view(view) {}
            RenderViewWPtr  view;
        };
    };
    
    //! Available texture filtering modes.
    enum TextureFilter
    {
        FilterNearest,      //!< The texture filtering is disabled.
        FilterLinear,       //!< Interpolate texels.
        FilterBilinear,     //!< Use texture mip-maps.
        FilterTrilinear,    //!< Use texture mip-map and interpolate between the mip levels.
    };
    
    //! Render frame unique pointer type.
    typedef UPtr<class RenderFrame> RenderFrameUPtr;
    
    struct State;
    class StateBlock;
    class VertexFormat;
    struct UniformElement;
    class RenderFrame;
    class PipelineFeatureLayout;
    
    class CommandBuffer;
        class RenderCommandBuffer;
        class ResourceCommandBuffer;
    
    //! Render command buffer unique pointer type.
    typedef UPtr<class CommandBuffer> CommandBufferUPtr;

    //! Hardware rendering API.
    enum RenderingHal {
        OpenGL,
        Direct3D
    };

    //! Clear mask bits that are passed to Hal::clear method.
    enum ClearMask
    {
        ClearColor      = BIT( 0 ),                                 //!< Clear color buffer.
        ClearDepth      = BIT( 1 ),                                 //!< Clear depth buffer.
        ClearStencil    = BIT( 2 ),                                 //!< Clear stencil buffer.
        ClearAll        = ClearColor | ClearDepth | ClearStencil    //!< Clear all buffers.
    };

    // ** enum StencilAction
    enum StencilAction
    {
        StencilDecSaturate,
        StencilDecWrap,
        StencilIncSaturate,
        StencilIncWrap,
        StencilInvert,
        StencilKeep,
        StencilReplace,
        StencilZero,
    };

    // ** enum BlendFactor
    enum BlendFactor {
        BlendDisabled,
        BlendZero,
        BlendOne,
        BlendSrcColor,
        BlendInvSrcColor,
        BlendDstColor,
        BlendInvDstColor,
        BlendSrcAlpha,
        BlendInvSrcAlpha,
        BlendDstAlpha,
        BlendInvDstAlpha,
        TotalBlendFactors
    };

    //! Color mask bits used as an argument for Hal::setColorMask method.
    enum ColorMask {
        ColorMaskRed    = BIT( 0 ),                                                         //!< Render only RED channel.
        ColorMaskGreen    = BIT( 1 ),                                                         //!< Render only GREEN channel.
        ColorMaskBlue    = BIT( 2 ),                                                         //!< Render only BLUE channel.
        ColorMaskAlpha    = BIT( 3 ),                                                         //!< Render only ALPHA channel.
        ColorMaskAll    = ColorMaskRed | ColorMaskGreen | ColorMaskBlue | ColorMaskAlpha,   //!< Render all color channels.
        ColorMaskRgb    = ColorMaskRed | ColorMaskGreen | ColorMaskBlue,                    //!< Render RED, GREEN and BLUE color channels.
    };

    //! Texture wrapping control.
    enum TextureWrap {
        Clamp,              //!< Clamp texture.
        Repeat              //!< Repeat texture.
    };

    // ** enum RenderStateValue
    enum RenderStateValue {
        RSValueDefault = -1,
        RSValueNotSet  = -2
    };

    // ** enum PolygonMode
    enum PolygonMode {
        PolygonWire = 0,
        PolygonFill
    };

    //! Triangle face enum (used in Hal::setCulling, Hal::setStencilAction, etc.)
    enum TriangleFace {
        TriangleFaceFront,          //!< Front face.
        TriangleFaceBack,           //!< Back face.
        TriangleFaceFrontAndBack,   //!< Front and back face.
        TriangleFaceNone            //!< Nothing.
    };

    //! Available fog modes
    enum FogMode {
        FogDisabled,
        FogLinear,
        FogExp,
        FogExp2
    };

    //! Comparison function used for alpha, depth and stencil tests.
    enum Compare {
        Always,                     //!< Always passes.
        Equal,                      //!< Passes if the incoming value is equal to the stored value.
        Greater,                    //!< Passes if the incoming value is greater than the stored value.
        GreaterEqual,               //!< Passes if the incoming value is greater than or equal to the stored value.
        Less,                       //!< Passes if the incoming value is less than the stored value.
        LessEqual,                  //!< Passes if the incoming value is less than or equal to the stored value.
        Never,                      //!< Never passes.
        NotEqual,                   //!< Passes if the incoming value is not equal to the stored value.

        CompareDisabled             //!< The test is disabled.
    };

    // ** enum Transform
    /*enum Transform {
        TransformModel,
        TransformView,
        TransformProjection,

        TransformTotal
    };*/

    //! Supported pixel format for textures and render targets.
    enum PixelFormat
    {
        PixelUnknown,
        PixelLuminance8,    //!< 8-bit luminance.
        PixelRgb8,          //!< 8-bit RGB color.
        PixelRgba8,         //!< 8-bit RGBA color.

        // ** TODO: add ETC/ATITC/S3TC formats
        PixelDxtc1,         //!< Compressed DXT format with 8 bytes per block with 1-bit alpha.
        PixelDxtc3,         //!< Compressed DXT format with 16 bytes per block.
        PixelDxtc5,         //!< Compressed DXT format with 16 bytes per block.
        PixelPvrtc2,        //!< Compressed PVR format with 2-bits per pixel.
        PixelPvrtc4,        //!< Compressed PVR format with 4-bits per pixel.

        PixelR16F,          //!< 16-bit floating point format with a single color channel.
        PixelRg16F,         //!< 16-bit floating point format with two color channels.
        PixelRgba16F,       //!< 16-bit floating point RGBA color.
        PixelR32F,          //!< 32-bit floating point format with a single color channel.
        PixelRg32F,         //!< 32-bit floating point RG color.
        PixelRgb32F,        //!< 32-bit floating point RGB color.
        PixelRgba32F,       //!< 32-bit floating point RGBA color.
    };
    
    //! Enumeration of depth and stencil format bits.
    enum DepthStencilFormat
    {
          Depth16 = 1   //!< Use a 16-bit depth buffer.
        , Depth24       //!< Use a 24-bit depth buffer.
        , Depth32       //!< Use a 32-bit depth buffer.
        , Stencil8      //!< Enable an 8-bit stencil buffer.
    };
    
    //! A texture/framebuffer creation options are packed into an integer as follows:
    // [2-bits filter | 2-bits stencil type | 2-bits depth type | 4-bits pixel format]
    enum TextureFormat
    {
          TextureLuminance8 = PixelLuminance8   << 0    //!< 8-bit luminance.
        , TextureRgb8       = PixelRgb8         << 0    //!< 8-bit RGB color.
        , TextureRgba8      = PixelRgba8        << 0    //!< 8-bit RGBA color.
        , TextureDxtc1      = PixelDxtc1        << 0    //!< Compressed DXT format with 8 bytes per block with 1-bit alpha.
        , TextureDxtc3      = PixelDxtc3        << 0    //!< Compressed DXT format with 16 bytes per block.
        , TextureDxtc5      = PixelDxtc5        << 0    //!< Compressed DXT format with 16 bytes per block.
        , TexturePvrtc2     = PixelPvrtc2       << 0    //!< Compressed PVR format with 2-bits per pixel.
        , TexturePvrtc4     = PixelPvrtc4       << 0    //!< Compressed PVR format with 4-bits per pixel.
        , TextureR16F       = PixelR16F         << 0    //!< 16-bit floating point format with a single color channel.
        , TextureRg16F      = PixelRg16F        << 0    //!< 16-bit floating point format with two color channels.
        , TextureRgba16F    = PixelRgba16F      << 0    //!< 16-bit floating point RGBA color.
        , TextureR32F       = PixelR32F         << 0    //!< 32-bit floating point format with a single color channel.
        , TextureRg32F      = PixelRg32F        << 0    //!< 32-bit floating point RG color.
        , TextureRgb32F     = PixelRgb32F       << 0    //!< 32-bit floating point RGB color.
        , TextureRgba32F    = PixelRgba32F      << 0    //!< 32-bit floating point RGBA color.
        
        , TextureD16        = Depth16           << 4    //!< A 16-bit depth buffer option.
        , TextureD24        = Depth24           << 4    //!< A 24-bit depth buffer option.
        , TextureD32        = Depth32           << 4    //!< A 32-bit depth buffer option.
        , TextureS8         = Stencil8          << 6    //!< An 8-bit stencil buffer option.
        
        , TextureLinear     = FilterLinear      << 9    //!< Use a linear texture filtering.
        , TextureBilinear   = FilterBilinear    << 9    //!< Use a bilinear texture filtering.
        , TextureTrilinear  = FilterTrilinear   << 9    //!< Use a trilinear texture filtering.
    };
    
    namespace Private
    {
        //! Returns a pixel format from options.
        NIMBLE_INLINE PixelFormat pixelFormatFromOptions(u32 options)
        {
            return static_cast<PixelFormat>(options & 0xF);
        }
        
        //! Returns a depth bits from options
        NIMBLE_INLINE s32 depthBitsFromOptions(u32 options)
        {
            switch ((options >> 4) & 0x3)
            {
                case 0:         return 0;
                case Depth16:   return 16;
                case Depth24:   return 24;
                case Depth32:   return 32;
                default: NIMBLE_NOT_IMPLEMENTED;
            }
            
            return 0;
        }
        
        //! Returns a stencil bits from options
        NIMBLE_INLINE s32 stencilBitsFromOptions(u32 options)
        {
            switch ((options >> 6) & 0x4)
            {
                case 0:         return 0;
                case Stencil8:  return 8;
                default: NIMBLE_NOT_IMPLEMENTED;
            }
            
            return 0;
        }
        
        //! Returns a texture filtering from options
        NIMBLE_INLINE TextureFilter textureFilterFromOptions(u32 options)
        {
            return static_cast<TextureFilter>((options >> 9) & 0xF);
        }
    } // namespace Private

    //! A list of available primitive types.
    enum PrimitiveType {
        PrimLines,          //!< Render lines.
        PrimLineStrip,      //!< Render line strip.
        PrimTriangles,      //!< Render triangles.
        PrimTriangleStrip,  //!< Render triangle strip.
        PrimTriangleFan,    //!< Render triangle fan.
        PrimQuads,          //!< Render quads.
        PrimPoints,         //!< Render points.

        TotalPrimitiveTypes    //!< Total primitive types available.
    };
    
    //! A normalized viewport coordinates.
    struct NormalizedViewport
    {
        f32     x;              //!< A coordinate of a left side of a viewport rectangle.
        f32     y;              //!< A coordinate of a lower side of the viewport rectangle.
        f32     width;          //!< Width of the viewport.
        f32     height;         //!< Height of the viewport.
    };
    
    //! A texture instance info.
    struct TextureInfo
    {
        u16                 width;          //!< A texture width.
        u16                 height;         //!< A texture height.
        u32                 options;        //!< A texture options.
        TextureType         type;           //!< A texture type.
    };
    
    //! Base class for windowed applications.
    class RenderingApplicationDelegate : public Platform::WindowedApplicationDelegate
    {
    protected:
        
        //! Creates a window and a context.
        virtual bool            initialize(s32 width, s32 height) NIMBLE_OVERRIDE;
        
        //! Renders a single frame.
        virtual void            handleRenderFrame(f32 dt) NIMBLE_ABSTRACT;
        
    private:
        
        //! Handles a window update event and renders a single frame.
        void                    handleViewUpdate(const RenderView::Update& e);
        
    protected:
        
        RenderViewPtr           m_view;             //!< A target view.
        RenderingContextPtr     m_renderingContext; //!< A rendering context instance.
        u64                     m_lastFrameTime;    //!< Used to measure the rendering time.
    };
    
    u32 bytesPerBlock(PixelFormat format);
    u32 bytesPerMipChain(PixelFormat format, u16 width, u16 height, u16 mipLevels);
    u32 bytesPerPixel(PixelFormat format);

} // namespace Renderer

DC_END_DREEMCHEST


#ifndef DC_BUILD_LIBRARY
    #include "BatchRenderer.h"
    #include "Renderer2D.h"
    #include "RenderingContext.h"
    #include "RenderingContext.h"
#endif

#endif
