//---------------------------------------------------------------------------
/*
file delegate_impl.h
Written by Pavel Kazmerchuk   
Space Concept 2005 http://www.sconcept.ru
*/
//---------------------------------------------------------------------------
template<class T>class CDelegate;
//---------------------------------------------------------------------------
template<class TReturn TEMPLATE_PARAMS>
class CDelegate<TReturn(TEMPLATE_ARGS)>
{
    typedef CClosure<TReturn(TEMPLATE_ARGS)> TClosure;
    typedef CDelegate<TReturn(TEMPLATE_ARGS)> TDelegate;

    typedef std::vector<TClosure> TClosuresStorage;
    typedef typename TClosuresStorage::const_iterator const_iterator;
    

    void add(const TClosure& closure)
    {
        if(closure)
            closures_.push_back(closure);
    }

    void remove(const TClosure& closure)
    {
        TClosuresStorage::iterator i = 
            std::find(closures_.begin(), closures_.end(), closure);

        closures_.erase(i);
    }

public:

    TDelegate& override(const TClosure& oldClosure, const TClosure& newClosure)
    {
        TClosuresStorage::iterator i = 
            std::find(closures_.begin(), closures_.end(), oldClosure);

        if(i != closures_.end())
            (*i) = newClosure;
        else 
            add(newClosure);

        return *this;
    }
         
    TDelegate& operator+=(const TClosure& closure)
    {
        add(closure);
        return *this;
    }

    TDelegate& operator-=(const TClosure& closure)
    {
        remove(closure);
        return *this;
    }
    
    TDelegate& operator=(const TClosure& closure)          
    {               
        clear();               
        add(closure);
        return *this;          
    }

    void operator()(PARAMS) const
    {
         for(const_iterator i = closures_.begin(), end = closures_.end(); i != end; ++i)
              (*i)(ARGS);
    }

    void clear()
    {
         closures_.clear();
    }

private:
    
    TClosuresStorage closures_;
};

//---------------------------------------------------------------------------

