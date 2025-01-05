﻿#include "../exercise.h"
#include <cstring>
// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = 1;
        // 填入正确的 shape 并计算 size
        for (int i = 0; i < 4; i++) {
            shape[i] = shape_[i];
            size *= shape[i];
        }
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    }
    ~Tensor4D() {
        delete[] data;
    }

    // 为了保持简单，禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。
    Tensor4D &operator+=(Tensor4D const &others) {
        // 实现单向广播的加法
        // 检查广播条件：others 的非 1 维度必须与 this 相同
        for (int i = 0; i < 4; i++) {
            if (others.shape[i] != 1 && others.shape[i] != shape[i]) {
                throw std::invalid_argument("Invalid broadcast shape");
            }
        }

        unsigned int size = shape[0] * shape[1] * shape[2] * shape[3];
        for (unsigned int i = 0; i < size; i++) {
            // 计算当前索引对应的 4 维坐标
            // 将一维索引 i 转换为四维坐标 (d0, d1, d2, d3)
            // 转换方法：
            // d3：i % shape[3]，表示当前元素在第四维的索引
            // d2：(i / shape[3]) % shape[2]，表示当前元素在第三维的索引
            // d1：(i / (shape[3] * shape[2])) % shape[1]，表示当前元素在第二维的索引
            // d0：i / (shape[3] * shape[2] * shape[1])，表示当前元素在第一维的索引
            unsigned int idx = i;
            unsigned int d3 = idx % shape[3];
            idx /= shape[3];
            unsigned int d2 = idx % shape[2];
            idx /= shape[2];
            unsigned int d1 = idx % shape[1];
            unsigned int d0 = idx / shape[1];

            // 计算 others 中对应的索引
            /*对于 this 的每个元素，计算 other_idx：
                如果 d2 = 0，d2 % others.shape[2] = 0 % 1 = 0。
                如果 d2 = 1，d2 % others.shape[2] = 1 % 1 = 0。
                如果 d2 = 2，d2 % others.shape[2] = 2 % 1 = 0。
                因此，无论 d2 的值是多少，others 的数据在第三个维度上的索引始终为 0。*/
            unsigned int other_idx = (d0 % others.shape[0]) * (others.shape[1] * others.shape[2] * others.shape[3]) +
                                     (d1 % others.shape[1]) * (others.shape[2] * others.shape[3]) +
                                     (d2 % others.shape[2]) * others.shape[3] +
                                     (d3 % others.shape[3]);

            data[i] += others.data[other_idx];
        }
        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}
