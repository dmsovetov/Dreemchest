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

#ifndef __DC_Particles_ParticleModelNode_H__
#define __DC_Particles_ParticleModelNode_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include "../Dreemchest.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

DC_BEGIN_DREEMCHEST

namespace Particles {

	// ** class IParticleModelNode
	class IParticleModelNode {
	public:

		virtual						~IParticleModelNode( void ) {}
	};

	typedef Array<f32> FloatArray;

	//! Available particle modifiers types.
	enum ModifierType {
		ModifierFriction,
		ModifierForce,
		ModifierForceField,

		TotalModifierTypes
	};
        
	//! Particle rendering modes.
	enum RendererType {
		RenderPoints,
        RenderQuads,
        RenderLines,
        RenderThickLines,
        RenderPaths,
        RenderThickPaths,

        TotalRenderTypes,
	};

	//! Particle blending modes.
	enum BlendMode {
		BlendNormal,
		BlendAdditive,
		BlendPremultiplied,

		TotalBlendModes
	};

    //! Particle emission zones.
    enum ZoneType {
		ZoneNone,
        ZoneDisk,
        ZoneLine,

        TotalZoneTypes,
    };

	class IParticleModelNode;
		class Parameter;
		class ParameterContainer;
			class Zone;
            class ZoneContainer;
                class Modifier;
                class ModifierContainer;
                    class ParticleSystemModel;
                    class EmitterModel;
                    class ParticleModel;

	class Renderer;
	class ParticleSystem;
	class Emitter;
	class Particles;

	struct sParticle;

	typedef Array<Modifier*>		ModifierArray;
	typedef Array<EmitterModel*>	EmitterModelArray;
	typedef Array<ParticleModel*>	ParticleModelArray;
	typedef Array<Emitter*>			EmitterArray;
	typedef Array<Particles*>		ParticlesArray;
	typedef List<sParticle*>		ParticleItemList;

	typedef void*					dcBatchRenderer;
	typedef void*					dcTextureAsset;

} // namespace Particles

DC_END_DREEMCHEST

#endif /*	!__DC_Particles_ParticleModelNode_H__	*/