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

#ifndef __DC_Mvvm_BindingsQt_H__
#define __DC_Mvvm_BindingsQt_H__

#include "QtView.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	//! Handles the view change and triggers the property update.
	class QSignalDelegate : public QObject {

		Q_OBJECT

	public:

							//! Constructs the QSignalDelegate instance.
							QSignalDelegate( BindingWPtr binding, QWidget* widget, CString signal = NULL );

	private slots:

		//! Triggers the property update
		void				refreshProperty( void );

	private:

		BindingWPtr			m_binding;	//!< Binding to be refreshed.
	};

	// ** QSignalDelegate::QSignalDelegate
	inline QSignalDelegate::QSignalDelegate( BindingWPtr binding, QWidget* widget, CString signal ) : m_binding( binding )
	{
		if( signal ) {
			connect( widget, signal, this, SLOT(refreshProperty()));
		}
	}

	// ** QSignalDelegate::refreshProperty
	inline void QSignalDelegate::refreshProperty( void )
	{
		m_binding->refreshProperty();
	}

	//! Handles the textChanged signal and dispatches the event.
	class QTextChangedDelegate : public QSignalDelegate {
	public:

							//! Constructs QTextChangedDelegate instance.
							QTextChangedDelegate( BindingWPtr binding, QWidget* widget )
								: QSignalDelegate( binding, widget, SIGNAL( textChanged(const QString&) ) ) {}
	};

	//! Handles the clicked signal and dispatched the event.
	class QClickedDelegate : public QSignalDelegate {
	public:

							//! Constructs QClickedDelegate instance.
							QClickedDelegate( BindingWPtr binding, QWidget* widget )
								: QSignalDelegate( binding, widget, SIGNAL( clicked() ) ) {}
	};

    //! A template class to bind a property to a Qt widget.
    template<typename TWidget, typename TValue, typename TSignalDelegate = QSignalDelegate>
    class QtPropertyBinding : public GenericBinding<TValue> {
    public:

                                //! Constructs QtPropertyBinding.
                                QtPropertyBinding( View* view, TWidget* widget, const Property& property );
		virtual					~QtPropertyBinding( void );

    protected:

        TWidget*                m_widget;   //!< Target Qt widget.
		TSignalDelegate*		m_delegate;	//!< Qt siagnal delegate.
    };

    // ** QtPropertyBinding::QtPropertyBinding
    template<typename TWidget, typename TValue, typename TSignalDelegate>
    QtPropertyBinding<TWidget, TValue, TSignalDelegate>::QtPropertyBinding( View* view, TWidget* widget, const Property& property )
        : GenericBinding<TValue>( view, property ), m_widget( widget )
    {
    //    m_widget = static_cast<QtView*>( view )->widget()->findChild<TWidget*>( name.c_str() );
	//    DC_BREAK_IF( m_widget == NULL );
		m_delegate = DC_NEW TSignalDelegate( this, m_widget );
    }

    // ** QtPropertyBinding::~QtPropertyBinding
    template<typename TWidget, typename TValue, typename TSignalDelegate>
	QtPropertyBinding<TWidget, TValue, TSignalDelegate>::~QtPropertyBinding( void )
	{
		delete m_delegate;
	}

	//! Binds an array of strings to a list box.
    class QtListBoxBinding : public QtPropertyBinding<QListWidget, StringArray> {
	public:

								//! Constructs QtListBoxBinding instance.
                                QtListBoxBinding( View* view, QListWidget* widget, const Property& property );

	private:

		//! Handles property change.
		virtual void		    handlePropertyChanged( const StringArray& value );
	};

	//! Binds the string property to a stacked widget state.
	class QtStackedWidgetBinding : public QtPropertyBinding<QStackedWidget, String> {
	public:

								//! Constructs QtStackedWidgetBinding instance.
                                QtStackedWidgetBinding( View* view, QStackedWidget* widget, const Property& property );

	private:

		//! Handles property change.
		virtual void		    handlePropertyChanged( const String& value );
	};
/*
    //! Binds a caption to a string.
    class MyGUICaptionBinding : public MyGUIPropertyBinding<MyGUI::TextBox, String> {
    public:

                                //! Constructs MyGUICaptionBinding instance.
                                MyGUICaptionBinding( View* view, const String& name, const GenericProperty<String>& property );

    private:

        //! Handles property change.
        void                    handlePropertyChanged( const String& value );
    };
*/
	//! Binds the click event to a widget.
	class QtPushButtonBinding : public QtPropertyBinding<QPushButton, String, QClickedDelegate> {
    public:

                                //! Constructs QtPushButtonBinding instance.
                                QtPushButtonBinding( View* view, QPushButton* widget, const Property& property, const String& event );

    private:

        //! Handles property change.
        virtual void			handlePropertyChanged( const String& value );

		//! Emits the event.
		virtual void            refreshProperty( void );

	private:

		String					m_event;	//!< Event dispatched on click.
	};

    //! Binds a line edit to a string.
    class QtLineEditBinding : public QtPropertyBinding<QLineEdit, String, QTextChangedDelegate> {
    public:

                                //! Constructs QtLineEditBinding instance.
                                QtLineEditBinding( View* view, QLineEdit* widget, const Property& property );

    private:

        //! Handles property change.
        virtual void			handlePropertyChanged( const String& value );

		//! Updates the property value.
		virtual void            refreshProperty( void );
    };

	//! Binds a label to a string.
	class QtLabelBinding : public QtPropertyBinding<QLabel, String> {
	public:

                                //! Constructs QtLabelBinding instance.
                                QtLabelBinding( View* view, QLabel* widget, const Property& property );

    private:

        //! Handles property change.
        virtual void			handlePropertyChanged( const String& value );
	};
/*
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
*/
    //! Binds a widget visibility to a property.
    class QtVisibilityBinding : public QtPropertyBinding<QWidget, bool> {
    public:

                                //! Constructs QtVisibilityBinding instance.
                                QtVisibilityBinding( View* view, QWidget* widget, const Property& property, bool inversed = false );

        //! Handles property change.
        void                    handlePropertyChanged( const bool& value );

    private:

        bool                    m_isInversed;   //!< Will the binding inverse a property before the visibility change.
    };

    //! Binds a widget enabled/disabled flag to a property.
    class QtEnabledBinding : public QtPropertyBinding<QWidget, bool> {
    public:

                                //! Constructs QtEnabledBinding instance.
                                QtEnabledBinding( View* view, QWidget* widget, const Property& property, bool inversed = false );

        //! Handles property change.
        void                    handlePropertyChanged( const bool& value );

    private:

        bool                    m_isInversed;   //!< Will the binding inverse a property before the enabled/disabled change.
    };

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_BindingsQt_H__ */