#include <vector>
#include <thread>

namespace utils {

template <typename F, typename C, typename D, typename T>
void task_scheduler(F f, const C& src, D& dest, size_t split, size_t offset,
        const T& t)
{
    auto dit = std::begin(dest) + offset;
    auto sit = std::begin(src) + offset;
    auto sentry = sit + offset + split;

    while (sit != std::end(src) && sit != sentry) {
        *dit = f(t, *sit);
        ++dit;
        ++sit;
    }
}

template <typename F, typename C, typename D, typename T>
void split_task(F task, const C& src, D& dest, const T& t)
{
    if (dest.size() < src.size())   // ensure destination is at least same size
        dest.resize(src.size());

    auto num_threads = std::thread::hardware_concurrency();
    auto split = src.size() / num_threads;
    if (split < 2)                  // a split of 1 is not desired at this time
        split = 2;
    else if (src.size() % split != 0)   // ensure splits will cover size
        ++split;

    auto sched = task_scheduler<F,C,D,T>;
    std::vector<std::thread> vt (num_threads);
    for (size_t i = 0; i < vt.size(); ++i) {
        size_t offset = i * split;
        vt[i] = std::thread{sched, task, std::ref(src), std::ref(dest), split,
                            offset, std::ref(t)};
    }

    for (auto& t : vt)
        t.join();
}

}   // utils
