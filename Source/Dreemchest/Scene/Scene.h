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

#include <Assets/Asset.h>
#include <Assets/AssetHandle.h>
#include <Assets/Assets.h>
#include <Assets/AssetSource.h>

#include <Ecs/Entity/Entity.h>
#include <Ecs/Entity/DataCache.h>
#include <Ecs/Component/Component.h>
#include <Ecs/System/GenericEntitySystem.h>
#include <Ecs/System/ImmutableEntitySystem.h>
#include <Ecs/System/SystemGroup.h>

#include <Io/DiskFileSystem.h>
#include <Io/streams/Stream.h>

#include <Fx/ParticleSystem.h>
#include <Fx/Particles.h>
#include <Fx/Emitter.h>
#include <Fx/Renderers.h>
#include <Fx/Zones.h>
#include <Fx/Modules.h>

#include <Reflection/MetaObject/Property.h>
#include <Reflection/MetaObject/Class.h>
#include <Reflection/MetaObject/Enum.h>
#include <Reflection/MetaObject/Instance.h>

#include "PlaneClipper.h"

#define DEV_PROFILE_RVM_CPU             (0)

#if DEV_PROFILE_RVM_CPU
    #define DEV_DISABLE_DRAW_CALLS          (1)
    #define DEV_DISABLE_CULLING             (1)
    #define DEV_DISABLE_TRANSFORMS          (1)
#else
    #define DEV_DISABLE_DRAW_CALLS          (0)
    #define DEV_DISABLE_CULLING             (0)
    #define DEV_DISABLE_TRANSFORMS          (0)
#endif  /*  DEV_PROFILE_RVM_CPU */

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

#if DEV_DEPRECATED_SCENE_RENDERER
    class Renderable;
    class Texture;
    class Technique;
    struct RasterizationOptions;
    class  Commands;

	//! Rvm unique pointer type.
    typedef AutoPtr<class Rvm> RvmUPtr;

    //! Rvm command buffer unique pointer type.
    typedef AutoPtr<class Commands> CommandsUPtr;

    //! Rop emitter unique pointer type.
    typedef AutoPtr<class AbstractRopEmitter> RopEmitterUPtr;

    //! Render system unique pointer type.
    typedef AutoPtr<class RenderSystemBase> RenderSystemUPtr;

    //! Render pass unique pointer type.
    typedef AutoPtr<class RenderPassBase> RenderPassUPtr;
#else
    dcDeclarePtrs( Ubershader )
    dcDeclareNamedPtrs( AbstractRenderCache, RenderCache )

    //! A render resource handle type.
    typedef u32 RenderResource;

    //! Render frame unique pointer type.
    typedef AutoPtr<class RenderFrame> RenderFrameUPtr;

    //! Render system unique pointer type.
    typedef AutoPtr<class RenderSystemBase> RenderSystemUPtr;

    class RenderStateBlock;

    //! Render command buffer unique pointer type.
    typedef AutoPtr<class RenderCommandBuffer> RenderCommandBufferUPtr;
#endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

	class Transform;
	class StaticMesh;
	class Sprite;
	class Camera;
    class PointCloud;

    //! Spatial index unique pointer type.
    typedef AutoPtr<class Spatial> SpatialUPtr;

    //! Image handle type.
    typedef Assets::DataHandle<class Image> ImageHandle;

    //! Mesh handle type.
    typedef Assets::DataHandle<class Mesh> MeshHandle;

    //! Material handle type.
    typedef Assets::DataHandle<class Material> MaterialHandle;

    //! Terrain handle type.
    typedef Assets::DataHandle<class Terrain> TerrainHandle;

