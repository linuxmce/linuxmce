/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/**
 * @file property.h
 Header file for ??? .
 */
#ifndef CPP_PROPERTIES_HPP
#define CPP_PROPERTIES_HPP


#pragma warning (disable: 4355)


/*
*		C++ object properties library
*		(C) 2005 Achilleas Margaritis
*
*		The header file 'property.h' provides object properties.
*
*		Licence is freeware: you can do anything with it, except claim it for yours.
*
*/


/**
@namespace cpp
Generic namespace for all libraries from PlutoUtils.
 */
namespace cpp {




/**
@namespace properties

Provides properties to objects.

    <p>The purpose of this library is to provide object properties. Instead of
    coding setter and getter methods, it is better to use properties because
    it is a much more intuitive property_interface. Unfortunately, C++ does not offer
    native properties, but they can be emulated using templates and
    operator overloading, with a small memory overhead.</p>

    <p> In order to use the property lib, you have to do include the
        file "property.hpp" in your project, then use the following fragment
        in your code:
    </p>

    <pre>
    #include "property.hpp"
    using namespace cpp::properties;
    </pre>

    <p>The main class of this library is the class 'property'. It can be
       used to declare a property member. For example:</p>

    <pre>
    class MyClass {
    public:
        property<MyClass, int> data;

        MyClass() : data(this, &MyClass::data_changed, 5) {
        }

    protected:
        virtual void data_changed() {
        }
    };
    </pre>

    <p>
    In the above example, a property 'data' is declared. The property class
    has two main parameters: the type of owner class (needed in order to
    make a typesafe callback property_interface) and the type of the property value.
    </p>

    <p>
    The property's callback (and optional initial value) must be declared at
    construction time. It can not be changed afterwards. Callback parameters
    must not be null, otherwise your application will crash.
    </p>

    <p>
    The default property declaration declares a property that has a read-write
    value stored inside the property. The type of access (read-write, read-only,
    write-only) and the type of storage (variable or property_interface) can be
    changed by supplying different template parameters.
    </p>

    <p>property_interface properties are properties that don't store the value, but
    they call the owner object for getting and setting the value of the
    property.
    </p>

    <p>
    For example, a read-write property_interface property must be declared like this:
    </p>

    <pre>
    class MyClass {
    public:
        property<MyClass, int, read_write, property_interface> data;

        MyClass() :
            m_data(0),
            data(this, &MyClass::data_get, &MyClass::data_set) {
        }

    private:
        int m_data;

        const int &data_get() const {
            return m_data;
        }

        void data_set(const int &value) {
            m_data = value;
        }
    };
    </pre>

    <p>Usage of properties is like data members. For example:
    </p>

    <pre>
    MyClass obj;
    obj.data = 5;
    int i = obj.data + 1;
    cout << obj.data() << endl;
    </pre>
 */
namespace properties {


///class used as id for selecting a specific property implementation.
class variable {};
///class used as id for selecting a specific property implementation.
class property_interface {};
///class used as id for selecting a specific property implementation.
class read_write {};
///class used as id for selecting a specific property implementation.
class read_only {};
///class used as id for selecting a specific property implementation.
class write_only {};


//MSVC compiler
#ifdef _MSC_VER


//property selector
template <class C, class T> class _property1 {
public:
    //template of access/storage
    template <class A, class S> class _property2 {};

    //read_write, variable
    template <> class _property2<read_write, variable> {
    public:
        //constructor
        _property2<read_write, variable>(C *o, void (C::*m)(), const T &v = T()) :
            m_object(o), m_method(m), m_value(v) {
        }

        //get
        const T &get() const { return m_value; }

        //set
        void set(const T &v) {
            if (v == m_value) return;
            m_value = v;
            (m_object->*m_method)();
        }

    private:
        C *m_object;
        void (C::*m_method)();
        T m_value;
    };

    //read_write, property_interface
    template <> class _property2<read_write, property_interface> {
    public:
        //constructor
        _property2<read_write, property_interface>(C *o, const T &(C::*g)() const, void (C::*s)(const T &)) :
            m_object(o), m_getter(g), m_setter(s)
        {
        }

        //get
        const T &get() const { return (m_object->*m_getter)(); }

        //set
        void set(const T &v) { (m_object->*m_setter)(v); }

    private:
        C *m_object;
        const T &(C::*m_getter)() const;
        void (C::*m_setter)(const T &);
    };

    //read_only, variable
    template <> class _property2<read_only, variable> {
    public:
        //constructor
        _property2<read_only, variable>(const T &v = T()) : m_value(v) {
        }

        //get
        const T &get() const { return m_value; }

    private:
        T m_value;
    };

    //read_only, property_interface
    template <> class _property2<read_only, property_interface> {
    public:
        //constructor
        _property2<read_only, property_interface>(C *o, const T &(C::*g)() const) :
            m_object(o), m_getter(g)
        {
        }

        //get
        const T &get() const { return (m_object->*m_getter)(); }

    private:
        C *m_object;
        const T &(C::*m_getter)() const;
    };

    //write_only, variable
    template <> class _property2<write_only, variable> {
    public:
        //constructor
        _property2<write_only, variable>(C *o, void (C::*m)(), const T &v = T()) :
            m_object(o), m_method(m), m_value(v) {
        }

        //set
        void set(const T &v) {
            if (v == m_value) return;
            m_value = v;
            (m_object->*m_method)();
        }

    private:
        C *m_object;
        void (C::*m_method)();
        T m_value;
    };

    //write_only, property_interface
    template <> class _property2<write_only, property_interface> {
    public:
        //constructor
        _property2<write_only, property_interface>(C *o, void (C::*s)(const T &)) :
            m_object(o), m_setter(s)
        {
        }

        //set
        void set(const T &v) { (m_object->*m_setter)(v); }

    private:
        C *m_object;
        void (C::*m_setter)(const T &);
    };
};


//GCC and other compilers that allow template templates and partial specialization
#else


//default property template
template <class C, class T, class A, class S> class _property {};


//read_write, variable
template <class C, class T> class _property<C, T, read_write, variable> {
public:
    //constructor
    _property(C *o, void (C::*m)(), const T &v = T()) :
        m_object(o), m_method(m), m_value(v) {
    }

    //get
    const T &get() const { return m_value; }

    //set
    void set(const T &v) {
        if (v == m_value) return;
        m_value = v;
        (m_object->*m_method)();
    }

private:
    C *m_object;
    void (C::*m_method)();
    T m_value;
};


//read_write, property_interface
template <class C, class T> class _property<C, T, read_write, property_interface> {
public:
    //constructor
    _property(C *o, const T &(C::*g)() const, void (C::*s)(const T &)) :
        m_object(o), m_getter(g), m_setter(s)
    {
    }

    //get
    const T &get() const { return (m_object->*m_getter)(); }

    //set
    void set(const T &v) { (m_object->*m_setter)(v); }

private:
    C *m_object;
    const T &(C::*m_getter)() const;
    void (C::*m_setter)(const T &);
};


//read_only, variable
template <class C, class T> class _property<C, T, read_only, variable> {
public:
    //constructor
    _property(const T &v = T()) : m_value(v) {
    }

    //get
    const T &get() const { return m_value; }

private:
    T m_value;
};


//read_only, property_interface
template <class C, class T> class _property<C, T, read_only, property_interface> {
public:
    //constructor
    _property(C *o, const T &(C::*g)() const) :
        m_object(o), m_getter(g)
    {
    }

    //get
    const T &get() const { return (m_object->*m_getter)(); }

private:
    C *m_object;
    const T &(C::*m_getter)() const;
};


/** @template
//write_only, variable
*/
template <class C, class T> class _property<C, T, write_only, variable> {
public:
    //constructor
    _property(C *o, void (C::*m)(), const T &v = T()) :
        m_object(o), m_method(m), m_value(v) {
    }

    //set
    void set(const T &v) {
        if (v == m_value) return;
        m_value = v;
        (m_object->*m_method)();
    }

private:
    C *m_object;
    void (C::*m_method)();
    T m_value;
};


//write_only, property_interface
template <class C, class T> class _property<C, T, write_only, property_interface> {
public:
    //constructor
    _property(C *o, void (C::*s)(const T &)) :
        m_object(o), m_setter(s)
    {
    }

    //set
    void set(const T &v) { (m_object->*m_setter)(v); }

private:
    C *m_object;
    void (C::*m_setter)(const T &);
};


#endif


/** Provides property capability to C++objects.
    @param C owner class
    @param T property value type
    @param A access policy: one of read_write, read_only, write_only
    @param S storage policy: one of variable, property_interface
 */
template <class C, class T, class A = read_write, class S = variable> class property : public
#ifdef _MSC_VER
    _property1<C, T>::_property2<A, S>
#else
    _property<C, T, A, S>
#endif
{
public:
#ifdef _MSC_VER
    typedef typename _property1<C, T>::_property2<A, S> base_class;
#else
    typedef _property<C, T, A, S> base_class;
#endif

    /** read_write or write_only, variable constructor
        @param o owner object; usually 'this'; must not be null
        @param m method to use for notifying object; must not be null
        @param v initial value
     */
    property(C *o, void (C::*m)(), const T &v = T()) :
        base_class(o, m, v)
    {
    }

    /** read_write, property_interface constructor
        @param o owner object; usually 'this'; must not be null
        @param g getter method; must not be null
        @param s setter method; must not be null
     */
    property(C *o, const T &(C::*g)() const, void (C::*s)(const T &)) :
        base_class(o, g, s)
    {
    }

    /** read_only, variable constructor
        @param v initial value
     */
    property(const T &v = T()) :
        base_class(v)
    {
    }

    /** read_only, property_interface constructor
        @param o owner object; usually 'this'; must not be null
        @param g getter method; must not be null
     */
    property(C *o, const T &(C::*g)() const) :
        base_class(o, g)
    {
    }

    /** write_only, property_interface constructor
        @param o owner object; usually 'this'; must not be null
        @param s setter method; must not be null
     */
    property(C *o, void (C::*s)(const T &)) :
        base_class(o, s)
    {
    }

    /** returns the property's value. Valid for readable properties.
        @return the property's value.
     */
    operator const T &() const {
        return get();
    }

    /** returns the property's value. Valid for readable properties.
        @return the property's value.
     */
    const T &operator ()() const {
        return get();
    }

    /** sets the property's value. Valid for writable properties.
        @param v new value.
        @return reference to this
     */
    property<C, T, A, S> &operator = (const T &v) {
        set(v);
        return *this;
    }

    /** sets the property's value from another property. The other property
        might have a different access/storage policy.
        @param p property to copy the value of to this
        @return reference to this
     */
    template <class A1, class S1>
    property<C, T, A, S> &operator = (const property<C, T, A1, S1> &p) {
        set(p.get());
        return *this;
    }

private:
    //copy construction is not allowed
    property(const property<C, T, A, S> &p);
};


}} //end of namespaces


#endif //CPP_PROPERTIES_HPP
