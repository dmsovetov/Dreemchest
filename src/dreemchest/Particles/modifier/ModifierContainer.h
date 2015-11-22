//
//  Filename:   ModifierContainer.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_ModifierContainer_H__
#define		__DC_ModifierContainer_H__

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

namespace dreemchest {

namespace particles {

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

} // namespace particles
    
} // namespace dreemchest

#endif		/*	!__DC_ModifierContainer_H__	*/