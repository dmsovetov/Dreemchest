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

#ifndef __DC_Scene_H__
#define __DC_Scene_H__

#include "../Dreemchest.h"

#ifdef DC_CPP11_DISABLED
	#error C++11 should be enabled to use scene module.
#endif

#include <Platform/Platform.h>
#include <Platform/Input.h>
#include <Platform/Window.h>
#include <Platform/Time.h>

#include <Renderer/Renderer.h>
#include <Renderer/Hal.h>
#include <Renderer/Renderer2D.h>

#include <Assets/AssetHandle.h>
#include <Assets/Assets.h>
#include <Assets/AssetSource.h>

#include <Ecs/Entity/Entity.h>
#include <Ecs/Entity/Archetype.h>
#include <Ecs/Component/Component.h>
#include <Ecs/System/GenericEntitySystem.h>
#include <Ecs/System/ImmutableEntitySystem.h>
#include <Ecs/System/SystemGroup.h>

#include <Io/DiskFileSystem.h>
#include <Io/JsonLoader.h>

#include <Fx/ParticleSystem.h>
#include <Fx/Particles.h>
#include <Fx/Emitter.h>
#include <Fx/Renderers.h>
#include <Fx/Zones.h>
#include <Fx/Modules.h>

#include <Reflection/MetaObject/Property.h>
#include <Reflection/MetaObject/Class.h>
#include <Reflection/MetaObject/Enum.h>

