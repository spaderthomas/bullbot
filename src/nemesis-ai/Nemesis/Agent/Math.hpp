#pragma once

#include <vector>
#include <stdexcept>

template<typename T>
T dot(std::vector<T> a, std::vector<T> b) {
    if (a.size() == b.size() && !a.empty()) {
        a[0] *= b[0];
        for (size_t i = 1; i < a.size(); ++i) {
            a[0] += a[i] * b[i];
        }
        return a[0];
    }
    throw std::runtime_error("dot: vectors must be of equal length and non-empty");
}

template<typename T>
std::vector<T> multiply(std::vector<T> a, std::vector<T> b) {
    if (a.size() == b.size()) {
        for (size_t i = 0; i < a.size(); ++i) {
            a[i] *= b[i];
        }
        return a;
    }
    throw std::runtime_error("multiply: vectors be of equal length");
}

template<typename T>
std::vector<T> multiply(T a, std::vector<T> b) {
    if (b.size()) {
        for (size_t i = 0; i < b.size(); ++i) {
            b[i] *= a;
        }
        return b;
    }
    throw std::runtime_error("multiply: vector non-empty");
}

template<typename T>
std::vector<T> multiply(std::vector<T> a, T b) {
    return multiply(b, a);
}

template<typename T>
std::vector<T> subtract(std::vector<T> a, std::vector<T> b) {
    if (a.size() == b.size()) {
        for (size_t i = 0; i < a.size(); ++i) {
            a[i] -= b[i];
        }
        return a;
    }
    throw std::runtime_error("Subtract: vectors be of equal length");
}


template<typename T>
std::vector<T> subtract(std::vector<T> a, T b) {
    if (b.size()) {
        for (size_t i = 0; i < a.size(); ++i) {
            a[i] -= b;
        }
        return a;
    }
    throw std::runtime_error("multiply: vector non-empty");
}

template<typename T>
std::vector<T> add(std::vector<T> a, std::vector<T> b) {
    if (a.size() == b.size()) {
        for (size_t i = 0; i < a.size(); ++i) {
            a[i] += b[i];
        }
        return a;
    }
    throw std::runtime_error("Subtract: vectors be of equal length");
}

template<typename T>
std::vector<T> add(std::vector<T> a, T b) {
    if (b.size()) {
        for (size_t i = 0; i < a.size(); ++i) {
            a[i] += b;
        }
        return a;
    }
    throw std::runtime_error("multiply: vector non-empty");
}