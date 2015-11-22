//
//  Filename:   ModifierContainer.cpp
//	Created:	28:05:2011   18:32

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "ModifierContainer.h"

#include	"Modifiers.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

// ** ModifierContainer::ModifierContainer
ModifierContainer::ModifierContainer( void )
{
	m_modifiers.push_back( new Force );
	m_modifiers.push_back( new ForceField );
	m_modifiers.push_back( new Friction );

	registerModifier( "Force", m_modifiers[0] );
	registerModifier( "ForceField", m_modifiers[1] );
	registerModifier( "Friction", m_modifiers[2] );
}

ModifierContainer::~ModifierContainer( void )
{
	for( int i = 0, n = m_modifiers.size(); i < n; i++ ) {
		delete m_modifiers[i];
	}
}

// ** ModifierContainer::totalModifiers
int ModifierContainer::totalModifiers( void ) const
{
	return ( int )m_modifierInfo.size();
}

// ** ModifierContainer::modifierInfo
const sModifierInfo& ModifierContainer::modifierInfo( int index ) const
{
	DC_BREAK_IF( index < 0 || index >= totalModifiers() );
	return m_modifierInfo[index];
}

// ** ModifierContainer::findModifierByName
Modifier* ModifierContainer::findModifierByName( const char *name ) const
{
	for( int i = 0, n = totalModifiers(); i < n; i++ ) {
		if( m_modifierInfo[i].m_name == name ) {
			return m_modifierInfo[i].m_modifier;
		}
	}

	return NULL;
}

// ** ModifierContainer::registerModifier
void ModifierContainer::registerModifier( const char *name, Modifier *modifier )
{
	DC_BREAK_IF( findModifierByName( name ) );

	sModifierInfo info;
	info.m_name = name;
	info.m_modifier = modifier;

	m_modifierInfo.push_back( info );
}

} // namespace particles
    
} // namespace dreemchest