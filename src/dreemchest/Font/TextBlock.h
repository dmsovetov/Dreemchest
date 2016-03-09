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

#ifndef __DC_Font_TextBlock_H__
#define __DC_Font_TextBlock_H__

#include "FontProvider.h"
#include "../StageObject.h"

DC_BEGIN_DREEMCHEST

namespace Font {

    // ** struct sTextChunk
    struct sTextChunk {
        enum {
            NewLine     = BIT( 0 ),
            EmptySpace  = BIT( 1 ),
        };

        const char*     m_text;
        int             m_index, m_count;
        int             m_width;
        int             m_x, m_y;
        cFlagSet8       m_flags;

                        sTextChunk( void )
                            : m_text( NULL ), m_index( 0 ), m_count( 0 ), m_width( 0 ), m_x( 0 ), m_y( 0 ) {}
                        sTextChunk( const char* text, int index, int count )
                            : m_text( text ), m_index( index ), m_count( count ), m_width( 0 ), m_x( 0 ), m_y( 0 ) {}

        bool            IsNewLine( void ) const { return m_flags.Is( NewLine ); }
    };

    typedef std::list<sTextChunk> TextChunkList;

    // ** struct sTextLine
    struct sTextLine {
        enum {
            HasLineEnd  = BIT( 0 ),
            LastLine    = BIT( 1 ),
        };

        int             m_width;
        int             m_y;
        cFlagSet8       m_flags;
        TextChunkList   m_chunks;

                        sTextLine( void )
                            : m_width( 0 ), m_y( 0 ) {}
                        sTextLine( int y )
                            : m_width( 0 ), m_y( y ) {}
    };

    typedef std::vector<sTextLine> TextLineArray;

    // ** struct sTextFormat
    struct sTextFormat {
        dcFontAsset     m_font;
        int             m_size;
        Align           m_align;
        VerticalAlign   m_verticalAlign;

                        sTextFormat( void )
                            : m_font( NULL ), m_size( 10 ), m_align( Align::None ), m_verticalAlign( VerticalAlign::None ) {}
    };

    // ** class TextBlock
    class TextBlock {
    friend class TextFormatter;
    friend class cText;
    public:

                                    TextBlock( dcFontProvider fontProvider );
                                    ~TextBlock( void );

        StageVertexArray&           RequestVertexBuffer( void );
        const TextLineArray&        RequestLines( void );
        int                         RequestWidth( void );
        int                         RequestHeight( void );
        
        const sTextFormat&          GetTextFormat( void ) const;
        dcTexture2D                 GetTexture( void ) const;
        const std::string&          GetString( void ) const;
        void                        SetString( const std::string& value );
        void                        SetFont( dcFontAsset value );
        void                        SetColor( const rgba& value );
        void                        SetSize( int value );
        void                        SetAlign( Align value );
        void                        SetVerticalAlign( VerticalAlign value );
        void                        SetMaxWidth( int value );
        void                        SetMaxHeight( int value );

    private:

        void                        UpdateTextFormat( void );
        void                        UpdateVertexBuffer( void );
        void                        OnFormatChanged( void );

    private:

        std::string                 m_string;
        dcFontProvider              m_fontProvider;
        TextLineArray               m_lines;
        sTextFormat                 m_textFormat;
        int                         m_maxWidth, m_maxHeight, m_width, m_height;
        rgba                        m_color;
        mutable dcTextureAtlasAsset m_atlas;
        mutable StageVertexArray    m_vertices;
        bool                        m_formatDirty, m_vertexBufferDirty;
    };

} // namespace Font

DC_END_DREEMCHEST

#endif	/*	!__DC_Font_TextBlock_H__	*/