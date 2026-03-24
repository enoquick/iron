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
#include <string>
#include <map>
#include <set>
#include <random>
#include <iostream>
#include <vector>
#include <ranges>
#define _IRON_CHECK_BTREE
#include "iron/btree_table.h"
#include "iron/collector.h"
#endif 


#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:26472)
#pragma warning(disable:26481)
#pragma warning(disable:26490)
#pragma warning(disable:26496)
#endif

template <typename ITER>
constexpr void check_iter(ITER b, ITER e, std::initializer_list<int> list) {
	for (auto& i : list) {
		ASSERT(b != e);
		const auto& vt = *b++;
		if constexpr (iron::concepts::std_pair<decltype(vt)>) {
			ASSERT_EQ(vt.first, i);
		}
		else {
			ASSERT_EQ(vt, i);
		}
	}
}

template <typename ITER,typename ITER1>
constexpr void check_iter(ITER b, ITER e,ITER1 b1,ITER1 e1) {
	while(b1 != e1) {
		ASSERT_NE(b,e);
		const auto& vt = *b++;
		const auto& i = *b1++;
		if constexpr (iron::concepts::std_pair<decltype(vt)>) {
			ASSERT_EQ(vt.first, i);
		}
		else {
			ASSERT_EQ(vt, i);
		}
	}
	ASSERT_EQ(b,e);
}



template <typename T>
constexpr T btree_populate(const int min, const int max)noexcept {
	T bt;
	using common_t = typename T::common_t;;
	ASSERT_EQ(bt.begin(), bt.end());
	ASSERT_EQ(bt.rbegin(), bt.rend());
	ASSERT(min > 0 && min < max);
	for (int i = min; i <= max; ++i) { //insert min..max
		[[maybe_unused]] const auto r = bt.insert(i, 3);
		ASSERT(r);
		const auto& [iter,inserted] = r.unchecked_unwrap();
		ASSERT(inserted);
		ASSERT_EQ(iter->first, i);
		common_t::check_recursive_node(*(bt.data()));
	}
	auto iota = std::views::iota(min, max + 1);
	std::vector<int> v(iota.begin(), iota.end());
	auto b = bt.begin();
	auto e = bt.end();
	check_iter(b, e, v.begin(), v.end());
	auto rb = bt.rbegin();
	auto re = bt.rend();
	check_iter(rb, re, v.rbegin(), v.rend());
	return bt;
}

template <typename T>
constexpr T btree_populate_reverse(const int min,const int max)noexcept {
	T bt;
	using common_t = typename T::common_t;;
	ASSERT_EQ(bt.begin(), bt.end());
	ASSERT_EQ(bt.rbegin(), bt.rend());
	ASSERT(min > 0 && min < max);
	for (int i = max; i >= min; --i) {
		ASSERT(i);
		const auto r = bt.insert(i, 3);
		ASSERT(r);
		auto& [iter,inserted] = r.unchecked_unwrap();
		ASSERT(inserted);
		ASSERT_EQ(iter->first, i);
		common_t::check_recursive_node(*(bt.data()));
	}
	auto iota = std::views::iota(min, max + 1);
	std::vector<int> v(iota.begin(), iota.end());
	auto b = bt.begin();
	auto e = bt.end();
	check_iter(b, e, v.begin(), v.end());
	auto rb = bt.rbegin();
	auto re = bt.rend();
	check_iter(rb, re, v.rbegin(), v.rend());
	return bt;
}

template <typename T>
constexpr bool btree_insert(const int min, const int max)noexcept {
	const auto bt = btree_populate<T>(min, max);
	return true;
}

template <typename T>
constexpr bool btree_insert_reverse(const int min, const int max)noexcept {
	const auto bt = btree_populate_reverse<T>(min, max);
	return true;
}

template <typename BT,typename T>
constexpr typename BT::iter_t find(BT& bt,const T& key)noexcept {
	auto b = bt.begin();
	const auto e = bt.end();
	while (b != e) {
		using common_t = typename BT::common_t;
		auto& k = common_t::get_key(*b);
		if (common_t::compare(k, key) == common_t::compare_t::eq) {
			break;
		}
		++b;
	}
	auto b1 = bt.find(key);
	ASSERT_EQ(b1, b);
	return b;
}

