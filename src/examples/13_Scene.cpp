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

// Open the Scene namespace.
using namespace Scene;

// This class will handle notifyUpdate and just clear the viewport
class WindowHandler : public WindowDelegate {
public:
    
    // Constructs the WindowHandler with a particle system instance.
    WindowHandler( HalWPtr hal, SceneWPtr scene )
		: m_hal( hal ), m_scene( scene )
	{
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

        // And now just present all rendered data to the screen
        m_hal->present();
    }

    // The previously created HAL instance.
    HalWPtr	m_hal;

	//! Loaded scene instance.
	SceneWPtr			m_scene;

	//! Scene rendering context.
	RenderingContextPtr	m_renderingContext;
};

// Application delegate is used to handle an events raised by application instance.
class ParticleSystems : public ApplicationDelegate {

    // This method will be called once an application is launched.
    virtual void handleLaunched( Application* application ) {
		::Scene::log::setStandardHandler();

        // Create a 800x600 window like we did in previous example.
        // This window will contain a rendering viewport.
        Window* window = Window::create( 800, 600 );

        // Create a rendering view.
        RenderView* view   = Hal::createOpenGLView( window->handle() );

        // Now create the main renderer interface called HAL (hardware abstraction layer).
        m_hal = Hal::create( OpenGL, view );

		// Create the particle system
		m_scene = ::Scene::Scene::create();

		// Create an empty asset bundle
		m_assets = AssetBundle::create( "assets" );
		TerrainPtr terrain = m_assets->addTerrain( "terrain", "terrain", 128 );

/*		for( s32 i = 0; i < terrain->chunkCount(); i++ ) {
			for( s32 j = 0; j < terrain->chunkCount(); j++ ) {
				Array<Terrain::Vertex> vertices = terrain->chunkVertexBuffer( j, i );
				Array<u16>			   indices  = terrain->chunkIndexBuffer();
			}
		}*/

		m_scene->addSystem<AssetSystem>( m_hal );
		m_scene->addSystem<AffineTransformSystem>();
		m_scene->addSystem<ParticlesSystem>();
		
		m_scene->addRenderingSystem<SinglePassRenderingSystem<RenderParticles, ParticleSystemsPass>>();
		m_scene->addRenderingSystem<BoundingVolumesRenderer>();

		SceneObjectPtr camera = m_scene->createSceneObject();
		camera->attach<Camera>( Camera::Perspective, WindowTarget::create( window ), Rgb::fromHashString( "#484848" ) );
		camera->attach<RenderParticles>();
		camera->attach<::Scene::Transform>();
		camera->attach<RenderBoundingVolumes>();

		Renderer::Renderer2DPtr renderer = Renderer::Renderer2D::create( m_hal, 4096 );

        // Finally set the window delegate to handle updates.
        window->setDelegate( new WindowHandler( m_hal, m_scene ) );
    }

	Renderer::HalPtr	m_hal;
	ScenePtr			m_scene;
	AssetBundlePtr		m_assets;
};

// Now declare an application entry point with Particles application delegate.
dcDeclareApplication( new ParticleSystems )