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

#include "TextBlock.h"

#include "../Text.h"
#include "../../asset/FontAsset.h"
#include "../../asset/TextureAtlasAsset.h"
#include "../../renderer/Hal.h"

#include    <utf8/utf8.h>

DC_BEGIN_DREEMCHEST

namespace Font {

// ** TextBlock::TextBlock
TextBlock::TextBlock( dcFontProvider fontProvider )
{
    m_fontProvider      = fontProvider;
    m_atlas             = NULL;
    m_width             = 0;
    m_height            = 0;
	m_maxWidth			= INT_MAX;
	m_maxHeight			= INT_MAX;
    m_vertexBufferDirty = false;
    m_formatDirty       = false;
}

TextBlock::~TextBlock( void )
{
    DC_RELEASE( m_textFormat.m_font );
}

// ** TextBlock::RequestVertexBuffer
StageVertexArray& TextBlock::RequestVertexBuffer( void )
{
    if( m_vertexBufferDirty ) {
        UpdateVertexBuffer();
    }

    return m_vertices;
}

// ** TextBlock::GetTextFormat
const sTextFormat& TextBlock::GetTextFormat( void ) const
{
    return m_textFormat;
}

// ** TextBlock::RequestLines
const TextLineArray& TextBlock::RequestLines( void )
{
    if( m_formatDirty ) {
        UpdateTextFormat();
    }

    return m_lines;
}

// ** TextBlock::RequestWidth
int TextBlock::RequestWidth( void )
{
    if( m_formatDirty ) {
        UpdateTextFormat();
    }

    return m_width;
}

// ** TextBlock::RequestHeight
int TextBlock::RequestHeight( void )
{
    if( m_formatDirty ) {
        UpdateTextFormat();
    }
    
    return m_height;
}

// ** TextBlock::GetTexture
dcTexture2D TextBlock::GetTexture( void ) const
{
    dcTexture texture = m_atlas ? m_atlas->GetTexturePointer() : NULL;
    return texture ? texture->isTexture2D() : NULL;
}

// ** TextBlock::SetSize
void TextBlock::SetSize( int value )
{
    if( m_textFormat.m_size == value ) {
        return;
    }
    
    m_textFormat.m_size = value;
    OnFormatChanged();
}

// ** TextBlock::SetColor
void TextBlock::SetColor( const rgba& value )
{
    StageVertexArray& vertices = RequestVertexBuffer();

    m_color = value;

    for( int i = 0, n = ( int )vertices.size(); i < n; i++ ) {
        vertices[i].r = m_color.r;
        vertices[i].g = m_color.g;
        vertices[i].b = m_color.b;
        vertices[i].a = m_color.a;
    }
}

// ** TextBlock::SetAlign
void TextBlock::SetAlign( Align value )
{
    if( m_textFormat.m_align == value ) {
        return;
    }
    
    m_textFormat.m_align = value;
    OnFormatChanged();
}

// ** TextBlock::SetVerticalAlign
void TextBlock::SetVerticalAlign( VerticalAlign value )
{
    if( m_textFormat.m_verticalAlign == value ) {
        return;
    }
    
    m_textFormat.m_verticalAlign = value;
    OnFormatChanged();
}

// ** TextBlock::SetMaxWidth
void TextBlock::SetMaxWidth( int value )
{
    if( m_maxWidth == value ) {
        return;
    }
    
    m_maxWidth = value;
    OnFormatChanged();
}

// ** TextBlock::SetMaxHeight
void TextBlock::SetMaxHeight( int value )
{
    if( m_maxHeight == value ) {
        return;
    }

    m_maxHeight = value;
    OnFormatChanged();
}

// ** TextBlock::SetFont
void TextBlock::SetFont( dcFontAsset value )
{
    if( m_textFormat.m_font == value ) {
        return;
    }

    DC_RELEASE( m_textFormat.m_font );
    if( (m_textFormat.m_font = value) ) {
		m_textFormat.m_font->Ref();
	}
    OnFormatChanged();
}

// ** TextBlock::SetString
void TextBlock::SetString( const std::string& value )
{
    if( m_string == value ) {
        return;
    }

    m_string = value;
    OnFormatChanged();
}

// ** TextBlock::OnFormatChanged
void TextBlock::OnFormatChanged( void )
{
    m_vertexBufferDirty = m_formatDirty = true;
}

// ** TextBlock::UpdateTextFormat
void TextBlock::UpdateTextFormat( void )
{
    DC_BREAK_IF( m_formatDirty == false );

    m_lines.clear();

	if( m_textFormat.m_font == NULL ) {
		return;
	}

    TextFormatter formatter;
    formatter.FormatText( *this, m_string.c_str(), m_maxWidth, m_maxHeight, m_textFormat );
    m_formatDirty = false;
}

// ** TextBlock::UpdateVertexBuffer
void TextBlock::UpdateVertexBuffer( void )
{
    DC_BREAK_IF( m_vertexBufferDirty == false );

    m_vertices.clear();

    if( !m_textFormat.m_font ) {
        return;
    }

    // ** Conver to utf-16
    std::vector<utf8::uint16_t> utf16line;
    int length = utf8::unchecked::distance( m_string.begin(), m_string.end() );
    utf16line.reserve( length );
    utf8::unchecked::utf8to16( m_string.begin(), m_string.end(), std::back_inserter( utf16line ) );

	if( length == 0 ) {
		m_vertexBufferDirty = false;
		return;
	}

    // ** Generate vertex buffer
	m_vertices.resize( length * 4 );
    renderer::sVertex *primitive = &m_vertices[0];

    dcFont font = m_textFormat.m_font->GetFontPointer();
    int    size = m_textFormat.m_size;

    const TextLineArray& lines = RequestLines();

	for( TextLineArray::const_iterator line = lines.begin(), lastLine = lines.end(); line != lastLine; ++line ) {
		int lineY = line->m_y;

		for( TextChunkList::const_iterator chunk = line->m_chunks.begin(), lastBlock = line->m_chunks.end(); chunk != lastBlock; ++chunk ) {
			int x = chunk->m_x;

			for( int i = chunk->m_index; i < chunk->m_index + chunk->m_count; i++ ) {
				const sFontGlyph *glyph = m_fontProvider->GetTextureForGlyph( font, utf16line[i], size );
				if( !glyph ) {
					continue;
				}

				int			w  = glyph->m_width;
				int			h  = glyph->m_height;
				const vec4& uv = glyph->m_uv;
				int			y  = lineY + glyph->m_offset - glyph->m_height;

				primitive[0].x = x;		primitive[0].y = y + h;
				primitive[1].x = x + w;	primitive[1].y = y + h;
				primitive[2].x = x + w;	primitive[2].y = y;
				primitive[3].x = x;		primitive[3].y = y;

				primitive[0].u = uv.x;	primitive[0].v = uv.w;
				primitive[1].u = uv.z;	primitive[1].v = uv.w;
				primitive[2].u = uv.z;	primitive[2].v = uv.y;
				primitive[3].u = uv.x;	primitive[3].v = uv.y;

				primitive += 4;

				m_atlas = glyph->m_atlas;
				x += glyph->m_advance;
			}
		}
	}

    m_vertexBufferDirty = false;

    SetColor( m_color );
}

} // namespace Font

DC_END_DREEMCHEST