#if DEV_DEPRECATED_SCENE_RENDERER
    //! Renderable handle type.
    typedef Assets::DataHandle<class Renderable> RenderableHandle;

    //! Texture handle type.
    typedef Assets::DataHandle<class Texture> TextureHandle;

    //! Technique handle type.
    typedef Assets::DataHandle<class Technique> TechniqueHandle;

    //! ShaderSource handle type.
    typedef Assets::DataHandle<class ShaderSource> ShaderSourceHandle;

    //! Program handle type.
    typedef Assets::DataHandle<class Program> ProgramHandle;

    //! Integer handle type to access render assets.
    typedef s32 RenderAssetIndex;
#else
    //! Renderable vertex format.
    class VertexFormat {
    public:

        //! Available vertex attributes
        enum Attribute {
              Position  = BIT( 0 )  //!< Vertex position.
            , Normal    = BIT( 1 )  //!< Vertex normal.
            , Color     = BIT( 2 )  //!< Vertex 8-bit color.
            , Uv0       = BIT( 3 )  //!< Texture #1 UV coordinates.
            , Uv1       = BIT( 4 )  //!< Texture #2 UV coordinates.
        };

                                //! Constructs a VertexFormat instance.
                                VertexFormat( u8 attributes );

                                //! Converts a VertexFormat to u8 value.
                                operator u8( void ) const;

        //! Returns true if a specified attribute exists.
        bool                    operator & ( Attribute attribute ) const;

        //! Tests two vertex formats for an equality.
        bool                    operator == ( const VertexFormat& other ) const;

        //! Returns a vertex size.
        s32                     vertexSize( void ) const;

        //! Returns a vertex attribute value.
        template<typename TValue>
        TValue                  vertexAttribute( Attribute attribute, const void* vertices, s32 index ) const;

        //! Sets a vertex attribute value.
        template<typename TValue>
        void                    setVertexAttribute( Attribute attribute, const TValue& value, void* vertices, s32 index ) const;

        //! Returns a vertex attribute offset.
        s32                     attributeOffset( Attribute attribute ) const;

        //! Returns a vertex attribute size.
        s32                     attributeSize( Attribute attribute ) const;

        //! Returns a vertex offset.
        s32                     vertexOffset( s32 index ) const;

        //! Returns a vertex position size in bytes.
        s32                     sizeOfPosition( void ) const;

        //! Returns a vertex color size in bytes.
        s32                     sizeOfColor( void ) const;

        //! Returns a vertex normal size in bytes.
        s32                     sizeOfNormal( void ) const;

        //! Returns a vertex uv layer size in bytes.
        s32                     sizeOfUv0( void ) const;

        //! Returns a vertex uv layer size in bytes.
        s32                     sizeOfUv1( void ) const;

    private:

        u8                      m_attributes;   //!< Vertex attribute mask.
    };

    // ** VertexFormat::VertexFormat
    NIMBLE_INLINE VertexFormat::VertexFormat( u8 attributes )
        : m_attributes( attributes | Position )
    {
    }

    // ** VertexFormat::operator u8
    NIMBLE_INLINE VertexFormat::operator u8( void ) const
    {
        return m_attributes;
    }

    // ** VertexFormat::operator &
    NIMBLE_INLINE bool VertexFormat::operator & ( Attribute attribute ) const
    {
        return m_attributes & attribute ? true : false;
    }

    // ** VertexFormat::operator ==
    NIMBLE_INLINE bool VertexFormat::operator == ( const VertexFormat& other ) const
    {
        return m_attributes == other.m_attributes;
    }

    // ** VertexFormat::vertexSize
    NIMBLE_INLINE s32 VertexFormat::vertexSize( void ) const
    {
        return   sizeOfPosition()
               + sizeOfNormal()
               + sizeOfColor()
               + sizeOfUv0()
               + sizeOfUv1()
               ;
    }

    // ** VertexFormat::attributeOffset
    NIMBLE_INLINE s32 VertexFormat::attributeOffset( Attribute attribute ) const
    {
        DC_ABORT_IF( (m_attributes & attribute) == 0, "a vertex format does not contain a specified attribute" );

        s32 offset = 0;

        switch( attribute ) {
        case Uv1:       offset += sizeOfUv0();
        case Uv0:       offset += sizeOfColor();
        case Color:     offset += sizeOfNormal();
        case Normal:    offset += sizeOfPosition();
        case Position:  break;
        }

        return offset;
    }

    // ** VertexFormat::attributeSize
    NIMBLE_INLINE s32 VertexFormat::attributeSize( Attribute attribute ) const
    {
        switch( attribute ) {
        case Position:  return sizeof( f32 ) * 3;
        case Normal:    return sizeof( f32 ) * 3;
        case Color:     return sizeof( u8  ) * 4;
        case Uv0:       return sizeof( f32 ) * 2;
        case Uv1:       return sizeof( f32 ) * 2;
        }

        return 0;
    }

    // ** VertexFormat::vertexOffset
    NIMBLE_INLINE s32 VertexFormat::vertexOffset( s32 index ) const
    {
        return vertexSize() * index;
    }

    // ** VertexFormat::vertexAttribute
    template<typename TValue>
    TValue VertexFormat::vertexAttribute( Attribute attribute, const void* vertices, s32 index ) const
    {
        if( m_attributes & attribute ) {
            DC_ABORT_IF( sizeof( TValue ) != attributeSize( attribute ), "vertex attribute size mismatch" );
            return *reinterpret_cast<const TValue*>( reinterpret_cast<const u8*>( vertices ) + vertexOffset( index ) + attributeOffset( attribute ) );
        }

        return TValue();
    }

    // ** VertexFormat::setVertexAttribute
    template<typename TValue>
    void VertexFormat::setVertexAttribute( Attribute attribute, const TValue& value, void* vertices, s32 index ) const
    {
        if( m_attributes & attribute ) {
            DC_ABORT_IF( sizeof( TValue ) != attributeSize( attribute ), "vertex attribute size mismatch" );
            *reinterpret_cast<TValue*>( reinterpret_cast<u8*>( vertices ) + vertexOffset( index ) + attributeOffset( attribute ) ) = value;
        }
    }

    // ** VertexFormat::sizeOfPosition
    NIMBLE_INLINE s32 VertexFormat::sizeOfPosition( void ) const
    {
        if( m_attributes & Position ) {
            return attributeSize( Position );
        }

        return 0;
    }

    // ** VertexFormat::sizeOfColor
    NIMBLE_INLINE s32 VertexFormat::sizeOfColor( void ) const
    {
        if( m_attributes & Color ) {
            return attributeSize( Color );
        }

        return 0;
    }

    // ** VertexFormat::sizeOfNormal
    NIMBLE_INLINE s32 VertexFormat::sizeOfNormal( void ) const
    {
        if( m_attributes & Normal ) {
            return attributeSize( Normal );
        }

        return 0;
    }

    // ** VertexFormat::sizeOfUv0
    NIMBLE_INLINE s32 VertexFormat::sizeOfUv0( void ) const
    {
        if( m_attributes & Uv0 ) {
            return attributeSize( Uv0 );
        }

        return 0;
    }

    // ** VertexFormat::sizeOfUv1
    NIMBLE_INLINE s32 VertexFormat::sizeOfUv1( void ) const
    {
        if( m_attributes & Uv1 ) {
            return attributeSize( Uv1 );
        }

        return 0;
    }
#endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

	//! Available rendering modes.
    NIMBLE_DECLARE_ENUM( RenderingMode, Opaque, Cutout, Translucent, Additive )

#if DEV_DEPRECATED_SCENE_RENDERER
	//! Rendering mode mask.
	enum RenderingModeBits {
		  RenderOpaqueBit		= BIT( RenderOpaque )		//!< Opaque rendering bit.
		, RenderCutoutBit		= BIT( RenderCutout )		//!< Cutout rendering bit.
		, RenderTranslucentBit	= BIT( RenderTranslucent )	//!< Translucent rendering bit.
		, RenderAdditiveBit		= BIT( RenderAdditive )		//!< Additive rendering bit.
		, AllRenderModesBit		= RenderOpaqueBit | RenderCutoutBit | RenderTranslucentBit | RenderAdditiveBit
	};
#endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

	//! Available lighting models.
    NIMBLE_DECLARE_ENUM( LightingModel, Unlit, Ambient, Phong )

#if DEV_DEPRECATED_SCENE_RENDERER
	//! Lighting models bit masks.
	enum LightingModelBits {
		  LightingModelUnlitBit     = BIT( LightingModelUnlit )		//!< Unlit model bit.
		, LightingModelAmbientBit   = BIT( LightingModelAmbient )   //!< Ambient lighting model bit.
		, LightingModelPhongBit	    = BIT( LightingModelPhong )	    //!< Phong lighting model bit.
		, AllLightingModelsBit		= LightingModelUnlit | LightingModelAmbient | LightingModelPhong
	};
#endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

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

	dcDeclarePtrs( Physics2D )
	dcDeclarePtrs( RigidBody2D )

	dcDeclarePtrs( RenderingContext )
    dcDeclarePtrs( RenderScene )
    dcDeclarePtrs( Rvm )

	dcDeclarePtrs( Vec3Binding )

#if DEV_DEPRECATED_ECS_ARCHETYPES
	dcDeclarePtrs( SpectatorCamera )
	dcDeclarePtrs( Camera2D )
#endif  /*  #if DEV_DEPRECATED_ECS_ARCHETYPES   */

	//! Container type to store a set of scene objects.
	typedef Set<SceneObjectPtr> SceneObjectSet;

    //! Asset manager used by scene module.
    class Resources : public Assets::Assets {
    public:

                        //! Constructs Resources instance.
                        Resources( void );

    private:


        //! Calculates the total size allocated for mesh asset.
        static s32      bytesAllocatedForMesh( const Mesh& asset );

        //! Calculates the total size allocated for image asset.
        static s32      bytesAllocatedForImage( const Image& asset );

        //! Calculates the total size allocated for material asset.
        static s32      bytesAllocatedForMaterial( const Material& asset );

    #if DEV_DEPRECATED_SCENE_RENDERER
        //! Calculates the total size allocated for texture asset.
        static s32      bytesAllocatedForTexture( const Texture& asset );

        //! Calculates the total size allocated for renderable asset.
        static s32      bytesAllocatedForRenderable( const Renderable& asset );

        //! Calculates the total size allocated for technique asset.
        static s32      bytesAllocatedForTechnique( const Technique& asset );

        //! Calculates the total size allocated for shader source asset.
        static s32      bytesAllocatedForShaderSource( const ShaderSource& asset );

        //! Calculates the total size allocated for shader source asset.
        static s32      bytesAllocatedForProgram( const Program& asset );
    #endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */
    };

	//! The root class for a scene subsystem.
	class Scene : public InjectEventEmitter<RefCounted> {
	public:

		//! Performs a scene update.
		void							update( u32 currentTime, f32 dt );

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

        //! Returns internal entity component system instance.
        Ecs::EcsWPtr                    ecs( void ) const;

        //! Returns spatial index built for this scene.
        const Spatial*                  spatial( void ) const;

		//! Returns a scene system of specified type.
		template<typename TSystem>
		WeakPtr<TSystem>				system( void ) const;

		//! Adds a new system to the scene.
		template<typename TSystem, typename ... Args>
		WeakPtr<TSystem>				addSystem( Args ... args );

		//! Creates an empty scene.
		static ScenePtr					create( void );

		//! Creates scene and loads it from JSON file.
		static ScenePtr					createFromFile( const Resources& assets, const String& fileName );

		//! Creates scene and loads it from JSON string.
		static ScenePtr					createFromJson( const Resources& assets, const String& json );

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
		Ecs::IndexPtr					m_named;			//!< All named entities that reside in scene stored inside this family.
        SpatialUPtr                     m_spatial;          //!< Scene spatial index.
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

	// ** Scene::createArchetype
	template<typename TArchetype>
	StrongPtr<TArchetype> Scene::createArchetype( const SceneObjectId& id, const KeyValue* data )
	{
		return m_ecs->createArchetype<TArchetype>( id, data );
	}

#if DEV_DEPRECATED_SCENE_SERIALIZATION

#ifdef HAVE_JSON

    //! Base class for all JSON object loaders.
	class JsonLoaderBase {
	public:

		//! Object loader type.
		typedef cClosure<bool(const Json::Value&)>	Loader;

		virtual						~JsonLoaderBase( void ) {}

		//! Registers a new object loader.
		void						registerLoader( const String& name, const Loader& loader );

		//! Loads objects from JSON string.
		bool						load( const String& json );

	protected:

		//! Reads the Vec3 from a JSON object.
		static Vec3					readVec3( const Json::Value& value );

		//! Reads the Rect from a JSON object.
		static Rect					readRect( const Json::Value& value );

		//! Reads the Rgba from JSON object.
		static Rgba					readRgba( const Json::Value& value );

		//! Reads the Rgba from JSON object.
		static Rgb					readRgb( const Json::Value& value );

		//! Reads the Quat from JSON object.
		static Quat					readQuat( const Json::Value& value );

		//! Reads the array of floats from JSON object.
		static Array<f32>			readFloats( const Json::Value& value );

	private:

		//! Constructs an object from JSON object.
		bool						constructObject( const String& name, const Json::Value& value );

	private:

		//! Container type to store loaders by type name.
		typedef Map<String, Loader>	Loaders;

		Loaders						m_loaders;	//!< Object loaders.
		Json::Value					m_json;		//!< Parsed JSON object.
	};

	//! Loads the scene from JSON file.
	class JsonSceneLoader : public JsonLoaderBase {
	public:

									//! Constructs the JsonSceneLoader instance.
									JsonSceneLoader( const Resources& assets );

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

		const Resources&	        m_assets;					//!< Available assets.
		Json::Value					m_json;						//!< Parsed JSON.
		ScenePtr					m_scene;					//!< The scene to be loaded.
		SceneObjects				m_sceneObjects;				//!< Parsed scene objects.
		Components					m_components;				//!< Parsed components.
		ComponentLoaders			m_loaders;					//!< Available component loaders.
		ModuleLoaders				m_moduleLoaders;			//!< Available module loaders.
		Fx::IMaterialFactoryPtr		m_particleMaterialFactory;	//!< Constructs particle system materials.
	};

#endif	/*	HAVE_JSON	*/

#endif  /*  #if DEV_DEPRECATED_SCENE_SERIALIZATION    */

} // namespace Scene

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "Viewport.h"
    #include "Spatial/Spatial.h"
	#include "Components/Rendering.h"
	#include "Components/Transform.h"
	#include "Components/Physics.h"
    #include "Components/Debug.h"
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
    #if DEV_DEPRECATED_SCENE_RENDERER
        #include "Rendering/RenderScene.h"
        #include "Rendering/RenderAssets.h"
        #include "Rendering/RenderSystems/Unlit.h"
        #include "Rendering/RenderSystems/ForwardLighting.h"
        #include "Rendering/RenderSystems/DepthComplexity.h"
    #else
        #include "Rendering/RenderScene.h"
        #include "Rendering/RenderCache.h"
        #include "Rendering/Rvm/RenderingContext.h"
        #include "Rendering/Rvm/Rvm.h"
        #include "Rendering/Debug/TestRenderSystem.h"
    #endif  /*  DEV_DEPRECATED_SCENE_RENDERER   */
#endif

#endif    /*    !__DC_Scene_H__    */