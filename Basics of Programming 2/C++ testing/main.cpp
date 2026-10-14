// =============================================================================
// C++ COMPLETE COURSE SUMMARY
// Topics: References, Overloading, OOP, Dynamic Memory, Constants, Statics,
//         Friends, Namespaces, Operator Overloading, Inheritance, Polymorphism,
//         Multiple Inheritance, Type Conversion, Exceptions, Templates
// =============================================================================
// NOTE: This file is organized into sections. Each section is wrapped in its
//       own scope or function so everything compiles together. Uncomment
//       individual main() calls at the bottom to run specific demos.
// =============================================================================

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;


// =============================================================================
// TOPIC 1: C++ AS A REVISED VERSION OF C
// =============================================================================

// --- 1.1 Reference Types ---
// A reference is an alias for an existing variable.
// Must be initialized at declaration. Cannot be reseated.
void topic1_references() {
    int a = 5;
    int& ref = a;   // ref is an alias for a
    ref = 10;       // a is now 10
    cout << "a = " << a << "\n";  // 10
}

// Pass by value    — caller unchanged
void byValue(int x)     { x += 2; }
// Pass by pointer  — works but ugly
void byPointer(int* x)  { *x += 2; }
// Pass by reference — clean, modifies caller's variable
void byRef(int& x)      { x += 2; }

// --- 1.2 Function Overloading ---
// Same name, different parameter lists. Return type alone is not enough.
int    add(int a, int b)       { return a + b; }
double add(double a, double b) { return a + b; }

// --- 1.3 Default Parameters ---
// Defaults must be at the rightmost end; defined in the declaration.
int power(int base, int exp = 2) {
    int result = 1;
    for (int i = 0; i < exp; i++) result *= base;
    return result;
}

// --- 1.4 Inline Functions (prefer over macros) ---
// Type-safe, debuggable, no double-evaluation side effects.
#define SQUARE_MACRO(x) ((x)*(x))   // dangerous — evaluates x twice
inline int square(int x) { return x * x; }  // safe

// --- 1.5 const ---
// const variable: must be initialized, never changes.
// const parameter: documents intent; by-ref version is most important.
void printVal(const int x)         { /* x cannot be modified */ (void)x; }
void printRef(const string& s)     { cout << s << "\n"; }  // no copy, no modify

void topic1_demo() {
    cout << "\n=== TOPIC 1 ===\n";
    topic1_references();

    int n = 0;
    byRef(n);
    cout << "after byRef: " << n << "\n";   // 2

    cout << "add(1,2)     = " << add(1, 2)       << "\n";
    cout << "add(1.0,2.0) = " << add(1.0, 2.0)   << "\n";
    cout << "power(3)     = " << power(3)         << "\n";  // 9
    cout << "power(3,3)   = " << power(3, 3)      << "\n";  // 27
    cout << "square(5)    = " << square(5)        << "\n";  // 25
}


// =============================================================================
// TOPIC 2 & 3: CLASSES, ENCAPSULATION, CONSTRUCTORS, DESTRUCTORS
// =============================================================================

class Rectangle {
public:
    // Default constructor
    Rectangle() : width(0), height(0) {}

    // Parameterized constructor — negative values clamped to 0
    Rectangle(double w, double h)
        : width(w < 0 ? 0 : w), height(h < 0 ? 0 : h) {}

    // Square constructor (one side)
    explicit Rectangle(double side) : width(side), height(side) {}

    // Destructor
    ~Rectangle() { /* nothing to free here */ }

    // const methods — do not modify the object
    double area()      const { return width * height; }
    double perimeter() const { return 2 * (width + height); }

    void print() const {
        cout << "Rectangle(" << width << " x " << height
             << ") area=" << area() << " perim=" << perimeter() << "\n";
    }

private:
    double width, height;   // private — not accessible from outside
};

void topic2_demo() {
    cout << "\n=== TOPIC 2: Classes ===\n";
    Rectangle r1;           // default
    Rectangle r2(4, 5);    // parameterized
    Rectangle r3(3.0);     // square

    r1.print();
    r2.print();
    r3.print();
}


// =============================================================================
// TOPIC 3 (cont.): DYNAMIC MEMORY — new, delete, The Rule of Three
// =============================================================================

class DynArray {
public:
    // Constructor — allocates on heap
    DynArray(int size) : size(size) {
        data = new int[size];
        for (int i = 0; i < size; i++) data[i] = 0;
        cout << "  DynArray constructed (size=" << size << ")\n";
    }

