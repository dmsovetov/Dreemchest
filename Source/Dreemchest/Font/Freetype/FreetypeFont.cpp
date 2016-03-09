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

#include "FreetypeFont.h"
#include "FreetypeFontProvider.h"

#include "../../../platform/Engine.h"

#include <ft2build.h>
#include <freetype/freetype.h>

DC_BEGIN_DREEMCHEST

namespace Font {

// ** FreetypeFont::FreetypeFont
FreetypeFont::FreetypeFont( FreetypeFontProvider* provider, FT_Face face ) : m_provider( provider ), m_face( face ), m_size( -1 )
{

}

FreetypeFont::~FreetypeFont( void )
{
    FT_Done_Face( m_face );
}

// ** FreetypeFont::RenderBitmap
bool FreetypeFont::RenderBitmap( sFreetypeBitmap& bitmap, u16 code, int size )
{
	SelectSize( size );

    float scaleFactor = m_provider->m_ctx->iEngine->GetScaleFactor();

    SetScaleFactor( scaleFactor );

	if ( FT_Load_Char( m_face, code, FT_LOAD_DEFAULT ) ) {
		return false;
	}

    if( FT_Render_Glyph( m_face->glyph, FT_RENDER_MODE_NORMAL ) ) {
        return false;
    }

	FT_GlyphSlot slot = m_face->glyph;

	bitmap.m_advance  = slot->metrics.horiAdvance  >> 6;
	bitmap.m_offset   = slot->metrics.horiBearingY >> 6;
    bitmap.m_width    = slot->bitmap.width / scaleFactor;
    bitmap.m_height   = slot->bitmap.rows  / scaleFactor;
    bitmap.m_image    = new Image( renderer::PixelAlpha8, slot->bitmap.width, slot->bitmap.rows );

    memcpy( bitmap.m_image->pixels(), slot->bitmap.buffer, bitmap.m_image->size() );

    SetScaleFactor( 1 );

	return true;
}

// ** FreetypeFont::GetAscender
int FreetypeFont::GetAscender( int size ) const
{
    SelectSize( size );
    return m_face->size->metrics.ascender >> 6;
}

// ** FreetypeFont::GetDescender
int FreetypeFont::GetDescender( int size ) const
{
    SelectSize( size );
    return -m_face->size->metrics.descender >> 6;
}

// ** FreetypeFont::GetLineGap
int FreetypeFont::GetLineGap( int size ) const
{
    SelectSize( size );
    return (m_face->size->metrics.ascender - m_face->size->metrics.descender) >> 6;
}

// ** FreetypeFont::CalculateLineWidth
int FreetypeFont::CalculateLineWidth( const char* text, int count, int size ) const
{
    SelectSize( size );

    int result = 0;

    for( int i = 0; i < count; i++ )
    {
        u32 code      = utf8::unchecked::next( text );
        FT_UInt glyph = FT_Get_Char_Index( m_face, code );

        if( FT_Load_Glyph( m_face, glyph, FT_LOAD_DEFAULT ) != 0 )
        {
            continue;
        }

        result += m_face->glyph->metrics.horiAdvance >> 6;
    }

    return result;
}

// ** FreetypeFont::SetScaleFactor
void FreetypeFont::SetScaleFactor( float value ) const
{
    FT_Matrix matrix;
    FT_Vector pen;

    matrix.xx = (FT_Fixed)( value * 0x10000L );
    matrix.xy = (FT_Fixed)( 0 * 0x10000L );
    matrix.yx = (FT_Fixed)( 0 * 0x10000L );
    matrix.yy = (FT_Fixed)( value * 0x10000L );

    pen.x = 0;
    pen.y = 0;

    FT_Set_Transform( m_face, &matrix, &pen );
}

// ** FreetypeFont::SelectSize
void FreetypeFont::SelectSize( int value ) const
{
    if( m_size == value ) {
        return;
    }
    
	m_size = value;
	FT_Error error = FT_Set_Char_Size( m_face, m_size << 6, m_size << 6, 96, 96 );
    DC_BREAK_IF( error );
}

// ------------------------------------- sFreetypeBitmap ------------------------------------- //

// ** sFreetypeBitmap::sFreetypeBitmap
sFreetypeBitmap::sFreetypeBitmap( void )
{
    m_image     = NULL;
	m_advance	= 0;
	m_offset	= 0;
}

sFreetypeBitmap::~sFreetypeBitmap( void )
{
}

} // namespace Font

DC_END_DREEMCHEST