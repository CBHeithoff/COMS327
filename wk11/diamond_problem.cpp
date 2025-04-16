#include <iostream>

using namespace std;

class A {
protected:
    int a;
public: 
    A() : a(0) { cout << __PRETTY_FUNCTION__ << endl; }
    A(int a) : a(a) { cout << __PRETTY_FUNCTION__ << endl; }
    A(const A &a) : a(a.a) { cout << __PRETTY_FUNCTION__ << endl; }
    virtual ~A() { cout << __PRETTY_FUNCTION__ << endl; }
    void print() { cout << "A(" << a << ")" << endl; }
};

// virtual keyword gets diamond space (not double As)
class B : virtual public A {
    protected:
        int b;
    public: 
        B() : b(0) { cout << __PRETTY_FUNCTION__ << endl; }
        B(int b) : A(b), b(b) { cout << __PRETTY_FUNCTION__ << endl; }
        B(const B &b) : A(b.b), b(b.b) { cout << __PRETTY_FUNCTION__ << endl; }
        virtual ~B() { cout << __PRETTY_FUNCTION__ << endl; }
        void print() { cout << "B(" << b << ")" << endl; }
};

class C : virtual public A {
    protected:
        int c;
    public: 
        C() : c(0) { cout << __PRETTY_FUNCTION__ << endl; }
        C(int c) : A(c), c(c) { cout << __PRETTY_FUNCTION__ << endl; }
        C(const C &c) : A(c.c), c(c.c) { cout << __PRETTY_FUNCTION__ << endl; }
        virtual ~C() { cout << __PRETTY_FUNCTION__ << endl; }
        void print() { cout << "C(" << c << ")" << endl; }
};

// virtual keyword gets diamond space (not double As)
class D : virtual public B, virtual public C {
    protected:
        int d;
    public: 
        D() : d(0) { cout << __PRETTY_FUNCTION__ << endl; }
        D(int d) : B(d), C(d), d(d) { cout << __PRETTY_FUNCTION__ << endl; }
        D(const D &d) : B(d.d), C(d.d), d(d.d) { cout << __PRETTY_FUNCTION__ << endl; }
        virtual ~D() { cout << __PRETTY_FUNCTION__ << endl; }
        void print() { cout << "D(" << d << ")" << endl; }
};


int main(int argc, char *argv[])
{
    //A a(0);
    //B b(0);
    D d(0);

    /** two As
    d.print();          // D(0)
    d.B::print();       // B(0)
    d.C::print();       // C(0)
    //d.B::A::print();    // error, ambigious base of 'D"
    //d.C::A::print();    // error, ambigious base of 'D'
    ((B)d).A::print();  // A(0), but additionally B and A deconstructure call
                        // compiler unexpectedly auto made copy constructure for the new B
                        // with two As: A, B, A, C, D, A, B, print, ~B, ~A, ~D, ~C, ~A, ~B, ~A
*/

    // with diamond shape: A, B, C, D, print, ~D, ~C, ~B, ~A
    d.print();
    d.B::print();
    d.C::print();
    d.A::print();


    return 0;
}