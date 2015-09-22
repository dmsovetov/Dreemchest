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

#ifndef __Base_Composition_H__
#define __Base_Composition_H__

DC_BEGIN_DREEMCHEST

	//! A base class for composite types.
	/*!
	 Composite type owns it's parts and they get destroyed with a parent object.
	 */
	template<typename TPart>
	class Composition {
	public:

		//! Container type to store composition parts.
		typedef Map<TypeIdx, TPart*>	Parts;

        //! Attaches a composition item.
        template<typename T>                         T* attach( void )                  { return set<T>( new T );                           }
        template<typename T, TemplateFunctionTypes1> T* attach( TemplateFunctionArgs1 ) { return set<T>( new T( arg0 ) );                   }
        template<typename T, TemplateFunctionTypes2> T* attach( TemplateFunctionArgs2 ) { return set<T>( new T( arg0, arg1 ) );             }
        template<typename T, TemplateFunctionTypes3> T* attach( TemplateFunctionArgs3 ) { return set<T>( new T( arg0, arg1, arg2 ) );       }
        template<typename T, TemplateFunctionTypes4> T* attach( TemplateFunctionArgs4 ) { return set<T>( new T( arg0, arg1, arg2, arg3 ) ); }

        //! Clears a composition.
        void clear( void )
        {
            for( typename Parts::iterator i = m_composition.begin(), end = m_composition.end(); i != end; ++i ) {
                delete i->second;
            }
            m_composition.clear();
        }

		//! Returns a pointer to a composition part with specified type T.
		template<typename T>
		const T* get( void ) const
		{
			return const_cast<Composition*>( this )->get<T>();
		}

		//! Returns a pointer to a composition part with specified type T.
		template<typename T>
		T* get( void )
		{
			typename Parts::iterator i = m_composition.find( TypeIndex<T>::idx() );
			return i != m_composition.end() ? static_cast<T*>( i->second ) : NULL;
		}

		//! Returns true if a composition holds a pointer of specified type T.
		template<typename T>
		bool has( void ) const
		{
			return m_composition.find( TypeIndex<T>::idx() ) != m_composition.end();
		}

		//! Removes the composition part of specified type T.
		template<typename T>
		void detach( void )
		{
			typename Parts::iterator i = m_composition.find( TypeIndex<T>::idx() );

			if( i != m_composition.end() ) {
				delete static_cast<T*>( i->second );
				m_composition.erase( i );
			}
		}

        //! Returns composition parts.
        const Parts&    parts( void ) const { return m_composition; }
        Parts&          parts( void ) { return m_composition; }

    private:

        //! Sets a part of composition to a specified type T.
		template<typename T>
		T* set( T* instance )
		{
			m_composition[TypeIndex<T>::idx()] = instance;
			return instance;
		}

	private:

		Parts	m_composition;	//!< Actual composition.
	};

DC_END_DREEMCHEST

#endif	/*	!__Base_Composition_H__	*/