    // Destructor — frees heap memory
    ~DynArray() {
        delete[] data;
        cout << "  DynArray destroyed\n";
    }

    // Copy constructor — DEEP copy (not just pointer copy)
    DynArray(const DynArray& other) : size(other.size) {
        data = new int[size];
        for (int i = 0; i < size; i++) data[i] = other.data[i];
        cout << "  DynArray copy constructed\n";
    }

    // Assignment operator — deep copy + self-assignment guard
    DynArray& operator=(const DynArray& other) {
        if (this == &other) return *this;   // self-assignment guard
        delete[] data;                      // free old memory
        size = other.size;
        data = new int[size];
        for (int i = 0; i < size; i++) data[i] = other.data[i];
        cout << "  DynArray assigned\n";
        return *this;                       // enables chaining a = b = c
    }

    void set(int index, int value) { data[index] = value; }
    int  get(int index)      const { return data[index]; }

private:
    int* data;
    int  size;
};

void topic3_demo() {
    cout << "\n=== TOPIC 3: Dynamic Memory ===\n";
    DynArray a(3);
    a.set(0, 10); a.set(1, 20); a.set(2, 30);

    DynArray b = a;     // copy constructor
    DynArray c(1);
    c = a;              // assignment operator

    cout << "b[0]=" << b.get(0) << " c[0]=" << c.get(0) << "\n";
    // destructors called automatically here
}


// =============================================================================
// TOPIC 4: CONSTANTS, STATIC MEMBERS, FRIEND, NAMESPACES
// =============================================================================

class Circle {
public:
    // static const — belongs to the class, shared by all objects
    static const double PI;

    Circle(double r) : radius(r < 0 ? 0 : r) { count++; }
    ~Circle() { count--; }

    // const member functions — promise not to modify the object
    double area()          const { return PI * radius * radius; }
    double circumference() const { return 2 * PI * radius; }
    double getRadius()     const { return radius; }

    // static method — no 'this', accesses only static members
    static int getCount() { return count; }

    void print() const {
        cout << "Circle(r=" << radius << ") area=" << area() << "\n";
    }

    // friend function — can access private members
    friend void scaleCircle(Circle& c, double factor);

private:
    double radius;
    static int count;   // declaration — shared by all Circle objects
};

// Definitions outside the class
const double Circle::PI    = 3.14159265358979;
int          Circle::count = 0;

// Friend function — accesses private 'radius'
void scaleCircle(Circle& c, double factor) {
    c.radius *= factor;
}

// Namespaces
namespace Geometry {
    double triangleArea(double base, double height) {
        return 0.5 * base * height;
    }
}

void topic4_demo() {
    cout << "\n=== TOPIC 4: Const, Static, Friend, Namespaces ===\n";
    cout << "Circles before: " << Circle::getCount() << "\n";  // 0
    {
        Circle c1(5), c2(3);
        cout << "Circles during: " << Circle::getCount() << "\n";  // 2
        c1.print();
        scaleCircle(c1, 2.0);   // friend access
        c1.print();
    }
    cout << "Circles after:  " << Circle::getCount() << "\n";  // 0

    cout << "Triangle area: " << Geometry::triangleArea(6, 4) << "\n";
}


// =============================================================================
// TOPIC 5: OPERATOR OVERLOADING
// =============================================================================

class Vector2D {
public:
    double x, y;

    Vector2D(double x = 0, double y = 0) : x(x), y(y) {}

    // Copy constructor (explicit — required when operator= is user-defined)
    Vector2D(const Vector2D& other) : x(other.x), y(other.y) {}

    // Unary minus (member)
    Vector2D operator-() const { return Vector2D(-x, -y); }

    // Assignment (member) — with self-assignment guard
    Vector2D& operator=(const Vector2D& other) {
        if (this == &other) return *this;
        x = other.x; y = other.y;
        return *this;
    }

    // Subscript — non-const (write) and const (read) versions
    double& operator[](int i)       { return i == 0 ? x : y; }
    double  operator[](int i) const { return i == 0 ? x : y; }

    // Prefix ++ (member) — increment then return self
    Vector2D& operator++() { ++x; ++y; return *this; }

    // Postfix ++ (member) — save old, increment, return old
    Vector2D operator++(int) { Vector2D old = *this; ++x; ++y; return old; }

    // Binary + (friend) — both operands equally important
    friend Vector2D operator+(const Vector2D& a, const Vector2D& b);
    friend bool     operator==(const Vector2D& a, const Vector2D& b);
    friend bool     operator<(const Vector2D& a, const Vector2D& b);

