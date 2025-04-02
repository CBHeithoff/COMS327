#include <iostream>
#include <cmath>    // C++ sanitized version of C's math.h
                    // doesn't effect g++, but might in other compiler, so it is more portabliliy
#include <vector>

using namespace std;

class shape {
public:
    // need virtual keyword to dynamically get correct function
    virtual ~shape() {}
    // declara method as abstract by assigning it to 0
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual ostream &print(ostream &) const = 0;
};

class rectangle : public shape{
protected:
    double width, height;
public:
    // initialization list can be used to assign primitive types.
    rectangle(double width, double height) : width(width), height(height){
        // this->width = width;
        // this->height = height;
    }
    virtual ~rectangle() {}
    virtual double perimeter() const
    {
        return 2 * (width + height);
    }
    virtual double area() const{
        return width * height;
    }
    virtual ostream &print(ostream &o) const
    {
        return o << "rectangle[" << width << "," << height <<"]";
    }
};

class square : public rectangle{
public:
    square(double side) : rectangle(side, side) {}
    virtual ~square() {}
    virtual ostream &print(ostream &o) const
    {
        return o << "square[" << width << "]";
    }
};

class circle : public shape {
protected:
    double radius;
public:
    circle(double radius) : radius(radius) {}
    virtual ~circle() {}
    virtual double perimeter() const
    {
        return 2 * M_PI * radius;
    }
    virtual double circumference() const
    {
        return perimeter();
    }
    virtual double area() const{
        return M_PI * radius * radius;
    }
    virtual ostream &print(ostream &o) const
    {
        return o << "circle[" << radius << "]";
    }
};

ostream &operator<<(ostream &o, const shape &s)
{
    return s.print(o);
}

#define randfloat(i) (i * (rand() + 1.0) / RAND_MAX)


int main(int argc, char *argv[])
{
    rectangle r(5, 8);

    cout << r << endl;

    shape &sr = r;

    cout << sr << endl;

    shape *sp = &r;

    cout << *sp << endl;


    vector<shape *> vsp;
    int i;

    // To dynamically allocate storage in C++, we can use malloc and free,
    // as in C, but we can also use the operators new and delete.
    // new and delete also run constructor and deconstructor respectively.
    // need to be used in pairs because malloc/free and new/delete are in different heaps
    // important differences:
    // 1) new and delete call the constructor and destructor, respectively
    //      malloc and free do not
    // 2) malloc/free and new/delete work in seperate heaps. You cannot, 
    //      e.g. free something that you created with new. You will corrupt
    //      both heaps!


    for ( i = 0; i < 1000; i++) {
        switch (rand() % 3){
            case 0:
                // push_back is equivelant to Java's Arraylist.add()
                vsp.push_back(new rectangle(randfloat(10), randfloat(10)));
                break;
            case 1:
                vsp.push_back(new square(randfloat(10)));
                break;
            case 2:
                vsp.push_back(new circle(randfloat(10)));
                break;
        }
    }

    for( i = 0; i < 1000; i++){
        cout << *vsp[i] << endl;
    }


    vector<shape *>::iterator vi;

    for (vi = vsp.begin(); vi != vsp.end(); vi++){
        // Inner * is to look inside the iterator
        // Outer * dereferences the pointer
        cout << **vi << endl;
    }

    // for (vi = vsp.begin(); vi != vsp.end(); vi++){
    //     cout << (*vi)->area() << endl;
    // }

    for (vi = vsp.begin(); vi != vsp.end(); vi++){
        cout << ((circle *) (*vi))->circumference() << endl; // compiles but crashes in runtime
    }

    return 0;
}