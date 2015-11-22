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

#ifndef __DC_Particles_ParameterContainer_H__
#define __DC_Particles_ParameterContainer_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include "Parameter.h"

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

#endif /*	!__DC_Particles_ParameterContainer_H__	*/