    // Stream operators (must be global/friend)
    friend ostream& operator<<(ostream& os, const Vector2D& v);
    friend istream& operator>>(istream& is, Vector2D& v);
};

Vector2D operator+(const Vector2D& a, const Vector2D& b) {
    return Vector2D(a.x + b.x, a.y + b.y);
}
bool operator==(const Vector2D& a, const Vector2D& b) {
    return a.x == b.x && a.y == b.y;
}
bool operator<(const Vector2D& a, const Vector2D& b) {
    return (a.x*a.x + a.y*a.y) < (b.x*b.x + b.y*b.y);  // compare lengths
}
ostream& operator<<(ostream& os, const Vector2D& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;  // return stream enables chaining: cout << a << b
}
istream& operator>>(istream& is, Vector2D& v) {
    is >> v.x >> v.y;
    return is;
}

void topic5_demo() {
    cout << "\n=== TOPIC 5: Operator Overloading ===\n";
    Vector2D v1(1, 2), v2(3, 4);

    cout << "v1       = " << v1 << "\n";
    cout << "v2       = " << v2 << "\n";
    cout << "v1 + v2  = " << (v1 + v2) << "\n";
    cout << "-v1      = " << (-v1) << "\n";
    cout << "v1 == v2 ? " << (v1 == v2 ? "yes" : "no") << "\n";
    cout << "v1 < v2  ? " << (v1 < v2  ? "yes" : "no") << "\n";
    cout << "v1[0]    = " << v1[0] << "\n";

    Vector2D v3 = v1;
    ++v3;
    cout << "++v1     = " << v3 << "\n";
}


// =============================================================================
// TOPIC 6: INHERITANCE & POLYMORPHISM
// =============================================================================

// Abstract base class — has at least one pure virtual function (= 0)
class Shape {
public:
    virtual double area()  const = 0;   // pure virtual — must be overridden
    virtual void   print() const = 0;

    // MUST be virtual if class has virtual functions — correct cleanup
    virtual ~Shape() { cout << "  ~Shape\n"; }
};

class CircleShape : public Shape {
public:
    CircleShape(double r) : radius(r) {}
    double area()  const override { return 3.14159 * radius * radius; }
    void   print() const override {
        cout << "  CircleShape(r=" << radius << ") area=" << area() << "\n";
    }
    ~CircleShape() { cout << "  ~CircleShape\n"; }
private:
    double radius;
};

class RectShape : public Shape {
public:
    RectShape(double w, double h) : width(w), height(h) {}
    double area()  const override { return width * height; }
    void   print() const override {
        cout << "  RectShape(" << width << "x" << height
             << ") area=" << area() << "\n";
    }
    ~RectShape() { cout << "  ~RectShape\n"; }
private:
    double width, height;
};

// Constructor/destructor order demo
class Base {
public:
    Base()  { cout << "  Base constructor\n"; }
    virtual ~Base() { cout << "  Base destructor\n"; }
};

class Derived : public Base {
public:
    Derived()  { cout << "  Derived constructor\n"; }
    ~Derived() { cout << "  Derived destructor\n"; }
};

void topic6_demo() {
    cout << "\n=== TOPIC 6: Inheritance & Polymorphism ===\n";

    // Polymorphic array — base pointers, different derived types
    Shape* shapes[2];
    shapes[0] = new CircleShape(5);
    shapes[1] = new RectShape(3, 4);

    for (int i = 0; i < 2; i++) {
        shapes[i]->print();     // correct method called at runtime
        delete shapes[i];       // correct destructor called (virtual ~Shape)
    }

    cout << "Constructor/destructor order:\n";
    Derived d;  // Base ctor → Derived ctor → Derived dtor → Base dtor
}


// =============================================================================
// TOPIC 7: MULTIPLE INHERITANCE & VIRTUAL BASE CLASSES
// =============================================================================

// Abstract interface classes (no data — diamond problem doesn't bite here)
class IComparable {
public:
    virtual bool operator>(const IComparable& other) const = 0;
    virtual ~IComparable() {}
};

class ISerializable {
public:
    virtual void save(ostream& os)  const = 0;
    virtual void load(istream& is)        = 0;
    virtual ~ISerializable() {}
};

// Person inherits from both interfaces
class Person : public IComparable, public ISerializable {
public:
    Person(string name = "", int age = 0) : name(name), age(age) {}

