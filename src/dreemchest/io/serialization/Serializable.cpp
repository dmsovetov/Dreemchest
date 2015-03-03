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

#include	"Serializable.h"

DC_BEGIN_DREEMCHEST

namespace io {

// ** Serializable::read
void Serializable::read( const StreamPtr& stream )
{
    Array<Field> items = fields();

    for( int i = 0; i < items.size(); i++ ) {
        const Field& field = items[i];

        if( !field.m_reader ) {
            log::warn( "Serializable::read : field '%s' has no reader set, do not know how to read this field\n", field.m_name );
            continue;
        }

        field.m_reader( stream, field.m_pointer );
    }
}

// ** Serializable::write
void Serializable::write( StreamPtr& stream ) const
{
    Array<Field> items = fields();

    for( int i = 0; i < items.size(); i++ ) {
        const Field& field = items[i];

        if( !field.m_writer ) {
            log::warn( "Serializable::write : field '%s' has no writer set, do not know how to write this field\n", field.m_name );
            continue;
        }

        field.m_writer( stream, field.m_pointer );
    }
}

// ** Serializable::fields
Array<Serializable::Field> Serializable::fields( void ) const
{
    log::warn( "Serializable::fields : not implemented in a subclass\n" );
    return Array<Field>();
}

} // namespace io

DC_END_DREEMCHEST