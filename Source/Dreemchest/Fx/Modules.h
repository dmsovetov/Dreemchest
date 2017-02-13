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

#ifndef __DC_Fx_Modules_H__
#define __DC_Fx_Modules_H__

#include "Particles.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

    //! Simulation state data.
    struct SimulationState {
        f32            m_time;            //!< Current emitter time.
        f32            m_dt;            //!< Frame delta time.
        Vec3        m_direction;    //!< Current emitter direction.
    };

    //! Base class for all particle modules.
    class AbstractModule : public RefCounted {
    public:

        //! Updates the block of particles.
        virtual void            update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const = 0;

        //! Returns module name.
        virtual String            name( void ) const = 0;

        //! Returns module update priority.
        virtual s32             priority( void ) const = 0;
    };

    //! Generic class to simplify new module declaraion.
    template<typename TModule, typename TParameter, s32 TPriority = 0>
    class Module : public AbstractModule {
    public:

        //! Returns module parameter value.
        const TParameter&        get( void ) const;
        TParameter&                get( void );

        //! Returns module name.
        virtual String            name( void ) const NIMBLE_OVERRIDE;

       //! Returns module execution priority.
        virtual s32             priority( void ) const NIMBLE_OVERRIDE;

    protected:

        TParameter                m_value;    //!< Parameter curve.
    };

    // ** Module::get
    template<typename TModule, typename TParameter, s32 TPriority>
    const TParameter& Module<TModule, TParameter, TPriority>::get( void ) const
    {
        return m_value;
    }

    // ** Module::get
    template<typename TModule, typename TParameter, s32 TPriority>
    TParameter& Module<TModule, TParameter, TPriority>::get( void )
    {
        return m_value;
    }

    // ** Module::name
    template<typename TModule, typename TParameter, s32 TPriority>
    String Module<TModule, TParameter, TPriority>::name( void ) const
    {
        return TypeInfo<TModule>::name();
    }

    // ** Module::priority
    template<typename TModule, typename TParameter, s32 TPriority>
    s32 Module<TModule, TParameter, TPriority>::priority( void ) const
    {
        return TPriority;
    }

    //! Initial life module setups the spawned particle.
    class InitialLife : public Module<InitialLife, FloatParameter, -1> {
    protected:

        //! Initializes the particle life properties.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Life module controls the particle life and calculates scalar value used to sample parameters.
    class Life : public Module<Life, FloatParameter, -1> {
    protected:

        //! Computes current life for each particle.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Velocity module computes the linear motion of particles.
    class LinearVelocity : public AbstractModule {
    public:

        //! Returns scalar parameter for linear velocity along the X axis.
        FloatParameter&        x( void );

        //! Returns scalar parameter for linear velocity along the Y axis.
        FloatParameter&        y( void );

        //! Returns scalar parameter for linear velocity along the Z axis.
        FloatParameter&        z( void );

        //! Returns module name.
        virtual String        name( void ) const NIMBLE_OVERRIDE { return "LinearVelocity"; }

        //! Returns module execution priority.
        virtual s32            priority( void ) const NIMBLE_OVERRIDE { return 2; }

    protected:

        //! Updates particle position according to a linear velocity.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;

    private:

        FloatParameter        m_velocity[3];    //!< Linear velocity along coordinate axes.
    };

    //! Clamp velocity module limits the maxiumum particle vlocity.
    class LimitVelocity : public Module<LimitVelocity, FloatParameter, 3> {
    protected:

        //! Clamps the particle's velocity.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Sets the initial particle size.
    class InitialSize : public Module<InitialSize, FloatParameter> {
    protected:

        //! Sets the initial particle size.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Calculates the particle size based on parameter and current particle scalar.
    class Size : public Module<Size, FloatParameter> {
    protected:

        //! Sets the life time particle size.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Sets the initial particle transparency.
    class InitialTransparency : public Module<InitialTransparency, FloatParameter> {
    protected:

        //! Sets the initial particle transparency.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Calculates the particle transparency based on parameter and current particle scalar.
    class Transparency : public Module<Transparency, FloatParameter> {
    protected:

        //! Sets the life time particle transparency.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Sets the initial angular velocity of a particle.
    class InitialAngularVelocity : public Module<InitialAngularVelocity, FloatParameter> {
    protected:

        //! Sets the initial angular velocity of a particle.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Sets the initial particle rotation.
    class InitialRotation : public Module<InitialRotation, FloatParameter> {
    protected:

        //! Sets the initial rotation of a particle.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;    
    };

    //! Calculates the particle rotation based on particle's angular velocity.
    class Rotation : public Module<Rotation, FloatParameter> {
    protected:

        //! Updates particle rotation.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Sets the initial particle force based on direction & speed.
    class InitialSpeed : public Module<InitialSpeed, FloatParameter> {
    protected:

        //! Sets the initial speed of a particle.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Adds the gravity to a particle force.
    class InitialGravity : public Module<InitialGravity, FloatParameter> {
    protected:

        //! Adds the gravity to a particle force.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;    
    };

    //! Modifies particle position by a velocity due to applied forces.
    class Acceleration : public Module<Acceleration, FloatParameter, 1> {
    protected:

        //! Calculates additional particle velocity based on acceleration. 
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;    
    };

    //! Modifies particle position by an aggregated linear velocity.
    class Position : public AbstractModule {
    protected:

        //! Returns module name.
        virtual String        name( void ) const NIMBLE_OVERRIDE { return "Position"; }

        //! Returns module execution priority.
        virtual s32            priority( void ) const NIMBLE_OVERRIDE { return INT_MAX; }

    protected:

        //! Updates particle position according to a linear velocity.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;
    };

    //! Sets the initial particle color.
    class InitialColor : public Module<InitialColor, RgbParameter> {
    protected:

        //! Sets the initial color for each constructed particle.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;        
    };

    //! Updates the particle color.
    class Color : public Module<Color, RgbParameter> {
    protected:

        //! Calculates particle color tint.
        virtual void        update( Particle* particles, s32 first, s32 last, const SimulationState& state ) const NIMBLE_OVERRIDE;        
    };

} // namespace Fx

DC_END_DREEMCHEST

#endif        /*    !__DC_Fx_Modules_H__    */
