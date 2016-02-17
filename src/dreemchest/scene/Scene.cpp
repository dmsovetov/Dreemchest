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

#include "Scene.h"

#include "Rendering/RenderingSystem.h"
#include "Rendering/Rvm.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/Passes/DebugPasses.h"
#include "Rendering/Passes/BasicPasses.h"
#include "Rendering/ForwardLighting/LightPass.h"

#include "Assets/Assets.h"
#include "Assets/Material.h"
#include "Assets/Mesh.h"

#include "Components/Rendering.h"
#include "Components/Transform.h"

#include "Systems/TransformSystems.h"
#include "Systems/CullingSystems.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Scene::Scene
Scene::Scene( void )
{
	m_ecs		= Ecs::Ecs::create();

	// Create entity indices
	m_cameras	= m_ecs->requestIndex( "Cameras", Ecs::Aspect::all<Camera>() );
	m_named		= m_ecs->requestIndex( "Named Entities", Ecs::Aspect::all<Identifier>() );
	m_meshes	= m_ecs->requestIndex( "Static Meshes", Ecs::Aspect::all<StaticMesh>() );

	// Create system groups.
	m_updateSystems = m_ecs->createGroup( "Update", UpdateSystems );

	// Add default update systems.
	addSystem<AffineTransformSystem>();
	addSystem<ParticlesSystem>();
	addSystem<MoveAlongAxesSystem>();
	addSystem<MoveToSystem>();
	addSystem<RotateAroundAxesSystem>();
	addSystem<WorldSpaceBoundingBoxSystem>();
	addSystem<FrustumCullingSystem>( cameras() );
		
	// Add default render systems.
	addRenderingSystem<BoundingVolumesRenderer>();
	addRenderingSystem<ForwardLightingRenderer>();
	addRenderingSystem<SinglePassRenderingSystem<RenderUnlit, UnlitPass>>();
	addRenderingSystem<SinglePassRenderingSystem<RenderParticles, ParticleSystemsPass>>();
	addRenderingSystem<SinglePassRenderingSystem<RenderWireframe, WireframePass>>();
	addRenderingSystem<SinglePassRenderingSystem<RenderGrid, GridPass>>();
	addRenderingSystem<SinglePassRenderingSystem<RenderVertexNormals, VertexNormalsPass>>();

    // Register component types
    m_ecs->registerComponent<Identifier>();
    m_ecs->registerComponent<StaticMesh>();
    m_ecs->registerComponent<Camera>();
    m_ecs->registerComponent<Transform>();
}

// ** Scene::update
void Scene::update( u32 currentTime, f32 dt )
{
	m_ecs->update( currentTime, dt, UpdateSystems );
}

// ** Scene::createSceneObject
SceneObjectPtr Scene::createSceneObject( void )
{
	return m_ecs->createEntity();
}

// ** Scene::createSceneObject
SceneObjectPtr Scene::createSceneObject( const SceneObjectId& id )
{
	return m_ecs->createEntity( id );
}

// ** Scene::addSceneObject
void Scene::addSceneObject( const SceneObjectPtr& sceneObject )
{
	DC_BREAK_IF( !sceneObject.valid() );
	m_ecs->addEntity( sceneObject );

	// Emit the event
	notify<SceneObjectAdded>( sceneObject );
}

// ** Scene::removeSceneObject
void Scene::removeSceneObject( const SceneObjectPtr& sceneObject )
{
	DC_BREAK_IF( !sceneObject.valid() );
	m_ecs->removeEntity( sceneObject->id() );

	// Emit the event
	notify<SceneObjectRemoved>( sceneObject );
}

// ** Scene::findSceneObject
SceneObjectPtr Scene::findSceneObject( const SceneObjectId& id ) const
{
	Ecs::EntityPtr entity = m_ecs->findEntity( id );

	if( entity == Ecs::EntityPtr() ) {
		return SceneObjectPtr();
	}

	return entity;
}

// ** Scene::findByAspect
SceneObjectSet Scene::findByAspect( const Ecs::Aspect& aspect ) const
{
	return m_ecs->findByAspect( aspect );
}

