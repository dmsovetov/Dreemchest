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

#ifndef __DC_Mvvm_View_H__
#define __DC_Mvvm_View_H__

#include "Mvvm.h"
#include "Data.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

    //! View is a base class for view renderers and is a composition of data providers and actions handlers.
    class View {
    public:

        //! Attaches the data provider to a view controller.
        template<typename T>                         T* attachData( void )                  { return m_data.attach<T>();                            }
        template<typename T, TemplateFunctionTypes1> T* attachData( TemplateFunctionArgs1 ) { return m_data.attach<T>( arg0 );                      }
        template<typename T, TemplateFunctionTypes2> T* attachData( TemplateFunctionArgs2 ) { return m_data.attach<T>( arg0, arg1 );                }
        template<typename T, TemplateFunctionTypes3> T* attachData( TemplateFunctionArgs3 ) { return m_data.attach<T>( arg0, arg1, arg2 );          }
        template<typename T, TemplateFunctionTypes4> T* attachData( TemplateFunctionArgs4 ) { return m_data.attach<T>( arg0, arg1, arg2, arg3 );    }
		
		//! Returns a data with specified type.
        template<typename T> const T*				    data( void ) const { return m_data.get<T>(); }
        template<typename T> T*				            data( void ) { return m_data.get<T>(); }

        //! Attaches the action handler to a view controller.
        template<typename T>                         T* attachHandler( void )                  { return m_actionHandlers.attach<T>( this );                            }
        template<typename T, TemplateFunctionTypes1> T* attachHandler( TemplateFunctionArgs1 ) { return m_actionHandlers.attach<T>( this, arg0 );                      }
        template<typename T, TemplateFunctionTypes2> T* attachHandler( TemplateFunctionArgs2 ) { return m_actionHandlers.attach<T>( this, arg0, arg1 );                }
        template<typename T, TemplateFunctionTypes3> T* attachHandler( TemplateFunctionArgs3 ) { return m_actionHandlers.attach<T>( this, arg0, arg1, arg2 );          }
        template<typename T, TemplateFunctionTypes4> T* attachHandler( TemplateFunctionArgs4 ) { return m_actionHandlers.attach<T>( this, arg0, arg1, arg2, arg3 );    }

        //! Binds the property with a target.
        template<typename T>                         void bind( void )                  { addBinding( new T( this ) );                           }
        template<typename T, TemplateFunctionTypes1> void bind( TemplateFunctionArgs1 ) { addBinding( new T( this, arg0 ) );                     }
        template<typename T, TemplateFunctionTypes2> void bind( TemplateFunctionArgs2 ) { addBinding( new T( this, arg0, arg1 ) );               }
        template<typename T, TemplateFunctionTypes3> void bind( TemplateFunctionArgs3 ) { addBinding( new T( this, arg0, arg1, arg2 ) );         }
        template<typename T, TemplateFunctionTypes4> void bind( TemplateFunctionArgs4 ) { addBinding( new T( this, arg0, arg1, arg2, arg3 ) );   }

        //! Notifies action handler about an event.
        void                                            notify( const String& event );

        //! Clears the view data providers & handlers.
        void                                            clear( void );

    private:

        //! Adds a new binding to view.
        void                                            addBinding( Binding* instance );
        
    private:

        //! Actions handlers composite type.
        typedef Composition<ActionHandler>              ActionHandlers;

        //! Data providers composite type.
        typedef Composition<Data>                       DataProviders;

        DataProviders			                        m_data;             //!< Data composition.
        ActionHandlers	                                m_actionHandlers;   //!< Action handlers composition.
		BindingsList				                    m_bindings;			//!< Active bindings.
    };

} // namespace mvvm
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Mvvm_View_H__	*/