template <typename T,typename LIST,typename K>
constexpr void erase_list(const T& bt,LIST list,const K& k,std::size_t& sz)noexcept {
	while (list) {
		using common_t = typename T::common_t;
		const auto cmp = common_t::compare(list->value().first, k);
		ASSERT_EQ(cmp,common_t::compare_t::eq);
		auto next = list->next();
#ifndef NDEBUG
		list->reset_links();
#endif 
		T::data_node_t::destroy(list);
		T::data_node_t::deallocate(list, bt.allocator());
		DEBUG_ASSERT(sz);
		--sz;
		list = next;
	}
}


template <typename T>
void f_simple_random_insert() {
	using common_t = typename T::common_t;
	constexpr int min = 1;
	constexpr int max = 10000;
	static_assert(min > 0 && max > min);
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<int> dist6(min, max);
	T bt;
	std::vector<int> vkeys;
	vkeys.reserve(static_cast<std::size_t>(max));
	for (auto i = 0; i < max; ++i) {
		const auto v = dist6(rng);
		const auto sz = bt.size();
		auto r = bt.insert(int{ v }, 0);
		if (r) {
			common_t::check_recursive_node(*(bt.data()));
			auto& [iter, inserted] = r.unchecked_unwrap();
			if (inserted) {
				ASSERT_EQ(bt.size(), sz + 1);
				auto b = bt.begin();
				const auto e = bt.end();
				common_t::check_sequence(b, e);
				vkeys.push_back(int{ v });
			}
			else {
				ASSERT_EQ(bt.size(), sz);
			}
		}
		else {
			auto& h = r.unchecked_unwrap_err();
			auto e = h.unsafe_error();
			ASSERT(e);
			std::cerr << "error " 
				<< e->code() 
				<< ' ' 
				<< reinterpret_cast<const char*>(e->description()) 
				<< " (" << e->file() << ',' << e->line() << ") " 
				<< std::endl;
			ASSERT_EQ(bt.size(), sz);
		}
	}
	ASSERT_EQ(vkeys.size(), bt.size());
	std::sort(vkeys.begin(), vkeys.end());
	{
		auto bm = bt.begin();
		const auto em = bt.end();
		auto bv = vkeys.begin();
		const auto ev = vkeys.end();
		while (bm != em) {
			ASSERT_NE(bv,ev);
			auto& vk = *bv;
			auto pair = *bm;
			auto& mk = pair.first;
			ASSERT_EQ(vk, mk);
			++bv;
			++bm;
		}
		ASSERT_EQ(bv,ev);
	}
	{
		auto bm = bt.rbegin();
		const auto em = bt.rend();
		auto bv = vkeys.rbegin();
		const auto ev = vkeys.rend();
		while (bm != em) {
			ASSERT(bv != ev);
			auto& vk = *bv;
			auto pair = *bm;
			auto& mk = pair.first;
			ASSERT_EQ(vk, mk);
			++bv;
			++bm;
		}
		ASSERT_EQ(bv,ev);
	}

}

template <typename T>
constexpr bool  f_simple_erase()noexcept {
	using common_t = typename T::common_t;
	T bt;
	auto r=bt.insert(1, 3);
	r=bt.insert(2, 4);
	r=bt.insert(3, 5);
	ASSERT_EQ(bt.size(), 3);
	common_t::check_recursive_node(*(bt.data()));
	while (bt.size()) {
		auto b = bt.begin();
		const auto c= bt.erase(b);
		ASSERT_EQ(c, 1);
		b = bt.begin();
		const auto e = bt.end();
		common_t::check_sequence(b, e);
	}
	return true;
}

template <typename T>
void f_simple_random_erase(int min,int max,unsigned int seed) {
	using common_t = typename T::common_t;
	DEBUG_ASSERT(min > 0 && max > min);
	if (!seed) {
		std::random_device dev;
		seed = dev();
	}
	std::mt19937 rng(seed);
	std::uniform_int_distribution<int> dist6(min, max);
	T bt;
	for (auto i = 0; i < max; ++i) {
		const auto v = dist6(rng);
		[[maybe_unused]] const auto r=bt.insert(int{ v }, 0);
		common_t::check_recursive_node(*(bt.data()));
	}
	common_t::check_sequence(bt.begin(),bt.end());
	auto sz = bt.size();
	for (auto i = 0; i < max; ++i) {
		const auto k = dist6(rng);
		auto b = find(bt,k);
		if (b != bt.end()) {
			//std::cout << "random erase - erase: " << k << std::endl;
			auto list = bt.detach(b);
			common_t::check_recursive_node(*(bt.data()));
			ASSERT(list);
			erase_list<T>(bt, list, k, sz);
			const auto b1 = bt.begin();
			const auto e1 = bt.end();
			common_t::check_sequence(b1, e1);
			ASSERT_EQ(sz, bt.size());
			if (bt.empty()) {
				break;
			}
		}
	}
}