#include "PlaneClipper.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    NIMBLE_LOGGER_TAG( Scene )

	class Scene;
	class RenderTarget;

	class Image;
	class Mesh;
	class Material;
	class Terrain;
    class Prefab;
    class Renderable;
    class Texture;

	class Transform;
	class StaticMesh;
	class Sprite;
	class Camera;

	class Rvm;

    //! Image handle type.
    typedef Assets::GenericHandle<class Image> ImageHandle;

    //! Mesh handle type.
    typedef Assets::GenericHandle<class Mesh> MeshHandle;

    //! Material handle type.
    typedef Assets::GenericHandle<class Material> MaterialHandle;

    //! Terrain handle type.
    typedef Assets::GenericHandle<class Terrain> TerrainHandle;

	//! Available rendering modes.
	enum RenderingMode {
		  RenderOpaque		//!< Renders opaque.
		, RenderCutout		//!< Renders object with an alpha test.
		, RenderTranslucent	//!< Renders object with alpha blending.
		, RenderAdditive	//!< Renders objects with additive blending.
		, TotalRenderModes	//!< Total number of render modes.
	};

	//! Rendering mode mask.
	enum RenderingModeBits {
		  RenderOpaqueBit		= BIT( RenderOpaque )		//!< Opaque rendering bit.
		, RenderCutoutBit		= BIT( RenderCutout )		//!< Cutout rendering bit.
		, RenderTranslucentBit	= BIT( RenderTranslucent )	//!< Translucent rendering bit.
		, RenderAdditiveBit		= BIT( RenderAdditive )		//!< Additive rendering bit.
		, AllRenderModesBit		= RenderOpaqueBit | RenderCutoutBit | RenderTranslucentBit | RenderAdditiveBit
	};

	//! 3 coordinate plane types.
	enum CoordinatePlane {
		  PlaneXY				//!< The XY coordinate plane.
		, PlaneYZ				//!< The YZ coordinate plane.
		, PlaneXZ				//!< The XZ coordinate plane.

		, TotalCoordinatePlanes	//!< The total number of coordinate planes.
	};

	//! Coordinate plane bit masks.
	enum CoordinatePlaneBit {
		  PlaneXYBit	= BIT( PlaneXY )	//!< The XY coordinate plane bit mask.
		, PlaneYZBit	= BIT( PlaneYZ )	//!< The YZ coordinate plane bit mask.
		, PlaneXZBit	= BIT( PlaneXZ )	//!< The XZ coordinate plane bit mask.
	};

	// Alias the Ecs::Entity type
	typedef StrongPtr<Ecs::Entity>	SceneObjectPtr;
	typedef WeakPtr<Ecs::Entity>	SceneObjectWPtr;
	typedef Ecs::EntityId			SceneObjectId;

	dcDeclarePtrs( Scene )
	dcDeclarePtrs( Viewport )
	dcDeclarePtrs( RenderTarget )
	dcDeclarePtrs( TextureTarget )
	dcDeclarePtrs( WindowTarget )
	dcDeclarePtrs( Transform )
	dcDeclarePtrs( Camera )
	dcDeclarePtrs( StaticMesh )
	dcDeclarePtrs( Light )
	dcDeclarePtrs( RenderPassBase )
	dcDeclarePtrs( ShaderCache )

	dcDeclarePtrs( Physics2D )
	dcDeclarePtrs( RigidBody2D )

	dcDeclarePtrs( RenderingContext )
	dcDeclarePtrs( Rvm )
	dcDeclarePtrs( RenderPassBase )
	dcDeclarePtrs( RenderingSystemBase )
	dcDeclarePtrs( Shader )
	dcDeclarePtrs( RopEmitterBase )

	dcDeclarePtrs( Vec3Binding )

	dcDeclarePtrs( SpectatorCamera )
	dcDeclarePtrs( Camera2D )

	//! Scene systems mask.
	enum Systems {
		  UpdateSystems = BIT( 0 )
		, RenderSystems = BIT( 1 )
	};

	//! Scene query mask flags.
	enum SceneQueryFlags {
		  QuerySingle		= BIT( 0 )	//!< Scene query will result a sinle best matching result.
		, QueryBackToFront	= BIT( 1 )	//!< Scene query will return results in back to front order. 
	};

	//! Container type to store a set of scene objects.
	typedef Set<SceneObjectPtr> SceneObjectSet;

	//! A helper struct to store results of a scene ray tracing.
	struct RayTracingResult {
		SceneObjectWPtr		sceneObject;	//!< The scene object hit.
		Vec3				point;			//!< The intersection world space point.
		f32					time;			//!< The distance value in range [0, 1] from ray origin to a hit point.

							//! Constructs RayTracingResult instance.
							RayTracingResult( SceneObjectWPtr sceneObject = SceneObjectWPtr(), const Vec3& point = Vec3( 0, 0, 0 ), f32 time = -1.0f )
								: sceneObject( sceneObject ), point( point ), time( time ) {}

							//! Returns true if this is a valid result (contains a hit scene object).
							operator bool( void ) const { return sceneObject.valid(); }

							//! Returns true if this ray tracing result has a smaller time value.
		bool				operator < ( const RayTracingResult& other ) const { return time < other.time; }
	};

	//! Array of ray tracing results.
	typedef Array<RayTracingResult> RayTracingResultArray;

	//! The root class for a scene subsystem.
	class Scene : public InjectEventEmitter<RefCounted> {
	public:

		//! Performs a scene update.
		void							update( u32 currentTime, f32 dt );

		//! Renders a scene.
		void							render( const RenderingContextPtr& context );

		//! Creates a new scene object instance.
		SceneObjectPtr					createSceneObject( void );

		//! Adds new scene object to scene.
		void							addSceneObject( const SceneObjectPtr& sceneObject );

		//! Removes scene object to scene.
		void							removeSceneObject( const SceneObjectPtr& sceneObject );

		//! Creates an archetype instance.
		template<typename TArchetype>
		StrongPtr<TArchetype>			createArchetype( const SceneObjectId& id, const KeyValue* data = NULL );

		//! Creates a new scene object instance.
		SceneObjectPtr					createSceneObject( const SceneObjectId& id );

		//! Returns the scene object with specified id.
		SceneObjectPtr					findSceneObject( const SceneObjectId& id ) const;

		//! Returns the list of scene object with specified name.
		SceneObjectSet					findAllWithName( const String& name ) const;

		//! Returns a list of scene objects that match a specified aspect.
		SceneObjectSet					findByAspect( const Ecs::Aspect& aspect ) const;

		//! Performs the ray tracing.
		RayTracingResultArray			queryRay( const Ray& ray, const FlagSet8& flags = QuerySingle ) const;

		//! Returns cameras that reside in scene.
		const Ecs::IndexPtr&			cameras( void ) const;

        //! Returns internal entity component system instance.
        Ecs::EcsWPtr                    ecs( void ) const;

		//! Returns a scene system of specified type.
		template<typename TSystem>
		WeakPtr<TSystem>				system( void ) const;

		//! Adds a new system to the scene.
		template<typename TSystem, typename ... Args>
		WeakPtr<TSystem>				addSystem( Args ... args );

		//! Adds a new rendering system to the scene.
		template<typename TRenderingSystem>
		void							addRenderingSystem( void );

		//! Creates an empty scene.
		static ScenePtr					create( void );

		//! Creates scene and loads it from JSON file.
		static ScenePtr					createFromFile( const Assets::Assets& assets, const String& fileName );

		//! Creates scene and loads it from JSON string.
		static ScenePtr					createFromJson( const Assets::Assets& assets, const String& json );

		//! This event is emitted when a new scene object was added.
		struct SceneObjectAdded {
										SceneObjectAdded( SceneObjectPtr sceneObject )
											: sceneObject( sceneObject ) {}
			SceneObjectPtr				sceneObject;	//!< The scene object that was added.
		};

		//! This event is emitted when the scene object was removed.
		struct SceneObjectRemoved {
										SceneObjectRemoved( SceneObjectPtr sceneObject )
											: sceneObject( sceneObject ) {}
			SceneObjectPtr				sceneObject;	//!< The scene object that was removed.		
		};

	private:

										//! Constructs a Scene instance.
										Scene( void );

	private:

		Ecs::EcsPtr						m_ecs;				//!< Internal entity component system.
		Ecs::SystemGroupPtr				m_updateSystems;	//!< Update systems group.
		Array<RenderingSystemBasePtr>	m_renderingSystems;	//!< Entity rendering systems.
		Ecs::IndexPtr					m_cameras;			//!< All cameras that reside in scene.
		Ecs::IndexPtr					m_named;			//!< All named entities that reside in scene stored inside this family.
		Ecs::IndexPtr					m_meshes;			//!< All static meshes that reside in scene.
	};

	// ** Scene::addSystem
	template<typename TSystem, typename ... Args>
	WeakPtr<TSystem> Scene::addSystem( Args ... args )
	{
		WeakPtr<TSystem> system = m_updateSystems->add<TSystem>( args... );
		m_ecs->rebuildIndices();
		return system;
	}

	// ** Scene::system
	template<typename TSystem>
	WeakPtr<TSystem> Scene::system( void ) const
	{
		return m_updateSystems->get<TSystem>();
	}

	// ** Scene::addRenderingSystem
	template<typename TRenderingSystem>
	void Scene::addRenderingSystem( void )
	{
		m_renderingSystems.push_back( DC_NEW TRenderingSystem( m_ecs ) );
		m_ecs->rebuildIndices();
	}

	// ** Scene::createArchetype
	template<typename TArchetype>
	StrongPtr<TArchetype> Scene::createArchetype( const SceneObjectId& id, const KeyValue* data )
	{
		return m_ecs->createArchetype<TArchetype>( id, data );
	}

