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

#ifndef __DC_Fx_ParticleBundle_H__
#define __DC_Fx_ParticleBundle_H__

#include "Parameter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

    //! Particle contains data for each of used modules.
    struct Particle {
        //! Stores the particle life time.
        struct Life {
            f32        current;        //!< Particle's life left in seconds.
            f32        initial;        //!< Particle's initial life.
            f32        scalar;            //!< Particle's parameter scalar.
        };

        //! Stores the data of a scalar particle parameter.
        struct Scalar {
            f32        initial;        //!< Initial parameter value.
            f32        current;        //!< Current parameter value.
        };

        //! Stores aggregated forces applied to particle.
        struct Force {
            Vec3    velocity;        //!< Additional particle velocity due to particle acceleration.
            Vec3    acceleration;    //!< Particle acceleration due to applied force.
        };

        //! Stores particle color.
        struct Color {
            Rgb        initial;        //!< Initial particle color.
            Rgb        current;        //!< Current particle color.
        };

        u32*            indices;            //!< Particle indices.
        Vec3*            position;            //!< Particle position.
        Vec3*           velocity;           //!< Particle linear velocity.
        f32*            rotation;            //!< Particle rotation.
        Life*            life;                //!< Particle life.
        Scalar*            size;                //!< Particle size.
        Scalar*            transparency;        //!< Particle transparency.
        Color*            color;                //!< Particle color.
        Scalar*            angularVelocity;    //!< Particle angular velocity.
        Force*            force;                //!< Particle external forces.

                        //! Constructs Particle instance.
                        Particle( void )
                            : indices( NULL ), position( NULL ), rotation( NULL ), life( NULL ), size( NULL ), transparency( NULL ), color( NULL ), angularVelocity( NULL ), force( NULL ) {}
                        ~Particle( void )
                        {
                            delete[]indices;
                            delete[]position;
                            delete[]velocity;
                            delete[]rotation;
                            delete[]life;
                            delete[]size;
                            delete[]transparency;
                            delete[]color;
                            delete[]angularVelocity;
                            delete[]force;
                        }
    };

    //! Particles contains an array of particles and a set of simulation parameters.
    class Particles : public RefCounted {
    friend class Emitter;
    public:

        //! Returns the maximum number of particles.
        s32                        count( void ) const;

        //! Sets the maximum number of particles.
        void                    setCount( s32 value );

        //! Returns the particle rendering mode.
        RenderingMode            renderingMode( void ) const;

        //! Sets the particle rendering mode.
        void                    setRenderingMode( RenderingMode value );

        //! Returns the particle material name.
        const String&            material( void ) const;

        //! Sets the particle material name.
        void                    setMaterial( const String& value );

        //! Returns parent emitter.
        EmitterWPtr                emitter( void ) const;

        //! Returns the particles name.
        const String&            name( void ) const;

        //! Sets the particles name.
        void                    setName( const String& value );

        //! Adds new module instance.
        void                    addModule( const ModulePtr& module );

        //! Updates the group of particles.
        void                    update( Particle* items, s32 first, s32 last, f32 dt ) const;

        //! Creates particles instance with a specified material factory.
        ParticlesInstancePtr    createInstance( IMaterialFactoryWPtr materialFactory ) const;

    private:

                                //! Constructs Particles instance.
                                Particles( EmitterWPtr emitter );

    private:

        EmitterWPtr                m_emitter;                            //!< Parent emitter instance.
        RenderingMode            m_renderingMode;                    //!< Particle rendering mode.
        s32                        m_count;                            //!< The maximum number of particles.
        String                    m_name;                                //!< Particles name.
        String                    m_material;                            //!< Particles material name.
        Array<ModulePtr>        m_modules;                            //!< Life time modules used by this particle system.
    };

    //! The particles instance.
    class ParticlesInstance : public RefCounted {
    friend class Particles;
    friend class EmitterInstance;
    public:

        //! Returns the total number of alive particles.
        s32                        aliveCount( void ) const;

        //! Returns the maximum number of particles.
        s32                        maxCount( void ) const;

        //! Returns the particle items.
        const Particle&            items( void ) const;
        Particle&                items( void );

        //! Returns the particles bounding box.
        const Bounds&            bounds( void ) const;

        //! Returns the particle rendering mode.
        RenderingMode            renderingMode( void ) const;

        //! Returns the particles material.
        IMaterialWPtr            material( void ) const;

        //! Sets the particles material.
        void                    setMaterial( const IMaterialWPtr& value );

        //! Updates the particles.
        s32                        update( f32 dt );

    private:

                                //! Constructs the ParticlesInstance instance.
                                ParticlesInstance( IMaterialFactoryWPtr materialFactory, ParticlesWPtr particles );

        //! Adds alive particles.
        void                    addAliveCount( s32 value );

    private:

        ParticlesWPtr            m_particles;    //!< Parent particles.
        Particle                m_items;        //!< Particle simulation data stored here.
        s32                        m_aliveCount;    //!< The total number of alive particles.
        Bounds                    m_bounds;        //!< Particles bounding box.
        IMaterialPtr            m_material;        //!< Particle material reference.
    };

} // namespace Fx
    
DC_END_DREEMCHEST

#endif        /*    !__DC_Fx_ParticleBundle_H__    */
