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

#ifndef __DC_Mvvm_BindingsMyGUI_H__
#define __DC_Mvvm_BindingsMyGUI_H__

#include "MvvmMyGUI.h"
#include "MyGUIView.h"
#include "../Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

    //! A template class to bind a property to a MyGUI widget.
    template<typename TWidget, typename TValue>
    class MyGUIPropertyBinding : public GenericBinding<TValue> {
    public:

                                //! Constructs MyGUIPropertyBinding.
                                MyGUIPropertyBinding( View* view, const String& name, const GenericProperty<TValue>& property );

    protected:

        TWidget*                m_widget;   //!< Target MyGUI widget.
    };

    // ** MyGUIPropertyBinding::MyGUIPropertyBinding
    template<typename TWidget, typename TValue>
    MyGUIPropertyBinding<TWidget, TValue>::MyGUIPropertyBinding( View* view, const String& name, const GenericProperty<TValue>& property )
        : GenericBinding<TValue>( view, property )
    {
        m_widget = static_cast<TWidget*>( static_cast<class MyGUIView*>( view )->widget()->findWidget( name ) );
	    DC_BREAK_IF( m_widget == NULL );
    }

	//! Binds an array of strings to an edit view.
    class MyGUIEditViewBinding : public MyGUIPropertyBinding<MyGUI::EditBox, StringArray> {
	public:

								//! Constructs MyGUIEditViewBinding instance.
                                MyGUIEditViewBinding( View* view, const String& name, const GenericProperty<StringArray>& property );

	private:

		//! Handles property change.
		virtual void		    handlePropertyChanged( const StringArray& value );
	};

    //! Binds a caption to a string.
    class MyGUICaptionBinding : public MyGUIPropertyBinding<MyGUI::TextBox, String> {
    public:

                                //! Constructs MyGUICaptionBinding instance.
                                MyGUICaptionBinding( View* view, const String& name, const GenericProperty<String>& property );

    private:

        //! Handles property change.
        void                    handlePropertyChanged( const String& value );
    };

    //! Binds a line edit to a string.
    class MyGUILineEditBinding : public MyGUIPropertyBinding<MyGUI::EditBox, String> {
    public:

                                //! Constructs MyGUILineEditBinding instance.
                                MyGUILineEditBinding( View* view, const String& name, const GenericProperty<String>& property );
        virtual                 ~MyGUILineEditBinding( void );

    private:

        //! Handles property change.
        void                    handlePropertyChanged( const String& value );

        //! Handles line edit changed event.
        void                    handleTextChanged( MyGUI::Widget* sender );
    };

	//! Binds the button click event.
	class MyGUIButtonBinding : public Binding  {
	public:

								//! Constructs MyGUIButtonBinding instance.
								MyGUIButtonBinding( View* view, const String& name, const String& event );
		virtual					~MyGUIButtonBinding( void );

	private:

		//! Handles button click.
		void					handleClick( MyGUI::Widget* sender );

	private:

		MyGUI::Button*			m_button;	//!< Target MyGUI button.
        String                  m_event;    //!< Bound event.
	};

    //! Binds a widget visibility to a property.
    class MyGUIVisibilityBinding : public MyGUIPropertyBinding<MyGUI::Widget, bool> {
    public:

                                //! Constructs MyGUIVisibilityBinding instance.
                                MyGUIVisibilityBinding( View* view, const std::string& name, const GenericProperty<bool>& property, bool inversed = false );

        //! Handles property change.
        void                    handlePropertyChanged( const bool& value );

    private:

        bool                    m_isInversed;   //!< Will the binding inverse a property before the visibility change.
    };

    //! Binds a widget enabled/disabled flag to a property.
    class MyGUIEnabledBinding : public MyGUIPropertyBinding<MyGUI::Widget, bool> {
    public:

                                //! Constructs MyGUIEnabledBinding instance.
                                MyGUIEnabledBinding( View* view, const std::string& name, const GenericProperty<bool>& property, bool inversed = false );

        //! Handles property change.
        void                    handlePropertyChanged( const bool& value );

    private:

        bool                    m_isInversed;   //!< Will the binding inverse a property before the enabled/disabled change.
    };

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_BindingsMyGUI_H__ */