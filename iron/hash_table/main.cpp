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
#include "iron/hash_table.h"
#include "iron/collector.h"
#include <string>
#include <unordered_map>
#endif 


template <typename SEQ, typename T>
constexpr void check_seq(SEQ seq, const std::initializer_list<T> v) {
	constexpr auto has_mapped = iron::concepts::std_pair<typename SEQ::value_t>;
	const auto c = seq.clone().count();
	std::vector<T> order;
	while (seq) {
		const auto ref = *seq++;
		if constexpr (has_mapped) {
			auto& p = ref;
			T x = iron::clone::clone_from(p.first);
			order.push_back(std::move(x));
		}
		else {
			auto& p = *ref;
			T x = iron::clone::clone_from(p);
			order.push_back(std::move(x));
		}
	}
	std::sort(order.begin(), order.end());
	ASSERT_EQ(c, v.size());
	auto b = order.begin();
	for (auto& i : v) {
		ASSERT_EQ(i, *b++);
	}
}

void test_reash() {
#if 1
	using key_t = int;
	using mapped_t = std::string;
	using hash_map_t = iron::hash_map<key_t, mapped_t>;
	hash_map_t ht;
	mapped_t x;
	ht.rehash(0.75);
	ASSERT_EQ(ht.load_factor_threshold(),0.75);
	for (int i = 100; i < 20000; ++i) {
		x = "ABC";
		const auto _oldsz = ht.bucket_count();
		auto r = ht.try_insert(i, std::move(x));
		if (ht.bucket_count() != _oldsz) {
			std::cout << "reash " << _oldsz << ' ' << ht.bucket_count() << ' ' << ht.load_factor() << std::endl;
		}
	}

	std::unordered_map<key_t, mapped_t> hs;
	hs.max_load_factor(0.75);
	hs.rehash(100);
	for (int i = 100; i < 20000; ++i) {
		x = "ABC";
		const auto _oldsz = hs.bucket_count();
		hs.insert(std::make_pair(i, std::move(x)));
		if (hs.bucket_count() != _oldsz) {
			std::cout << "std um reash " << _oldsz << ' ' << hs.bucket_count() << ' ' << hs.load_factor() << std::endl;
		}
	}
#endif 
}


