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

#ifndef __DC_OpenAL_H__
#define __DC_OpenAL_H__

#include    "../SoundEngine.h"

#if defined( DC_PLATFORM_OSX ) || defined( DC_PLATFORM_IOS )
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#elif defined( DC_PLATFORM_ANDROID ) || defined( DC_PLATFORM_WINDOWS )
    extern "C" {
        #include <openal/include/AL/al.h>
        #include <openal/include/AL/alc.h>
    }
#endif

DC_BEGIN_DREEMCHEST

namespace sound {

    // ** class OpenAL
    class OpenAL : public SoundEngine {
    public:

                                OpenAL( void );
        virtual                 ~OpenAL( void );

        // ** SoundEngine
        virtual SoundSource*    createSource( void );
        virtual SoundBuffer*    createBuffer( SoundDecoder* decoder, u32 chunks );

        // ** OpenAL
        static ALuint           soundSampleFormat( SoundSampleFormat format );
        static void             dumpErrors( const char *label );
        static void             flushErrors( void );
        static ALuint           maxSources( void );

    private:

        ALCdevice*              m_device;
        ALCcontext*             m_context;
    };
    
} // namespace sound
    
DC_END_DREEMCHEST

#endif        /*    __DC_OpenALSound_H__    */