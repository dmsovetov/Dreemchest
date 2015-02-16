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

#ifndef __DC_Platform_Application_H__
#define __DC_Platform_Application_H__

#include    "Platform.h"

DC_BEGIN_DREEMCHEST

namespace platform {

    typedef cClosure<void(Application*)>    ApplicationLaunchedCallback;

    // ** class IApplication
    //! A platform-specific application implementation interface.
    class IApplication {
    public:

        virtual             ~IApplication( void ) {}

        //! Quits application with an exit code.
        virtual void        quit( u32 exitCode ) = 0;

        //! Launches application with a callback.
        virtual void        launch( Application* application, ApplicationLaunchedCallback callback )  = 0;
    };

    // ** class Application
    class Application {
    public:

        virtual             ~Application( void );

        //! Quits application with an exit code.
        void                quit( u32 exitCode = 0 );

        //! Launches application with a callback.
        void                launch( ApplicationLaunchedCallback callback );

        //! Creates a new Application instance.
        static Application* create( void );

    private:

                            //! Constructs a new Application instance.
                            Application( IApplication* impl );

    private:

        IApplication*       m_impl;
    };

} // namespace platform

DC_END_DREEMCHEST

#endif /*   !defined( __DC_Platform_Application_H__ )   */
