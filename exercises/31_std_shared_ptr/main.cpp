#include "../exercise.h"
#include <iostream>
#include <memory>

// READ: `std::shared_ptr` <https://zh.cppreference.com/w/cpp/memory/shared_ptr>
// READ: `std::weak_ptr` <https://zh.cppreference.com/w/cpp/memory/weak_ptr>

// TODO: 将下列 `?` 替换为正确的值
int main(int argc, char **argv) {
    auto shared = std::make_shared<int>(10);
    std::shared_ptr<int> ptrs[]{shared, shared, shared};

    std::weak_ptr<int> observer = shared;
    ASSERT(observer.use_count() == 4, "");

    ptrs[0].reset();
    ASSERT(observer.use_count() == 3, "");

    ptrs[1] = nullptr;
    ASSERT(observer.use_count() == 2, "");

    ptrs[2] = std::make_shared<int>(*shared);
    // 这里创建了一个新对象，新对象的值是由 *shared 复制而来的
    // 随后 ptrs[2] 指向这个新对象，原对象引用 -1
    ASSERT(observer.use_count() == 1, "");

    ptrs[0] = shared;
    ptrs[1] = shared;
    ptrs[2] = std::move(shared); // shared 不指向原对象了，但 ptrs[2] 现在指向原对象
    ASSERT(observer.use_count() == 3, "");

    std::ignore = std::move(ptrs[0]); // 忽略 ptrs[0]，相当于 ptrs[0] = nullptr;
    ptrs[1] = std::move(ptrs[1]);
    ptrs[1] = std::move(ptrs[2]);
    ASSERT(observer.use_count() == 2, "");

    shared = observer.lock(); // 原对象还存在，返回一个指向原对象的 shared_ptr
    ASSERT(observer.use_count() == 3, "");

    shared = nullptr;
    for (auto &ptr : ptrs) ptr = nullptr;
    ASSERT(observer.use_count() == 0, "");

    shared = observer.lock(); // 原对象已经不存在，返回一个空的 shared_ptr
    ASSERT(observer.use_count() == 0, "");

    return 0;
}