template <typename SEQ, typename T>
constexpr void check_seq(SEQ seq, const std::initializer_list<T> v) {
	constexpr auto has_mapped = iron::concepts::std_pair<typename SEQ::value_t>;
	const auto c = seq.clone().count();
	ASSERT_EQ(c, v.size());
	for (auto& i : v) {
		if constexpr (has_mapped) {
			const auto pair = *seq++;
			ASSERT_EQ(i, pair.first);
		}
		else {
			const auto ref = *seq++;
			auto& k = *ref;
			ASSERT_EQ(i,k);
		}
	}
}

template <typename T>
constexpr bool f() {
	using table_t = T;
	table_t ht;
	using key_t = typename table_t::key_t;
	using mapped_t = typename table_t::mapped_t;
	constexpr auto is_multi = table_t::is_multi;
	constexpr auto has_mapped = table_t::has_mapped;
	ASSERT_EQ(ht.size(), 0);
	{ //empty 
		const auto seq = ht.cseq();
		ASSERT(!seq);
		ASSERT(!ht.size());
	}
	if constexpr (has_mapped) { //insert
		mapped_t x("abc");
		const auto [seq, inserted] = ht.insert(10, x);
		ASSERT(inserted);
		ASSERT_EQ((*seq).first, 10);
		ASSERT_EQ(ht.size(), 1);
	}
	else {
		ht.insert(10);
		ASSERT_EQ(ht.size(), 1);

	}
	if constexpr (has_mapped) { //insert dup key
		ht.insert(10, "ABC"); //duplicate key if !multi
		if constexpr (is_multi) {
			ASSERT_EQ(ht.size(), 2);
		}
		else {
			ASSERT_EQ(ht.size(), 1);
		}
	}
	if constexpr (has_mapped) {
		auto x = "DEF";
		[[maybe_unused]] const auto [rseq, inserted] = ht.insert(11, x);
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
		const auto rseq1 = ht.crseq();
		if constexpr (is_multi) {
			check_seq(rseq1, { 11,10,10 });
		}
		else {
			check_seq(rseq1, { 11,10 });
		}
	}
	else {
		const auto [seq, inserted] = ht.insert(11);
		ASSERT(inserted);
		check_seq(seq, { 11 });
		ASSERT_EQ(ht.size(), 2);
		const auto seq1 = ht.cseq();
		check_seq(seq1, { 10,11 });
	}
	{ //find
		const auto seq = ht.cfind(11);
		ASSERT(seq);
		check_seq(seq, { 11 });
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
		ht.reset();
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
		const auto seq = ht.seq();
		check_seq(seq, { 20});
		const auto rseq = ht.seq();
		check_seq(rseq, { 20 });

	}
#if 1
	{ //merge
		if (!std::is_constant_evaluated()) {
			const auto res = ht.try_merge(ht);
			ASSERT(!res); //cannot merge itself 
		}
		table_t ht1;
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
			ASSERT_EQ(sz, 2);
			ASSERT_EQ(ht.size(), 3);
			ASSERT_EQ(ht1.size(),1);
		}
	}
