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

#include "Arguments.h"

DC_BEGIN_DREEMCHEST

namespace Platform {

// ** Arguments::Arguments
Arguments::Arguments( s8** argv, u32 argc )
{
    for( u32 i = 1; i < argc; i++ ) {
        CString arg   = argv[i];
        CString value = i + 1 < argc ? argv[i + 1] : "";
        u32        len   = strlen( arg );

        if( len <= 2 ) {
            continue;
        }

        if( arg[0] == '-' && arg[1] == '-' ) {
            m_values[arg + 2] = value;
        }
    }
}

// ** Arguments::integer
s32 Arguments::integer( const String& name, s32 defaultValue ) const
{
    if( !has( name ) ) {
        return defaultValue;
    }

    return atoi( m_values.find( name )->second.c_str() );
}

// ** Arguments::number
f32 Arguments::number( const String& name, f32 defaultValue ) const
{
    if( !has( name ) ) {
        return defaultValue;
    }

    return static_cast<f32>( atof( m_values.find( name )->second.c_str() ) );
}

// ** Arguments::string
const String& Arguments::string( const String& name, const String& defaultValue ) const
{
    if( !has( name ) ) {
        return defaultValue;
    }

    return m_values.find( name )->second;
}

// ** Arguments::has
bool Arguments::has( const String& name ) const
{
    return m_values.find( name ) != m_values.end();
}

} // namespace Platform

DC_END_DREEMCHEST
