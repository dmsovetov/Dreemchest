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

#include "ParticleSystem.h"

#include "Emitter.h"
#include "Particles.h"
#include "Zones.h"
#include "Renderers.h"

#include "../Io/DiskFileSystem.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

IMPLEMENT_LOGGER(log)

// ------------------------------------------ ParticleSystem ------------------------------------------ //

// ** ParticleSystem::emitterCount
s32 ParticleSystem::emitterCount( void ) const
{
	return static_cast<s32>( m_emitters.size() );
}

// ** ParticleSystem::emitter
EmitterWPtr ParticleSystem::emitter( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= emitterCount() );
	return m_emitters[index];
}

// ** ParticleSystem::removeEmitter
void ParticleSystem::removeEmitter( const EmitterWPtr& emitter )
{
	EmittersArray::iterator i = std::find( m_emitters.begin(), m_emitters.end(), emitter );

	if( i == m_emitters.end() ) {
		DC_BREAK;
		return;
	}

	m_emitters.erase( i );
}

// ** ParticleSystem::addEmitter
EmitterWPtr ParticleSystem::addEmitter( void )
{
	Emitter* emitter = DC_NEW Emitter;
	m_emitters.push_back( emitter );

	return emitter;
}

// ** ParticleSystem::createInstance
ParticleSystemInstancePtr ParticleSystem::createInstance( IMaterialFactoryWPtr materialFactory ) const
{
	return ParticleSystemInstancePtr( DC_NEW ParticleSystemInstance( materialFactory, const_cast<ParticleSystem*>( this ) ) );
}

// ** ParticleSystem::createFromFile
ParticleSystemPtr ParticleSystem::createFromFile( const String& fileName, f32 scalingFactor )
{
	// Read the JSON file
	String json = io::DiskFileSystem::readTextFile( fileName );

	// Load particle system
	return createFromJson( json, scalingFactor );
}

// ** ParticleSystem::createFromJson
ParticleSystemPtr ParticleSystem::createFromJson( const String& json, f32 scalingFactor )
{
#ifdef DC_JSON_ENABLED
	// Create particle system instance
	ParticleSystemPtr particleSystem( DC_NEW ParticleSystem );

	// Load particle system from JSON
	JsonParticleSystemLoader loader;

	if( !loader.load( particleSystem, json, scalingFactor ) ) {
		return ParticleSystemPtr();
	}

	return particleSystem;
#else
	log::error( "ParticleSystem::createFromJson : failed to load particle system, built with no JSON support.\n" );
	return ParticleSystemPtr();
#endif
}

// ---------------------------------------- ParticleSystemInstance ---------------------------------------- //

// ** ParticleSystemInstance::ParticleSystemInstance
ParticleSystemInstance::ParticleSystemInstance( IMaterialFactoryWPtr materialFactory, ParticleSystemWPtr particleSystem ) : m_particleSystem( particleSystem ), m_position( 0.0f, 0.0f, 0.0f ), m_aliveCount( 0 ), m_timeScale( 1.0f )
{
	for( int i = 0, n = m_particleSystem->emitterCount(); i < n; i++ ) {
		m_emitters.push_back( m_particleSystem->emitter( i )->createInstance( materialFactory ) );
	}
}

// ** ParticleSystemInstance::position
const Vec3& ParticleSystemInstance::position( void ) const
{
	return m_position;
}

// ** ParticleSystemInstance::setPosition
void ParticleSystemInstance::setPosition( const Vec3& value )
{
	m_position = value;
}

// ** ParticleSystemInstance::timeScale
f32 ParticleSystemInstance::timeScale( void ) const
{
	return m_timeScale;
}

// ** ParticleSystemInstance::setTimeScale
void ParticleSystemInstance::setTimeScale( f32 value )
{
	m_timeScale = value;
}

// ** ParticleSystemInstance::aliveCount
s32 ParticleSystemInstance::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** ParticleSystemInstance::emitterCount
s32 ParticleSystemInstance::emitterCount( void ) const
{
	return static_cast<s32>( m_emitters.size() );
}

// ** ParticleSystemInstance::emitter
EmitterInstanceWPtr ParticleSystemInstance::emitter( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= emitterCount() );
	return m_emitters[index];
}

// ** ParticleSystemInstance::hasEnded
bool ParticleSystemInstance::hasEnded( void ) const
{
	for( u32 i = 0, n = ( u32 )m_emitters.size(); i < n; i++ ) {
		if( !m_emitters[i]->hasEnded() ) {
			return false;
		}
	}

	return true;
}

// ** ParticleSystemInstance::bounds
Bounds ParticleSystemInstance::bounds( void ) const
{
	Bounds result;

	for( s32 i = 0, ne = emitterCount(); i < ne; i++ ) {
		EmitterInstanceWPtr emitter = this->emitter( i );

		for( s32 j = 0, np = emitter->particlesCount(); j < np; j++ ) {
			ParticlesInstanceWPtr particles = emitter->particles( j );

			if( !particles->aliveCount() ) {
				continue;
			}

			result += particles->bounds();
		}
	}

	return result;
}

