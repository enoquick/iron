#ifdef USE_MODULES
#include "iron/macros.h"
import iron.core;
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:5050)
#endif 
import std.core;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif 
#ifdef _MSVC_LANG
#define stderr (__acrt_iob_func(2)) //stderr is not exported
#endif 
#else
#include <vector>
#include <string>
#include <sstream>
#include <cstdio>
#include "iron/memory/condition_variable.h"
#endif 

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning (disable:26490)
#pragma warning (disable:26447)
#pragma warning (disable:26472)
#endif

using buffer_t = std::string;

template <typename MUTEX>
struct global {
    using mutex_t = MUTEX;
    MUTEX mutex;
    iron::memory::base_condition_variable<mutex_t>  produce, consume;
    buffer_t buffer;
    std::atomic<size_t> counter{};
    static constexpr size_t max_counter = 50;
};

void producer_no_lock(buffer_t& buffer,int n)noexcept {
    DEBUG_ASSERT(buffer.empty());
    try {
        for (std::size_t i = 0; i < 50; ++i) {
            char t[100]{};
            snprintf(&t[0], sizeof(t), "%d", n);
            for (const auto& c : t) {
                if (!c) {
                    break;
                }
                buffer.push_back(c);
            }
        }
        printf("producer %d:%s\n", n, buffer.c_str());
    }
    catch (...) {
        const auto h = ERROR_HANDLER(std::current_exception());
        auto x = h.unsafe_description();
        fprintf(stderr, "%s\n",reinterpret_cast<const char*>(x));
    }
}

template <typename MUTEX>
void producer(global<MUTEX>* g,int n)noexcept {
    using g_t = global<MUTEX>;
    DEBUG_ASSERT(g);
    while (true) {
        try {
            std::unique_lock<MUTEX> lck(g->mutex);
            //g->produce.wait(lck, [&]()noexcept { return !(g->buffer.size() && g->counter <= g_t::max_counter); });
            while (g->buffer.size() && g->counter <= g_t::max_counter) {
                g->produce.wait(lck);
            }
            if (g->counter > g_t::max_counter) {
                printf("producer %d exit\n",n);
                g->consume.notify_all();
                g->produce.notify_all();
                break;
            }
            producer_no_lock(g->buffer, n);
            ++g->counter;
            g->consume.notify_one();
        }
        catch (...) {
            const auto h=ERROR_HANDLER(std::current_exception());
            printf("%s\n", reinterpret_cast<const char*>(h.unsafe_description()));
            break;
        }
    }
}

void consumer_no_lock(buffer_t& buffer,int n)noexcept {
    DEBUG_ASSERT(buffer.size());
    printf("consumer %d: %s\n",n,buffer.c_str());
    buffer.clear();
}

template <typename MUTEX>
void consumer(global<MUTEX>* g, int n)noexcept {
    using g_t = global<MUTEX>;
    DEBUG_ASSERT(g);
    while (true) {
        try {
            std::unique_lock<MUTEX> lck(g->mutex);
            while (g->buffer.empty() && g->counter <= g_t::max_counter) {
                g->consume.wait(lck);
            }
            if (g->counter > g_t::max_counter) {
                printf("consumer %d exit\n", n);
                break;
            }
            consumer_no_lock(g->buffer, n);
            g->produce.notify_one();
        }
        catch (...) {
            const auto h = ERROR_HANDLER(std::current_exception());
            printf("%s\n", reinterpret_cast<const char*>(h.unsafe_description()));
            break;
        }
    }
}

template <typename MUTEX>
void consumer_producer(const int producer_number,const int consumer_number) {
    DEBUG_ASSERT(producer_number > 0);
    DEBUG_ASSERT(consumer_number > 0);
    using mutex_t = MUTEX;
    global<mutex_t> g;
    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(producer_number) + static_cast<std::size_t>(consumer_number));
    for (int i = 0; i < producer_number; ++i) {
        threads.push_back(std::thread(producer<mutex_t> , &g, i));
    }
    for (int i = 0; i < consumer_number; ++i) {
        threads.push_back(std::thread(consumer<mutex_t>, &g, i));
    }
    for (auto& th : threads) {
        th.join();
    }

}

int main() {
    consumer_producer<std::mutex>(2,4);
    consumer_producer<iron::memory::mutex>(2,4);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
