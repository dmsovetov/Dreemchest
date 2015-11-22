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

#include "TextFormatter.h"

#include "../../asset/FontAsset.h"

DC_BEGIN_DREEMCHEST

namespace Font {

// ** TextFormatter::FormatText
void TextFormatter::FormatText( TextBlock& block, const char *text, int maxWidth, int maxHeight, const sTextFormat& format )
{
    TextChunkList chunks;

    m_text   = text;
    m_format = &format;

    SplitToChunks( chunks );
	ArrangeChunks( chunks, block.m_lines, maxWidth ? maxWidth : INT_MAX, block.m_width, block.m_height );

    if( maxWidth && m_format->m_align != Align::None ) {
		HorizontalAlignChunks( block.m_lines, maxWidth );
    }
    if( maxHeight && m_format->m_verticalAlign != VerticalAlign::None ) {
		VerticalAlignChunks( block.m_lines, block.m_height, maxHeight );
    }

    block.m_textFormat         = format;
    block.m_vertexBufferDirty  = true;
}

// ** TextFormatter::SplitToChunks
void TextFormatter::SplitToChunks( TextChunkList& chunks, const char *separators )
{
    chunks.clear();

    int blockStart = 0;
    int length     = utf8::unchecked::distance( m_text, m_text + strlen( m_text ) );

    const char* str   = m_text;
    const char* start = str;

    for( int i = 0; i < length; i++ ) {
        u32 code = utf8::unchecked::next( str );
        
        if( !strchr( separators, code ) ) {
            continue;
        }

        if( blockStart != i ) {
            chunks.push_back( sTextChunk( start, blockStart, i - blockStart ) );
        }

        chunks.push_back( sTextChunk( str - 1, i, 1 ) );
        blockStart = i + 1;
        start      = str;
    }

    if( blockStart != length ) {
        chunks.push_back( sTextChunk( start, blockStart, length - blockStart ) );
    }

    // ** Debug
#if 0
    for( TextChunkList::const_iterator i = blocks.begin(), end = blocks.end(); i != end; i++ )
    {
        std::string value = std::string( m_text ).substr( i->m_start, i->m_end - i->m_start );
        printf( "Block: '%s'\n", value == "\n" ? "new-line" : value.c_str() );
    }
#endif
}

// ** TextFormatter::ArrangeChunks
void TextFormatter::ArrangeChunks( TextChunkList& chunks, TextLineArray& lines, int maxWidth, int& width, int& height )
{
    PrepareChunks( chunks );

    const dcFont font = m_format->m_font->GetFontPointer();
    if( !font ) {
        return;
    }
    
    int ascender	= font->GetAscender( m_format->m_size );
    int descender   = font->GetDescender( m_format->m_size );
    int gap         = font->GetLineGap( m_format->m_size );

    // ** Put blocks to lines
    lines.push_back( sTextLine( ascender ) );
    
    for( TextChunkList::iterator chunk = chunks.begin(), end = chunks.end(); chunk != end; ++chunk ) {
	//	if( block->m_width > maxWidth ) {
	//		Warning( "TextFormatter::ArrangeBlocks : text block width is greater then text area width\n" );
	//	}

        sTextLine *line = &lines.back();

        // ** Check block overlap or new line block
        if( chunk->IsNewLine() || (line->m_width + chunk->m_width > maxWidth) ) {
			// ** Remove trailing white space from current line
			while( !line->m_chunks.empty() && line->m_chunks.back().m_flags & sTextChunk::EmptySpace ) {
				line->m_chunks.pop_back();
			}

            lines.push_back( sTextLine( line->m_y + gap ) );
			line = &lines.back();
        }

        // ** Format block and line
        chunk->m_x = line->m_width;
        chunk->m_y = line->m_y;

        line->m_width += chunk->m_width;
        line->m_chunks.push_back( *chunk );
    }

	// ** Strip starting white space and recalculate width for each line
	for( int i = 0, n = ( int )lines.size(); i < n; i++ ) {
		sTextLine& line = lines[i];

		line.m_width = 0;

		while( !line.m_chunks.empty() && line.m_chunks.back().m_flags & sTextChunk::EmptySpace ) {
			line.m_chunks.pop_front();
		}

		for( TextChunkList::iterator chunk = line.m_chunks.begin(), end = line.m_chunks.end(); chunk != end; ++chunk ) {
			line.m_width += chunk->m_width;
		}
	}

    // ** Calculate text block dimensions & each line width
    width = height = 0;

    for( TextLineArray::iterator line = lines.begin(), end = lines.end(); line != end; ++line ) {
        if( line->m_width         > width  ) width  = line->m_width;
        if( line->m_y + descender > height ) height = line->m_y + descender;
    }

//    DC_BREAK_IF( width > maxWidth );
}

// ** TextFormatter::HorizontalAlignChunks
void TextFormatter::HorizontalAlignChunks( TextLineArray& lines, int maxWidth )
{
    int align = m_format->m_align;
    
	for( int i = 0, n = ( int )lines.size(); i < n; i++ ) {
		sTextLine *line = &lines[i];

		if( line->m_chunks.empty() ) {
			continue;
		}

		if( align == Align::Justify && (line->m_chunks.size() <= 1 || i == n - 1) ) {
			continue;
		}

        int delta  = maxWidth - line->m_width;
        int offset = 0;

        switch( align )
        {
        case Align::Left:       break;
        case Align::Right:      offset = delta;
                                break;
        case Align::Center:     offset = delta / 2;
                                break;
        case Align::Justify:    offset = delta / (line->m_chunks.size() - 1);
                                break;
        default:                DC_BREAK;
        }

        int blockOffset = offset;

        for( TextChunkList::iterator chunk = /*++*/line->m_chunks.begin(), lastChunk = line->m_chunks.end(); chunk != lastChunk; ++chunk )
        {
            chunk->m_x += blockOffset;
            
            if( align == Align::Justify ) {
                blockOffset += offset;
            }
        }
    }
}

// ** TextFormatter::VerticalAlignChunks
void TextFormatter::VerticalAlignChunks( TextLineArray& lines, int height, int maxHeight )
{
    int align = m_format->m_verticalAlign;
    int delta = maxHeight - height;

    for( TextLineArray::iterator line = lines.begin(), lastLine = lines.end(); line != lastLine; ++line )
    {
        int offset = 0;

        switch( align )
        {
		case VerticalAlign::None:
        case VerticalAlign::Top:    break;
        case VerticalAlign::Bottom: offset = delta;
                                    break;
        case VerticalAlign::Center: offset = delta / 2;
                                    break;
        }

        line->m_y += offset;

        for( TextChunkList::iterator chunk = line->m_chunks.begin(), lastChunk = line->m_chunks.end(); chunk != lastChunk; ++chunk )
        {
            chunk->m_y = line->m_y;
        }
    }
}

// ** TextFormatter::PrepareChunks
void TextFormatter::PrepareChunks( TextChunkList& chunks )
{
    const dcFont font = m_format->m_font->GetFontPointer();
    if( !font ) {
        return;
    }
    
    for( TextChunkList::iterator chunk = chunks.begin(), end = chunks.end(); chunk != end; ++chunk )
    {
        char c      = chunk->m_text[0];
        int  width  = font->CalculateLineWidth( chunk->m_text, chunk->m_count, m_format->m_size );
        
        chunk->m_width = width;
        chunk->m_flags.Set( sTextChunk::NewLine,    c == '\n' );
        chunk->m_flags.Set( sTextChunk::EmptySpace, c == ' ' || c == '\t' || c == '\n' );
    }
}

} // namespace Font
    
DC_END_DREEMCHEST