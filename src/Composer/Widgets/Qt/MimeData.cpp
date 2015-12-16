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

#include "MimeData.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** MimeData::MimeData
MimeData::MimeData( const QMimeData* mime ) : m_mime( mime )
{

}

// ** MimeData::hasFormat
bool MimeData::hasFormat( const String& value ) const
{
	return m_mime->hasFormat( value.c_str() );
}

// ** MimeData::strings
StringArray MimeData::strings( void ) const
{
	StringArray result;

	foreach( const QUrl& url, m_mime->urls() ) {
		result.push_back( url.toLocalFile().toStdString() );
	}

	return result;
}

} // namespace Ui

DC_END_COMPOSER