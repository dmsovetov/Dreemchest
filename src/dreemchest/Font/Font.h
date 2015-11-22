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

#ifndef __DC_Font_Font_H__
#define __DC_Font_Font_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Font {

	// ** struct sFontGlyph
	struct sFontGlyph {
		dcTextureAtlasAsset	m_atlas;
		vec4				m_uv;
		int					m_width, m_height;
		int					m_advance;
		int					m_offset;
	};

    // ** class IFont
    class IFont {
    public:

        virtual				~IFont( void ) {}

        virtual int			GetAscender( int size ) const		= 0;
        virtual int			GetDescender( int size ) const		= 0;
        virtual int			GetLineGap( int size ) const		= 0;
        virtual int			CalculateLineWidth( const char *text, int count, int size ) const = 0;
    };

    // ** class Font
    class Font {
    friend class FontProvider;
    public:

        int					GetAscender( int size ) const;
        int					GetDescender( int size ) const;
        int					GetLineGap( int size ) const;
        int					CalculateLineWidth( const char *text, int count, int size ) const;
        void                Release( void );

    private:

        IFont*				m_impl;
    };

} // namespace Font

DC_END_DREEMCHEST

#endif	/*	!__DC_Font_Font_H__	*/