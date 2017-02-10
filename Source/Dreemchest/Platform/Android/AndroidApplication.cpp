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

#include "AndroidApplication.h"
#include <unistd.h>

DC_BEGIN_DREEMCHEST

namespace Platform
{

// ** createApplication
IApplication* createApplication(void* userData)
{
    return DC_NEW AndroidApplication(reinterpret_cast<android_app*>(userData));
}
    
// ** createServiceApplication
IApplication* createServiceApplication()
{
    return NULL;
}

// ** currentTime
u64 currentTime( void )
{
    timeval time;
    gettimeofday(&time, NULL);
    return static_cast<u64>(((time.tv_sec) * 1000 + time.tv_usec/1000.0) + 0.5);
}

// ------------------------------------------------------------------------ AndroidApplication ------------------------------------------------------------------------ //

// ** AndroidApplication::AndroidApplication
AndroidApplication::AndroidApplication(android_app* state)
    : m_state(state)
{
    // Suppress link-time optimization that removes unreferenced code
    // to make sure glue isn't stripped.
    app_dummy();
    
    // Setup application state
    m_state->userData = this;
    m_state->onAppCmd = handleCommand;
    m_state->onInputEvent = handleInput;
}
    
// ** AndroidApplication::quit
void AndroidApplication::quit( u32 exitCode )
{
    exit( exitCode );
}

// ** AndroidApplication::launch
s32 AndroidApplication::launch( Application* application )
{
    // Save the application instance
    m_application = application;

	// Notify application about a launch.
    application->notifyLaunched();

	// Start an event loop
	loop();

	return 0;
}

// **  AndroidApplication::createDisplay
bool AndroidApplication::createDisplay()
{
    // initialize OpenGL ES and EGL
    
    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    /*const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;
    
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    
    eglInitialize(display, 0, 0);
    
    // Here, the application chooses the configuration it desires.
    // find the best match if possible, otherwise use the very first one
    //
    eglChooseConfig(display, attribs, nullptr,0, &numConfigs);
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    assert(supportedConfigs);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);
    assert(numConfigs);
    auto i = 0;
    for (; i < numConfigs; i++) {
        auto& cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)   &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)  &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 && d == 0 ) {
            
            config = supportedConfigs[i];
            break;
        }
    }
    if (i == numConfigs) {
        config = supportedConfigs[0];
    }
    
    // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
    // guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
    // As soon as we picked a EGLConfig, we can safely reconfigure the
    // ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID.
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);
    
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }
    
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
    
    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->state.angle = 0;
    
    // Check openGL on the system
    auto opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};
    for (auto name : opengl_info) {
        auto info = glGetString(name);
        LOGI("OpenGL Info: %s", info);
    }
    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);*/
    
    return true;
}
    
    /*static void engine_draw_frame(struct engine* engine) {
        if (engine->display == NULL) {
            // No display.
            return;
        }
        
        // Just fill the screen with a color.
        glClearColor(((float)engine->state.x)/engine->width, engine->state.angle,
                     ((float)engine->state.y)/engine->height, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        eglSwapBuffers(engine->display, engine->surface);
    }*/

// ** AndroidApplication::destroyDisplay
void AndroidApplication::destroyDisplay()
{
    /*if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;*/
}

// ** AndroidApplication::loop
void AndroidApplication::loop()
{
    while (1)
    {
        // Read all pending events.
        s32 events;
        android_poll_source* source;
        s32 ident = ALooper_pollAll(/*engine.animating ? 0 : -1*/0, NULL, &events, (void**)&source);
        
        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while (ident >= 0)
        {
            // Process this event.
            if (source != NULL)
            {
                source->process(m_state, source);
            }
            
            // If a sensor has data, process it now.
            //if (ident == LOOPER_ID_USER)
            //{
            //    if (engine.accelerometerSensor != NULL) {
            //        ASensorEvent event;
            //        while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
            //                                           &event, 1) > 0) {
            //            LOGI("accelerometer: x=%f y=%f z=%f",
            //                 event.acceleration.x, event.acceleration.y,
            //                 event.acceleration.z);
            //        }
            //    }
            //}
            
            // Check if we are exiting.
            if (m_state->destroyRequested != 0)
            {
            //    engine_term_display(&engine);
                return;
            }
        }
        
        //if (engine.animating)
        //{
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
        //    engine_draw_frame(&engine);
        //}
    }
}

// ** AndroidApplication::resourcePath
const String& AndroidApplication::resourcePath( void ) const
{
    if (m_workingDirectory.empty())
    {
        s8 buffer[256];
        getcwd(buffer, sizeof(buffer));
        m_workingDirectory = buffer;
        m_workingDirectory += "/";
    }
    
    return m_workingDirectory;
}
    
// ** AndroidApplication::handleCommand
void AndroidApplication::handleCommand(android_app* android, int32_t cmd)
{
    AndroidApplication* app = reinterpret_cast<AndroidApplication*>(android->userData);
    
    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            //android->savedState = malloc(sizeof(struct saved_state));
            //*((struct saved_state*)android->savedState) = engine->state;
            //android->savedStateSize = sizeof(struct saved_state);
            break;
            
        case APP_CMD_INIT_WINDOW:
            if (android->window != NULL)
            {
            //    engine_init_display(engine);
            //    engine_draw_frame(engine);
            }
            break;
            
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            //engine_term_display(engine);
            break;
            
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            //if (engine->accelerometerSensor != NULL) {
            //    ASensorEventQueue_enableSensor(engine->sensorEventQueue,
            //                                   engine->accelerometerSensor);
            //    // We'd like to get 60 events per second (in us).
            //    ASensorEventQueue_setEventRate(engine->sensorEventQueue,
            //                                   engine->accelerometerSensor,
            //                                   (1000L/60)*1000);
            //}
            break;
            
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            //if (engine->accelerometerSensor != NULL) {
            //    ASensorEventQueue_disableSensor(engine->sensorEventQueue,
            //                                    engine->accelerometerSensor);
            //}
            // Also stop animating.
            //engine->animating = 0;
            //engine_draw_frame(engine);
            break;
    }
}
    
// ** AndroidApplication::handleInput
int32_t AndroidApplication::handleInput(android_app* android, AInputEvent* event)
{
    AndroidApplication* app = reinterpret_cast<AndroidApplication*>(android->userData);
    
    switch (AInputEvent_getType(event))
    {
        case AINPUT_EVENT_TYPE_MOTION:
            //AMotionEvent_getX(event, 0);
            //AMotionEvent_getY(event, 0);
            return 1;
    }
    
    return 0;
}

} // namespace Platform

DC_END_DREEMCHEST
