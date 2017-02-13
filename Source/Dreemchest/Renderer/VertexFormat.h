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

#ifndef __DC_Renderer_VertexFormat_H__
#define __DC_Renderer_VertexFormat_H__

#include "Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Renderable vertex format.
    class VertexFormat
    {
    public:
        
        //! Available vertex attributes
        enum Attribute
        {
              Position      = BIT(VertexPosition)
            , Normal        = BIT(VertexNormal)
            , Color         = BIT(VertexColor)
            , TexCoord0     = BIT(VertexTexCoord0)
            , TexCoord1     = BIT(VertexTexCoord1)
            , TexCoord2     = BIT(VertexTexCoord2)
            , TexCoord3     = BIT(VertexTexCoord3)
            , TexCoord4     = BIT(VertexTexCoord4)
            , TexCoord5     = BIT(VertexTexCoord5)
            , TexCoord6     = BIT(VertexTexCoord6)
            , TexCoord7     = BIT(VertexTexCoord7)
            , Tangent       = BIT(VertexTangent)
            , Bitangent     = BIT(VertexBitangent)
            , PointSize     = BIT(VertexPointSize)
        };
        
                                //! Constructs a VertexFormat instance.
                                VertexFormat( u8 attributes );
        
        //! Converts a VertexFormat to u8 value.
        operator                u8( void ) const;
        
        //! Returns true if a specified attribute exists.
        bool                    operator & ( Attribute attribute ) const;
        
        //! Tests two vertex formats for an equality.
        bool                    operator == ( const VertexFormat& other ) const;
        
        //! Returns a vertex size.
        s32                     vertexSize( void ) const;
        
        //! Returns a vertex attribute value.
        template<typename TValue>
        TValue                  vertexAttribute( Attribute attribute, const void* vertices, s32 index ) const;
        
        //! Sets a vertex attribute value.
        template<typename TValue>
        void                    setVertexAttribute( Attribute attribute, const TValue& value, void* vertices, s32 index ) const;
        
        //! Returns a vertex attribute offset.
        s32                     attributeOffset( Attribute attribute ) const;
        
        //! Returns a vertex attribute size.
        s32                     attributeSize( Attribute attribute ) const;
        
        //! Returns a vertex offset.
        s32                     vertexOffset( s32 index ) const;
        
        //! Returns a vertex position size in bytes.
        s32                     sizeOfPosition( void ) const;
        
        //! Returns a vertex color size in bytes.
        s32                     sizeOfColor( void ) const;
        
        //! Returns a vertex normal size in bytes.
        s32                     sizeOfNormal( void ) const;
        
        //! Returns a vertex uv layer size in bytes.
        s32                     sizeOfUv0( void ) const;
        
        //! Returns a vertex uv layer size in bytes.
        s32                     sizeOfUv1( void ) const;
        
    private:
        
        u8                      m_attributes;   //!< Vertex attribute mask.
    };
    
    // ** VertexFormat::VertexFormat
    NIMBLE_INLINE VertexFormat::VertexFormat( u8 attributes )
        : m_attributes( attributes | Position )
    {
    }
    
    // ** VertexFormat::operator u8
    NIMBLE_INLINE VertexFormat::operator u8( void ) const
    {
        return m_attributes;
    }
    
    // ** VertexFormat::operator &
    NIMBLE_INLINE bool VertexFormat::operator & ( Attribute attribute ) const
    {
        return m_attributes & attribute ? true : false;
    }
    
    // ** VertexFormat::operator ==
    NIMBLE_INLINE bool VertexFormat::operator == ( const VertexFormat& other ) const
    {
        return m_attributes == other.m_attributes;
    }
    
    // ** VertexFormat::vertexSize
    NIMBLE_INLINE s32 VertexFormat::vertexSize( void ) const
    {
        return    sizeOfPosition()
                + sizeOfNormal()
                + sizeOfColor()
                + sizeOfUv0()
                + sizeOfUv1()
                ;
    }
    
    // ** VertexFormat::attributeOffset
    NIMBLE_INLINE s32 VertexFormat::attributeOffset( Attribute attribute ) const
    {
        NIMBLE_ABORT_IF( (m_attributes & attribute) == 0, "a vertex format does not contain a specified attribute" );
        
        s32 offset = 0;
        
        switch( attribute )
        {
            case TexCoord1: offset += sizeOfUv0();
            case TexCoord0: offset += sizeOfColor();
            case Color:     offset += sizeOfNormal();
            case Normal:    offset += sizeOfPosition();
            case Position:  break;
            default:        NIMBLE_NOT_IMPLEMENTED
        }
        
        return offset;
    }
    
    // ** VertexFormat::attributeSize
    NIMBLE_INLINE s32 VertexFormat::attributeSize( Attribute attribute ) const
    {
        switch( attribute )
        {
            case Position:  return sizeof( f32 ) * 3;
            case Normal:    return sizeof( f32 ) * 3;
            case Color:     return sizeof( u8  ) * 4;
            case TexCoord0: return sizeof( f32 ) * 2;
            case TexCoord1: return sizeof( f32 ) * 2;
            default:        NIMBLE_NOT_IMPLEMENTED
        }
        
        return 0;
    }
    
    // ** VertexFormat::vertexOffset
    NIMBLE_INLINE s32 VertexFormat::vertexOffset( s32 index ) const
    {
        return vertexSize() * index;
    }
    
    // ** VertexFormat::vertexAttribute
    template<typename TValue>
    TValue VertexFormat::vertexAttribute( Attribute attribute, const void* vertices, s32 index ) const
    {
        if( m_attributes & attribute )
        {
            NIMBLE_ABORT_IF( sizeof( TValue ) != attributeSize( attribute ), "vertex attribute size mismatch" );
            return *reinterpret_cast<const TValue*>( reinterpret_cast<const u8*>( vertices ) + vertexOffset( index ) + attributeOffset( attribute ) );
        }
        
        return TValue();
    }
    
    // ** VertexFormat::setVertexAttribute
    template<typename TValue>
    void VertexFormat::setVertexAttribute( Attribute attribute, const TValue& value, void* vertices, s32 index ) const
    {
        if( m_attributes & attribute )
        {
            NIMBLE_ABORT_IF( sizeof( TValue ) != attributeSize( attribute ), "vertex attribute size mismatch" );
            *reinterpret_cast<TValue*>( reinterpret_cast<u8*>( vertices ) + vertexOffset( index ) + attributeOffset( attribute ) ) = value;
        }
    }
    
    // ** VertexFormat::sizeOfPosition
    NIMBLE_INLINE s32 VertexFormat::sizeOfPosition( void ) const
    {
        if( m_attributes & Position )
        {
            return attributeSize( Position );
        }
        
        return 0;
    }
    
    // ** VertexFormat::sizeOfColor
    NIMBLE_INLINE s32 VertexFormat::sizeOfColor( void ) const
    {
        if( m_attributes & Color )
        {
            return attributeSize( Color );
        }
        
        return 0;
    }
    
    // ** VertexFormat::sizeOfNormal
    NIMBLE_INLINE s32 VertexFormat::sizeOfNormal( void ) const
    {
        if( m_attributes & Normal )
        {
            return attributeSize( Normal );
        }
        
        return 0;
    }
    
    // ** VertexFormat::sizeOfUv0
    NIMBLE_INLINE s32 VertexFormat::sizeOfUv0( void ) const
    {
        if( m_attributes & TexCoord0 )
        {
            return attributeSize( TexCoord0 );
        }
        
        return 0;
    }
    
    // ** VertexFormat::sizeOfUv1
    NIMBLE_INLINE s32 VertexFormat::sizeOfUv1( void ) const
    {
        if( m_attributes & TexCoord1 )
        {
            return attributeSize( TexCoord1 );
        }
        
        return 0;
    }
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  #ifndef __DC_Renderer_VertexFormat_H__  */
