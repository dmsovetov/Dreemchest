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

#include "FreetypeFontProvider.h"
#include "FreetypeFont.h"

#include "../../../asset/AssetManager.h"
#include	"../../../asset/TextureAtlasAsset.h"
#include "../../../io/Stream.h"

DC_BEGIN_DREEMCHEST

namespace Font {

// ** FreetypeFontProvider::FreetypeFontProvider
FreetypeFontProvider::FreetypeFontProvider( dcContext ctx ) : m_ctx( ctx ), m_freetype( NULL ), m_atlas( NULL )
{
    FT_Error error = FT_Init_FreeType( &m_freetype );
    DC_BREAK_IF( error );
}

FreetypeFontProvider::~FreetypeFontProvider( void )
{
	for( Glyphs::iterator i = m_glyphs.begin(), n = m_glyphs.end(); i != n; ++i ) {
		for( FontTextures::iterator j = i->second.begin(); j != i->second.end(); j++ ) {
			delete j->second;
		}
	}


	if( m_atlas ) {
		m_atlas->Clear();
	}

    FT_Done_FreeType( m_freetype );
}

// ** FreetypeFontProvider::CreateFont
IFont* FreetypeFontProvider::CreateFont( dcStream stream )
{
    FT_Face      face = NULL;
    FT_Open_Args args;

    if( !CreateStream( stream, &args ) ) {
        return NULL;
    }

    FT_Error error = FT_Open_Face( m_freetype, &args, 0, &face );
    if( error ) {
        DC_BREAK;
        return NULL;
    }
    
    return DC_NEW FreetypeFont( this, face );
}

// ** FreetypeFontProvider::GetTextureForGlyph
const sFontGlyph* FreetypeFontProvider::GetTextureForGlyph( IFont *font, u16 code, int size )
{
	if( m_glyphs.count( font ) == 0 ) {
		m_glyphs[font] = FontTextures();
	}

	FontTextures& textures = m_glyphs[font];

	int key = (size << 16) | code;
	FontTextures::const_iterator i = textures.find( key );
	if( i != textures.end() ) {
		return i->second;
	}

	sFreetypeBitmap bitmap;
	if( !static_cast<FreetypeFont*>( font )->RenderBitmap( bitmap, code, size ) ) {
		return NULL;
	}

    sFontGlyph *glyph = PutGlyphToTexture( bitmap );
    if( !glyph ) {
        return NULL;
    }

    textures[key] = glyph;
    return glyph;
}

// ** FreetypeFontProvider::PutGlyphToTexture
sFontGlyph* FreetypeFontProvider::PutGlyphToTexture( const sFreetypeBitmap& bitmap )
{
    if( bitmap.m_image == NULL ) {
        return NULL;
    }

	sFontGlyph *glyph = DC_NEW sFontGlyph;
	glyph->m_width    = bitmap.m_width;
	glyph->m_height   = bitmap.m_height;
	glyph->m_advance  = bitmap.m_advance;
	glyph->m_offset	  = glyph->m_height - bitmap.m_offset;

#ifdef DC_PLATFORM_FLASH
/*
	unsigned char *data     = new unsigned char[glyph->m_width * glyph->m_height * 4];
    int           textureId = ( int )character;
    int           width     = glyph->m_width;
    int           height    = glyph->m_height;
    
    character->m_texture = ( dcTextureAtlasAsset )textureId;
    character->m_uv      = cVector4( 0, 0, 1, 1 );
    
    for( int j = 0; j < character->m_height; j++ ) {
        for( int i = 0; i < character->m_width; i++ ) {
            byte color = character->m_bitmap[j*width + i];

            data[j*width*4 + i*4 + 0] = 255;
            data[j*width*4 + i*4 + 1] = 255;
            data[j*width*4 + i*4 + 2] = 255;
            data[j*width*4 + i*4 + 3] = color;
        }
    }

	static_cast<cFlashStageRenderer*>( m_ctx->iStageRenderer )->CreateBitmap( textureId, character->m_width, character->m_height, data, true );
    
	delete[]data;
*/
#else
	if( !m_atlas ) {
		m_atlas = ( dcTextureAtlasAsset )m_ctx->iAssetManager->CreateAssetInstance( asset::AssetTextureAtlas, "glyps" );
		m_atlas->Create( 1024, 1024, renderer::PixelAlpha8 );
	}

	bool result = m_atlas->AddTexture( bitmap.m_image.Get(), glyph->m_uv );
	DC_BREAK_IF( result == false );
	m_atlas->Upload();

    glyph->m_atlas = m_atlas;
#endif

    return glyph;
}

// ** FreetypeFontProvider::CreateStream
bool FreetypeFontProvider::CreateStream( dcStream stream, FT_Open_Args *args ) const
{
    DC_BREAK_IF( stream == NULL );
    
    memset( args, 0, sizeof( FT_Open_Args ) );

    FT_Stream fontStream = ( FT_Stream )malloc( sizeof( FT_StreamRec_ ) );

    fontStream->size                = stream->length();
    fontStream->pos                 = 0;
    fontStream->base                = NULL;
    fontStream->descriptor.pointer  = stream;
    fontStream->read                = &FreetypeFontProvider::StreamIOCallback;
    fontStream->close               = &FreetypeFontProvider::StreamCloseCallback;

    args->flags  = FT_OPEN_STREAM;
    args->stream = fontStream;

    return true;
}

// ** FreetypeFontProvider::StreamIOCallback
unsigned long FreetypeFontProvider::StreamIOCallback( FT_Stream stream, unsigned long offset, unsigned char *buffer, unsigned long count )
{
    if( !buffer ) {
        return 0;
    }

    dcStream file = ( dcStream )stream->descriptor.pointer;

    file->setPosition( offset );
    return file->read( buffer, count );
}

// ** FreetypeFontProvider::StreamCloseCallback
void FreetypeFontProvider::StreamCloseCallback( FT_Stream stream )
{
    dcStream file = ( dcStream )stream->descriptor.pointer;
    file->release();
    
    free( stream );
}

} // namespace Font

DC_END_DREEMCHEST