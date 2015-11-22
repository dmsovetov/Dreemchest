//
//  Filename:   ParameterContainer.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_ParameterContainer_H__
#define		__DC_ParameterContainer_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "Parameter.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

	// ** struct sParameterInfo
	struct sParameterInfo {
		enum {
			Initial  = BIT( 0 ),
			Lifetime = BIT( 1 ),
		};

		std::string		m_name;
		Parameter*		m_parameter;
		cFlagSet8		m_flags;
	};

	// ** class ParameterContainer
	class ParameterContainer : public IParticleModelNode {

		DC_DECLARE_IS( ParameterContainer, ParameterContainer, this );

		typedef std::vector<sParameterInfo> ParameterInfoArray;

	public:

		virtual					~ParameterContainer( void ) {}

		int						totalParameters( void ) const;
		const sParameterInfo&	parameterInfo( int index ) const;
		Parameter*				findParameterByName( const char *name ) const;

	protected:

		void					registerParameter( const char *name, Parameter *parameter, u8 flags );

	private:

		ParameterInfoArray		m_parameters;
	};

} // namespace particles
    
} // namespace dreemchest

#endif		/*	!__DC_ParameterContainer_H__	*/