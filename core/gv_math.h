#ifndef __GVM_MATH_H__
#define __GVM_MATH_H__

#include "gv_platform.h"
#include "gv_log.h"
#include "Eigen/Eigen"

GV_NS_BEGIN

typedef Eigen::Vector2f         Vec2f;
typedef Eigen::Vector3f         Vec3f;
using Eigen::Matrix4f;
using Eigen::Matrix3f;
using Eigen::Affine3f;
using Eigen::Affine2f;
using Eigen::Translation3f;
using Eigen::AngleAxisf;
using Eigen::Quaternionf;
using Eigen::Scaling;

enum class Axis {
    X,
    Y,
    Z,
};

struct Size2f {
    float width, height;

    Size2f() noexcept : width(), height() {}
    Size2f(float w, float h) noexcept : width(w), height(h) {}
    Size2f(const Size2f &x) noexcept : width(x.width), height(x.height) {}

    bool operator==(const Size2f &rhs) const noexcept {
        return width == rhs.width && height == rhs.height;
    }
    bool operator!=(const Size2f &rhs) const noexcept {
        return width != rhs.width || height != rhs.height;
    }
};

struct Box2f final {
    Vec2f min;
    Vec2f max;
    Box2f() noexcept : min(0, 0), max(0, 0) {}
    Box2f(const Vec2f &pmin, const Vec2f &pmax) noexcept : min(pmin), max(pmax) {}
    Box2f(const Box2f &x) noexcept : min(x.min), max(x.max) {}
    Box2f(float x, float y, float width, float height) noexcept : min(x, y), max(x + width, y + height) {}

    static Box2f Zero() noexcept {
        return Box2f();
    }
    void setZero() noexcept {
        min.setZero();
        max.setZero();
    }
    float x() const noexcept {
        return min.x();
    }
    float y() const noexcept {
        return min.y();
    }
    Size2f size() const noexcept {
        return Size2f(max.x() - min.x(), max.y() - min.y());
    }
    float width() const noexcept {
        return max.x() - min.x();
    }
    float height() const noexcept {
        return max.y() - min.y();
    }
    Vec2f center() const noexcept {
        return (min + max) / 2;
    }
    bool contains(const Vec2f &x) const noexcept {
        return (min.array() <= x.array()).all() && (x.array() <= max.array()).all();
    }
    bool contains(const Box2f &x) const noexcept {
        return (min.array() <= x.min.array()).all() && (x.max.array() <= max.array()).all();
    }
    bool inside(const Box2f &x) const noexcept {
        return (min.array() > x.min.array()).all() && (x.max.array() > max.array()).all();
    }
    Box2f &operator |=(const Vec2f &x) noexcept {
        if (!empty()) {
            min = min.cwiseMin(x); 
            max = max.cwiseMax(x);
        }
        return *this;
    }
    const Box2f operator |(const Vec2f &x) const noexcept {
        Box2f result(*this);
        result |= x;
        return result;
    }
    Box2f &operator |=(const Box2f &x) noexcept {
        if (!x.empty()) {
            if (empty()) {
                *this = x;
            }
            else {
                min = min.cwiseMin(x.min); 
                max = max.cwiseMax(x.max);
            }
        }
        return *this;
    }
    const Box2f operator |(const Box2f &x) const noexcept {
        Box2f result(*this);
        result |= x;
        return result;
    }
    Box2f &operator &=(const Box2f &x) noexcept {
        if (empty() || x.empty()) {
            setZero();
        }
        else {
            min = min.cwiseMax(x.min); 
            max = max.cwiseMin(x.max);
        }
        return *this;
    }
    const Box2f operator &(const Box2f &x) const noexcept {
        Box2f result(*this);
        result &= x;
        return result;
    }
    Box2f &operator+=(const Vec2f &x) noexcept {
        min += x;
        max += x;
        return *this;
    }
    const Box2f operator+(const Vec2f &x) const noexcept {
        return Box2f(min + x, max + x);
    }
    Box2f &operator-=(const Vec2f &x) noexcept {
        min -= x;
        max -= x;
        return *this;
    }
    const Box2f operator-(const Vec2f &x) const noexcept {
        return Box2f(min - x, max - x);
    }
    bool empty() const noexcept {
        return (min.array() <= max.array()).any();
    }
    bool operator==(const Box2f &rhs) const noexcept {
        if (empty() && rhs.empty()){
            return true;
        }
        return min == rhs.min && max == rhs.max; 
    }
    bool operator!=(const Box2f &rhs) const noexcept {
        if (empty() && rhs.empty()){
            return false;
        }
        return min != rhs.min || max != rhs.max;
    }
};

struct Transform : Affine3f {
    using Affine3f::operator*;
    using Affine3f::operator=;
    using Affine3f::linear;

    void linear(Matrix3f *rot, Matrix3f *scale) const noexcept {
        computeRotationScaling(rot, scale);
    }
    void linear(Vec3f *rot, Matrix3f *scale) const noexcept {
        Matrix3f m;
        linear(&m, scale);
        *rot = m.eulerAngles(0, 1, 2);
    }
};

inline Box2f operator*(const Affine3f &lhs, const Box2f &rhs) noexcept {
    Vec3f min(rhs.min.x(), rhs.min.y(), 0.0f);
    Vec3f max(rhs.max.x(), rhs.max.y(), 0.0f);
    min = lhs * min;
    max = lhs * max;
    return Box2f(Vec2f(min.x(), min.y()), Vec2f(max.x(), max.y()));
}

GV_MATH_BEGIN

inline AngleAxisf angleAxis(float value, Axis axis) noexcept {
    static AngleAxisf zerox(0, Vec3f::UnitX());
    static AngleAxisf zeroy(0, Vec3f::UnitY());
    static AngleAxisf zeroz(0, Vec3f::UnitZ());
    switch (axis) {
    case Axis::X:
        return value == 0 ? zerox : AngleAxisf(value, Vec3f::UnitX());
    case Axis::Y:
        return value == 0 ? zeroy : AngleAxisf(value, Vec3f::UnitY());
    case Axis::Z:
        return value == 0 ? zeroz : AngleAxisf(value, Vec3f::UnitZ());
    default:
        gv_assert(false, "unknown axis.");
        return AngleAxisf();
    }
}
inline Quaternionf rotation(float x, float y, float z = 0) noexcept  {
    return angleAxis(x, Axis::X) * angleAxis(y, Axis::Y) * angleAxis(z, Axis::Z);
}
inline Quaternionf rotation(Vec3f rot) {
    return angleAxis(rot.x(), Axis::X) * angleAxis(rot.y(), Axis::Y) * angleAxis(rot.z(), Axis::Z);
}

inline float radian(float angle) noexcept {
    return angle == 0  ? 0 : fmodf(angle, 180.f) / 180.f * (float)M_PI;
}

inline Vec3f radian(const Vec3f &v) noexcept {
    return Vec3f(radian(v.x()), radian(v.y()), radian(v.z()));
}

inline float angle(float radian) noexcept {
    return radian == 0 ? 0 : radian * 180.f / (float)M_PI;
}

inline Vec3f angle(const Vec3f &v) noexcept {
    return Vec3f(angle(v.x()), angle(v.y()), angle(v.z()));
}
GV_MATH_END

GV_NS_END

#endif