#endif 
	{ //clone and release all
		ht.reset();
		ASSERT(!ht.size());
		for (key_t i = 0; i < 5; ++i) {
			auto j = iron::clone::clone_from(i);
			if constexpr (has_mapped) {
				[[maybe_unused]] const auto [rseq, inserted] = ht.insert(std::move(j), std::string("ABC"));
				ASSERT(inserted);
			}
			else {
				[[maybe_unused]] const auto [rseq, inserted] = ht.insert(std::move(j));
				ASSERT(inserted);
			}
		}
		ASSERT_EQ(ht.size(), 5);
		auto t = ht.clone();
		ASSERT_EQ(t, ht);
		//ASSERT_EQ(t <=> ht, std::strong_ordering::equal);
		auto seq = t.seq();
		check_seq(seq, { 0,1,2,3,4 });
		t = ht.clone(2);
		if constexpr (is_multi) {
			ASSERT_NE(t, ht);
		}
		else {
			ASSERT_EQ(t, ht);
		}
		seq = t.seq();
		if constexpr (is_multi) {
			check_seq(seq, { 0,0,1,1,2,2,3,3,4,4 });
		}
		else {
			check_seq(seq, { 0,1,2,3,4 });
		}
		seq = {};
		auto rseq = t.rseq();
		if constexpr (is_multi) {
			check_seq(rseq, { 4,4,3,3,2,2,1,1,0,0 });
		}
		else {
			check_seq(rseq, { 4,3,2,1,0 });
		}
		rseq.reset();
		auto adapter = t.release_all().unsafe_release();

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif 
		if constexpr (is_multi) {
			ASSERT_EQ(adapter.size(), 10);
		}
		else {
			ASSERT_EQ(adapter.size(), 5);
		}
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
			if constexpr (is_multi) {
				++b;
				ASSERT(b != adapter.data() + adapter.size());
				if constexpr (has_mapped) {
					ASSERT_EQ(b->first, i);
					ASSERT_EQ(b->second, "ABC");
				}
				else {
					ASSERT_EQ(*b, i);
				}
			}
			++b;
		}
		ASSERT_EQ(b, adapter.data() + adapter.size());
		ASSERT(!t.size());
#if defined(_MSC_VER)
#pragma warning(pop)
#endif 
	}
	{ //insert range
		ht.reset();
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
		table_t t;
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
		ht.reset();
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
				auto hm = iron::collector::make_multi_btree_map<key_t, mapped_t>(key_t{ 1 }, mapped_t{}, key_t{ 2 }, mapped_t{});
				auto seq = hm.cseq();
				check_seq(seq, { 1,2 });
				hm = iron::collector::make_multi_btree_map<key_t, mapped_t>(key_t{ 1 }, mapped_t{}, key_t{ 2 }, mapped_t{}, key_t{ 2 }, mapped_t{}, key_t{ 3 });
				seq = hm.cseq();
				check_seq(seq, { 1,2,2,3 });
				auto a = adapter.clone();
				const auto const_seq = seq;
				auto x = seq;
				auto hm1 = iron::collector::make_multi_btree_map<key_t, mapped_t>(std::move(a), adapter.clone(), key_t{ 3 }, const_seq, key_t{ 4 }, seq, std::move(x));
				seq = hm1.cseq();
				check_seq(seq, { 1,1,1,2,2,2,2,2,2,3,3,3,3,4,10,10,20,20,30,30 });
			}
			else {
				auto hm = iron::collector::make_btree_map<key_t, mapped_t>(key_t{ 1 }, mapped_t{}, key_t{ 2 }, mapped_t{});
				auto seq = hm.cseq();
				check_seq(seq, { 1,2 });
				hm = iron::collector::make_btree_map<key_t, mapped_t>(key_t{ 1 }, mapped_t{}, key_t{ 2 }, mapped_t{}, key_t{ 2 }, mapped_t{}, key_t{ 3 });
				seq = hm.cseq();
				check_seq(seq, { 1,2,3 });
				auto a = adapter.clone();
				const auto const_seq = seq;
				auto x = seq;
				auto hm1 = iron::collector::make_btree_map<key_t, mapped_t>(std::move(a), adapter.clone(), key_t{ 3 }, const_seq, key_t{ 4 }, seq, std::move(x));
				seq = hm1.cseq();
				check_seq(seq, { 1,2,3,4,10,20,30 });
			}
		}
		else {
			if constexpr (is_multi) {
				auto hm = iron::collector::make_multi_btree_set<key_t>(key_t{ 1 }, key_t{ 2 });
				auto seq = hm.cseq();
				check_seq(seq, { 1,2 });
				hm = iron::collector::make_multi_btree_set<key_t>(key_t{ 1 }, key_t{ 2 }, key_t{ 2 }, key_t{ 3 });
				seq = hm.cseq();
				check_seq(seq, { 1,2,2,3 });
				auto a = adapter.clone();
				const auto const_seq = seq;
				auto x = seq;
				auto hm1 = iron::collector::make_multi_btree_set<key_t>(std::move(a), adapter.clone(), key_t{ 3 }, const_seq, key_t{ 4 }, seq, std::move(x));
				seq = hm1.cseq();
				check_seq(seq, { 1,1,1,2,2,2,2,2,2,3,3,3,3,4,10,10,20,20,30,30 });
			}
			else {
				auto hm = iron::collector::make_btree_set<key_t>(key_t{ 1 }, key_t{ 2 });
				auto seq = hm.cseq();
				check_seq(seq, { 1,2 });
				hm = iron::collector::make_btree_set<key_t>(key_t{ 1 }, key_t{ 2 }, key_t{ 2 }, key_t{ 3 });
				seq = hm.cseq();
				check_seq(seq, { 1,2,3 });
				auto a = adapter.clone();
				const auto const_seq = seq;
				auto x = seq;
				auto hm1 = iron::collector::make_btree_set<key_t>(std::move(a), adapter.clone(), key_t{ 3 }, const_seq, key_t{ 4 }, seq, std::move(x));
				seq = hm1.cseq();
				check_seq(seq, { 1,2,3,4,10,20,30 });

			}
		}
	}
	return true;
}

