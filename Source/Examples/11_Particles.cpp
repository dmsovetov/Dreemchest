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

// Declare the log tag in global namespace
DREEMCHEST_LOGGER_TAG( Particles )

// Application delegate is used to handle an events raised by application instance.
class ParticleSystems : public Platform::ApplicationDelegate {
public:

	ParticleSystems( void )
		: m_timeStep( 16 ) {}

    // This method will be called once an application is launched.
    virtual void handleLaunched( Platform::Application* application ) {
        // Set the default log handler.
        Logger::setStandardLogger();

        // Create a 800x600 window like we did in previous example.
        // This window will contain a rendering viewport.
        Platform::Window* window = Platform::Window::create( 800, 600 );

        // Create a rendering view.
        Renderer::RenderView* view   = Renderer::Hal::createOpenGLView( window->handle() );

        // Now create the main renderer interface called HAL (hardware abstraction layer).
        m_hal = Renderer::Hal::create( Renderer::OpenGL, view );

		//m_renderingContext = Scene::RenderingContext::create( m_hal );

		// Create the particle system
		//m_assets = Scene::AssetBundle::createFromFile( "particles", "assets", "assets/assets.json" );
		//m_assets->setUuidFileNames( false );

		//m_scene = Scene::Scene::createFromFile( m_assets, "assets/smoke_missile.prefab" );
		//m_scene->addSystem<Scene::AssetSystem>( m_assets );

		m_scene->findAllWithName( "smoke_missile" ).begin()->get()->get<Scene::Transform>()->setPosition( Vec3( -1000, 0, 0 ) );
		m_scene->findAllWithName( "smoke_missile" ).begin()->get()->attach<Scene::MoveAlongAxes>( 1000.0f, Scene::CSWorld, new Scene::Vec3Binding( Vec3( 1.0f, 0.0f, 0.0f ) ) );

    #if 0
		Scene::Camera2DPtr camera = Scene::Camera2D::create( Scene::WindowTarget::create( window ) );
		camera->attach<Scene::RenderParticles>();
		camera->attach<Scene::RenderGrid>();
		camera->attach<Scene::RenderBoundingVolumes>();
		camera->setZoom( 0.2f );
		camera->get<Scene::Camera>()->setClearColor( Rgba( 0.5f, 0.5f, 0.5f ) );
		m_scene->addSceneObject( camera );
    #else
        NIMBLE_NOT_IMPLEMENTED
    #endif

		Scene::SceneObjectPtr grid = m_scene->createSceneObject();
		grid->attach<Scene::Grid>( 1000.0f, 10.0f );
		grid->attach<Scene::Transform>()->rotate( 90.0f, 1.0f, 0.0f, 0.0f );
		m_scene->addSceneObject( grid );
		
		Renderer::Renderer2DPtr renderer = Renderer::Renderer2D::create( m_hal, 4096 );

		// Finally subscribe for window events
		window->subscribe<Platform::Window::Update>( dcThisMethod( ParticleSystems::update ) );
    }

	void update( const Platform::Window::Update& e )
	{
		Rgb clearColor = Rgb::fromHashString( "#314D79" );

        // First clear a viewport with a color
        m_hal->clear( Rgba( clearColor.r, clearColor.g, clearColor.b ) );

	//	for( s32 i = 0, n = m_timeStep.stepCount(); i < n; i++ ) {
	//		m_scene->update( 0, m_timeStep.seconds() );
	//	}
        NIMBLE_NOT_IMPLEMENTED;

	//	m_scene->render( m_renderingContext );

        // And now just present all rendered data to the screen
        m_hal->present();	
	}

	Renderer::HalPtr		m_hal;
	//Scene::AssetBundlePtr	m_assets;
	Scene::ScenePtr			m_scene;
	Platform::FixedTimeStep	m_timeStep;
	//! Scene rendering context.
	Scene::RenderingContextPtr	m_renderingContext;
};

// Now declare an application entry point with Particles application delegate.
dcDeclareApplication( new ParticleSystems )
