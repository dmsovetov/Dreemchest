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

#include "Particles.h"

#include "Renderers.h"
#include "Zones.h"
#include "Modules.h"

#define ScalarParam( name ) m_scalar[name] ? &m_scalar[name] : NULL
#define ColorParam( name )  m_color[name]  ? &m_color[name]  : NULL

DC_BEGIN_DREEMCHEST

namespace Fx {

// ----------------------------------------------- Particles ----------------------------------------------- //

// ** Particles::Particles
Particles::Particles( EmitterWPtr emitter ) : m_emitter( emitter ), m_count( 1 )
{
	setRenderingMode( RenderQuads );

	// Add default particle modules
	addModule( DC_NEW Life );
	addModule( DC_NEW Acceleration );
};

// ** Particles::createInstance
ParticlesInstancePtr Particles::createInstance( IMaterialFactoryWPtr materialFactory ) const
{
	return ParticlesInstancePtr( DC_NEW ParticlesInstance( materialFactory, const_cast<Particles*>( this ) ) );
}

// ** Particles::emitter
EmitterWPtr Particles::emitter( void ) const
{
	return m_emitter;
}

// ** Particles::count
s32 Particles::count( void ) const
{
	return m_count;
}

// ** Particles::setCount
void Particles::setCount( s32 value )
{
	m_count = value;
}

// ** Particles::name
const String& Particles::name( void ) const
{
	return m_name;
}

// ** Particles::setName
void Particles::setName( const String& value )
{
	m_name = value;
}

// ** Particles::material
const String& Particles::material( void ) const
{
	return m_material;
}

// ** Particles::setMaterial
void Particles::setMaterial( const String& value )
{
	m_material = value;
}

// ** Particles::renderingMode
RenderingMode Particles::renderingMode( void ) const
{
	return m_renderingMode;
}

// ** Particles::setRenderingMode
void Particles::setRenderingMode( RenderingMode value )
{
	m_renderingMode = value;
}

// ** Particles::addModule
void Particles::addModule( const ModulePtr& module )
{
	DC_BREAK_IF( !module.valid() );
	log::verbose( "%s particle module added\n", module->name().c_str() );
	m_modules.push_back( module );
}

// ** Particles::update
void Particles::update( Particle* particles, s32 first, s32 last, f32 dt ) const
{
	// Construct the simulation state.
	SimulationState state;
	state.m_direction = Vec3( 0.0f, 0.0f, 0.0f );
	state.m_dt		  = dt;
	state.m_time	  = 0.0f;

	// Run all life time modules
	for( s32 i = 0, n = ( s32 )m_modules.size(); i < n; i++ ) {
		m_modules[i]->update( particles, first, last, state );
	}
}

// ------------------------------------------------- ParticlesInstance ------------------------------------------------- //

// ** ParticlesInstance::ParticlesInstance
ParticlesInstance::ParticlesInstance( IMaterialFactoryWPtr materialFactory, ParticlesWPtr particles ) : m_particles( particles ), m_aliveCount( 0 )
{
	s32 count = m_particles->count();

	m_items.life			= new Particle::Life[count];
	m_items.position		= new Vec3[count];
	m_items.size			= new Particle::Scalar[count];
	m_items.indices			= new u32[count];
	m_items.transparency	= new Particle::Scalar[count];
	m_items.rotation		= new f32[count];
	m_items.angularVelocity	= new Particle::Scalar[count];
	m_items.force			= new Particle::Force[count];
	m_items.color			= new Particle::Color[count];

	if( materialFactory.valid() ) {
		m_material = materialFactory->createMaterial( particles->material() );
	}
}

// ** ParticlesInstance::aliveCount
s32 ParticlesInstance::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** ParticlesInstance::addAliveCount
void ParticlesInstance::addAliveCount( s32 value )
{
	m_aliveCount += value;
}

// ** ParticlesInstance::maxCount
s32 ParticlesInstance::maxCount( void ) const
{
	return m_particles->count();
}

// ** ParticlesInstance::items
const Particle& ParticlesInstance::items( void ) const
{
	return m_items;
}

// ** ParticlesInstance::items
Particle& ParticlesInstance::items( void )
{
	return m_items;
}

// ** ParticlesInstance::renderingMode
RenderingMode ParticlesInstance::renderingMode( void ) const
{
	return m_particles->renderingMode();
}

// ** ParticlesInstance::material
IMaterialWPtr ParticlesInstance::material( void ) const
{
	return m_material;
}

// ** ParticlesInstance::setMaterial
void ParticlesInstance::setMaterial( const IMaterialWPtr& value )
{
	m_material = value;
}

// ** ParticlesInstance::bounds
const Bounds& ParticlesInstance::bounds( void ) const
{
	return m_bounds;
}

// ** ParticlesInstance::update
s32 ParticlesInstance::update( f32 dt )
{
	// Lock particles
	Particle* particles = &items();

	// Update particles
	m_particles->update( particles, 0, m_aliveCount, dt );

	// Calculate alive particles count & particle bounding box.
	s32    count = m_aliveCount;
	Bounds bounds;
	
	for( s32 i = 0; i < count; i++ ) {
		// Particle is alive - skip it
		if( m_items.life[i].current >= 0.0f ) {
			f32 size = -particles->size[i].current;
			bounds << particles->position[i] - Vec3( size, size, size ) << particles->position[i] + Vec3( size, size, size );
			continue;
		}

		// Decrease the alive particles counter
        count = count - 1;

		// Swap data
		particles->indices[i]			= particles->indices[count];
		particles->position[i]			= particles->position[count];
		particles->rotation[i]			= particles->rotation[count];
		particles->life[i]				= particles->life[count];
		particles->size[i]				= particles->size[count];
		particles->transparency[i]		= particles->transparency[count];
		particles->color[i]				= particles->color[count];
		particles->angularVelocity[i]	= particles->angularVelocity[count];
		particles->force[i]				= particles->force[count];

		// Decrease counter
        i = i - 1;
	}

	// Save alive count
	m_aliveCount = count;

	// Save particle bounds
	m_bounds = bounds;

	return m_aliveCount;
}

} // namespace Fx

DC_END_DREEMCHEST
