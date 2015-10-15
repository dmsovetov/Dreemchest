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

#include "../Binding.h"

#if defined( DC_QT4_ENABLED )
	#include <QtGui>
#elif defined( DC_QT5_ENABLED )
	#include <QtWidgets>
#endif

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

    //! A template class to bind a property to a Qt widget.
    template<typename TBinding, typename TWidget, typename TValue>
    class QtPropertyBinding : public Binding<TBinding, TValue> {
	protected:

		//! Alias the widget type.
		typedef TWidget				WidgetType;

		//! Binds the value to a Qt widget instance.
		virtual bool				bind( ValueWPtr value, Widget widget );

		//! Returns the type casted Qt widget pointer.
        TWidget*					widget( void ) const;

		//! Creates the signal delegate.
		virtual QSignalDelegate*	createSignalDelegate( void );

    protected:

		AutoPtr<QSignalDelegate>	m_delegate;	//!< Qt siagnal delegate.
    };

	// ** QtPropertyBinding::bind
    template<typename TBinding, typename TWidget, typename TValue>
	bool QtPropertyBinding<TBinding, TWidget, TValue>::bind( ValueWPtr value, Widget widget )
	{
		if( !Binding::bind( value, widget ) ) {
			return false;
		}

		m_delegate = createSignalDelegate();
		return true;
	}

	// ** QtPropertyBinding::createSignalDelegate
    template<typename TBinding, typename TWidget, typename TValue>
	QSignalDelegate* QtPropertyBinding<TBinding, TWidget, TValue>::createSignalDelegate( void )
	{
		return NULL;
	}

	// ** QtPropertyBinding::widget
    template<typename TBinding, typename TWidget, typename TValue>
	TWidget* QtPropertyBinding<TBinding, TWidget, TValue>::widget( void ) const
	{
		return qobject_cast<TWidget*>( reinterpret_cast<QWidget*>( m_widget ) );
	}

	//! Binds an array of strings to a list widget.
    class QtListWidgetBinding : public QtPropertyBinding<QtListWidgetBinding, QListWidget, TextArray> {
	protected:

		//! Handles property change.
		virtual void		    handleValueChanged( void );
	};

	//! Binds the string property to a stacked widget state.
	class QtStackedWidgetBinding : public QtPropertyBinding<QtStackedWidgetBinding, QStackedWidget, Text> {
	protected:

		//! Handles property change.
		virtual void		    handleValueChanged( void );
	};

	//! Binds the click event to a widget.
	class QtPushButtonBinding : public QtPropertyBinding<QtPushButtonBinding, QPushButton, CommandValue> {
    protected:

		//! Invokes the command.
		virtual void            handleViewChanged( void );
	};

	//! Binds spin box to an integer.
	class QtSpinBoxBinding : public QtPropertyBinding<QtSpinBoxBinding, QSpinBox, Integer> {
    protected:

		//! Creates the signal delegate instance.
		virtual QSignalDelegate*	createSignalDelegate( void );

        //! Handles property change.
        virtual void				handleValueChanged( void );

		//! Updates the property value.
		virtual void				handleViewChanged( void );	
	};

	//! Binds double spin box to an integer.
	class QtDoubleSpinBoxBinding : public QtPropertyBinding<QtDoubleSpinBoxBinding, QDoubleSpinBox, Float> {
    protected:

		//! Creates the signal delegate instance.
		virtual QSignalDelegate*	createSignalDelegate( void );

        //! Handles property change.
        virtual void				handleValueChanged( void );

		//! Updates the property value.
		virtual void				handleViewChanged( void );	
	};

    //! Binds a line edit to a string.
    class QtLineEditBinding : public QtPropertyBinding<QtLineEditBinding, QLineEdit, Text> {
    protected:

		//! Creates the signal delegate instance.
		virtual QSignalDelegate*	createSignalDelegate( void );

        //! Handles property change.
        virtual void				handleValueChanged( void );

		//! Updates the property value.
		virtual void				handleViewChanged( void );
    };

    //! Binds a text edit to a string.
    class QtTextEditBinding : public QtPropertyBinding<QtTextEditBinding, QTextEdit, Text> {
    protected:

		//! Creates the signal delegate instance.
		virtual QSignalDelegate*	createSignalDelegate( void );

        //! Handles property change.
        virtual void				handleValueChanged( void );

		//! Updates the property value.
		virtual void				handleViewChanged( void );
    };

	//! Binds a label to a string.
	class QtLabelBinding : public QtPropertyBinding<QtLabelBinding, QLabel, Text> {
    protected:

        //! Handles property change.
        virtual void				handleValueChanged( void );
	};

    //! Binds a widget visibility to a property.
    class QtVisibilityBinding : public QtPropertyBinding<QtVisibilityBinding, QWidget, Boolean> {
    protected:

        //! Handles property change.
        void						handleValueChanged( void );
    };

    //! Binds a widget enabled/disabled flag to a property.
    class QtEnabledBinding : public QtPropertyBinding<QtEnabledBinding, QWidget, Boolean> {
    protected:

        //! Handles property change.
        void						handleValueChanged( void );
    };

	// ---------------------------------------------------- QtBindingFactory ---------------------------------------------------- //

	//! Constructs the Qt bindings.
	class QtBindingFactory : public BindingFactory {
	public:

		//! Creates the instance of QtBindingFactory.
		static BindingFactoryPtr	create( void );

		//! Registers the Qt binding.
		template<typename TBinding, typename TWidget>
		void						registerBinding( const String& widgetProperty = "" );

	private:

									//! Constructs the QtBindingFactory instance.
									QtBindingFactory( void );
	};

	//! Constructs the Qt widgets.
	class QtWidgetFactory : public RefCounted {
	public:

		//! Creates the widget for specified value type.
		QWidget*					create( ValueTypeIdx valueType, QWidget* parent = NULL );

		//! Registers the Qt widget.
		template<typename TValue, typename TWidget>
		void						add( void );

	private:

		//! A helper struct to create Qt widget factories.
		struct Factory : public RefCounted {
			//! Creates the widget instance.
			virtual QWidget* create( QWidget* parent ) const = 0;
		};

		//! Container type to store registered widget factories.
		typedef Map< ValueTypeIdx, StrongPtr<Factory> > Factories;

		Factories					m_factories;	//!< Registered widget factories.
	};

	// ** QtWidgetFactory::add
	template<typename TValue, typename TWidget>
	void QtWidgetFactory::add( void )
	{
		//! Declare the custom widget factory.
		struct WidgetFactory : public Factory {
			virtual QWidget* create( QWidget* parent ) const { return DC_NEW TWidget( parent ); }
		};

		ValueTypeIdx valueType = Value::valueType<TValue>();
		m_factories[valueType] = DC_NEW WidgetFactory;
	}

	// ** QtWidgetFactory::create
	inline QWidget* QtWidgetFactory::create( ValueTypeIdx valueType, QWidget* parent )
	{
		Factories::const_iterator i = m_factories.find( valueType );
		return i != m_factories.end() ? i->second->create( parent ) : NULL;
	}

	// ** QtBindingFactory::registerBinding
	template<typename TBinding, typename TWidget>
	void QtBindingFactory::registerBinding( const String& widgetProperty )
	{
		CString		  widgetName = TWidget::staticMetaObject.className();
		WidgetTypeIdx widgetType = StringHash( widgetName );

		BindingFactory::registerBinding<TBinding>( widgetType, widgetProperty );
	}

	// ---------------------------------------------------- QtBindings ---------------------------------------------------- //

	//! Binds the Qt widgets with values.
	class QtBindings : public Bindings {
	public:

		//! Creates the instance of QtBindings.
		static BindingsPtr			create( const BindingFactoryPtr& factory, const ObjectWPtr& root, QWidget* widget );

	protected:

									//! Constructs the QtBindingFactory instance.
									QtBindings( const BindingFactoryPtr& factory, const ObjectWPtr& root, QWidget* widget );

		//! Returns the widget type chain.
		WidgetTypeChain				resolveWidgetTypeChain( const String& name ) const;

		//! Returns the widget with specified name.
		Widget						findWidget( const String& name ) const;

	private:

		QWidget*					m_widget;	//!< The root widget.
	};

} // namespace mvvm

DC_END_DREEMCHEST

#endif  /*  !__DC_Mvvm_BindingsQt_H__ */