// ** Scene::render
void Scene::render( const RenderingContextPtr& context )
{
	Renderer::HalPtr hal = context->hal();

	// Clear all cameras & assign ids
	const Ecs::EntitySet& cameras  = m_cameras->entities();
	u8					  cameraId = 0;

	for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
		Camera*					camera = (*i)->get<Camera>();
		const RenderTargetPtr&	target = camera->target();

		if( !target.valid() ) {
			continue;
		}

		target->begin( context );
		{
			hal->setViewport( camera->viewport() );
			u32 mask = ( camera->clearMask() & Camera::ClearColor ? Renderer::ClearColor : 0 ) | ( camera->clearMask() & Camera::ClearDepth ? Renderer::ClearDepth : 0 );
			hal->clear( camera->clearColor(), 1.0f, 0, mask );
			hal->setViewport( target->rect() );
		}
		target->end( context );
	}

	// Update all rendering systems
	for( u32 i = 0; i < ( u32 )m_renderingSystems.size(); i++ ) {
		m_renderingSystems[i]->render( context );
	}

	// Reset RVM
	context->rvm()->reset();
}

// ** Scene::cameras
const Ecs::IndexPtr& Scene::cameras( void ) const
{
	return m_cameras;
}

// ** Scene::ecs
Ecs::EcsWPtr Scene::ecs( void ) const
{
    return m_ecs;
}

// ** Scene::findAllWithName
SceneObjectSet Scene::findAllWithName( const String& name ) const
{
	const Ecs::EntitySet& entities = m_named->entities();
	SceneObjectSet objects;

	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		SceneObjectPtr sceneObject = *i;

		if( sceneObject->get<Identifier>()->name() == name ) {
			objects.insert( sceneObject );
		}
	}

	return objects;
}

// ** Scene::queryRay
RayTracingResultArray Scene::queryRay( const Ray& ray, const FlagSet8& flags ) const
{
	// Get entity set from index
	const Ecs::EntitySet& entities = m_meshes->entities();

	// Resulting array
	RayTracingResultArray results;

	// Iterate over all meshes in scene an store all that are intersected by a ray.
	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		// Get the world space bounding box of a mesh.
		const Bounds& bounds = (*i)->get<StaticMesh>()->worldSpaceBounds();

		// Check for intersection.
		RayTracingResult result( *i );

		if( ray.intersects( bounds, &result.point, &result.time ) ) {
			results.push_back( result );
		}
	}

	// Sort results
	if( flags.is( QueryBackToFront ) ) {
		std::sort( results.begin(), results.end(), std::greater<RayTracingResult>() );
	} else {
		std::sort( results.begin(), results.end(), std::less<RayTracingResult>() );
	}

	// Remove all elements except the first one if single result requested
	if( !results.empty() && flags.is( QuerySingle ) ) {
		results.erase( results.begin() + 1, results.end() );
	}

	return results;
}

// ** Scene::create
ScenePtr Scene::create( void )
{
	return ScenePtr( DC_NEW Scene );
}

// ** Scene::createFromFile
ScenePtr Scene::createFromFile( const AssetBundlePtr& assets, const String& fileName )
{
	// Read the JSON file
	String json = Io::DiskFileSystem::readTextFile( fileName );

	// Load the scene
	return createFromJson( assets, json );
}

// ** Scene::createFromJson
ScenePtr Scene::createFromJson( const AssetBundlePtr& assets, const String& json )
{
#ifdef HAVE_JSON
	// Create scene instance
	ScenePtr scene( DC_NEW Scene );

	// Load scene from JSON
	JsonSceneLoader loader;

	if( !loader.load( scene, assets, json ) ) {
		return ScenePtr();
	}

	return scene;
#else
	LogError( "Scene::createFromJson : failed to load scene, built with no JSON support.\n" );
	return ScenePtr();
#endif	/*	HAVE_JSON	*/
}

// ------------------------------------------------- JsonSceneLoader ------------------------------------------------- //

#ifdef HAVE_JSON

