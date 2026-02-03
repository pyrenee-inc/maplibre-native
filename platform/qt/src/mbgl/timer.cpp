#include "timer_impl.hpp"

#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include <climits>
#include <memory>
#include <QDebug>

namespace mbgl {
namespace util {

Timer::Impl::Impl() {
    timer.setTimerType(Qt::PreciseTimer);
    connect(&timer, &QTimer::timeout, this, &Timer::Impl::timerFired);
}

void Timer::Impl::start(uint64_t timeout, uint64_t repeat_, std::function<void()>&& cb) {
    repeat = repeat_;
    callback = std::move(cb);

    // Clamp timeout to INT_MAX to avoid Qt warning about exceeding maximum interval
    int clampedTimeout = (timeout > static_cast<uint64_t>(INT_MAX))
        ? INT_MAX
        : static_cast<int>(timeout);

    timer.setSingleShot(true);
    timer.start(clampedTimeout);
}

void Timer::Impl::stop() {
    timer.stop();
}

void Timer::Impl::timerFired() {
    if (repeat) {
        timer.setSingleShot(false);
        timer.start(static_cast<std::chrono::milliseconds>(repeat));
    }

    callback();
}

Timer::Timer()
    : impl(std::make_unique<Impl>()) {}

Timer::~Timer() = default;

void Timer::start(Duration timeout, Duration repeat, std::function<void()>&& cb) {
    impl->start(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count(),
                std::chrono::duration_cast<std::chrono::milliseconds>(repeat).count(),
                std::move(cb));
}

void Timer::stop() {
    impl->stop();
}

} // namespace util
} // namespace mbgl
