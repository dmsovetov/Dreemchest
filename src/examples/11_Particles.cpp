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

		RvmPtr rvm( DC_NEW Rvm( hal, 1024 ) );
		ShaderCachePtr shaders( DC_NEW ShaderCache( "D:\\BFG9000\\externals\\src\\dreemchest\\src\\dreemchest\\scene\\Rendering\\Shaders", m_hal ) );
		Renderer2DPtr renderer2d = Renderer2D::create( hal, 1024 );

		m_renderingContext = RenderingContextPtr( DC_NEW RenderingContext( rvm, shaders, hal, renderer2d ) );
	}

    // Called each frame and renders a single frame
    virtual void handleUpdate( Window* window ) {
		Rgb clearColor = Rgb::fromHashString( "#314D79" );

        // First clear a viewport with a color
        m_hal->clear( Rgba( clearColor.r, clearColor.g, clearColor.b ) );

		thread::Thread::sleep( 30 );

		m_scene->update( 0, 0.03f );
		m_scene->render( m_renderingContext );

		{
			Renderer::Renderer2DPtr renderer = m_renderingContext->renderer();
			Rgba color = Rgba( 0.35f, 0.35f, 0.35f );

			renderer->begin( Matrix4::ortho( 0, 800, 0, 600, -1, 1 ) );
			for( s32 i = 1; i < 10; i++ ) {
				renderer->line( i * 100, 0, i * 100, 800, color, color );
			}
			for( s32 i = 1; i < 10; i++ ) {
				renderer->line( 0, i * 100, 800, i * 100, color, color );
			}

		//	StrongPtr<Scene::AssetTexture> tex = m_assets->find<Scene::Image>( "images/smoke" )->data();
		//	renderer->orientedQuad( tex.valid() ? tex->texture : Renderer::Texture2DPtr(), 100, 100, 100, 100, Vec2( 0, 1 ), Vec2( 1, 0 ) );
			renderer->end();
		}


        // And now just present all rendered data to the screen
        m_hal->present();
    }

	virtual void handleMouseMove( Window* window, u32 sx, u32 sy, u32 ex, u32 ey, int touchId = -1 )
	{
	//	m_particleSystem->setPosition( Vec3( 400, 300, 0 ) );
	//	m_particleSystem->setPosition( Vec3( ex, 600 - ey, 0.0f ) );
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

		// Create the particle system
		m_assets = Scene::AssetBundle::createFromFile( "particles", "assets", "assets/assets.json" );
		m_scene  = Scene::Scene::createFromFile( m_assets, "assets/7d0a92f3e153365498704e1b5a277ec6" );

		m_scene->addSystem<Scene::AssetSystem>( m_hal );
		m_scene->addSystem<Scene::AffineTransformSystem>();
		m_scene->addSystem<Scene::ParticlesSystem>();
		
		m_scene->addRenderingSystem<Scene::SinglePassRenderingSystem<Scene::RenderParticles, Scene::ParticleSystemsPass>>();
		m_scene->addRenderingSystem<Scene::BoundingVolumesRenderer>();

		Scene::SceneObjectPtr camera = m_scene->createSceneObject();
		camera->attach<Scene::Camera>( Scene::Camera::Ortho, Scene::WindowTarget::create( window ), Rgb::fromHashString( "#484848" ) );
		camera->attach<Scene::RenderParticles>();
		camera->attach<Scene::Transform>();
		//camera->attach<Scene::RenderBoundingVolumes>();

		Scene::SceneObjectSet objects = m_scene->findAllWithName( "smoke" );
		(*objects.begin())->get<Scene::Transform>()->setPosition( Vec3( 100, 100, 0 ) );

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