// ** JsonSceneLoader::JsonSceneLoader
JsonSceneLoader::JsonSceneLoader( void )
{
	m_loaders["Transform"]				= dcThisMethod( JsonSceneLoader::readTransform );
	m_loaders["Renderer"]				= dcThisMethod( JsonSceneLoader::readRenderer );
	m_loaders["Camera"]					= dcThisMethod( JsonSceneLoader::readCamera );
	m_loaders["Light"]					= dcThisMethod( JsonSceneLoader::readLight );
	m_loaders["Particles"]				= dcThisMethod( JsonSceneLoader::readParticles );

	m_moduleLoaders["color"]			= dcThisMethod( JsonSceneLoader::readModuleColor );
	m_moduleLoaders["emission"]			= dcThisMethod( JsonSceneLoader::readModuleEmission );
	m_moduleLoaders["velocity"]			= dcThisMethod( JsonSceneLoader::readModuleVelocity );
    m_moduleLoaders["limitVelocity"]    = dcThisMethod( JsonSceneLoader::readModuleLimitVelocity );
	m_moduleLoaders["acceleration"]		= dcThisMethod( JsonSceneLoader::readModuleAcceleration );
	m_moduleLoaders["angularVelocity"]	= dcThisMethod( JsonSceneLoader::readModuleAngularVelocity );
	m_moduleLoaders["size"]				= dcThisMethod( JsonSceneLoader::readModuleSize );
	m_moduleLoaders["initial"]			= dcThisMethod( JsonSceneLoader::readModuleInitial );
	m_moduleLoaders["shape"]			= dcThisMethod( JsonSceneLoader::readModuleShape );
}

// ** JsonSceneLoader::load
bool JsonSceneLoader::load( ScenePtr scene, const AssetBundlePtr& assets, const String& json )
{
	// Declare the particle material factory.
	struct ParticleMaterialFactory : public Fx::IMaterialFactory {

		//! Constructs ParticleMaterialFactory instance.
		ParticleMaterialFactory( AssetBundleWPtr assets )
			: m_assets( assets ) {}


		//! Creates a new material instance.
		virtual Fx::IMaterialPtr createMaterial( const String& identifier )
		{
			MaterialPtr material = m_assets->find<Material>( identifier );
	
			if( !material.valid() ) {
				return Fx::IMaterialPtr();
			}

			return DC_NEW Fx::IMaterial( material );
		}

	private:

		AssetBundleWPtr	m_assets;	//!< Asset bundle to use.		
	};

	Json::Reader reader;

	// Parse the JSON string
	if( !reader.parse( json, m_json ) ) {
		return false;
	}

	// Set the asset bundle reference
	m_assets = assets;

	// Save the scene reference.
	m_scene = scene;

	// Construct the particle material instance.
	m_particleMaterialFactory = Fx::IMaterialFactoryPtr( DC_NEW ParticleMaterialFactory( assets ) );

	// Read objects from JSON
	for( Json::ValueIterator i = m_json.begin(), end = m_json.end(); i != end; ++i ) {
		// Get the instance type.
		String type = i->get( "class", "" ).asString();

		// Read the scene object.
		if( type == "SceneObject" ) {
			requestSceneObject( i.key().asString() );
			continue;
		}

		// Read the component
		Ecs::ComponentPtr component = requestComponent( i.key().asString() );
		DC_BREAK_IF( !component.valid() );

		// Get the scene object to attach the component to.
		Ecs::EntityPtr entity = requestSceneObject( i->get( "sceneObject", Json::Value() ).asString() );

		// Attach the component.
		entity->attachComponent( component.get() );
	}

	// Update the scene to populate all families and systems
	m_scene->update( 0, 0.0f );

	return true;
}

// ** JsonSceneLoader::requestComponent
Ecs::ComponentPtr JsonSceneLoader::requestComponent( const String& id )
{
	Components::iterator i = m_components.find( id );

	if( i != m_components.end() ) {
		return i->second;
	}

	// Get the data
	Json::Value data = m_json.get( id, Json::Value() );

	// Get the component type
	String type = data.get( "class", "" ).asString();

	// Get the component loader.
	ComponentLoaders::iterator j = m_loaders.find( type );

	if( j == m_loaders.end() ) {
		LogError( "JsonSceneLoader::requestComponent : unknown object type '%s'\n", type.c_str() );
		return Ecs::ComponentPtr();
	}

	// Read the component.
	Ecs::ComponentPtr component = j->second( data );

	// Save parsed component
	m_components[id] = component;

	return component;
}

