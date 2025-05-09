#include <iostream>

// g++ decorator.cpp -o decorator

using namespace std;

class shape {
    public:
        virtual void draw() = 0;
};

class circle : public shape {
    public: 
        void draw()
        {
            cout << "0";
        }
};

class square : public shape {
    public: 
        void draw()
        {
            cout << "[]";
        }
};

//--------------------------------------------------------------------------
// imagine we do not have access to the source code here. Only binaries.
// But we want to extend these classes to allow different sizes, colors, etc.

class big_circle : public circle {
    public:
        void draw()
        {
            cout << "BIG-"; // in real graphics, probably use a transformation matrix.
            circle::draw();
        }
};

class green_circle : public circle {
    public:
        void draw()
        {
            cout << "green-"; // Turn on green
            circle::draw();
        }
};

// the above gets tedious; we could do it better with to the shape heirarchy to modify it directly.
// This isn't really a problem that should be solved by derived classes.

// Enter decorator pattern: 

// has-a relationship, instead of the is-a relationship like above
class shape_decorator : public circle {
    protected:
        shape *s;
    public:
        shape_decorator(shape *s) : s(s) {}
        void draw()
        {
            s->draw();
        }

};

class big_shape : public shape_decorator {
    public:
        big_shape(shape *s) : shape_decorator(s) {}
        void draw()
        {
            cout << "BIG-";
            s->draw();
        }
};

class green_shape : public shape_decorator {
    public:
        green_shape(shape *s) : shape_decorator(s) {}
        void draw()
        {
            cout << "green-";
            s->draw();
        }
};

int main(int argc, char *argv[])
{
    circle c;

    c.draw();
    cout << endl;

    big_shape bs(&c);

    bs.draw();
    cout << endl;
    
    shape *s = new big_shape(new green_shape(new circle));
    s->draw();
    cout << endl;

    s = new big_shape(new big_shape(new green_shape(new green_shape(new circle))));
    s->draw();
    cout << endl;

    return 0;
}