// ** ParticleSystemInstance::update
s32 ParticleSystemInstance::update( f32 dt )
{
	m_aliveCount = 0;

	for( u32 i = 0, n = ( u32 )m_emitters.size(); i < n; i++ ) {
		m_aliveCount += m_emitters[i]->update( dt * m_timeScale, m_position );
	}

	return m_aliveCount;
}

// ** ParticleSystemInstance::warmUp
void ParticleSystemInstance::warmUp( f32 dt )
{
	for( u32 i = 0, n = ( u32 )m_emitters.size(); i < n; i++ ) {
		m_emitters[i]->warmUp( dt, m_position );
	}
}

// -------------------------------------- JsonParticleSystemLoader -------------------------------------- //

#ifdef DC_JSON_ENABLED

// ** JsonParticleSystemLoader::readRenderer
bool JsonParticleSystemLoader::readRenderer( const Json::Value& object )
{
	// Find the emitter by id
	String id = object["sceneObject"].asString();
	EmitterWPtr emitter = m_emitters.find( id )->second;

	// Set the particles texture.
	emitter->particles( 0 )->setMaterial( object["materials"][0].asString() );

	return true;
}

// ** JsonParticleSystemLoader::readEmitter
bool JsonParticleSystemLoader::readEmitter( const Json::Value& object )
{
	// Add the emitter to a particle system.
	String id = object["id"].asString();
	m_emitters[id] = m_particleSystem->addEmitter();

	return true;
}

// ** JsonParticleSystemLoader::readParticles
bool JsonParticleSystemLoader::readParticles( const Json::Value& object )
{
	// Get the parent scene object
	String id = object["sceneObject"].asString();

	// Find emitter by id
	EmitterWPtr emitter = m_emitters.find( id )->second;

	// Add particles to the emitter
	ParticlesWPtr particles = emitter->addParticles();

	// Setup emitter
	emitter->setLooped( object["isLooped"].asBool() );
	emitter->setDuration( object["duration"].asFloat() );

	// Setup zone
	Json::Value shape = object["shape"];

	switch( shape["type"].asInt() ) {
	case 2: emitter->setZone( DC_NEW HemiSphereZone( shape["radius"].asFloat() * m_scalingFactor ) ); break;
	case 5: emitter->setZone( DC_NEW BoxZone( shape["width"].asFloat() * m_scalingFactor, shape["height"].asFloat() * m_scalingFactor, shape["depth"].asFloat() * m_scalingFactor ) ); break;
	default: DC_NOT_IMPLEMENTED;
	}

	// Setup particles
	for( Json::ValueConstIterator i = object.begin(), end = object.end(); i != end; ++i ) {
		if( !i->isObject() ) {
			continue;
		}

		// Find the module loader
		ModuleLoaders::const_iterator j = m_moduleLoaders.find( i.key().asString() );

		if( j == m_moduleLoaders.end() ) {
			printf( "unhandled module %s\n", i.key().asString().c_str() );
			continue;
		}

		j->second( particles, *i );
	}

	return true;
}

// ** JsonParticleSystemLoader::readColor
bool JsonParticleSystemLoader::readColor( ParticlesWPtr particles, const Json::Value& object )
{
	readColorParameter( particles->colorParameter( Particles::ColorOverLife ), object["rgb"] );
	readScalarParameter( particles->scalarParameter( Particles::TransparencyOverLife ), object["alpha"] );
	return true;
}

// ** JsonParticleSystemLoader::readEmission
bool JsonParticleSystemLoader::readEmission( ParticlesWPtr particles, const Json::Value& object )
{
	readScalarParameter( particles->scalarParameter( Particles::Emission ), object["rate"] );

	Json::Value bursts = object.get( "bursts", Json::Value::null );

	for( s32 i = 0, n = bursts.size() / 2; i < n; i++ ) {
		particles->addBurst( bursts[i * 2 + 0].asFloat(), bursts[i * 2 + 1].asInt() );
	}

	return true;
}

// ** JsonParticleSystemLoader::readAcceleration
bool JsonParticleSystemLoader::readAcceleration( ParticlesWPtr particles, const Json::Value& object )
{
	readScalarParameter( particles->scalarParameter( Particles::AccelerationXOverLife ), object["x"], m_scalingFactor );
	readScalarParameter( particles->scalarParameter( Particles::AccelerationYOverLife ), object["y"], m_scalingFactor );
	readScalarParameter( particles->scalarParameter( Particles::AccelerationZOverLife ), object["z"], m_scalingFactor );

	return true;
}

