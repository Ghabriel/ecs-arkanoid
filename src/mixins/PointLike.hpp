#pragma once

template<typename T>
struct PointLike {
    float x;
    float y;

    template<typename U>
    T& operator+=(const U& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *static_cast<T*>(this);
    }

    template<typename U>
    T& operator-=(const U& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *static_cast<T*>(this);
    }

    T& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *static_cast<T*>(this);
    }

    T& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *static_cast<T*>(this);
    }
};

template<typename T, typename U>
inline T operator+(PointLike<T> lhs, const U& rhs) {
    return lhs += rhs;
}

template<typename T, typename U>
inline T operator-(PointLike<T> lhs, const U& rhs) {
    return lhs -= rhs;
}

template<typename T>
inline T operator*(PointLike<T> lhs, float scalar) {
    return lhs *= scalar;
}

template<typename T>
inline T operator/(PointLike<T> lhs, float scalar) {
    return lhs /= scalar;
}
