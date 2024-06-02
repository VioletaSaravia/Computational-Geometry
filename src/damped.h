#include <concepts>

#include "types.h"

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

    T start(T x0) {
        y      = x0;
        yd     = x0;
        xp     = x0;
        target = x0;

        started = true;
        return y;
    }

   public:
    operator T() const { return y; }

    explicit Damped(f32 _f = 1, f32 _z = 1, f32 _r = 0)
        : k1(_z / (PI * _f)),
          k2(1 / powf32(2 * PI * _f, 2)),
          k3(_r * _z / (2 * PI * _f)),
          f(_f),
          z(_z),
          r(_r) {}

    T Get() { return y; }

    void Toggle() { enabled = !enabled; }

    T By(T offset) {
        target = target + offset;  // TODO op+=()
        return (*this)();
    }

    T operator()() { return (*this)(target); }

    T operator()(T newVal) {
        if (!started)
            return start(newVal);

        if (!enabled) {
            y = newVal;
            return y;
        }

        f32 delta = GetFrameTime();

        T xd   = (newVal - xp) / delta;
        xp     = newVal;
        target = newVal;
        y      = y + delta * yd;

        f32 k2_stable = std::max(k2, f32(1.1 * (delta * delta / 4 + delta * k1 / 2)));
        yd            = yd + delta * (newVal + k3 * xd - y - k1 * yd) / k2_stable;
        return y;
    }
};
