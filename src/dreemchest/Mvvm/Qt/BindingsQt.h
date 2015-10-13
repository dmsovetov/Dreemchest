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
		m_binding->handleViewChanged();
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
    template<typename TBinding, typename TWidget, typename TValue, typename TSignalDelegate = QSignalDelegate>
    class QtPropertyBinding : public Binding<TBinding, TValue> {
	protected:

		//! Binds the value to a Qt widget instance.
		virtual bool				bind( ValueWPtr value, Widget widget );

		//! Returns the type casted Qt widget pointer.
        TWidget*					widget( void );

    protected:

		AutoPtr<TSignalDelegate>	m_delegate;	//!< Qt siagnal delegate.
    };

	// ** QtPropertyBinding::bind
    template<typename TBinding, typename TWidget, typename TValue, typename TSignalDelegate>
	bool QtPropertyBinding<TBinding, TWidget, TValue, TSignalDelegate>::bind( ValueWPtr value, Widget widget )
	{
		if( !Binding::bind( value, widget ) ) {
			return false;
		}

		m_delegate = DC_NEW TSignalDelegate( this, this->widget() );
		return true;
	}

	// ** QtPropertyBinding::widget
    template<typename TBinding, typename TWidget, typename TValue, typename TSignalDelegate>
	TWidget* QtPropertyBinding<TBinding, TWidget, TValue, TSignalDelegate>::widget( void )
	{
		return qobject_cast<TWidget*>( reinterpret_cast<QWidget*>( m_widget ) );
	}

	//! Binds an array of strings to a list box.
 //   class QtListBoxBinding : public QtPropertyBinding<QListWidget, StringArray> {
	//public:

	//							//! Constructs QtListBoxBinding instance.
 //                          //     QtListBoxBinding( View* view, QListWidget* widget, BoundProperty property );

	//private:

	//	//! Handles property change.
	//	virtual void		    handleValueChanged( void );
	//};

	//! Binds the string property to a stacked widget state.
	class QtStackedWidgetBinding : public QtPropertyBinding<QtStackedWidgetBinding, QStackedWidget, Text> {
	protected:

		//! Handles property change.
		virtual void		    handleValueChanged( void );
	};

	//! Binds the click event to a widget.
	class QtPushButtonBinding : public QtPropertyBinding<QtPushButtonBinding, QPushButton, Text, QClickedDelegate> {
    private:

        //! Handles property change.
        virtual void			handleValueChanged( void );

		//! Emits the event.
		virtual void            handleViewChanged( void );

	private:

		String					m_event;	//!< Event dispatched on click.
	};

    //! Binds a line edit to a string.
    class QtLineEditBinding : public QtPropertyBinding<QtLineEditBinding, QLineEdit, Text, QTextChangedDelegate> {
    protected:

        //! Handles property change.
        virtual void			handleValueChanged( void );

		//! Updates the property value.
		virtual void            handleViewChanged( void );
    };

	//! Binds a label to a string.
	class QtLabelBinding : public QtPropertyBinding<QtLabelBinding, QLabel, Text> {
    protected:

        //! Handles property change.
        virtual void			handleValueChanged( void );
	};

    //! Binds a widget visibility to a property.
    class QtVisibilityBinding : public QtPropertyBinding<QtVisibilityBinding, QWidget, Boolean> {
    protected:

        //! Handles property change.
        void                    handleValueChanged( void );
    };

    //! Binds a widget enabled/disabled flag to a property.
    class QtEnabledBinding : public QtPropertyBinding<QtEnabledBinding, QWidget, Boolean> {
    protected:

        //! Handles property change.
        void                    handleValueChanged( void );
    };

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_BindingsQt_H__ */