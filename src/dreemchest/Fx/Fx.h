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

#ifndef __DC_Fx_H__
#define __DC_Fx_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

	DECLARE_LOG(log)

	typedef Array<f32> FloatArray;

	struct Particle;

	dcDeclarePtrs( Emitter )
	dcDeclarePtrs( EmitterInstance )
	dcDeclarePtrs( Particles )
	dcDeclarePtrs( ParticlesInstance )
	dcDeclarePtrs( ParticleSystem )
	dcDeclarePtrs( ParticleSystemInstance )
	dcDeclarePtrs( Zone )
	dcDeclarePtrs( ParticleRenderer )
	dcDeclarePtrs( IRenderingInterface )
	dcDeclarePtrs( IMaterial )
	dcDeclarePtrs( IMaterialFactory )

	//! Container type to store particle system emitters.
	typedef Array<EmitterPtr>			EmittersArray;
	typedef Array<EmitterInstancePtr>	EmitterInstancesArray;

	//! Container type to store particles.
	typedef Array<ParticlesPtr>			ParticlesArray;
	typedef Array<ParticlesInstancePtr>	ParticlesInstancesArray;

	//! Modifier type.
	enum ModifierType {
		  ModifierFriction
		, ModifierForce
		, ModifierForceField

		, TotalModifierTypes
	};
        
	//! Particle rendering modes.
	enum RenderingMode {
		  RenderPoints
		, RenderQuads
        , RenderLines
        , RenderThickLines
        , RenderPaths
        , RenderThickPaths

        , TotalRenderingModes
	};

	//! Rendering blending modes.
	enum BlendingMode {
		  BlendAlpha
		, BlendAdditive
		, BlendPremultiplied

		, TotalBlendingModes
	};

	//! Parameter sampling mode.
	enum SamplingMode {
		  SampleConstant
		, SampleCurve
		, SampleRandomBetweenConstants
		, SampleRandomBetweenCurves
		
		, TotalSamplingModes
	};

    //! Available particle emission zones.
    enum ZoneType {
          ZoneDisk
        , ZoneLine
		, ZoneBox
		, ZoneHemiSphere
		, ZoneSphere

        , TotalZoneTypes
	};

} // namespace Fx

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "ParticleSystem.h"
	#include "Emitter.h"
	#include "Particles.h"
	#include "Renderers.h"
#endif

#endif	/*	!__DC_Fx_H__	*/