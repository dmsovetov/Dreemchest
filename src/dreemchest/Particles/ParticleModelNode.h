//
//  Filename:   ParticleModelNode.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_ParticleModelNode_H__
#define		__DC_ParticleModelNode_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include	"../Common.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

	// ** class IParticleModelNode
	class IParticleModelNode {

		DC_DECLARE_IS( ParticleSystemModel, ParticleSystem,		NULL );
		DC_DECLARE_IS( EmitterModel,		Emitter,			NULL );
		DC_DECLARE_IS( ParticleModel,		Particles,			NULL );
		DC_DECLARE_IS( Parameter,			Parameter,			NULL );
		DC_DECLARE_IS( ParameterContainer,	ParameterContainer,	NULL );
		DC_DECLARE_IS( Modifier,			Modifier,			NULL );
		DC_DECLARE_IS( ModifierContainer,	ModifierContainer,	NULL );
        DC_DECLARE_IS( ZoneContainer,       ZoneContainer,      NULL );
		DC_DECLARE_IS( Zone,				Zone,				NULL );
	
	public:

		virtual						~IParticleModelNode( void ) {}
	};

} // namespace particles

} // namespace dreemchest

#endif		/*	!__DC_ParticleModelNode_H__	*/