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

// Include the engine header file.
#include <Dreemchest.h>

// Open a root engine namespace
DC_USE_DREEMCHEST

// Open a platform namespace to use shorter types.
using namespace Platform;

// Open a renderer namespace.
using namespace Renderer;

// Open an Fx namespace.
using namespace Fx;

// This class will handle notifyUpdate and just clear the viewport
class WindowHandler : public WindowDelegate {
public:
    
    // Constructs the WindowHandler with a particle system instance.
    WindowHandler( HalWPtr hal, Scene::SceneWPtr scene, Scene::AssetBundleWPtr assets )
		: m_hal( hal ), m_scene( scene ), m_assets( assets )
	{
		using namespace Scene;
		m_renderingContext = RenderingContext::create( hal );
	}

    // Called each frame and renders a single frame
    virtual void handleUpdate( Window* window ) {
		Rgb clearColor = Rgb::fromHashString( "#314D79" );

        // First clear a viewport with a color
        m_hal->clear( Rgba( clearColor.r, clearColor.g, clearColor.b ) );

		Threads::Thread::sleep( 30 );

		m_scene->update( 0, 0.03f );
		m_scene->render( m_renderingContext );

        // And now just present all rendered data to the screen
        m_hal->present();
    }

    // The previously created HAL instance.
    HalWPtr	m_hal;

	//! Loaded scene instance.
	Scene::SceneWPtr			m_scene;

	//! Asset bundle to use.
	Scene::AssetBundleWPtr		m_assets;

	//! Scene rendering context.
	Scene::RenderingContextPtr	m_renderingContext;
};

// Application delegate is used to handle an events raised by application instance.
class ParticleSystems : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
        // Create a 800x600 window like we did in previous example.
        // This window will contain a rendering viewport.
        Window* window = Window::create( 800, 600 );

        // Create a rendering view.
        RenderView* view   = Hal::createOpenGLView( window->handle() );

        // Now create the main renderer interface called HAL (hardware abstraction layer).
        m_hal = Hal::create( OpenGL, view );

		Fx::log::setStandardHandler();

		// Create the particle system
		m_assets = Scene::AssetBundle::createFromFile( "particles", "assets", "assets/assets.json" );
		m_assets->setUuidFileNames( false );

		m_scene = Scene::Scene::createFromFile( m_assets, "assets/platform_smoke.prefab" );
		m_scene->addSystem<Scene::AssetSystem>( m_assets );

		Scene::Camera2DPtr camera = Scene::Camera2D::create( Scene::WindowTarget::create( window ) );
		camera->attach<Scene::RenderParticles>();
		camera->attach<Scene::RenderGrid>();
		camera->get<Scene::Camera>()->setClearColor( Rgba( 0.5f, 0.5f, 0.5f ) );
		m_scene->addSceneObject( camera );

		Scene::SceneObjectPtr grid = m_scene->createSceneObject();
		grid->attach<Scene::Grid>( 1000.0f, 10.0f );
		grid->attach<Scene::Transform>()->rotate( 90.0f, 1.0f, 0.0f, 0.0f );
		m_scene->addSceneObject( grid );
		
		Renderer::Renderer2DPtr renderer = Renderer::Renderer2D::create( m_hal, 4096 );

        // Finally set the window delegate to handle updates.
        window->setDelegate( new WindowHandler( m_hal, m_scene, m_assets ) );
    }

	Renderer::HalPtr		m_hal;
	Scene::AssetBundlePtr	m_assets;
	Scene::ScenePtr			m_scene;
};

// Now declare an application entry point with Particles application delegate.
dcDeclareApplication( new ParticleSystems )