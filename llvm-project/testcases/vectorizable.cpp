#include <cstddef>

void vecadd1(float *x, float *y, float *z, std::size_t len) {
    for (std::size_t i = 0; i != len; i++) {
        z[i] = x[i] + y[i];
    }
}

void vecadd2(const float *x, const float *y, float *__restrict z, std::size_t len) {
    for (std::size_t i = 0; i != len; i++) {
        z[i] = x[i] + y[i];
    }
}

struct alignas(64) vec_friendly {
    const static std::size_t size = 512;
    float v[size];
};

void vecadd3(const vec_friendly& x, const vec_friendly& y, vec_friendly& __restrict z) {
    for (std::size_t i = 0; i != vec_friendly::size; i++) {
        z.v[i] = x.v[i] + y.v[i];
    }
}