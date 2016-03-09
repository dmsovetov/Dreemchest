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

#ifndef __DC_Font_TextFormatter_H__
#define __DC_Font_TextFormatter_H__

#include "TextBlock.h"

DC_BEGIN_DREEMCHEST

namespace Font {

    // ** class TextFormatter
    class TextFormatter {
    public:

        void                FormatText( TextBlock& block, const char *text, int maxWidth, int maxHeight, const sTextFormat& format );

    private:

        void                SplitToChunks( TextChunkList& chunks, const char *separators = " \t\n()" );
        void                ArrangeChunks( TextChunkList& chunks, TextLineArray& lines, int maxWidth, int& width, int& height );
        void                HorizontalAlignChunks( TextLineArray& lines, int maxWidth );
        void                VerticalAlignChunks( TextLineArray& lines, int height, int maxHeight );
        void                PrepareChunks( TextChunkList& chunks );

    private:

        const char*         m_text;
        const sTextFormat*  m_format;
    };
    
} // namespace Font

DC_END_DREEMCHEST

#endif	/*	!__DC_Font_TextFormatter_H__	*/