template <typename T>
constexpr bool f() {
#if 1
	using hash_table_t = T;
	hash_table_t ht;
	using key_t = typename hash_table_t::key_t;
	using mapped_t = typename hash_table_t::mapped_t;
	constexpr auto is_multi = hash_table_t::is_multi;
	constexpr auto has_mapped = hash_table_t::has_mapped;
	ASSERT_EQ(ht.size(), 0);
	ASSERT_EQ(ht.bucket_count(), 0);
	ASSERT_EQ(ht.load_factor(), 0);
	{ //empty hash
		const auto seq = ht.cseq();
		ASSERT(!seq);
		ASSERT(!ht.size());
	}
	if constexpr(has_mapped) { //insert
		mapped_t x("abc");
		const auto [seq,inserted]=ht.insert(10,x);
		ASSERT(inserted);
		ASSERT_EQ((*seq).first, 10);
		ASSERT_EQ(ht.size(), 1);
	}
	else {
		ht.insert(10);
		ASSERT_EQ(ht.size(), 1);

	}
	if constexpr(has_mapped){ //insert dup key
		ht.insert(10,"ABC"); //duplicate key if !multi
		if constexpr (is_multi) {
			ASSERT_EQ(ht.size(), 2);
		}
		else {
			ASSERT_EQ(ht.size(), 1);
		}
	}
	if constexpr(has_mapped) { //insert test result req
		auto x = "DEF";
		[[maybe_unused]] const auto [rseq,inserted]=ht.insert(11,x);
		ASSERT(inserted);
		if constexpr (has_mapped) {
			ASSERT_EQ((*rseq).first, 11);
		}
		else {
			ASSERT_EQ(*rseq, 11);
		}
		if constexpr (is_multi) {
			ASSERT_EQ(ht.size(), 3);
		}
		else {
			ASSERT_EQ(ht.size(), 2);
		}
		const auto seq = ht.cseq();
		if constexpr (is_multi) {
			check_seq(seq, { 10,10,11 });
		}
		else {
			check_seq(seq, { 10,11 });
		}
	}
	else {
		const auto [rseq,inserted] = ht.insert(11);
		ASSERT(inserted);
		ASSERT_EQ(ht.size(), 2);
		const auto seq = ht.cseq();
		check_seq(seq, { 10,11 });
	}
	{ //find
		const auto seq = ht.cfind(11);
		ASSERT(seq);
		if constexpr (has_mapped) {
			ASSERT_EQ((*seq).first, 11);
		}
		else {
			ASSERT_EQ(**seq, 11);
		}
	}
	{ //release key
		const auto sz = ht.size();
		ASSERT(sz);
		const auto adapter = ht.release_list(11).unsafe_release();
		ASSERT_EQ(adapter.size(), 1);
		if constexpr (has_mapped) {
			ASSERT_EQ(adapter.front()->first, 11);
			ASSERT_EQ(adapter.front()->second, "DEF");
		}
		else {
			ASSERT_EQ(*(adapter.front()), 11);
		}
		ASSERT_EQ(ht.size(), sz - 1);
	}
	{ //erase keys
		ht.clear();
		if constexpr (has_mapped) {
			auto x = "DEF";
			ht.insert(10, x);
			ht.insert(10, x);
			ht.insert(10, x);
			ht.insert(20, x);
			ht.insert(30, x);
		}
		else {
			ht.insert(10);
			ht.insert(10);
			ht.insert(10);
			ht.insert(20);
			ht.insert(30);
		}
		const auto sz = ht.erase(10, 30);
		if constexpr (is_multi) {
			ASSERT_EQ(sz, 4);
		}
		else {
			ASSERT_EQ(sz, 2);
		}
	}
#if 1
	{ //merge
		if (!std::is_constant_evaluated()) {
			const auto res = ht.try_merge(ht);
			ASSERT(!res); //cannot merge itself 
		}
		hash_table_t ht1;
		if constexpr (has_mapped) {
			auto x = "DEF";
			ht1.insert(10, x);
			ht1.insert(10, x);
			ht1.insert(10, x);
			ht1.insert(20, x);
			ht1.insert(30, x);
		}
		else {
			ht1.insert(10);
			ht1.insert(10);
			ht1.insert(10);
			ht1.insert(20);
			ht1.insert(30);
		}
		const auto sz = ht.merge(ht1);
		if constexpr (is_multi) {
			ASSERT_EQ(sz, 5);
			ASSERT_EQ(ht.size(), 6);
			ASSERT(ht1.empty());
		}
		else {
			ASSERT_EQ(sz,2);
			ASSERT_EQ(ht.size(), 3);
			ASSERT_EQ(ht1.size(), 1);
		}
	}
#endif 
	{ //clone and release all
		ht.clear();
		ASSERT(!ht.size());
		ASSERT(ht.bucket_count());
		ht.reset();
		ASSERT(!ht.bucket_count());
		for (key_t i = 0; i < 5; ++i) {
			auto j=iron::clone::clone_from(i);
			if constexpr (has_mapped) {
				[[maybe_unused]] const auto [rseq,inserted]=ht.insert(std::move(j), std::string("ABC"));
				ASSERT(inserted);
			}
			else {
				[[maybe_unused]] const auto [rseq, inserted]=ht.insert(std::move(j));
				ASSERT(inserted);
			}
		}
		ASSERT_EQ(ht.size(), 5);
		auto t = ht.clone();
		auto seq = t.seq();
		check_seq(seq, { 0,1,2,3,4 });
		t = ht.clone(ht.load_factor());
		seq = t.seq();
		check_seq(seq, { 0,1,2,3,4 });
		seq = {};
		t.rehash(0.5);
		seq = t.seq();
		check_seq(seq, { 0,1,2,3,4 });
		auto adapter = ht.release_all().unsafe_release();

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif 

		std::sort(adapter.data(), adapter.data() + adapter.size());
		ASSERT_EQ(adapter.size(), 5);
		auto b = adapter.data();
		ASSERT(b);
		for (key_t i = 0; i < 5; ++i) {
			ASSERT(b != adapter.data() + adapter.size());
			if constexpr (has_mapped) {
				ASSERT_EQ(b->first, i);
				ASSERT_EQ(b->second, "ABC");
			}
			else {
				ASSERT_EQ(*b, i);
			}
			++b;
		}
		ASSERT_EQ(b, adapter.data() + adapter.size());
		ASSERT(!ht.size());
		ASSERT(ht.bucket_count());
#if defined(_MSC_VER)
#pragma warning(pop)
#endif 
	}
	{ //insert range
		ht.clear();
		if constexpr (has_mapped) {
			auto x = "DEF";
			ht.insert(10, x);
			ht.insert(20, x);
			ht.insert(30, x);
		}
		else {
			ht.insert(10);
			ht.insert(20);
			ht.insert(30);
		}
		hash_table_t t;
		if constexpr (has_mapped) {
			auto x = "DEF";
			t.insert(10, x);
			t.insert(40, x);
		}
		else {
			t.insert(10);
			t.insert(40);
		}
		auto seq = t.seq();
		auto sz = ht.insert_range(seq);
		ASSERT(!seq);
		if constexpr (is_multi) {
			ASSERT_EQ(sz, 2);
		}
		else {
			ASSERT_EQ(sz, 1);
		}
		seq = t.seq();
		sz = ht.transactional_insert_range(seq);
		if constexpr (is_multi) {
			ASSERT_EQ(sz, 2);
		}
		else {
			ASSERT_EQ(sz, 0);
		}

	}
	{ //collector
		ht.clear();
		if constexpr (has_mapped) {
			auto x = "DEF";
			ht.insert(10, x);
			ht.insert(20, x);
			ht.insert(30, x);
		}
		else {
			ht.insert(10);
			ht.insert(20);
			ht.insert(30);
		}
		const auto adapter = ht.release_all();
		if constexpr (has_mapped) {
			if constexpr (is_multi) {
				auto hm = iron::collector::make_multi_hash_map<key_t, mapped_t>(key_t{ 1 }, mapped_t{}, key_t{ 2 }, mapped_t{});
				auto seq = hm.cseq();
				check_seq(seq, { 1,2 });
				hm = iron::collector::make_multi_hash_map<key_t, mapped_t>(key_t{ 1 }, mapped_t{}, key_t{ 2 }, mapped_t{}, key_t{ 2 }, mapped_t{}, key_t{ 3 });
				seq = hm.cseq();
				check_seq(seq, { 1,2,2,3 });
				auto a = adapter.clone();
				const auto const_seq = seq;
				auto x = seq;
				auto hm1 = iron::collector::make_multi_hash_map<key_t, mapped_t>(std::move(a), adapter.clone(), key_t{ 3 }, const_seq, key_t{ 4 }, seq, std::move(x));
				seq = hm1.cseq();
				check_seq(seq, { 1,1,1,2,2,2,2,2,2,3,3,3,3,4,10,10,20,20,30,30 });
			}
			else {
				auto hm = iron::collector::make_hash_map<key_t, mapped_t>(key_t{ 1 }, mapped_t{}, key_t{ 2 }, mapped_t{});
				auto seq = hm.cseq();
				check_seq(seq, { 1,2 });
				hm = iron::collector::make_hash_map<key_t, mapped_t>(key_t{ 1 }, mapped_t{}, key_t{ 2 }, mapped_t{}, key_t{ 2 }, mapped_t{}, key_t{ 3 });
				seq = hm.cseq();
				check_seq(seq, { 1,2,3 });
				auto a = adapter.clone();
				const auto const_seq = seq;
				auto x = seq;
				auto hm1 = iron::collector::make_hash_map<key_t, mapped_t>(std::move(a), adapter.clone(), key_t{ 3 }, const_seq, key_t{ 4 }, seq, std::move(x));
				seq = hm1.cseq();
				check_seq(seq, { 1,2,3,4,10,20,30 });
			}
		}
		else {
			if constexpr (is_multi) {
				auto hm = iron::collector::make_multi_hash_set<key_t>(key_t{ 1 },key_t{ 2 });
				auto seq = hm.cseq();
				check_seq(seq, { 1,2 });
				hm = iron::collector::make_multi_hash_set<key_t>(key_t{ 1 }, key_t{ 2 }, key_t{ 2 }, key_t{ 3 });
				seq = hm.cseq();
				check_seq(seq, { 1,2,2,3 });
				auto a = adapter.clone();
				const auto const_seq = seq;
				auto x = seq;
				auto hm1 = iron::collector::make_multi_hash_set<key_t>(std::move(a), adapter.clone(), key_t{ 3 }, const_seq, key_t{ 4 }, seq, std::move(x));
				seq = hm1.cseq();
				check_seq(seq, { 1,1,1,2,2,2,2,2,2,3,3,3,3,4,10,10,20,20,30,30 });
			}
			else {
				auto hm = iron::collector::make_hash_set<key_t>(key_t{ 1 }, key_t{ 2 });
				auto seq = hm.cseq();
				check_seq(seq, { 1,2 });
				hm = iron::collector::make_hash_set<key_t>(key_t{ 1 }, key_t{ 2 }, key_t{ 2 }, key_t{ 3 });
				seq = hm.cseq();
				check_seq(seq, { 1,2,3 });
				auto a = adapter.clone();
				const auto const_seq = seq;
				auto x = seq;
				const auto hm1 = iron::collector::make_hash_set<key_t>(std::move(a), adapter.clone(), key_t{ 3 }, const_seq, key_t{ 4 }, seq, std::move(x));
				seq = hm1.cseq();
				check_seq(seq, { 1,2,3,4,10,20,30 });
				[[maybe_unused]] auto data = hm1.unsafe_data();
			}
		}

	}
#endif 
	return true;
}

