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
//#define IRON_NOCHECK_SIZE
#include "iron/vector.h"
#include "iron/collector.h"
#include <vector>
#endif 
 
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26472)
#pragma warning (disable:26446)
#endif

template <typename SEQ, typename T>
constexpr void check_seq(SEQ seq, const std::initializer_list<T> v) {
	using v_t = typename SEQ::value_t;
	for (auto& i : v) {
		ASSERT(seq);
		const auto r = *seq++;
		ASSERT_EQ(r, v_t(i));
	}
	ASSERT(!seq);
}
template <typename SEQ, typename T>
constexpr void check_seq_fill(size_t n,SEQ seq, const T& v) {
	using v_t = typename SEQ::value_t;
	size_t c{};
	while (seq) {
		ASSERT_EQ(*seq++, v_t(v));
		++c;
	}
	ASSERT_EQ(c, n);
}

template <typename T>
constexpr bool f()noexcept {
	static_assert(std::is_nothrow_convertible_v<int,T>,"internal erroor");
	using v_t = T;
	{
		iron::vector<T> v;
		{
			const auto h = v.try_shrink_to_fit();
			ASSERT(h);
		}
		v.reserve_exact(10);
		constexpr auto insert_n = 100;
		for (int i = 1; i < insert_n; ++i) {
			auto seq=v.push_back(v_t(i));
			ASSERT_EQ(*seq++, v_t{ i });
			ASSERT(!seq);
		}
		ASSERT(v.size() == static_cast<size_t>(insert_n) - 1);
		for (int i = 1; i < insert_n; ++i) {
			const auto r = v[static_cast<size_t>(i) - 1];
			ASSERT_EQ(r, v_t(i));
		}
		v.reserve_exact(1000);
		ASSERT(v.size() == static_cast<size_t>(insert_n) - 1);
		for (int i = 1; i < 100; ++i) {
			const auto r = v.nth(static_cast<size_t>(i) - 1).unwrap();
			ASSERT_EQ(v_t(i), r);
		}
		const auto size = v.size();
		auto opt=v.pop_back();
		ASSERT_EQ(v.size(),size - 1);
		ASSERT(opt);
		ASSERT_EQ(opt.unwrap(), v_t(99));
		const auto capacity = v.capacity();
		v.clear();
		ASSERT(!v.size());
		ASSERT_EQ(v.capacity(),capacity);
		v.push_back(999);
		ASSERT_EQ(v.size(),1);
		{
			const auto r = v.nth(0).unwrap();
			ASSERT_EQ(r,v_t(999));
		}
	}

	{
		auto v = iron::collector::make_vector<int>(1, 2, 3);
		for (auto& i : v.seq()) {
			i *= 2;
		}
		const auto vm = iron::collector::make_vector<int>(v.seq());
		auto cseq = vm.cseq();
		for (int i = 1; i < 4; ++i) {
			const auto x = *cseq++;
			ASSERT_EQ(x, i*2);
		}
		ASSERT(!cseq);
		auto iter = v.seq();
		while (auto o = iter.next()) {
			o.unwrap() *= 3;
		}
		iter = v.seq();
		ASSERT_EQ(iter.next().unwrap(), 6);
		ASSERT_EQ(iter.next().unwrap(), 12);
		ASSERT_EQ(iter.next().unwrap(), 18);
		ASSERT(!iter.next());
		iter = v.seq();
		auto v1 = iron::collector::make_vector<int>(iter);
		ASSERT_EQ(v, v1);
		auto citer = v.cseq();
		ASSERT_EQ(citer.next().unwrap(), 6);
		ASSERT_EQ(citer.next().unwrap(), 12);
		ASSERT_EQ(citer.next().unwrap(), 18);
		ASSERT(!citer.next());

		auto riter = v.rseq();
		ASSERT_EQ(riter.next().unwrap(), 18);
		ASSERT_EQ(riter.next().unwrap(), 12);
		ASSERT_EQ(riter.next().unwrap(), 6);
		ASSERT(!riter.next());

		auto criter = v.crseq();
		ASSERT_EQ(criter.next().unwrap(), 18);
		ASSERT_EQ(criter.next().unwrap(), 12);
		ASSERT_EQ(criter.next().unwrap(), 6);
		ASSERT(!criter.next());
		v1 = iron::collector::make_vector<int>(v.crseq());
		ASSERT_NE(v, v1);
		ASSERT_EQ(v.size(), v1.size());
		ASSERT_EQ(v1.front().unwrap(), 18);
		auto adapter = v1.clone().release();
		const auto v2 = iron::collector::make_vector<int>(std::move(adapter));
		ASSERT_EQ(v1, v2);
	}
	{
		iron::vector<int> v;
		v.resize(3);
		check_seq(v.seq(), { 0,0,0 });
		v.push_back(3);
		v.resize(3);
		check_seq(v.seq(), { 0,0,0 });
		v.reset();
		v.push_back(1);
		v.push_back(2);
		v.resize(v.size());
		check_seq(v.seq(), { 1,2 });
		const auto c = v.capacity() + 10;
		v.resize(c);
		auto seq = v.cseq();
		ASSERT_EQ(seq.clone().count(), c);
		ASSERT_EQ(*seq++, 1);
		ASSERT_EQ(*seq++, 2);
		check_seq_fill(c - 2,seq, 0);
	}
	{ //push range
		auto v1 = iron::collector::make_vector<v_t>(1,2);
		auto v2= iron::collector::make_vector<v_t>(10);
		auto seq1 = v1.seq();
		const auto seq2=v2.push_back_range(seq1);
		ASSERT(!seq1);
		ASSERT_EQ(seq2.size().unwrap(), 2);
		check_seq(seq2, { 1,2});
		check_seq(v2.cseq(), { 10,1,2 });
		v1.shrink_to_fit();
		v1.resize(4, v_t{ 100 });
		check_seq(v1.cseq(), { 1,2,100,100 });
	}
	{ //make 
		auto list = iron::collector::make_vector<v_t>(1, 2, 3);
		const auto adapter = list.release();
		list = iron::collector::make_vector<v_t>(10, 20, 30);
		const auto seq = list.seq();
		const auto list1 = iron::collector::make_vector<v_t>(adapter.clone(), seq, adapter.clone(), 40, seq);
		const auto seq1 = list1.seq();
		check_seq(seq1, { 1,2,3,10,20,30,1,2,3,40,10,20,30 });
	}
	return true;
}