class not_copiable {
public:
	constexpr not_copiable()noexcept : m_i() {}
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
	constexpr operator int()const noexcept {
		IRON_SECURITY_CHECK_TEMPORARY_THIS();
		return m_i;
	}
	constexpr not_copiable& operator++()noexcept { ++m_i; return *this; }
//	constexpr bool operator==(const not_copiable& v)const noexcept { return m_i == v.m_i; }
	IRON_SECURITY_METHOD();
	
private:
	constexpr void _swap(not_copiable& v)noexcept {
		std::swap(m_i, v.m_i);
		IRON_SECURITY_SWAP(v);
	}
	int m_i;
	IRON_SECURITY_DCL();
};


int main() {
	f<iron::btree_map<int, std::string>>();
	f<iron::btree_map<not_copiable, std::string>>();
	f<iron::multi_btree_map<int, std::string>>();
	f<iron::multi_btree_map<not_copiable, std::string>>();
	f<iron::btree_set<int>>();
	f<iron::btree_set<not_copiable>>();
	f<iron::multi_btree_set<int>>();
	f<iron::multi_btree_set<not_copiable>>();
 

	using btree_t = iron::unsafe::btree::raw_table<
		not_copiable
		, int
		, iron::comparables::less_to<not_copiable>
		, 5 //order
		, iron::unsafe::allocator<std::pair<not_copiable, int>>
		, false //multi
		, true //mapped
	>;
	f_simple_erase<btree_t>();

//	constexpr unsigned int seed = 3328144790;
	constexpr unsigned int seed = 4187184192;
#ifdef NDEBUG
	f_simple_random_erase<btree_t>(1, 1000, seed);
#else 
	f_simple_random_erase<btree_t>(1, 100, seed);
#endif 
	btree_insert<btree_t>(1, 100);
	btree_insert_reverse<btree_t>(1, 100);
	using btree_multi_t = iron::unsafe::btree::raw_table<
		not_copiable
		, int
		, iron::comparables::less_to<not_copiable>
		, 5 //order
		, iron::unsafe::allocator<std::pair<not_copiable, int>>
		, true //multi
		, true //mapped
	>;
	f_simple_random_insert<btree_multi_t>();
	btree_insert<btree_multi_t>(1, 100);
	btree_insert_reverse<btree_multi_t>(1, 100);
#ifdef NDEBUG
	f_simple_random_erase<btree_t>(1, 1000, seed);
#else 
	f_simple_random_erase<btree_t>(1, 100, seed);
#endif 

#if 1
	[[maybe_unused]] constexpr bool b1 = f_simple_erase<btree_t>();
	[[maybe_unused]] constexpr bool b2=btree_insert<btree_t>(1, 100);
	[[maybe_unused]] constexpr bool b3=btree_insert_reverse<btree_t>(1, 100);

	[[maybe_unused]] constexpr bool b4=f<iron::btree_map<int, std::string>>();
	[[maybe_unused]] constexpr bool b5=f<iron::btree_map<not_copiable, std::string>>();
	[[maybe_unused]] constexpr bool b6=f<iron::multi_btree_map<int, std::string>>();
	[[maybe_unused]] constexpr bool b7=f<iron::multi_btree_map<not_copiable, std::string>>();
	
	
	[[maybe_unused]] constexpr bool b8=f<iron::btree_set<int>>();
	[[maybe_unused]] constexpr bool b9=f<iron::btree_set<not_copiable>>();
	[[maybe_unused]] constexpr bool b10=f<iron::multi_btree_set<int>>();
	[[maybe_unused]] constexpr bool b11=f<iron::multi_btree_set<not_copiable>>();

#endif 

}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
