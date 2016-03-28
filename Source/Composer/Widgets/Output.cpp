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

#include "Output.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** Output::Output
Output::Output( QWidget* parent )
    : QTextEdit( parent )
{
    QFont font( "Monospace" );
    font.setStyleHint( QFont::TypeWriter );

    Logger::setWriter( new Writer( this ) );
    setReadOnly( true );
    setLineWrapMode( QTextEdit::NoWrap );
    setFont( font );
}

// ** Output::Writer::write
void Output::Writer::write( Logger::Level level, const String& text ) const
{
    QString line = QString::fromStdString( text );

    if( line.endsWith( "\n" ) ) {
        line.remove( line.size() - 1, 1 );
    }

    QString color = "Black";

    switch( level ) {
    case Logger::Debug:     color = "Silver";   break;
    case Logger::Verbose:   color = "Black";    break;
    case Logger::Warning:   color = "Orange";   break;
    case Logger::Error:     color = "Red";      break;
    }

    output->insertHtml( "<font color='" + color + "'>" + line + "</font><br>" );
}

} // namespace Ui