class not_copiable {
public:
	constexpr not_copiable(int x)noexcept : m_i(x) { }
	constexpr not_copiable(const not_copiable&) = delete;
	constexpr not_copiable(not_copiable&& _v)noexcept 
	:m_i()
	{
		IRON_SECURITY_CHECK_TEMPORARY(_v);
		_swap(_v);
		IRON_SECURITY_MARK_TEMPORARY(_v);
	}
	constexpr not_copiable& operator=(const not_copiable&) = delete;
	constexpr not_copiable& operator=(not_copiable&& _v)noexcept {
		IRON_SECURITY_CHECK_TEMPORARY(_v);
		_swap(_v);
		IRON_SECURITY_MARK_TEMPORARY(_v);
		return *this;
	}
	constexpr ~not_copiable() = default;
	constexpr not_copiable clone()const noexcept {
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return not_copiable(m_i); 
	}
	constexpr iron::result<not_copiable, iron::error_handler> try_clone()const noexcept { 
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return iron::ok(clone()); 
	}
	constexpr int get()const noexcept { 
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return m_i; 
	}
	constexpr bool operator==(const not_copiable& v)const noexcept { 
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		IRON_SECURITY_CHECK_TEMPORARY(v);
		return m_i == v.m_i; 
	}
#if IRON_SECURITY_CHECKER
	[[nodiscard]] constexpr const iron::unsafe::security_checker& checker()const noexcept { return _m_ck; }
#endif
private:
	constexpr void _swap(not_copiable& v)noexcept {
		std::swap(m_i, v.m_i);
		IRON_SECURITY_SWAP(v);
	}
	int m_i;
	IRON_SECURITY_DCL();
};

int main() {
	f<int>();
	f<not_copiable>();

#if 1
	[[maybe_unused]] constexpr bool b0 = f<int>();
	[[maybe_unused]] constexpr bool b1 = f<not_copiable>();
#endif

}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