class not_copiable {
public:
	constexpr not_copiable(int x)noexcept : m_i(x) { }
	constexpr not_copiable(const not_copiable&) = delete;
	constexpr not_copiable(not_copiable&&)noexcept = default;
	constexpr not_copiable& operator=(const not_copiable&) = delete;
	constexpr not_copiable& operator=(not_copiable&&)noexcept = default;
	constexpr ~not_copiable() = default;
	constexpr not_copiable clone()const noexcept { return not_copiable(m_i); }
	constexpr iron::result<not_copiable, iron::error_handler> try_clone()const noexcept { return iron::ok(clone()); }
//	constexpr bool operator==(const not_copiable& v)const noexcept { return m_i == v.m_i; }
//	constexpr bool operator<(const not_copiable& v)const noexcept { return m_i < v.m_i; }
	//constexpr bool operator!=(const not_copiable& v)const noexcept { return m_i != v.m_i; }
	constexpr int get()const noexcept { return m_i; }
	constexpr not_copiable& operator++()noexcept { ++m_i; return *this; }
	constexpr operator int()const noexcept { return m_i; }
	std::size_t hash()const noexcept {
		PANIC("not used - convert into integral");
	}
private:
	int m_i;
};

inline std::ostream& operator << (std::ostream& os, const not_copiable& v) {
	os << v.get();
	return os;
}

