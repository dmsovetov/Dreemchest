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

#include "ModifierContainer.h"

#include "Modifiers.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

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

} // namespace Fx
    
DC_END_DREEMCHEST
