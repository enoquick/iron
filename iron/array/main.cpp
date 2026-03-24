
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
#else
#include <iostream>
#include <algorithm>
#include <array>
#include <string>
#include <map>
#include <vector>
#include "iron/array.h"
#include "iron/collector.h"
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26483)
#endif 

template <typename SEQ,typename T>
constexpr void check_seq(SEQ seq, const std::initializer_list<T> v) {
    for (auto& i : v) {
        ASSERT(seq);
        const auto r = *seq++;
        ASSERT_EQ(r, i);
    }
    ASSERT(!seq);
}


class not_copiable { //and not default constructable
public:
    constexpr not_copiable()noexcept : m_i() {}
    constexpr explicit not_copiable(int x)noexcept : m_i(x) { }
    constexpr not_copiable(const not_copiable&) = delete;
    constexpr not_copiable(not_copiable&&)noexcept = default;
    constexpr not_copiable& operator=(const not_copiable&) = delete;
    constexpr not_copiable& operator=(not_copiable&&)noexcept = default;
    [[nodiscard]] constexpr not_copiable clone()const noexcept { return not_copiable(m_i); }
    [[nodiscard]] constexpr iron::result<not_copiable, iron::error_handler> try_clone()const noexcept { return iron::ok(clone()); }
    [[nodiscard]] constexpr bool eq(const not_copiable& v)const noexcept { return m_i == v.m_i; }    
private:
    int m_i;
};

[[nodiscard]] inline constexpr bool operator==(const not_copiable& v1,const not_copiable& v2)noexcept { 
	return v1.eq(v2); 
}

constexpr int iron_arr_swap() {
    auto a1 = iron::collector::make_array<int>(1, 2);
    auto a2 = iron::collector::make_array<int>(3, 4);
    static_assert(iron::concepts::array<decltype(a1)>, "internal err");
    static_assert(iron::concepts::array<decltype(a2)>, "internal err");

    a2.swap(a1);
    ASSERT_EQ(a1.nth(0).unwrap(), 3);
    ASSERT_EQ(a1.nth(1).unwrap(), 4);
    ASSERT_EQ(a2.nth(0).unwrap(), 1);
    ASSERT_EQ(a2.nth(1).unwrap(), 2);
    return a2.nth(1).unwrap();
}

template <typename T,size_t _N>
union x {
    x()noexcept {}
    T _x[_N];
//    char c;
};

constexpr bool f()noexcept {
    {
        const auto a = iron::collector::make_array('a', 'b');
        const auto seq = a.cseq();
        check_seq(seq, { 'a','b' });
        const auto rseq = a.crseq();
        check_seq(rseq, { 'b','a' });
    }
    {
        auto a = iron::collector::make_array(1, 2);
        const auto seq = a.seq();
        std::for_each(seq.begin(), seq.end(), [](decltype(a)::value_t& v) { v *= 2; });
        check_seq(seq, { 2,4 });
    }
    {
        auto empty = iron::collector::make_array<char, 0>();
        ASSERT(!empty.cseq());
        ASSERT(!empty.crseq());
        ASSERT(!empty.seq());
        ASSERT(!empty.rseq());
    }
    {
        const auto a = iron::collector::make_array<int, 2>();
        const auto seq = a.seq();
        check_seq(seq, { 0,0 });
    }
    {
        auto a = iron::collector::make_array(1,2,3);
        static_assert(a.size() == 3,"internal error");
        auto seq = a.seq();
        check_seq(seq, { 1,2,3 });
        seq={};
        a.fill(10);
        seq = a.seq();
        check_seq(seq, { 10,10,10 });
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:26814)
#endif 
        const int i = 20;
#if defined(_MSC_VER)
#pragma warning(pop)
#endif 
		seq={};
        a.fill(i);
        seq = a.seq();
        check_seq(seq, { 20,20,20 });

    }
    {
        const auto a = iron::collector::make_array_fill<3>(10);
        static_assert(a.size() == 3, "internal error");
        const auto seq = a.seq();
        check_seq(seq, { 10,10,10 });
    }
    {
        constexpr int i = 10;
        const auto a = iron::collector::make_array_fill<3>(i);
        static_assert(a.size() == 3, "internal error");
        const auto seq = a.seq();
        check_seq(seq, { 10,10,10 });
    }
    {
        using n_t = not_copiable;
        auto a = iron::collector::make_array(n_t(1), n_t(2));
        ASSERT_EQ(a.size(), 2);
        auto seq = a.seq();
        check_seq(seq, { n_t{1},n_t{2} });
        seq={};
        a.fill(n_t(10));
        seq=a.seq();
        check_seq(seq, { n_t{10},n_t{10} });
    }
    {
        using n_t = not_copiable;
        const auto a = iron::collector::make_array_fill<2>(n_t(4));
        static_assert(a.size() == 2, "internal error");
        auto seq = a.seq();
        check_seq(seq, { n_t(4),n_t(4)});
        const auto empty = a.clone<0>();
        ASSERT(!empty);
        const auto cl = a.clone();
        seq = cl.seq();
        check_seq(seq, { n_t{4},n_t{4} });
        const auto cl4 = a.clone<2>();
        const auto seq1 = cl4.seq();
        check_seq(seq1, { n_t{4},n_t{4},n_t{4},n_t{4} });
    }
    {
        using n_t = not_copiable;
        auto a = iron::collector::make_array_fill<2>(n_t{ 1 });
        auto b = iron::collector::make_array_fill<2>(n_t{ 2 });
        a.swap(b);
        auto seq = a.seq();
        check_seq(seq, { n_t{2},n_t{2} });
        seq = b.seq();
        check_seq(seq, { n_t{1},n_t{1} });
    }
    {
        using n_t = not_copiable;
        constexpr std::size_t n = 3;
        iron::unsafe::raw_vector<n_t, iron::unsafe::allocator<n_t>> v;
        auto h = v.reserve(n);
        ASSERT(h);
        int j=0;
        for (std::size_t i = 0; i < n; ++i) {
            h = v.push_back(n_t(j++));
            ASSERT(h);
        }
        auto box=iron::adapter_box<decltype(v)>::unchecked_from(std::move(v));
        const auto a = iron::array<n_t, n>::from(std::move(box));
        const auto seq = a.seq(); 
        check_seq(seq, { n_t{0},n_t{1},n_t{2 } });
    }
    {
        int v[3]={ 1,2,3 };
        const auto a=iron::array<int, 3>::unchecked_from(std::move(v));
        check_seq(a.seq(), { 1,2,3 });
    }
    {
        using n_t = not_copiable;
        auto arr = iron::collector::make_array(n_t(1), n_t(2));
        auto a = arr.release().unsafe_release();
        ASSERT_EQ(*(a.nth(0)), n_t(1));
        ASSERT_EQ(*(a.nth(1)), n_t(2));
        ASSERT_EQ(arr.unchecked_nth(0), n_t{});
        ASSERT_EQ(arr.unchecked_nth(1), n_t{});
        ASSERT_EQ(arr.front(), n_t{});
        ASSERT_EQ(arr.back(), n_t{});
    }
    return true;

}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif 

int main() {
    f();
    [[maybe_unused]] constexpr auto b = f();
    return 0;
}


