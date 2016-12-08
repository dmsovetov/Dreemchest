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

#include "RenderingFrame.h"
#include "Menu.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** RenderingFrame::kOpenGLFormat;
QGLFormat RenderingFrame::kOpenGLFormat;

// ** RenderingFrame::RenderingFrame
RenderingFrame::RenderingFrame( const QGLWidget* sharedWidget, QWidget* parent )
    : QGLWidget( kOpenGLFormat, parent, sharedWidget ), m_timer( -1 )/*, m_hasLostFocus( false )*/
{
    setAcceptDrops( true );
    setMouseTracking( true );
    setMinimumSize( 1, 1 );
}

// ** RenderingFrame::SetupOpenGLFormat
void RenderingFrame::setupOpenGLFormat( void )
{
    // Construct the custom OpenGL format
    kOpenGLFormat.setSwapInterval( 1 );
    kOpenGLFormat.setSamples( 8 );
    
    // Construct the test OpenGL widget
    AutoPtr<QGLWidget> widget = new QGLWidget( kOpenGLFormat );

    // The widget was successfully initialized - use this format
    if( widget->isValid() ) {
        return;
    }

    // Fallback to default one
    kOpenGLFormat = QGLFormat::defaultFormat();
    kOpenGLFormat.setDirectRendering( false );
}

// ** RenderingFrame::setInterval
void RenderingFrame::setInterval( s32 value )
{
    if( m_timer != -1 ) {
        killTimer( m_timer );
    }

    m_timer = startTimer( value );
    m_deltaTime.start();
}

// ** RenderingFrame::timerEvent
void RenderingFrame::timerEvent( QTimerEvent *event )
{
    bool shouldUpdate = isContinuousRendering() || hasFocus();

    if( isVisible() && shouldUpdate ) {
        QGLWidget::update();
    }
}

// ** RenderingFrame::paintGL
void RenderingFrame::paintGL( void )
{
    // Get the delta time
    f32 dt = m_deltaTime.elapsed() * 0.001f;
    m_deltaTime.restart();

    // Emit the signal
    emit update( dt );
}

// ** RenderingFrame::setCursor
void RenderingFrame::setCursor( const String& cursor, bool centered )
{
    if( cursor == "" ) {
        QGLWidget::setCursor( QCursor( Qt::ArrowCursor ) );
        return;
    }

    QPixmap pixmap( cursor.c_str() );

    if( !centered ) {
        s32       width    = pixmap.width();
        s32       height    = pixmap.height();
        QImage srcImage    = pixmap.toImage();
        QImage dstImage    = QImage( width * 2, height * 2, QImage::Format_ARGB32 );

        dstImage.fill( 0 );
        for( int j = 0; j < height; j++ ) {
            for( int i = 0; i < width; i++ ) {
                dstImage.setPixel( width + i, height + j, srcImage.pixel( i, j ) );
            }
        }

        pixmap = QPixmap::fromImage( dstImage );
    }

    QGLWidget::setCursor( QCursor( pixmap ) );
}

// ** RenderingFrame::setContinuousRendering
void RenderingFrame::setContinuousRendering( bool value )
{
    m_isContinuousRendering = value;
}

// ** RenderingFrame::isContinuousRendering
bool RenderingFrame::isContinuousRendering( void ) const
{
    return m_isContinuousRendering;
}

} // namespace Ui

DC_END_COMPOSER