    bool operator>(const IComparable& other) const override {
        return age > static_cast<const Person&>(other).age;
    }
    void save(ostream& os) const override { os << name << " " << age; }
    void load(istream& is)       override { is >> name >> age; }

    void print() const { cout << "  Person(" << name << ", " << age << ")\n"; }

private:
    string name;
    int age;
};

// Virtual base class — solves the diamond problem
class Animal        { public: string name; Animal(string n):name(n){} };
class DogAnimal     : virtual public Animal { public: DogAnimal(string n):Animal(n){} };
class CatAnimal     : virtual public Animal { public: CatAnimal(string n):Animal(n){} };
class DogCat        : public DogAnimal, public CatAnimal {
public:
    DogCat(string n) : Animal(n), DogAnimal(n), CatAnimal(n) {}
};

void topic7_demo() {
    cout << "\n=== TOPIC 7: Multiple Inheritance ===\n";
    Person p1("Alice", 30), p2("Bob", 25);
    p1.print(); p2.print();
    cout << "  Alice > Bob? " << (p1 > p2 ? "yes" : "no") << "\n";

    // Save/load via ISerializable interface
    ostringstream oss;
    p1.save(oss);
    cout << "  Saved: " << oss.str() << "\n";

    DogCat dc("Fluffy");
    cout << "  DogCat name: " << dc.name << "\n";  // one copy — no ambiguity
}


// =============================================================================
// TOPIC 8: TYPE CONVERSION & CASTING
// =============================================================================

class MyInt {
public:
    // Conversion constructor — converts int → MyInt (implicit by default)
    MyInt(int v) : value(v) {
        cout << "  MyInt constructed from int: " << v << "\n";
    }

    // Type-cast operator — converts MyInt → int
    operator int() const {
        cout << "  MyInt cast to int\n";
        return value;
    }

    void print() const { cout << "  MyInt(" << value << ")\n"; }

private:
    int value;
};

void topic8_demo() {
    cout << "\n=== TOPIC 8: Type Conversion & Casting ===\n";

    // Implicit conversion via constructor
    MyInt m = 42;
    m.print();

    // Type-cast operator
    int i = m;
    cout << "  i = " << i << "\n";

    // static_cast — compile-time, related types
    double d = 3.99;
    int truncated = static_cast<int>(d);
    cout << "  static_cast<int>(3.99) = " << truncated << "\n";  // 3

    // dynamic_cast — safe runtime downcast (requires virtual functions)
    Shape* s = new CircleShape(5);
    CircleShape* c = dynamic_cast<CircleShape*>(s);
    if (c) cout << "  dynamic_cast to CircleShape succeeded\n";
    RectShape* r = dynamic_cast<RectShape*>(s);
    if (!r) cout << "  dynamic_cast to RectShape failed (nullptr)\n";
    delete s;

    // const_cast — remove const (use sparingly)
    const int ci = 10;
    (void)ci;
    // int* p = const_cast<int*>(&ci);  // removes const — dangerous

    // reinterpret_cast — raw bits, almost never needed
    // int* ip = reinterpret_cast<int*>(somePointer);
}


// =============================================================================
// TOPIC 9: EXCEPTION HANDLING
// =============================================================================

// Custom exception
class DivisionByZeroException : public exception {
public:
    const char* what() const noexcept override {
        return "Division by zero!";
    }
};

double safeDivide(double a, double b) {
    if (b == 0) throw DivisionByZeroException();
    return a / b;
}

// Stack unwinding — destructor called even when exception is thrown
class ResourceHolder {
public:
    ResourceHolder()  { cout << "  Resource acquired\n"; }
    ~ResourceHolder() { cout << "  Resource released (stack unwind)\n"; }
};

void riskyFunction() {
    ResourceHolder res;     // constructor called
    throw runtime_error("Something went wrong");
    // ~ResourceHolder() called automatically before exception propagates
}

void topic9_demo() {
    cout << "\n=== TOPIC 9: Exception Handling ===\n";

    // Basic try/catch
    try {
        cout << "  10 / 2 = " << safeDivide(10, 2) << "\n";
        cout << "  10 / 0 = " << safeDivide(10, 0) << "\n";  // throws
    }
    catch (const DivisionByZeroException& e) {
        cout << "  Caught: " << e.what() << "\n";
    }
    catch (const exception& e) {
        cout << "  Caught std::exception: " << e.what() << "\n";
    }
    catch (...) {
        cout << "  Caught unknown exception\n";
    }

    // Stack unwinding demo
    try {
        riskyFunction();
    }
    catch (const runtime_error& e) {
        cout << "  Caught runtime_error: " << e.what() << "\n";
    }
}