#if DEV_DEPRECATED_SERIALIZATION

#ifdef HAVE_JSON

	//! Loads the scene from JSON file.
	class JsonSceneLoader : public Io::JsonLoaderBase {
	public:

									//! Constructs the JsonSceneLoader instance.
									JsonSceneLoader( const Assets::Assets& assets );

		//! Loads the scene from string.
		bool						load( ScenePtr scene, const String& json );

	private:

		//! Returns the scene object by it's id.
		Ecs::EntityPtr				requestSceneObject( const String& id );

		//! Returns the component by it's id.
		Ecs::ComponentPtr			requestComponent( const String& id );

		//! Reads the Transform component from JSON object.
		Ecs::ComponentPtr			readTransform( const Json::Value& value );

		//! Reads the Renderer component from JSON object.
		Ecs::ComponentPtr			readRenderer( const Json::Value& value );

		//! Reads the Camera component from JSON object.
		Ecs::ComponentPtr			readCamera( const Json::Value& value );

		//! Reads the Light component from JSON object.
		Ecs::ComponentPtr			readLight( const Json::Value& value );

		//! Reads the Particles component from JSON object.
		Ecs::ComponentPtr			readParticles( const Json::Value& value );

		//! Reads the shape module from JSON object.
		bool						readModuleShape( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the color module from JSON object.
		bool						readModuleColor( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the emission module from JSON object.
		bool						readModuleEmission( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the size module from JSON object.
		bool						readModuleSize( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the rotation module from JSON object.
		bool						readModuleAngularVelocity( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the acceleration module from JSON object.
		bool						readModuleAcceleration( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the velocity module from JSON object.
		bool						readModuleVelocity( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the limit velocity module from JSON object.
		bool						readModuleLimitVelocity( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the initial module from JSON object.
		bool						readModuleInitial( Fx::ParticlesWPtr particles, const Json::Value& object );

		//! Reads the color parameter from JSON object.
		void						readColorParameter( Fx::RgbParameter& parameter, const Json::Value& object );

		//! Reads the scalar parameter from JSON object.
		void						readScalarParameter( Fx::FloatParameter& parameter, const Json::Value& object );

	private:

		//! Component loader type.
		typedef cClosure<Ecs::ComponentPtr(const Json::Value&)> ComponentLoader;

		//! Particle system module loader
		typedef cClosure<bool(Fx::ParticlesWPtr, const Json::Value&)> ModuleLoader;

		//! Container type to store particle module loaders.
		typedef Map<String, ModuleLoader>	ModuleLoaders;

		//! Container to store all available component loaders.
		typedef Map<String, ComponentLoader> ComponentLoaders;

		//! Container type to store parsed scene objects.
		typedef Map<String, Ecs::EntityPtr> SceneObjects;

		//! Container type to store parsed components.
		typedef Map<String, Ecs::ComponentPtr> Components;

		const Assets::Assets&	    m_assets;					//!< Available assets.
		Json::Value					m_json;						//!< Parsed JSON.
		ScenePtr					m_scene;					//!< The scene to be loaded.
		SceneObjects				m_sceneObjects;				//!< Parsed scene objects.
		Components					m_components;				//!< Parsed components.
		ComponentLoaders			m_loaders;					//!< Available component loaders.
		ModuleLoaders				m_moduleLoaders;			//!< Available module loaders.
		Fx::IMaterialFactoryPtr		m_particleMaterialFactory;	//!< Constructs particle system materials.
	};

#endif	/*	HAVE_JSON	*/

#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

} // namespace Scene

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "Viewport.h"
	#include "Components/Rendering.h"
	#include "Components/Transform.h"
	#include "Components/Physics.h"
	#include "Assets/Mesh.h"
	#include "Assets/Material.h"
	#include "Assets/Image.h"
	#include "Assets/Terrain.h"
    #include "Assets/Prefab.h"
    #include "Assets/AssetFileSources.h"
	#include "Systems/InputSystems.h"
	#include "Systems/TransformSystems.h"
	#include "Systems/Physics2D.h"
	#include "Systems/CullingSystems.h"
	#include "Archetypes/Camera.h"
	#include "Rendering/RenderTarget.h"
	#include "Rendering/RenderingContext.h"
	#include "Rendering/RenderingSystem.h"
	#include "Rendering/ShaderCache.h"
	#include "Rendering/Passes/DebugPasses.h"
	#include "Rendering/Passes/BasicPasses.h"
	#include "Rendering/ForwardLighting/LightPass.h"
#endif

#endif    /*    !__DC_Scene_H__    */