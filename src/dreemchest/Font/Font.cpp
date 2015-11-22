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

#include "Font.h"

DC_BEGIN_DREEMCHEST

namespace Font {

// ** Font::Release
void Font::Release( void )
{
    DC_DELETE( m_impl );
    delete this;
}

// ** Font::GetAscender
int Font::GetAscender( int size ) const
{
    if( m_impl ) {
        return m_impl->GetAscender( size );
    }

    return 0;
}

// ** Font::GetDescender
int Font::GetDescender( int size ) const
{
    if( m_impl ) {
        return m_impl->GetDescender( size );
    }

    return 0;
}

// ** Font::GetLineGap
int Font::GetLineGap( int size ) const
{
    if( m_impl ) {
        return m_impl->GetLineGap( size );
    }

    return 0;
}

// ** Font::CalculateLineWidth
int Font::CalculateLineWidth( const char *text, int count, int size ) const
{
    if( m_impl ) {
        return m_impl->CalculateLineWidth( text, count, size );
    }

    return 0;
}

} // namespace Font
    
DC_END_DREEMCHEST