// =============================================================================
// TOPIC 10: TEMPLATES
// =============================================================================

// --- Function template ---
template <class T>
T getMax(T a, T b) {
    return a > b ? a : b;
}

// Multiple type parameters
template <class T, class U>
void printPair(T a, U b) {
    cout << "  (" << a << ", " << b << ")\n";
}

// --- Class template ---
template <class T>
class Pair {
public:
    Pair(T a, T b) : x(a), y(b) {}

    // Copy constructor with template parameter
    Pair(const Pair<T>& p) : x(p.x), y(p.y) {}

    T getMax() const { return x > y ? x : y; }
    T getMin() const { return x < y ? x : y; }

    void print() const {
        cout << "  Pair(" << x << ", " << y
             << ") max=" << getMax() << " min=" << getMin() << "\n";
    }

private:
    T x, y;
};

// Method defined outside class — needs template prefix
// (shown here as an example; already inlined above)
// template <class T>
// T Pair<T>::getMax() const { return x > y ? x : y; }

// Class template with two parameters
template <class K, class V>
class KeyValue {
public:
    KeyValue(K key, V value) : key(key), value(value) {}
    K getKey()   const { return key; }
    V getValue() const { return value; }
    void print() const {
        cout << "  KeyValue[" << key << "] = " << value << "\n";
    }
private:
    K key;
    V value;
};

void topic10_demo() {
    cout << "\n=== TOPIC 10: Templates ===\n";

    // Function templates — compiler generates version per type
    cout << "  getMax(3, 5)       = " << getMax(3, 5)         << "\n";
    cout << "  getMax(3.1, 2.7)   = " << getMax(3.1, 2.7)     << "\n";
    cout << "  getMax('a', 'z')   = " << getMax('a', 'z')     << "\n";

    printPair(42, "hello");
    printPair(3.14, true);

    // Class templates
    Pair<int>    p1(10, 20);
    Pair<double> p2(3.1, 2.7);
    Pair<char>   p3('a', 'z');
    p1.print();
    p2.print();
    p3.print();

    KeyValue<int, string> kv(1, "Alice");
    kv.print();
}


// =============================================================================
// QUICK REFERENCE — KEY RULES
// =============================================================================
//
//  REFERENCES
//    int& r = x;             alias, must init, cannot reseat
//    const int& r = x;       read-only alias, preferred for object params
//
//  CLASSES
//    private:                attributes go here
//    public:                 interface (methods) goes here
//    type method() const;    const method — does not modify object
//    ClassName(args)         constructor — init list preferred
//    ~ClassName()            destructor — free dynamic memory here
//
//  RULE OF THREE (if you have dynamic memory)
//    ~Destructor             free memory
//    Copy constructor        deep copy
//    operator=               deep copy + self-assign guard + return *this
//
//  STATIC
//    static T member;        belongs to class, not object
//    static T method();      no 'this', only accesses static members
//    T Class::member = val;  must be defined outside the class
//
//  INHERITANCE
//    class D : public B {}   D inherits B's public/protected interface
//    Base ctor called first, Base dtor called last
//    virtual ~Base()         REQUIRED if class has any virtual functions
//    virtual void f() = 0;   pure virtual — class becomes abstract
//    override keyword        compiler checks you're actually overriding
//
//  CASTING (prefer in this order)
//    static_cast<T>(x)       compile-time, related types
//    dynamic_cast<T*>(p)     runtime safe downcast, returns nullptr on fail
//    const_cast<T>(x)        add/remove const
//    reinterpret_cast<T>(x)  raw bits — almost never use
//
//  EXCEPTIONS
//    throw SomeException();
//    catch(const E& e) {}    catch by const reference
//    catch(...) {}           catch-all
//    destructors run during stack unwinding — safe cleanup guaranteed
//
//  TEMPLATES
//    template <class T>      before function or class definition
//    Pair<int> p(1,2);       instantiate with concrete type
//    template <class T>      required prefix for methods defined outside class
//    T Pair<T>::method() {}
//
// =============================================================================


int main() {
    topic1_demo();
    topic2_demo();
    topic3_demo();
    topic4_demo();
    topic5_demo();
    topic6_demo();
    topic7_demo();
    topic8_demo();
    topic9_demo();
    topic10_demo();

    cout << "\n=== All topics complete ===\n";
    return 0;
}