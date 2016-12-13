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

#ifndef __DC_Renderer_VertexBufferLayout_H__
#define __DC_Renderer_VertexBufferLayout_H__

#include "Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! VertexBufferLayout class instance is used to define a vertex buffer layout.
    class VertexBufferLayout
    {
#if DEV_DEPRECATED_HAL
    friend class Hal;
#endif  /*  #if DEV_DEPRECATED_HAL  */
    public:
        
        //! Supported vertex attributes.
        enum Attribute {
            Position,               //!< Vertex position.
            Normal,                 //!< Vertex normal.
            Color,                  //!< Vertex color.
            Tangent,                //!< Vertex tangent.
            Bitangent,              //!< Vertex bitangent.
            PointSize,              //!< Point sprite size.
            Uv0,                    //!< Texture 0 UV.
            Uv1,                    //!< Texture 1 UV.
            Uv2,                    //!< Texture 2 UV.
            Uv3,                    //!< Texture 3 UV.
            Uv4,                    //!< Texture 4 UV.
            Uv5,                    //!< Texture 2 UV.
            Uv6,                    //!< Texture 3 UV.
            Uv7,                    //!< Texture 4 UV.
            TotalAttributes,        //!< Total amount of supported vertex attributes.
        };
        
        
        //! Input layout element.
        struct Element {
            s32     count;      //!< Attribute size.
            s32     offset;     //!< Attribute offset.
            
            //! Constructs a new Element instance.
            Element( void )
                : count( -1 )
                , offset( -1 )
                {
                }
            
            //! Returns true if this vertex element is used.
            operator bool() const { return count > 0 && offset >= 0; }
        };
        
    public:
        
                                //! Constructs an VertexBufferLayout instance.
                                VertexBufferLayout( s32 vertexSize );
        
        //! Returns a bitmask of available features.
        PipelineFeatures        features( void ) const;
        
        //! Returns a vertex size in bytes.
        s32                     vertexSize( void ) const;
        
        //! Defines a vertex attribute location.
        void                    attributeLocation( Attribute attribute, s32 count, s32 offset );
        
        //! Returns the vertex position attribute.
        const Element&          position( void ) const;
        
        //! Enables a color input.
        void                    setColor( s32 size, s32 offset = -1 );
        
        //! Returns the vertex color attribute.
        const Element&          color( void ) const;
        
        //! Returns the vertex normal attribute.
        const Element&          normal( void ) const;
        
        //! Returns the vertex UV attribute for a given sampler index.
        const Element&          uv( u32 sampler ) const;
        
        //! Returns the point size attribute.
        const Element&          pointSize( void ) const;
        
    protected:
        
        PipelineFeatures        m_features;                     //!< An input layout features.
        s32                     m_vertexSize;                   //!< Vertex size in bytes.
        Element                 m_attributes[TotalAttributes];  //!< Array of vertex attributes.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_VertexBufferLayout_H__    */
