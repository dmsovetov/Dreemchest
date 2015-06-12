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

#include "Aspect.h"
#include "Entity.h"

DC_BEGIN_DREEMCHEST

namespace ecs {

// ** Aspect::Aspect
Aspect::Aspect( AspectType type, const Bitset& mask )
{
	switch( type ) {
	case All:		m_all = mask; break;
	case Any:		m_any = mask; break;
	case Exclude:	m_exc = mask; break;
	}
}

// ** Aspect::hasIntersection
bool Aspect::hasIntersection( const EntityPtr& entity ) const
{
	const Bitset& mask = entity->mask();

	if( m_all ) {
		for( int i = 0, n = m_all.size(); i < n; i++ ) {
			if( m_all.is( i ) && !mask.is( i ) ) {
				return false;
			}
		}
	}

	if( m_any ) {
		if( m_any * mask == false ) {
			return false;
		}
	}

	if( m_exc ) {
		if( m_exc * mask == true ) {
			return false;
		}
	}

	return true;
}

} // namespace ecs

DC_END_DREEMCHEST
