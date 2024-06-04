#include <concepts>

#include "types.h"

// Types that satisfy CanBeDamped may be used as type parameters for Damped<> without generating
// errors, though it may not make sense to do so (e.g. Damped<bool> satisfies this constraint, even
// though a damped bool is meaningless).
template <typename T>
concept CanBeDamped = requires(T const& value, f32 const& scalar) {
    { value + value } -> std::convertible_to<T>;
    { value - value } -> std::convertible_to<T>;
    { scalar* value } -> std::convertible_to<T>;
    { value / scalar } -> std::convertible_to<T>;
};

template <CanBeDamped T>
class Damped {
    bool enabled = true;
    bool started = false;

    T   target{};
    T   xp, yd, y;
    f32 k1, k2, k3;
    f32 f, z, r;

    // Sets the starting point of the damping function. Also allows Damped to not fail when starting
    // at frame zero, i.e. when frame delta time is zero.
    T start(T x0) {
        y      = x0;
        yd     = x0;
        xp     = x0;
        target = x0;

        started = true;
        return y;
    }

   public:
    explicit Damped(f32 _f = 1, f32 _z = 1, f32 _r = 0)
        : k1(_z / (PI * _f)),
          k2(1 / powf32(2 * PI * _f, 2)),
          k3(_r * _z / (2 * PI * _f)),
          f(_f),
          z(_z),
          r(_r) {}

    // When toggled off, target is passed through with no damping. On by default.
    void Toggle() { enabled = !enabled; }

    // Computes a new damped value tending towards the previous target + offset.
    T By(T offset) {
        target = target + offset;
        return (*this)();
    }

    // Computes a new damped value tending towards the previous target.
    T operator()() { return (*this)(target); }

    // Computes a new damped value tending towards the new target.
    T operator()(T newTarget) {
        if (!started)
            return start(newTarget);

        if (!enabled) {
            y = newTarget;
            return y;
        }

        f32 delta = GetFrameTime();

        T xd   = (newTarget - xp) / delta;
        xp     = newTarget;
        target = newTarget;
        y      = y + delta * yd;

        f32 k2_stable = std::max(k2, f32(1.1 * (delta * delta / 4 + delta * k1 / 2)));
        yd            = yd + delta * (newTarget + k3 * xd - y - k1 * yd) / k2_stable;
        return y;
    }
};
