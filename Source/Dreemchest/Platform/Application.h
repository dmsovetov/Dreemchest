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

#include "Platform.h"
#include "Arguments.h"

DC_BEGIN_DREEMCHEST

namespace Platform {

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

        //! Notifies that application is about to start.
        virtual void            handlePrepareToLaunch( Application* application ) {}

        //! Notifies about an application start.
        virtual void            handleLaunched( Application* application ) {}

        //! Notifies about an application update.
        virtual void            handleUpdate( Application* application ) {}
    };

    //! Base class for windowed applications.
    class WindowedApplicationDelegate : public ApplicationDelegate {
    public:

        //! Abstract method to be overridden in a subclass to setup application after window creation.
        virtual bool            initialize( void ) = 0;

    private:

        //! Handles the application start and creates the window.
        virtual void            handleLaunched( Application* application ) NIMBLE_OVERRIDE;

    protected:

        WindowPtr                m_window;    //!< Main application window.
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

        //! Notifies that application is about to start.
        void                    notifyPrepareToLaunch( void );

        //! Notifies an application about a launch.
        void                    notifyLaunched( void );

        //! Notifies an application about an update.
        void                    notifyUpdate( void );

        //! Returns application arguments passed on launch.
        const Arguments&        args( void ) const;

        //! Creates a new Application instance.
        static Application*     create( const Arguments& args );

        //! Returns a shared application instance.
        static Application*     sharedInstance( void );

    protected:

                                //! Constructs a new Application instance.
                                Application( const Arguments& args, IApplication* impl );

    private:

        static Application*     s_application;    //!< Shared application instance.
        IApplication*           m_impl;            //!< Platform specific application implementation.
        ApplicationDelegate*    m_delegate;        //!< Application delegate instance.
        Arguments                m_arguments;    //!< Passed arguments.
    };

    //! Service application
    class ServiceApplication : public Application {
    public:

        //! Creates a new Service instance.
        static ServiceApplication*  create( const Arguments& args );

    private:

                                    //! Constructs a new ServiceApplication instance.
                                    ServiceApplication( const Arguments& args, IApplication* impl );
    };

} // namespace Platform

DC_END_DREEMCHEST

//! Declares an application entry point
#define dcDeclareApplication( delegate )                                                            \
    s32 main( s32 argc, s8** argv ) {                                                                \
        DC_DREEMCHEST_NS Platform::Arguments args( argv, argc );                                    \
        return DC_DREEMCHEST_NS Platform::Application::create( args )->launch( delegate );            \
    }

//! Declares a service entry point
#define dcDeclareServiceApplication( delegate )                                                     \
    s32 main( s32 argc, s8** argv ) {                                                                \
        DC_DREEMCHEST_NS Platform::Arguments args( argv, argc );                                    \
        return DC_DREEMCHEST_NS Platform::ServiceApplication::create( args )->launch( delegate );    \
    }

#endif /*   !defined( __DC_Platform_Application_H__ )   */
