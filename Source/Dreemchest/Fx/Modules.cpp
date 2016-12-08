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

#include "Modules.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

// ------------------------------------------------------ InitialLife ------------------------------------------------------ //

// ** InitialLife::update
void InitialLife::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Life* life = particles->life;

    for( s32 i = first; i < last; i++ ) {
        life[i].current = life[i].initial = m_value.sample( 0, state.m_time, 1.0f );
    }
}

// --------------------------------------------------------- Life --------------------------------------------------------- //

// ** Life::update
void Life::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    for( s32 i = first; i < last; i++ ) {
        Particle::Life* life = &particles->life[i];
        life->current -= state.m_dt;
        life->scalar = min2( 1.0f, 1.0f - life->current / life->initial );
    }
}

// ------------------------------------------------------ LinearVelocity ------------------------------------------------------ //

// ** LinearVelocity::x
FloatParameter&    LinearVelocity::x( void )
{
    return m_velocity[0];
}

// ** LinearVelocity::y
FloatParameter&    LinearVelocity::y( void )
{
    return m_velocity[1];
}

// ** LinearVelocity::z
FloatParameter&    LinearVelocity::z( void )
{
    return m_velocity[2];
}

// ** LinearVelocity::update
void LinearVelocity::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    const Particle::Life* life       = particles->life;
    const u32*              indices  = particles->indices;
    Vec3*                 velocity = particles->velocity;

    for( s32 i = first; i < last; i++ ) {
        f32 scalar = life[i].scalar;
        u32 idx       = indices[i];

        velocity[i].x += m_velocity[0].sample( idx, scalar, 0.0f );
        velocity[i].y += m_velocity[1].sample( idx, scalar, 0.0f );
        velocity[i].z += m_velocity[2].sample( idx, scalar, 0.0f );
    }
}

// ----------------------------------------------------- LimitVelocity ----------------------------------------------------- //

// ** LimitVelocity::update
void LimitVelocity::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    const Particle::Life* life       = particles->life;
    const u32*              indices  = particles->indices;
    Vec3*                 velocity = particles->velocity;

    for( s32 i = first; i < last; i++ ) {
        f32 maximum = m_value.sample( indices[i], life[i].scalar, 0.0f );
        f32 current = velocity[i].length();

        if( current <= maximum ) {
            continue;
        }

        velocity[i] = velocity[i] / current * maximum;
    }
}

// ------------------------------------------------------ InitialSize ------------------------------------------------------ //

// ** InitialSize::update
void InitialSize::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    for( s32 i = first; i < last; i++ ) {
        particles->size[i].initial = particles->size[i].current = m_value.sample( 0, state.m_time, 5 );
    }
}

// --------------------------------------------------------- Size ---------------------------------------------------------- //

// ** Size::update
void Size::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Scalar*      size    = particles->size;
    const Particle::Life* life    = particles->life;
    const u32*              indices = particles->indices;

    for( s32 i = first; i < last; i++ ) {
        size[i].current = size[i].initial * m_value.sample( indices[i], life[i].scalar, 1.0f );
    }
}

// -------------------------------------------------- InitialTransparency -------------------------------------------------- //

// ** InitialTransparency::update
void InitialTransparency::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Scalar* transparency    = particles->transparency;

    for( s32 i = first; i < last; i++ ) {
        transparency[i].initial = transparency[i].current = m_value.sample( 0, state.m_time, 1.0f );
    }
}

// ----------------------------------------------------- Transparency ------------------------------------------------------ //

// ** Transparency::update
void Transparency::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Scalar*      transparency    = particles->transparency;
    const Particle::Life* life            = particles->life;
    const u32*              indices        = particles->indices;

    for( s32 i = first; i < last; i++ ) {
        transparency[i].current = transparency[i].initial * m_value.sample( indices[i], life[i].scalar, 1.0f );
    }
}

// ----------------------------------------------------- InitialAngularVelocity ------------------------------------------------------ //

// ** InitialAngularVelocity::update
void InitialAngularVelocity::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Scalar* angular = particles->angularVelocity;

    for( s32 i = first; i < last; i++ ) {
        angular[i].current = angular[i].initial = m_value.sample( 0, state.m_time, 0.0f );
    }
}

// -------------------------------------------------------- InitialRotation ---------------------------------------------------------- //

// ** InitialRotation::update
void InitialRotation::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    f32* rotation = particles->rotation;

    for( s32 i = first; i < last; i++ ) {
        rotation[i] = m_value.sample( 0, state.m_time, 0.0f );
    }
}

// ----------------------------------------------------------- Rotation -------------------------------------------------------------- //

// ** Rotation::update
void Rotation::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    const Particle::Scalar* angular  = particles->angularVelocity;
    f32*                    rotation = particles->rotation;

    for( s32 i = first; i < last; i++ ) {
        rotation[i] += angular[i].current * state.m_dt;
    }
}

// --------------------------------------------------------- InitialSpeed ------------------------------------------------------------ //

// ** InitialSpeed::update
void InitialSpeed::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Force* force = particles->force;

    for( s32 i = first; i < last; i++ ) {
        force[i].velocity *= m_value.sample( 0, state.m_time, 0 );
    }
}

// --------------------------------------------------------- InitialGravity ---------------------------------------------------------- //

// ** InitialGravity::update
void InitialGravity::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Force* force = particles->force;

    for( s32 i = first; i < last; i++ ) {
        force[i].acceleration.y -= m_value.sample( 0, state.m_time, 0 );
    }
}

// ---------------------------------------------------------- Acceleration ----------------------------------------------------------- //

// ** Acceleration::update
void Acceleration::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Vec3*            velocity = particles->velocity;
    Particle::Force* force      = particles->force;

    for( s32 i = first; i < last; i++ ) {
        force[i].velocity += force[i].acceleration * state.m_dt;
        velocity[i]        = force[i].velocity;
    }
}

// ------------------------------------------------------------ Position ------------------------------------------------------------- //

// ** Position::update
void Position::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    const Vec3* velocity = particles->velocity;
    const Particle::Force* force      = particles->force;
    Vec3*        position = particles->position;

    for( s32 i = first; i < last; i++ ) {
        position[i] += (velocity[i] /*+ force[i].velocity*/) * state.m_dt;
    }
}

// ----------------------------------------------------------- InitialColor ----------------------------------------------------------- //

// ** InitialColor::update
void InitialColor::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Color* color = particles->color;
    Rgb                 white = Rgb( 1.0f, 1.0f, 1.0f );

    for( s32 i = first; i < last; i++ ) {
        color[i].current = color[i].initial = m_value.sample( 0, state.m_time, white );
    }
}

// -------------------------------------------------------------- Color -------------------------------------------------------------- //

// ** Color::update
void Color::update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const
{
    Particle::Color*      color   = particles->color;
    const Particle::Life* life      = particles->life;
    const u32*              indices = particles->indices;
    Rgb                      white      = Rgb( 1.0f, 1.0f, 1.0f );

    for( s32 i = first; i < last; i++ ) {
        color[i].current = color[i].initial * m_value.sample( indices[i], life[i].scalar, white );
    }
}

} // namespace Fx

DC_END_DREEMCHEST