// ** JsonSceneLoader::requestSceneObject
Ecs::EntityPtr JsonSceneLoader::requestSceneObject( const String& id )
{
	SceneObjects::iterator i = m_sceneObjects.find( id );

	if( i != m_sceneObjects.end() ) {
		return i->second;
	}

	Json::Value data = m_json.get( id, Json::Value() );
	Ecs::EntityPtr sceneObject = m_scene->createSceneObject();

	sceneObject->attach<Identifier>( data["name"].asString() );
	m_sceneObjects[id] = sceneObject;

	m_scene->addSceneObject( sceneObject );

	return sceneObject;
}

// ** JsonSceneLoader::readTransform
Ecs::ComponentPtr JsonSceneLoader::readTransform( const Json::Value& value )
{
	Vec3 position = readVec3( value["position"] );
	Vec3 scale    = readVec3( value["scale"] );
	Quat rotation = readQuat( value["rotation"] );

	Transform* result = DC_NEW Transform;
	result->setPosition( Vec3( -position.x, position.y, position.z ) );
	result->setScale( scale );
	result->setRotation( Quat( -rotation.x, rotation.y, rotation.z, -rotation.w ) );

	Json::Value parent = value["parent"];

	if( parent != Json::nullValue ) {
		Ecs::ComponentPtr component = requestComponent( parent.asString() );
		result->setParent( castTo<Transform>( component.get() ) );
	}

	return result;
}

// ** JsonSceneLoader::readCamera
Ecs::ComponentPtr JsonSceneLoader::readCamera( const Json::Value& value )
{
	Camera* result = DC_NEW Camera;
	result->setFov( value["fov"].asFloat() );
	result->setNear( value["near"].asFloat() );
	result->setFar( value["far"].asFloat() );
	result->setClearColor( readRgba( value["backgroundColor"] ) );
	result->setNdc( readRect( value["ndc"] ) );

	return result;
}

// ** JsonSceneLoader::readRenderer
Ecs::ComponentPtr JsonSceneLoader::readRenderer( const Json::Value& value )
{
	StaticMesh* result = DC_NEW StaticMesh;
	String		asset  = value["asset"].asString();

	result->setMesh( m_assets->find<Mesh>( asset ) );

	Json::Value materials = value["materials"];

	for( u32 i = 0; i < materials.size(); i++ ) {
		result->setMaterial( i, m_assets->find<Material>( materials[i].asString() ) );
	}

	return result;
}

// ** JsonSceneLoader::readLight
Ecs::ComponentPtr JsonSceneLoader::readLight( const Json::Value& value )
{
	Light::Type types[] = { Light::Spot, Light::Directional, Light::Point };

	Light* result = DC_NEW Light;
	result->setColor( readRgb( value["color"] ) );
	result->setIntensity( value["intensity"].asFloat() );
	result->setRange( value["range"].asFloat() );
	result->setType( types[value["type"].asInt()] );

	return result;
}

// ** JsonSceneLoader::readParticles
Ecs::ComponentPtr JsonSceneLoader::readParticles( const Json::Value& value )
{
	// Create the particle system
	Fx::ParticleSystemPtr particleSystem( DC_NEW Fx::ParticleSystem );

	// Add an emitter
	Fx::EmitterWPtr emitter = particleSystem->addEmitter();

	// Setup emitter
	emitter->setLooped( value["isLooped"].asBool() );
	emitter->setDuration( value["duration"].asFloat() );

	// Add particles to the emitter
	Fx::ParticlesWPtr particles = emitter->addParticles();

	// Setup material
	particles->setMaterial( value["material"].asString() );

	// Setup particles
	for( Json::ValueConstIterator i = value.begin(), end = value.end(); i != end; ++i ) {
		if( !i->isObject() ) {
			continue;
		}

		// Find the module loader
		ModuleLoaders::const_iterator j = m_moduleLoaders.find( i.key().asString() );

		if( j == m_moduleLoaders.end() ) {
			LogWarning( "JsonSceneLoader::readParticles : unhandled module %s\n", i.key().asString().c_str() );
			continue;
		}

		j->second( particles, *i );
	}

	// Create particles instance
	Fx::ParticleSystemInstancePtr instance = particleSystem->createInstance( m_particleMaterialFactory );

	// Create Particles component instance
	Particles* component = DC_NEW Particles( particleSystem, instance );
	component->setMaterial( m_assets->find<Material>( particles->material() ) );

	return component;
}

