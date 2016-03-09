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

#ifndef __DC_Font_FreetypeFontProvider_H__
#define __DC_Font_FreetypeFontProvider_H__

#include "../FontProvider.h"
#include "FreetypeFont.h"

DC_BEGIN_DREEMCHEST

namespace Font {

    // ** class FreetypeFontProvider
    class FreetypeFontProvider : public IFontProvider {
    friend class FreetypeFont;

		typedef std::map<int, sFontGlyph*>		FontTextures;
		typedef std::map<IFont*, FontTextures>	Glyphs;

    public:

									FreetypeFontProvider( dcContext ctx );
        virtual						~FreetypeFontProvider( void );

        // ** IFontProvider
        virtual IFont*              CreateFont( dcStream stream );
        virtual const sFontGlyph*	GetTextureForGlyph( IFont *font, u16 code, int size );

    private:

		sFontGlyph*					PutGlyphToTexture( const sFreetypeBitmap& bitmap );
        bool						CreateStream( dcStream stream, FT_Open_Args *args ) const;

        static unsigned long		StreamIOCallback( FT_Stream stream, unsigned long offset, unsigned char *buffer, unsigned long count );
        static void					StreamCloseCallback( FT_Stream stream );

    private:

        dcContext					m_ctx;
        FT_Library					m_freetype;
		Glyphs						m_glyphs;
		dcTextureAtlasAsset			m_atlas;
    };

} // namespace Font

DC_END_DREEMCHEST

#endif	/*	!__DC_Font_FreetypeFontProvider_H__	*/