static_assert(iron::concepts::nothrow_equality_comparable<not_copiable>, "internal error (eq)");
static_assert(iron::concepts::copyable_or_cloneable<not_copiable>, "internal error (clone)");
static_assert(std::is_nothrow_convertible_v<int, not_copiable>, "internal error (from int)");
static_assert(iron::concepts::hashable<std::hash<int>, int>, "std::hash<int> is not an hasher");
static_assert(!iron::concepts::hashable<std::hash<not_copiable>,not_copiable>, "std::hash<not_copiable> is an hasher");
static_assert(iron::concepts::hashable<iron::hasher<int>, int>, "internal error");
static_assert(iron::concepts::hashable<iron::hasher<not_copiable>,not_copiable>, "internal error");


//static_assert(iron::concepts::comparable<std::equal_to<int>, int>, "internal error");
//NOTE: is not noexcept in all compilers
static_assert(iron::concepts::comparable<iron::comparables::equal_to<int>, int>, "internal error");
static_assert(iron::concepts::comparable<iron::comparables::equal_to<not_copiable>,not_copiable>, "internal error");

int main() {
	test_reash();
	f<iron::hash_map<int, std::string>>();
	f<iron::hash_map<not_copiable, std::string>>();
	f<iron::multi_hash_map<int, std::string>>();
	f<iron::multi_hash_map<not_copiable, std::string>>();
	f<iron::hash_set<int>>();
	f<iron::hash_set<not_copiable>>();
	f<iron::multi_hash_set<int>>();
	f<iron::multi_hash_set<not_copiable>>();

#if 1
	[[maybe_unused]] constexpr auto b0 = f<iron::hash_map<int, std::string>>();
	[[maybe_unused]] constexpr auto b1 = f<iron::hash_map<not_copiable, std::string>>();
	[[maybe_unused]] constexpr auto b2 = f<iron::multi_hash_map<int, std::string>>();
	[[maybe_unused]] constexpr auto b3 = f<iron::multi_hash_map<not_copiable, std::string>>();
	[[maybe_unused]] constexpr auto b4 = f<iron::hash_set<int>>();
	[[maybe_unused]] constexpr auto b5 = f<iron::hash_set<not_copiable>>();
	[[maybe_unused]] constexpr auto b6 = f<iron::multi_hash_set<int>>();
	[[maybe_unused]] constexpr auto b7 = f<iron::multi_hash_set<not_copiable>>();
#endif 
}
