#include "../exercise.h"

// READ: 虚函数 <https://zh.cppreference.com/w/cpp/language/virtual>

struct A {
    virtual char virtual_name() const {
        return 'A';
    }
    char direct_name() const {
        return 'A';
    }
};
struct B : public A {
    // READ: override <https://zh.cppreference.com/w/cpp/language/override>
    char virtual_name() const override {
        return 'B';
    }
    char direct_name() const {
        return 'B';
    }
};
struct C : public B {
    // READ: final <https://zh.cppreference.com/w/cpp/language/final>
    char virtual_name() const final {
        return 'C';
    }
    char direct_name() const {
        return 'C';
    }
};
struct D : public C {
    char direct_name() const {
        return 'D';
    }
};

int main(int argc, char **argv) {
    constexpr auto MSG = "Replace '?' with its correct name.";

    A a;
    B b;
    C c;
    D d;

    ASSERT(a.virtual_name() == 'A', MSG);
    ASSERT(b.virtual_name() == 'B', MSG);
    ASSERT(c.virtual_name() == 'C', MSG);
    ASSERT(d.virtual_name() == 'C', MSG);// 实际类型是 D，但 D 没有重写 virtual_name()，因此调用 C::virtual_name()
    ASSERT(a.direct_name() == 'A', MSG); // 非虚函数调用时，实际调用的是对象的静态类型的函数
    ASSERT(b.direct_name() == 'B', MSG);
    ASSERT(c.direct_name() == 'C', MSG);
    ASSERT(d.direct_name() == 'D', MSG);

    A &rab = b;
    B &rbc = c;
    C &rcd = d;

    ASSERT(rab.virtual_name() == 'B', MSG);// 基类引用调用派生类的虚函数，实际上调用的是派生类重写的函数
    ASSERT(rbc.virtual_name() == 'C', MSG);
    ASSERT(rcd.virtual_name() == 'C', MSG);
    ASSERT(rab.direct_name() == 'A', MSG);// 基类引用调用派生类的非虚函数，实际上调用的是基类的函数
    ASSERT(rbc.direct_name() == 'B', MSG);
    ASSERT(rcd.direct_name() == 'C', MSG);

    // 虚函数：调用时根据对象的实际类型决定（动态绑定）。
    // 非虚函数：调用时根据引用的静态类型决定（静态绑定）。
    A &rac = c;
    B &rbd = d;

    ASSERT(rac.virtual_name() == 'C', MSG);
    ASSERT(rbd.virtual_name() == 'C', MSG);// 虚函数调用，实际调用 C::virtual_name()
    ASSERT(rac.direct_name() == 'A', MSG); // 非虚函数调用，调用 A::direct_name()
    ASSERT(rbd.direct_name() == 'B', MSG);

    A &rad = d;

    ASSERT(rad.virtual_name() == 'C', MSG);// 基类指针调用派生类的虚函数，实际上调用的是派生类重写的函数
    ASSERT(rad.direct_name() == 'A', MSG); // 基类指针调用派生类的非虚函数，实际上调用的是基类的函数

    return 0;
}

// READ: 扩展阅读-纯虚、抽象 <https://zh.cppreference.com/w/cpp/language/abstract_class>
// READ: 扩展阅读-虚继承 <https://zh.cppreference.com/w/cpp/language/derived_class>
