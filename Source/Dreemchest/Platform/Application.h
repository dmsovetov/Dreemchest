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
#include "Window.h"

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
        
        //! Returns a resource path.
        virtual const String&   resourcePath( void ) const = 0;
    };

    //! ApplicationDelegate is used to handle events raised by application.
    class ApplicationDelegate {
    public:

                                ApplicationDelegate(const String& tag = "APP");
        virtual                 ~ApplicationDelegate( void ) {}

        //! Notifies that application is about to start.
        virtual void            handlePrepareToLaunch( Application* application ) {}

        //! Notifies about an application start.
        virtual void            handleLaunched( Application* application ) {}

        //! Notifies about an application update.
        virtual void            handleUpdate( Application* application ) {}
        
    protected:
        
        //! Outputs a debug message to a console.
        void                    debug(CString prefix, CString format, ...);
        
        //! Outputs a verbose message to a console.
        void                    verbose(CString prefix, CString format, ...);
        
        //! Outputs a warning message to a console.
        void                    warning(CString prefix, CString format, ...);
        
        //! Outputs an error message to a console.
        void                    error(CString prefix, CString format, ...);
        
        //! Outputs a fatal message to a console.
        void                    fatal(CString prefix, CString format, ...);
        
    private:
        
        String                  m_loggerTag;    //!< A tag string that is passed to a logger.
    };

    //! Base class for windowed applications.
    class WindowedApplicationDelegate : public ApplicationDelegate
    {
    protected:

        //! Creates and initializes a window instance.
        virtual bool            initialize(s32 width, s32 height);
        
        //! Sets a window caption.
        void                    setCaption(const String& value);
        
        // This method is called when mouse/touch is pressed.
        virtual void            handleTouchBegan(const Window::TouchBegan& e) {}
        
        // This method is called when mouse/touch is released.
        virtual void            handleTouchEnded(const Window::TouchEnded& e) {}
        
        // This method is called when mouse/touch is moved.
        virtual void            handleTouchMoved(const Window::TouchMoved& e) {}
        
        // This method is called when key is pressed.
        virtual void            handleKeyPressed(const Window::KeyPressed& e) {}
        
        // This method is called when key is released.
        virtual void            handleKeyReleased(const Window::KeyReleased& e) {}
        
        // This method is called each frame
        virtual void            handleWindowUpdate(const Window::Update& e) {}

    protected:

        WindowPtr               m_window;    //!< Main application window.
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
        
        //! Returns a current working directory.
        const String&           currentDirectory( void ) const;
        
        //! Returns a resource path.
        const String&           resourcePath( void ) const;
        
        //! Returns a full path for a resource.
        String                  pathForResource(const String& fileName) const;

        //! Creates a new Application instance.
        static Application*     create( const Arguments& args );

        //! Returns a shared application instance.
        static Application*     sharedInstance( void );

    protected:

                                //! Constructs a new Application instance.
                                Application( const Arguments& args, IApplication* impl );

    private:

        static Application*     s_application;      //!< Shared application instance.
        IApplication*           m_impl;             //!< Platform specific application implementation.
        ApplicationDelegate*    m_delegate;         //!< Application delegate instance.
        mutable String          m_currentDirectory; //!< A current directory.
        Arguments               m_arguments;        //!< Passed arguments.
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

//! A private macro definition to construct application instance and parse arguments
#define _DREEMCHEST_ENTRY_POINT(AppClass, delegate, argc, argv)                                     \
        DC_DREEMCHEST_NS Platform::Arguments args( argv, argc );                                    \
        return DC_DREEMCHEST_NS Platform::AppClass::create( args )->launch( delegate );      

#ifdef DC_PLATFORM_WINDOWS
    //! Declares an application entry point
    #define dcDeclareApplication( delegate )                                                                    \
        s32 main( s32 argc, s8** argv ) { _DREEMCHEST_ENTRY_POINT(Application, delegate, argc, argv) }          \
        s32 WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) { _DREEMCHEST_ENTRY_POINT(Application, delegate, 1, ( s8** )&pCmdLine) }

    //! Declares a service entry point
    #define dcDeclareServiceApplication( delegate )                                                             \
        s32 main( s32 argc, s8** argv ) { _DREEMCHEST_ENTRY_POINT(ServiceApplication, delegate, argc, argv) }   \
        s32 WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) { _DREEMCHEST_ENTRY_POINT(ServiceApplication, delegate, 1, ( s8** )&pCmdLine) }
#else
    //! Declares an application entry point
    #define dcDeclareApplication( delegate )            \
        s32 main( s32 argc, s8** argv ) { _DREEMCHEST_ENTRY_POINT(Application, delegate, argc, argv) }

    //! Declares a service entry point
    #define dcDeclareServiceApplication( delegate )     \
        s32 main( s32 argc, s8** argv ) { _DREEMCHEST_ENTRY_POINT(ServiceApplication, delegate, argc, argv) }
#endif  /*  #ifdef DC_PLATFORM_WINDOWS  */

#endif /*   !defined( __DC_Platform_Application_H__ )   */
