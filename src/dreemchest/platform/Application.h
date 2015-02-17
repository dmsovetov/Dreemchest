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

    //! A platform-specific application implementation interface.
    class IApplication {
    public:

        virtual                 ~IApplication( void ) {}

        //! Quits application with an exit code.
        virtual void            quit( u32 exitCode ) = 0;

        //! Launches application with a callback.
        virtual int             launch( Application* application )  = 0;
    };

    //! ApplicationDelegate is used to handle events raised by application.
    class ApplicationDelegate {
    public:

        virtual                 ~ApplicationDelegate( void ) {}

        //! Notifies about an application start.
        virtual void            handleLaunched( Application* application ) {}
    };

    //! Application class is an entry point for applications.
    class Application {
    public:

        virtual                 ~Application( void );

        //! Quits application with an exit code.
        /*!
         \param exitCode Application exit code.
         */
        void                    quit( u32 exitCode = 0 );

        //! Launches application with a callback.
        /*!
         \param delegate Application event delegate.
         */
        int                     launch( ApplicationDelegate* delegate );

        //! Notifies an application about a lunch.
        void                    notifyLaunched( void );

        //! Creates a new Application instance.
        static Application*     create( void );

        //! Returns a shared application instance.
        static Application*     sharedInstance( void );

    private:

                                //! Constructs a new Application instance.
                                Application( IApplication* impl );

    private:

        //! Shared application instance.
        static Application*     s_application;

        //! Platform specific application implementation.
        IApplication*           m_impl;

        //! Application delegate instance.
        ApplicationDelegate*    m_delegate;
    };

} // namespace platform

DC_END_DREEMCHEST

#define dcDeclareApplication( delegate )                                        \
    int main( int argc, char** argv ) {                                         \
        return platform::Application::create()->launch( delegate );             \
    }

#endif /*   !defined( __DC_Platform_Application_H__ )   */