// ** JsonParticleSystemLoader::readVelocity
bool JsonParticleSystemLoader::readVelocity( ParticlesWPtr particles, const Json::Value& object )
{
	readScalarParameter( particles->scalarParameter( Particles::VelocityXOverLife ), object["x"], m_scalingFactor );
	readScalarParameter( particles->scalarParameter( Particles::VelocityYOverLife ), object["y"], m_scalingFactor );
	readScalarParameter( particles->scalarParameter( Particles::VelocityZOverLife ), object["z"], m_scalingFactor );

	return true;
}

// ** JsonParticleSystemLoader::readSize
bool JsonParticleSystemLoader::readSize( ParticlesWPtr particles, const Json::Value& object )
{
	readScalarParameter( particles->scalarParameter( Particles::SizeOverLife ), object["curve"] );
	return true;
}

// ** JsonParticleSystemLoader::readAngularVelocity
bool JsonParticleSystemLoader::readAngularVelocity( ParticlesWPtr particles, const Json::Value& object )
{
	readScalarParameter( particles->scalarParameter( Particles::AngularVelocity ), object["curve"] );
	return true;
}

// ** JsonParticleSystemLoader::readInitial
bool JsonParticleSystemLoader::readInitial( ParticlesWPtr particles, const Json::Value& object )
{
	particles->setCount( object["maxParticles"].asInt() );

	readColorParameter( particles->colorParameter( Particles::Color ), object["rgb"] );
	readScalarParameter( particles->scalarParameter( Particles::Life ), object["life"] );
	readScalarParameter( particles->scalarParameter( Particles::Transparency ), object["alpha"] );
	readScalarParameter( particles->scalarParameter( Particles::Size ), object["size"], m_scalingFactor );
	readScalarParameter( particles->scalarParameter( Particles::Speed ), object["speed"], m_scalingFactor );
	readScalarParameter( particles->scalarParameter( Particles::Gravity ), object["gravity"], m_scalingFactor );

	return true;
}

// ** JsonParticleSystemLoader::readColorParameter
void JsonParticleSystemLoader::readColorParameter( RgbParameter& parameter, const Json::Value& object )
{
	DC_BREAK_IF( !object.isArray() );

	parameter.setEnabled( true );

	switch( object.size() ) {
	case 2:		{
					parameter.setRandomBetweenCurves( readFloats( object[0] ), readFloats( object[1] ) );
				}
				break;

	case 3:		{
					parameter.setConstant( readRgb( object ) );
				}
				break;

	default:	{
					parameter.setCurve( readFloats( object ) );
				}
				break;
	}
}

// ** JsonParticleSystemLoader::readScalarParameter
void JsonParticleSystemLoader::readScalarParameter( FloatParameter& parameter, const Json::Value& object, f32 scalingFactor )
{
	s32 size = object.isArray() ? object.size() : 1;

	switch( size ) {
	case 1:		{
					parameter.setConstant( object.asFloat() );
				}
				break;
	case 2:		{
					if( object[0].isArray() ) {
						parameter.setRandomBetweenCurves( readFloats( object[0] ), readFloats( object[1] ) );
					} else {
						FloatArray range = readFloats( object );
						parameter.setRandomBetweenConstants( range[0], range[1] );
					}
				}
				break;

	case 3:		{
					DC_NOT_IMPLEMENTED
				}
				break;

	default:	{
					parameter.setCurve( readFloats( object ) );
				}
	}

	parameter.setEnabled( true );
	parameter.scale( scalingFactor );
}

// ** JsonParticleSystemLoader::load
bool JsonParticleSystemLoader::load( ParticleSystemWPtr particleSystem, const String& json, f32 scalingFactor )
{
	// Save the particle system reference.
	m_particleSystem = particleSystem;

	// Sets the particle system scaling factor.
	m_scalingFactor = scalingFactor;

	m_moduleLoaders["color"]			= dcThisMethod( JsonParticleSystemLoader::readColor );
	m_moduleLoaders["emission"]			= dcThisMethod( JsonParticleSystemLoader::readEmission );
	m_moduleLoaders["velocity"]			= dcThisMethod( JsonParticleSystemLoader::readVelocity );
	m_moduleLoaders["acceleration"]		= dcThisMethod( JsonParticleSystemLoader::readAcceleration );
	m_moduleLoaders["angularVelocity"]	= dcThisMethod( JsonParticleSystemLoader::readAngularVelocity );
	m_moduleLoaders["size"]				= dcThisMethod( JsonParticleSystemLoader::readSize );
	m_moduleLoaders["initial"]			= dcThisMethod( JsonParticleSystemLoader::readInitial );

	registerLoader( "Particles", dcThisMethod( JsonParticleSystemLoader::readParticles ) );
	registerLoader( "SceneObject", dcThisMethod( JsonParticleSystemLoader::readEmitter ) );
	registerLoader( "ParticleRenderer", dcThisMethod( JsonParticleSystemLoader::readRenderer ) );

	JsonLoaderBase::load( json );

	return true;
}

#endif	/*	DC_JSON_ENABLED	*/

} // namespace Fx

DC_END_DREEMCHEST
