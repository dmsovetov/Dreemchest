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

#include "FontProvider.h"
#include "Freetype/FreetypeFontProvider.h"

#include "../../asset/AssetManager.h"
#include "../../io/DiskFileSystem.h"
#include "../../io/Stream.h"

DC_BEGIN_DREEMCHEST

namespace Font {

// ** FontProvider::FontProvider
FontProvider::FontProvider( dcContext ctx ) : cSubsystem( "FontProvider", ctx )
{
    m_impl = DC_NEW FreetypeFontProvider( ctx );
}

FontProvider::~FontProvider( void )
{
    delete m_impl;
}

// ** FontProvider::CreateFont
Font* FontProvider::CreateFont( const char *fileName )
{
    dcStream stream = m_ctx->iDiskFileSystem->openFile( m_ctx->iAssetManager->ResolveFileName( fileName ) );
    Font     *font  = CreateFont( stream );

    if( !font ) {
        stream->release();
    }

    return font;
}

// ** FontProvider::CreateFont
Font* FontProvider::CreateFont( dcStream stream )
{
    DC_CHECK_IMPL( m_ctx->iLog, NULL );

    Font *font	 = DC_NEW Font;
	font->m_impl = m_impl->CreateFont( stream );

    return font;
}

// ** FontProvider::GetTextureForGlyph
const sFontGlyph* FontProvider::GetTextureForGlyph( Font *font, u16 code, int size )
{
	DC_CHECK_IMPL( m_ctx->iLog, NULL );
	if( !font->m_impl ) {
		return NULL;
	}

	return m_impl->GetTextureForGlyph( font->m_impl, code, size );
}

} // namespace Font
    
DC_END_DREEMCHEST
