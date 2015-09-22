/*
file closure_impl.h
Written by Pavel Kazmerchuk   
Original code by _Winnie http://www.gamedev.ru/forum/?group=0&topic=11026&page=4
Space Concept 2005 http://www.sconcept.ru
*/
//---------------------------------------------------------------------------
template<class T>class cClosure;
//---------------------------------------------------------------------------
template<class TReturn TEMPLATE_PARAMS>
class cClosure<TReturn(TEMPLATE_ARGS)>
{    
    typedef TReturn (*TProxyFunc)(void* TEMPLATE_ARGS_COMMA);
    typedef cClosure<TReturn(TEMPLATE_ARGS)> TClosure;

    struct CDummy
    {
        void nonNull(){}
    };

    typedef void (CDummy::*ToBool)();

public:

    cClosure() 
        : pobj_(0),pFunc_(0)
    {
    }

    cClosure(void* pobj, TProxyFunc pFunc)
        :pobj_(pobj), pFunc_(pFunc)
    {
    }    
    
    TReturn operator()(PARAMS)const
    {
        return pFunc_(pobj_ OPERATOR_ARGS_COMMA ARGS);
    }

    bool operator==(const TClosure& c)const
    {
        return (pobj_ == c.pobj_) && (pFunc_ == c.pFunc_);
    }

    bool operator!=(const TClosure& c)const
    {
        return (pobj_ == c.pobj_) && (pFunc_ == c.pFunc_);
    }
    
    bool operator<(const TClosure& c)const
    {
        return (pobj_ < c.pobj_) || (!(pobj_ < c.pobj_) && (pFunc_ < c.pFunc_));
    }

    operator ToBool()const
    {
        return pFunc_ ? &CDummy::nonNull : 0;
    }

    bool operator !()const
    {
        return !ToBool(*this);
    }

    void* GetObject( void ) const { return pobj_; }

private:

    void* pobj_;
    TProxyFunc pFunc_;
};
//---------------------------------------------------------------------------


namespace detail
{
//---------------------------------------------------------------------------
template<class TObj, class TReturn TEMPLATE_PARAMS, TReturn(TObj::*pMemFunc)(TEMPLATE_ARGS)>
TReturn invoke(void* pobj PARAMS_COMMA)
{
return (static_cast<TObj*>(pobj)->*pMemFunc)(ARGS);
}
//---------------------------------------------------------------------------
template<class TObj, class TReturn TEMPLATE_PARAMS, TReturn(TObj::*pMemFunc)(TEMPLATE_ARGS)const>
TReturn invoke_const(void* pobj PARAMS_COMMA)
{
return (static_cast<TObj*>(pobj)->*pMemFunc)(ARGS);
}
//---------------------------------------------------------------------------
template<class TObj, class TReturn TEMPLATE_PARAMS, TReturn(TObj::*pMemFunc)(TEMPLATE_ARGS)volatile >
TReturn invoke_volatile(void* pobj PARAMS_COMMA)
{
return (static_cast<TObj*>(pobj)->*pMemFunc)(ARGS);
}
//---------------------------------------------------------------------------
template<class TReturn TEMPLATE_PARAMS, TReturn(*pMemFunc)(TEMPLATE_ARGS)>
TReturn invoke_static(void* pobj PARAMS_COMMA)
{
return (*pMemFunc)(ARGS);
}


namespace DETAIL_NUM
{
//---------------------------------------------------------------------------
template<class TObj, class TFObj, class TReturn TEMPLATE_PARAMS>
struct cClosureCreator
{
    template<TReturn(TFObj::*pFunc)(TEMPLATE_ARGS)>
    cClosure<TReturn(TEMPLATE_ARGS)> create(TObj *pobj)
    {
        return cClosure<TReturn(TEMPLATE_ARGS)>(
            const_cast<TFObj *>(pobj), 
            invoke<TFObj, TReturn TEMPLATE_ARGS_COMMA, pFunc> );
    }
};
//---------------------------------------------------------------------------
template<class TObj, class TFObj, class TReturn TEMPLATE_PARAMS>
struct cClosureCreatorConst
{
    template<TReturn(TFObj::*pFunc)(TEMPLATE_ARGS)const>
    cClosure<TReturn(TEMPLATE_ARGS)> create(TObj *pobj)
    {
        return cClosure<TReturn(TEMPLATE_ARGS)>(
            const_cast<TFObj *>(pobj), 
            invoke_const<TFObj, TReturn TEMPLATE_ARGS_COMMA, pFunc> );
    }
};
//---------------------------------------------------------------------------
template<class TObj, class TFObj, class TReturn TEMPLATE_PARAMS>
struct cClosureCreatorVolatile
{    
    template<TReturn(TFObj::*pFunc)(TEMPLATE_ARGS)volatile>
    cClosure<TReturn(TEMPLATE_ARGS)> create(TObj *pobj)
    {
        return cClosure<TReturn(TEMPLATE_ARGS)>(
            const_cast<TFObj *>(pobj), 
            invoke_volatile<TFObj, TReturn TEMPLATE_ARGS_COMMA, pFunc> );
    }
};
//---------------------------------------------------------------------------
template<class TReturn TEMPLATE_PARAMS>
struct cClosureCreatorStatic
{    
    template<TReturn(*pFunc)(TEMPLATE_ARGS)>
    cClosure<TReturn(TEMPLATE_ARGS)> create(...)
    {
        return cClosure<TReturn(TEMPLATE_ARGS)>(
            0, invoke_static<TReturn TEMPLATE_ARGS_COMMA, pFunc> );
    }
};
}//namespace DETAIL_NUM


//---------------------------------------------------------------------------
template<class TObj, class TFObj, class TReturn TEMPLATE_PARAMS> inline
DETAIL_NUM::cClosureCreator<TObj, TFObj, TReturn TEMPLATE_ARGS_COMMA> 
closure(TObj *, TReturn(TFObj::*)(TEMPLATE_ARGS))
{ 
return DETAIL_NUM::cClosureCreator<TObj, TFObj, TReturn TEMPLATE_ARGS_COMMA>();
}
//---------------------------------------------------------------------------
template<class TObj, class TFObj, class TReturn TEMPLATE_PARAMS> inline
DETAIL_NUM::cClosureCreatorConst<TObj, TFObj, TReturn TEMPLATE_ARGS_COMMA> 
closure(TObj*, TReturn(TFObj::*)(TEMPLATE_ARGS)const)
{
return DETAIL_NUM::cClosureCreatorConst<TObj, TFObj, TReturn TEMPLATE_ARGS_COMMA>();
}
//---------------------------------------------------------------------------
template<class TObj, class TFObj, class TReturn TEMPLATE_PARAMS> inline
DETAIL_NUM::cClosureCreatorVolatile<TObj, TFObj, TReturn TEMPLATE_ARGS_COMMA> 
closure(TObj *, TReturn(TFObj::*)(TEMPLATE_ARGS)volatile)
{
return DETAIL_NUM::cClosureCreatorVolatile<TObj, TFObj, TReturn TEMPLATE_ARGS_COMMA>();
}
//---------------------------------------------------------------------------
template<class TObj, class TReturn TEMPLATE_PARAMS> inline
DETAIL_NUM::cClosureCreatorStatic<TReturn TEMPLATE_ARGS_COMMA> 
closure(TObj, TReturn(*)(TEMPLATE_ARGS))
{
return DETAIL_NUM::cClosureCreatorStatic<TReturn TEMPLATE_ARGS_COMMA>();
}
}//namespace detail
//---------------------------------------------------------------------------