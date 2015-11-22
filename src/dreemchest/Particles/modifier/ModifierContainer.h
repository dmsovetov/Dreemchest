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

#ifndef __DC_Particles_ModifierContainer_H__
#define __DC_Particles_ModifierContainer_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "../zone/ZoneContainer.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

DC_BEGIN_DREEMCHEST

namespace Particles {

	// ** struct sModifierInfo
	struct sModifierInfo {
		std::string			m_name;
		Modifier*			m_modifier;
	};

    // ** class ModifierContainer
    class ModifierContainer : public ZoneContainer {

		DC_DECLARE_IS( ModifierContainer, ModifierContainer, this );

		typedef std::vector<sModifierInfo> ModifierInfoArray;

    public:

								ModifierContainer( void );
        virtual					~ModifierContainer( void );

		int						totalModifiers( void ) const;
		const sModifierInfo&	modifierInfo( int index ) const;
		Modifier*				findModifierByName( const char *name ) const;

	protected:

		void					registerModifier( const char *name, Modifier *parameter );

	private:

		ModifierInfoArray		m_modifierInfo;
		ModifierArray			m_modifiers;
    };

} // namespace Particles
    
DC_END_DREEMCHEST

#endif /*	!__DC_Particles_ModifierContainer_H__	*/