// ** JsonSceneLoader::readModuleShape
bool JsonSceneLoader::readModuleShape( Fx::ParticlesWPtr particles, const Json::Value& object )
{
	Fx::EmitterWPtr emitter = particles->emitter();
    s32             type    = object["type"].asInt();

	switch( type ) {
	case 4: break;	// not implemented
	case 0:
	case 1: emitter->setZone( DC_NEW Fx::SphereZone( object["radius"].asFloat() ) ); break;
	case 3:
	case 2: emitter->setZone( DC_NEW Fx::HemiSphereZone( object["radius"].asFloat() ) ); break;
	case 5: emitter->setZone( DC_NEW Fx::BoxZone( object["width"].asFloat(), object["height"].asFloat(), object["depth"].asFloat() ) ); break;
    case 12: emitter->setZone( DC_NEW Fx::LineZone( object["radius"].asFloat() ) ); break;
	default: DC_NOT_IMPLEMENTED;
	}

	return true;
}

// ** JsonSceneLoader::readModuleColor
bool JsonSceneLoader::readModuleColor( Fx::ParticlesWPtr particles, const Json::Value& object )
{
	Fx::Color* color = DC_NEW Fx::Color;
	readColorParameter( color->get(), object["rgb"] );
	particles->addModule( color );

	Fx::Transparency* transparency = DC_NEW Fx::Transparency;
	readScalarParameter( transparency->get(), object["alpha"] );
	particles->addModule( transparency );

	return true;
}

// ** JsonSceneLoader::readModuleEmission
bool JsonSceneLoader::readModuleEmission( Fx::ParticlesWPtr particles, const Json::Value& object )
{
	readScalarParameter( particles->emitter()->emission(), object["rate"] );

	Json::Value bursts = object.get( "bursts", Json::Value::null );

	for( s32 i = 0, n = bursts.size() / 2; i < n; i++ ) {
		particles->emitter()->addBurst( bursts[i * 2 + 0].asFloat(), bursts[i * 2 + 1].asInt() );
	}

	return true;
}

// ** JsonSceneLoader::readModuleAcceleration
bool JsonSceneLoader::readModuleAcceleration( Fx::ParticlesWPtr particles, const Json::Value& object )
{
#if 0
	readScalarParameter( particles->scalarParameter( Fx::Particles::AccelerationXOverLife ), object["x"] );
	readScalarParameter( particles->scalarParameter( Fx::Particles::AccelerationYOverLife ), object["y"] );
	readScalarParameter( particles->scalarParameter( Fx::Particles::AccelerationZOverLife ), object["z"] );
#else
	DC_BREAK
#endif
	return true;
}

// ** JsonSceneLoader::readModuleVelocity
bool JsonSceneLoader::readModuleVelocity( Fx::ParticlesWPtr particles, const Json::Value& object )
{
	Fx::LinearVelocity* module = DC_NEW Fx::LinearVelocity;
	particles->addModule( module );

	Fx::FloatParameter& x = module->x();
	Fx::FloatParameter& y = module->y();
	Fx::FloatParameter& z = module->z();

	readScalarParameter( x, object["x"] );
	readScalarParameter( y, object["y"] );
	readScalarParameter( z, object["z"] );

	if( x.samplingMode() == Fx::SampleRandomBetweenConstants ) {
		x.setSamplingMode( Fx::SampleRandomBetweenCurves );
		x.constructLifetimeCurves();
	}
	if( y.samplingMode() == Fx::SampleRandomBetweenConstants ) {
		y.setSamplingMode( Fx::SampleRandomBetweenCurves );
		y.constructLifetimeCurves();
	}
	if( z.samplingMode() == Fx::SampleRandomBetweenConstants ) {
		z.setSamplingMode( Fx::SampleRandomBetweenCurves );
		z.constructLifetimeCurves();
	}

	return true;
}

// ** JsonSceneLoader::readModuleLimitVelocity
bool JsonSceneLoader::readModuleLimitVelocity( Fx::ParticlesWPtr particles, const Json::Value& object )
{
    Fx::LimitVelocity* module = DC_NEW Fx::LimitVelocity;
    particles->addModule( module );

    readScalarParameter( module->get(), object["magnitude"] );

    return true;
}

// ** JsonSceneLoader::readModuleSize
bool JsonSceneLoader::readModuleSize( Fx::ParticlesWPtr particles, const Json::Value& object )
{
	Fx::Size* module = DC_NEW Fx::Size;
	readScalarParameter( module->get(), object["curve"] );
	particles->addModule( module );

	return true;
}

// ** JsonSceneLoader::readModuleAngularVelocity
bool JsonSceneLoader::readModuleAngularVelocity( Fx::ParticlesWPtr particles, const Json::Value& object )
{
	Fx::InitialAngularVelocity* module = DC_NEW Fx::InitialAngularVelocity;
	readScalarParameter( module->get(), object["curve"] );
	particles->emitter()->addModule( module );

	particles->addModule( DC_NEW Fx::Rotation );

	return true;
}

// ** JsonSceneLoader::readModuleInitial
bool JsonSceneLoader::readModuleInitial( Fx::ParticlesWPtr particles, const Json::Value& object )
{
	particles->setCount( object["maxParticles"].asInt() );

	Fx::EmitterWPtr emitter = particles->emitter();

	{
		Fx::InitialColor* module = DC_NEW Fx::InitialColor;
		readColorParameter( module->get(), object["rgb"] );
		emitter->addModule( module );
	}
	{
		Fx::InitialLife* module = DC_NEW Fx::InitialLife;
		readScalarParameter( module->get(), object["life"] );
		emitter->addModule( module );
	}
	{
		Fx::InitialTransparency* module = DC_NEW Fx::InitialTransparency;
		readScalarParameter( module->get(), object["alpha"] );
		emitter->addModule( module );
	}

	{
		Fx::InitialSize* module = DC_NEW Fx::InitialSize;
		readScalarParameter( module->get(), object["size"] );
		emitter->addModule( module );
	}
	{
		Fx::InitialSpeed* module = DC_NEW Fx::InitialSpeed;
		readScalarParameter( module->get(), object["speed"] );
		emitter->addModule( module );
	}
	{
		Fx::InitialGravity* module = DC_NEW Fx::InitialGravity;
		readScalarParameter( module->get(), object["gravity"] );
		emitter->addModule( module );
	}
	{
		Fx::InitialRotation* module = DC_NEW Fx::InitialRotation;
		readScalarParameter( module->get(), object["rotation"] );
		emitter->addModule( module );
	}

	return true;
}

// ** JsonSceneLoader::readColorParameter
void JsonSceneLoader::readColorParameter( Fx::RgbParameter& parameter, const Json::Value& object )
{
	String type = object.get( "type", "" ).asString();

	parameter.setEnabled( true );

	if( type == "curve" ) {
		parameter.setCurve( readFloats( object["value"] ) );
	}
	else if( type == "constant" ) {
		parameter.setConstant( readRgb( object["value"] ) );
	}
	else {
		DC_BREAK;
	}
}

// ** JsonSceneLoader::readScalarParameter
void JsonSceneLoader::readScalarParameter( Fx::FloatParameter& parameter, const Json::Value& object )
{
	parameter.setEnabled( true );

	if( object.isDouble() ) {
		parameter.setConstant( object.asFloat() );
		return;
	}

	String type = object.get( "type", "" ).asString();

	if( type == "curve" ) {
		parameter.setCurve( readFloats( object["value"] ) );
	}
	else if( type == "constant" ) {
		parameter.setConstant( object["value"].asFloat() );
	}
	else if( type == "randomBetweenConstants" ) {
		Fx::FloatArray range = readFloats( object["value"] );
		parameter.setRandomBetweenConstants( range[0], range[1] );
	}
	else if( type == "randomBetweenCurves" ) {
		parameter.setRandomBetweenCurves( readFloats( object["value"][0] ), readFloats( object["value"][1] ) );
		parameter.constructLifetimeCurves();
	}
	else {
		DC_BREAK;
	}
}

#endif	/*	HAVE_JSON	*/

} // namespace Scene

DC_END_DREEMCHEST