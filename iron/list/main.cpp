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
#include <list>
#include <forward_list>
#include <vector>
#include <iostream>
//#ifndef NDEBUG
#define _IRON_CHECK_LIST
//#endif 
#include "iron/collector.h"
#include "iron/list.h"
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

template <typename ITER, typename T>
constexpr void check_iter(ITER b,ITER e,const std::initializer_list<T> v) {
	for (auto& i : v) {
		ASSERT(b != e);
		const auto& vt = *b++;
		ASSERT_EQ(vt, i);
	}
	ASSERT(b == e);
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26481)
#endif

template <typename LIST>
constexpr bool f() {
	using v_t = typename LIST::value_t;
	constexpr auto is_bd = LIST::is_bidirectional;
	using s_t = LIST;
	if constexpr (is_bd) { //const 
		auto list = iron::collector::make_list<v_t>(1, 2, 4);
		const auto seq = list.cseq();
		const auto r = *seq;
		auto& v = *r;
		static_assert(std::is_const_v<std::remove_reference_t<decltype(v)>>, "internal error");
		const auto rseq = list.crseq();
		const auto r1 = *rseq;
		auto& v1 = *r1;
		static_assert(std::is_const_v<std::remove_reference_t<decltype(v1)>>, "internal error");
	}
	if constexpr (is_bd) { //not const 
		auto list = iron::collector::make_list<v_t>(1, 2, 4);
		const auto seq = list.seq();
		const auto r = *seq;
		auto& v = *r;
		static_assert(!std::is_const_v<std::remove_reference_t<decltype(v)>>, "internal error");
		const auto rseq = list.rseq();
		const auto r1 = *rseq;
		auto& v1 = *r1;
		static_assert(!std::is_const_v<std::remove_reference_t<decltype(v1)>>, "internal error");
	}
	if constexpr (is_bd) { //rev iter
		s_t list;
		auto seq = list.rseq();
		ASSERT(!seq);
		list.push_front(1);
		seq = list.rseq();
		ASSERT(seq);
		ASSERT_EQ(*seq++, v_t(1));
		ASSERT(!seq);

	}
	if constexpr(std::is_nothrow_default_constructible_v<v_t>) { //construction / front / back /push_front/ push back / adapter_box 
		s_t list;
		ASSERT(list.empty());
		v_t val{};
		list.push_front(++val);
		ASSERT_EQ(list.front().unwrap(), val);
		{
			auto r = list.try_push_front(++val);
			if (r) {
				//auto r1 = r.unwrap(); //TODO - ERROR - the problem is in refw
				//ASSERT_EQ(r.unwrap(), val); //TODO ambiguos
				ASSERT_EQ(list.front().unwrap(), val);
			}
		}
		list.push_front(++val);
		ASSERT_EQ(list.front().unwrap(), val);
		{
			auto r = list.try_push_front(val);
			if (r) {
				//ASSERT_EQ(r.unwrap(), n); //TODO ambiguos
				ASSERT_EQ(list.front().unwrap(), val);
			}
		}
		auto k = list.front().unwrap().value();
		ASSERT_EQ(k, list.pop_front().unwrap());
		if constexpr (is_bd) {
			list.push_back(++val);
			ASSERT_EQ(list.back().unwrap(), val);
			list.push_back(++val);
			ASSERT_EQ(list.back().unwrap(), val);
			{
				auto r = list.try_push_back(++val);
				if (r) {
					//ASSERT_EQ(r.unwrap(), n); //TODO ambiguos
					ASSERT_EQ(list.back().unwrap(), val);
				}
			}
			list.push_back(++val);
			ASSERT_EQ(list.back().unwrap(), val);
			k = list.back().unwrap().value();
			ASSERT_EQ(k, list.pop_back().unwrap());
			const auto k1 = list.back().unwrap().value();
			ASSERT_EQ(k1, k - 1);
		}
		list.reset();
		ASSERT(list.empty());
		ASSERT(!list.front());
		if constexpr (is_bd) {
			ASSERT(!list.back());
			list.push_back(v_t{ ++val });
			list.push_back(v_t{ ++val });
			list.pop_front();
			list.pop_front();
			ASSERT(list.empty());
			list.push_back(v_t{ ++val });
			list.push_back(v_t{ ++val });
			list.pop_back();
			list.pop_back();
			ASSERT(list.empty());
		}
		list.reset();
		list.push_front(v_t{ ++val });
		list.push_front(v_t{ ++val });
		auto box = list.release();
		ASSERT(list.empty());
		ASSERT_EQ(box.capacity(), 2);
		ASSERT_EQ(box.size(), 2);
		if constexpr (is_bd) {
			const auto l = iron::collector::make_list<v_t>(std::move(box));
			list = iron::collector::make_list<v_t>(l.seq());
			ASSERT_EQ(l, list);
		}
		else {
			const auto l = iron::collector::make_fwd_list<v_t>(std::move(box));
			list = iron::collector::make_fwd_list<v_t>(l.seq());
			list.reverse();
			ASSERT_EQ(l, list);
		}
	}
	if constexpr (is_bd) { //adapter_box
		auto list = iron::collector::make_list<v_t>(1, 2, 3);
		auto mbox = list.release().unsafe_release();
		ASSERT(list.empty());
		ASSERT_EQ(mbox.capacity(), 3);
		ASSERT_EQ(mbox.size(), 3);
		auto p = mbox.data();
		check_iter(p, p + mbox.size(), { 1,2,3 });
		p = {};
		list = iron::collector::make_list<v_t>(1, 2, 3, 4);
		list.reverse();
		auto box = list.release();
		auto cbox = box.clone();
		mbox = box.unsafe_release();
		ASSERT(list.empty());
		ASSERT_EQ(mbox.capacity(), 4);
		ASSERT_EQ(mbox.size(), 4);
		p = mbox.data();
		check_iter(p, p + mbox.size(), { 4,3,2,1 });
		p = {};
		list = iron::collector::make_list<v_t>(std::move(cbox));
		ASSERT_EQ(list.size(), 4);
		auto list1 = list.clone(2);
		ASSERT_EQ(list1.size(), 8);
		mbox = list1.release().unsafe_release();
		ASSERT(list1.empty());
		ASSERT_EQ(mbox.size(), 8);
		p = mbox.data();
		check_iter(p, p + mbox.size(), { 4,3,2,1,4,3,2,1 });
	}
	else {
		auto list = iron::collector::make_fwd_list<v_t>(1, 2, 3);
		auto cseq = list.cseq();
		check_seq(cseq, { 3,2,1 });
		cseq = {};
		auto mbox = list.release().unsafe_release();
		ASSERT(list.empty());
		ASSERT_EQ(mbox.capacity(), 3);
		ASSERT_EQ(mbox.size(), 3);
		auto p = mbox.data();
		check_iter(p, p + mbox.size(), { 3,2,1 });
		list = iron::collector::make_fwd_list<v_t>(1, 2, 3,4);
		cseq = list.cseq();
		check_seq(cseq, { 4,3,2,1 });
		list.reverse();
		cseq = list.cseq();
		check_seq(cseq, { 1,2,3,4});
		cseq = {};
		auto box = list.release();
		auto cbox = box.clone();
		ASSERT(list.empty());
		ASSERT_EQ(box.capacity(), 4);
		ASSERT_EQ(box.size(), 4);
		mbox = box.unsafe_release();
		p = mbox.data();
		check_iter(p, p + mbox.size(), { 1,2,3,4 });
		list = iron::collector::make_fwd_list<v_t>(std::move(cbox));
		cseq = list.cseq();
		check_seq(cseq, { 4,3,2,1 });
		auto list1 = list.clone(2);
		cseq = list1.cseq();
		check_seq(cseq, { 4,3,2,1,4,3,2,1 });
		cseq = {};
		mbox = list1.release().unsafe_release();
		ASSERT_EQ(mbox.capacity(), 8);
		ASSERT_EQ(mbox.size(), 8);
		p = mbox.data();
		check_iter(p, p + mbox.size(), { 4,3,2,1,4,3,2,1 });
	}
	{ //reverse
		s_t list;
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		ASSERT_EQ(seq.next().unwrap().value(), 1);
		list.reverse();
		ASSERT_EQ(seq.next().unwrap().value(), 2);
		ASSERT_EQ(seq.next().unwrap().value(), 1);
		ASSERT(!seq.next());

		if constexpr (is_bd) {
			auto rseq = list.rseq();
			{
				auto _b = rseq.begin();
				auto& v = *_b;
				ASSERT_EQ(v, 1);
				++_b;
				auto& v1 = *_b;
				ASSERT_EQ(v1, 2);
			}
			const auto rx = *rseq++;
			ASSERT_EQ(rx, v_t(1));
			check_seq(list.rseq(), { 1,2,3 });
		}
		if constexpr (is_bd) {
			auto rseq = list.crseq();
			{
				auto _b = rseq.begin();
				auto& v = *_b;
				ASSERT_EQ(v, 1);
				++_b;
				auto& v1 = *_b;
				ASSERT_EQ(v1, 2);
			}
			const auto rx = *rseq++;
			ASSERT_EQ(rx, v_t(1));
			check_seq(list.rseq(), { 1,2,3 });
		}
	}
	{ //slice 
		s_t list;
		auto seq=list.push_front(3);
		ASSERT_EQ(*seq, v_t(3));
		seq=list.push_front(2);
		ASSERT_EQ(*seq, v_t(2));
		seq=list.push_front(1);
		ASSERT_EQ(*seq, v_t(1));
		seq = list.seq();
		ASSERT_EQ(*seq, v_t(1));
		auto cseq3 = seq.clone().slice(1);
		ASSERT(cseq3);
		ASSERT_EQ(*cseq3, v_t(2));
		ASSERT_EQ(cseq3.count(), 2);
		auto cseq4 = seq.clone().slice(1, 1);
		ASSERT_EQ(*cseq4, v_t(2));
		ASSERT_EQ(cseq4.count(), 1);
		const auto cseq5 = seq.slice(4);
		ASSERT(cseq5.empty());
	}
	{ //seq insert after
		s_t list;
		if (!std::is_constant_evaluated()) {
			const auto seq = list.seq();
			const auto res = list.try_insert_after(seq, 6); //error - seq is empty
			ASSERT(!res);
		}
		list.push_front(6);
		check_seq(list.seq(), { 6 });
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		{
			auto cseq = list.cseq();
			++cseq;
			const auto r = *cseq;
			ASSERT_EQ(r, v_t(2));
			const auto seq=list.insert_after(cseq, 4);
			ASSERT_EQ(*seq, v_t(4));
			check_seq(list.cseq(), { 1,2,4,3,6 });
		}
	}
	if constexpr (is_bd) { //rseq insert after
		s_t list;
		if (!std::is_constant_evaluated()) {
			const auto seq = list.rseq();
			const auto res = list.try_insert_after(seq, 6); //error - seq empty
			ASSERT(!res);
		}
		list.push_back(6);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.crseq();
		ASSERT_EQ(*seq++, v_t(6));
		ASSERT_EQ(*seq, v_t(3));
		list.insert_after(seq, 4);
		check_seq(list.crseq(), { 6,4,3,2,1 });
		list.insert_after(seq, 10);
		check_seq(list.crseq(),{6,10,4,3,2,1});
	}

	if constexpr (is_bd) { //seq insert before
		s_t list;
		if (!std::is_constant_evaluated()) {
			const auto seq = list.seq();
			const auto res = list.try_insert_before(seq, 5);
			ASSERT(!res);
		}
		list.push_back(5);
		const auto seq1 = list.seq();
		auto seq=list.insert_before(seq1,1000);
		ASSERT_EQ(*seq, v_t(1000));
		check_seq(list.seq(), {1000, 5});
		seq=list.insert_before(seq1, 1001);
		ASSERT_EQ(*seq, v_t(1001));
		check_seq(list.seq(), { 1000,1001, 5 });
	}
	if constexpr (is_bd) { //rseq insert before
		s_t list;
		if (!std::is_constant_evaluated()) {
			const auto seq = list.rseq();
			const auto res = list.try_insert_before(seq, 5);
			ASSERT(!res); //error - empty seq
		}
		list.push_back(5);
		const auto seq1 = list.rseq();
		list.insert_before(seq1, 1000);
		check_seq(list.rseq(), { 1000,5 });
		const auto seq = list.rseq();
		list.insert_before(seq, 1001);
		check_seq(list.rseq(), { 1001,1000,5 });
	}

	if constexpr(is_bd) { //seq erase_n
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		{
			const auto r = *seq;
			ASSERT_EQ(r, v_t(1));
		}
		auto n = list.erase_n(seq, 2); //drop 1 and 2
		ASSERT_EQ(n, 2);
		ASSERT_EQ(seq.clone().count(), 3);
		check_seq(seq, { 3,4,5 });
		seq = list.seq();
		n = list.erase_n(seq, 1); //drop 3
		ASSERT_EQ(n, 1);
		ASSERT_EQ(seq.clone().count(), 2);
		check_seq(seq, { 4,5 });
	}
	{ //seq erase_n_after
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		{
			const auto r = *seq;
			ASSERT_EQ(r, v_t(1));
		}
		auto n = list.erase_n_after(seq, 2); //drop 2,3
		ASSERT_EQ(n, 2);
		ASSERT_EQ(seq.clone().count(), 2);
		check_seq(seq, { 4,5 });
		seq = list.seq();
		n = list.erase_n_after(seq, 1); //drop 4
		ASSERT_EQ(n, 1);
		ASSERT_EQ(seq.clone().count(), 1);
		check_seq(seq, { 5 });
	}

	if constexpr(is_bd) { //rseq erase_n
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.rseq();
		{
			const auto r = *seq;
			ASSERT_EQ(r, v_t(5));
		}
		auto n = list.erase_n(seq, 2); //drop 5 and 4
		ASSERT_EQ(n, 2);
		ASSERT_EQ(seq.count(), 3);
		check_seq(list.rseq(), { 3,2,1 });
		seq = list.rseq();
		check_seq(seq, { 3,2,1 });
		ASSERT_EQ(*seq, v_t(3));
		n = list.erase_n(seq, 1);
		ASSERT_EQ(n, 1);
		ASSERT_EQ(seq.clone().count(), 2);
		check_seq(seq, { 2,1 });

	}

	if constexpr(is_bd) { //seq erase all
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		++seq;
		const auto n = list.erase_all(seq); //erase 2..5
		ASSERT_EQ(n, 4);
		ASSERT(!seq);
		check_seq(list.seq(), { 1 });
	}
	{ //seq erase all after
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		++seq;
		const auto n = list.erase_all_after(seq); //erase 3..5
		ASSERT_EQ(n, 3);
		ASSERT(!seq);
		check_seq(list.seq(), { 1,2 });
	}
	if constexpr(is_bd) { //rseq erase all
		s_t list;
		auto i = 5;
		while (i) {
			list.push_front(i--);
		}
		auto seq = list.rseq();
		++seq;
		const auto n = list.erase_all(seq); //erase 4..1
		ASSERT_EQ(n, 4);
		ASSERT(!seq);
		check_seq(list.rseq(), { 5 });
	}

	if constexpr(is_bd) { //seq erase first
		s_t list;
		auto seq = list.seq();
		ASSERT(!seq);
		auto opt = list.erase_first(seq);
		ASSERT(!opt);
		list.push_front(5);
		list.push_front(4);
		seq = list.seq();
		opt = list.erase_first(seq);
		ASSERT(opt);
		ASSERT_EQ(opt.unwrap(), v_t(4));
		check_seq(seq, { 5 });
		list.push_front(100);
		seq = list.seq();
		const auto v = list.unchecked_erase_first(seq);
		ASSERT_EQ(v, 100);
		check_seq(seq, { 5 });
	}
	{ //seq erase first after
		s_t list;
		auto seq = list.seq();
		ASSERT(!seq);
		auto opt = list.erase_first_after(seq);
		ASSERT(!opt);
		list.push_front(6);
		list.push_front(5);
		list.push_front(4);
		seq = list.seq();
		opt = list.erase_first_after(seq);
		ASSERT(opt);
		ASSERT_EQ(opt.unwrap(), v_t(5));
		check_seq(seq, { 6 });
		list.push_front(100);
		seq = list.seq();
		const auto v = list.unchecked_erase_first_after(seq);
		ASSERT_EQ(v, 4);
		check_seq(seq, { 6 });
	}

	if constexpr(is_bd) { //rseq erase first
		s_t list;
		auto seq = list.rseq();
		ASSERT(!seq);
		auto opt = list.erase_first(seq);
		ASSERT(!opt);
		list.push_front(5);
		list.push_front(4);
		seq = list.rseq();
		opt = list.erase_first(seq);
		ASSERT(opt);
		ASSERT_EQ(opt.unwrap(), v_t(5));
		check_seq(seq, { 4 });
		list.push_front(100);
		seq = list.rseq();
		const auto v = list.unchecked_erase_first(seq);
		ASSERT_EQ(v, 4);
		check_seq(seq, { 100 });
	}

	{ //erase_if 
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		const auto lambda = [](const v_t& v)constexpr noexcept -> bool { return v == 3 || v == 1; };
		auto n = list.erase_if(lambda);
		ASSERT_EQ(n, 2);
		n = list.erase(5);
		ASSERT_EQ(n, 1);
		const auto seq = list.seq();
		ASSERT_EQ(seq.clone().count(), 2);
		check_seq(seq, { 2,4 });
	}
	if constexpr(is_bd) { //seq erase_if 
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		++seq;
		ASSERT_EQ(*seq, v_t(2));
		const auto lambda = [](const v_t& v)constexpr noexcept -> bool { return v == 1 || v == 2 || v == 5; };
		const auto n = list.erase_if(seq,lambda);
		ASSERT_EQ(n, 2U);
		ASSERT(!seq);
		check_seq(list.seq(), { 1,3,4 });
	}
	{ //seq erase_if_after
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		++seq;
		ASSERT_EQ(*seq, v_t(2));
		const auto lambda = [](const v_t& v)constexpr noexcept -> bool { return v == 1 || v == 2 || v == 5; };
		const auto n = list.erase_if_after(seq,lambda);//drop 5
		ASSERT_EQ(n, 1);
		ASSERT(!seq);
		check_seq(list.seq(), { 1,2,3,4 });
	}
	if constexpr (is_bd) { //rseq erase_if 
		s_t list;
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.rseq();
		++seq;
		ASSERT_EQ(*seq, v_t(4));
		const auto lambda = [](const v_t& v)constexpr noexcept -> bool { return v == 1 || v == 3 || v == 4; };
		const auto n = list.erase_if(seq,lambda);
		ASSERT_EQ(n, 3);
		ASSERT(!seq);
		check_seq(list.rseq(), { 5,2 });
	}

	{ //unique
		s_t list;
		list.push_front(4);
		list.push_front(4);
		list.push_front(3);
		list.push_front(3);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		list.push_front(1);
		list.push_front(1);
		list.push_front(1);
		list.push_front(0);
		const auto sz=list.unique();
		ASSERT_EQ(sz,5);
		check_seq(list.cseq(), { 0,1,2,3,4 });
	}
	if constexpr(std::is_default_constructible_v<v_t>) { //resize 
		s_t list;
		list.resize(0);
		ASSERT(list.empty());
		list.resize(10, v_t{2});
		ASSERT_EQ(list.count(),10);
		auto seq = list.seq();
		while (seq) {
			const auto r = *seq++;
			ASSERT_EQ(r, v_t{2});
		}
		list.reset();
		list.push_front(5);
		list.push_front(4);
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		list.resize(2);
		ASSERT_EQ(list.count(), 2);
		check_seq(list.seq(), { 1,2 });
		list.resize(2);
		ASSERT_EQ(list.count(), 2);
		check_seq(list.seq(), { 1,2 });
		list.resize(4);
		ASSERT_EQ(list.count(), 4);
		check_seq(list.seq(), { 1,2,0,0 });
		constexpr size_t n = 100;
		list.resize(n);
		ASSERT_EQ(list.count(),n);
	}
	{ //clone
		s_t list;
		auto list1 = list.clone();
		ASSERT(list1.empty());
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		list1 = list.clone(2);
		ASSERT_EQ(list1.count(), 6);
		check_seq(list1.seq(), { 1,2,3,1,2,3 });
	}
	if constexpr (is_bd) { //range insert before - seq
		s_t list;
		s_t list1;
		if (!std::is_constant_evaluated()) {
			auto seq = list.seq();
			const auto res = list1.try_insert_range_before(list1.seq(), seq);
			ASSERT(!res); //the first seq is empty
			using posix_t = iron::unsafe::errors::posix;
			ASSERT_EQ(res.unwrap_err()->numeric_error(), posix_t::numeric_code(posix_t::code_t::argument_out_of_domain)); 
		}
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		list1.push_front(10);
		auto seq = list.seq();
		auto sz=list1.insert_range_before(list1.seq(),seq);
		ASSERT_EQ(sz, 3);
		ASSERT_EQ(list1.count(),4);
		check_seq(list1.seq(), { 1,2,3,10 });
		seq = {};
		list.reset();
		list.push_front(200);
		list.push_front(100);
		seq = list.seq();
		auto seq1 = list1.seq();
		++seq1;
		const auto r = *seq1;
		ASSERT_EQ(r, v_t(2));
		sz = list1.insert_range_before(seq1, seq);
		ASSERT_EQ(sz, 2U);
		seq = list1.seq();
		check_seq(seq, { 1,100,200,2,3,10 });
	}
	if constexpr (is_bd) { //range insert before - rseq
		s_t list;
		s_t list1;
		if (!std::is_constant_evaluated()) {
			auto seq = list.seq();
			const auto res = list1.try_insert_range_before(list1.rseq(), seq);
			ASSERT(!res); //the first seq is empty
		}
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		list1.push_front(10);
		auto seq = list.seq();
		const auto rseq = list1.rseq();
		auto r = *rseq;
		auto sz = list1.insert_range_before(rseq, seq);
		ASSERT_EQ(sz, 3);
		ASSERT_EQ(list1.count(), 4);
		check_seq(list1.seq(), { 1,2,3,10 });
		seq = {};
		list.reset();
		list.push_front(200);
		list.push_front(100);
		seq = list.seq();
		auto seq1 = list1.rseq();
		++seq1;
		r = *seq1;
		ASSERT_EQ(r, v_t(3));
		sz = list1.insert_range_before(seq1, seq);
		ASSERT_EQ(sz, 2);
		seq = list1.seq();
		check_seq(seq, { 1,2,100,200,3,10 });
	}
	{ //range insert after - seq 
		s_t list;
		s_t list1;
		if (!std::is_constant_evaluated()) {
			auto seq = list.seq();
			const auto res = list1.try_insert_range_after(list1.seq(), seq);
			ASSERT(!res); //the first seq is empty
		}
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		list1.push_front(10);
		auto seq = list.seq();
		const auto seq1 = list1.seq();
		const auto r = *seq1;
		ASSERT_EQ(r, v_t(10));
		list1.insert_range_after(seq1, seq);
		seq = list1.seq();
		check_seq(seq, { 10,1,2,3 });
	}
	if constexpr(is_bd){ //range insert after - rseq
		s_t list;
		s_t list1;
		if (!std::is_constant_evaluated()) {
			auto seq = list.seq();
			const auto res = list1.try_insert_range_after(list1.rseq(), seq);
			ASSERT(!res); //the first seq is empty
		}
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		list1.push_front(10);
		auto seq = list.seq();
		const auto seq1 = list1.rseq();
		const auto r = *seq1;
		ASSERT_EQ(r, v_t(10));
		list1.insert_range_after(seq1, seq);
		seq = list1.seq();
		check_seq(seq, { 10,1,2,3 });
	}
	{ //range push front 
		s_t list,list1;
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		check_seq(seq, { 1,2,3});
		list1.push_front(10);
		seq = list.seq();
		list1.push_front_range(seq);
		seq = list1.seq();
		check_seq(seq, { 3,2,1,10 });
	}
	if constexpr(is_bd) {//range push back
		s_t list, list1;
		list.push_front(3);
		list.push_front(2);
		list.push_front(1);
		auto seq = list.seq();
		check_seq(seq, { 1,2,3 });
		list1.push_front(10);
		seq = list.seq();
		list1.push_back_range(seq);
		seq = list1.seq();
		check_seq(seq, { 10,1,2,3 });
	}
	if constexpr (is_bd) { //make list
		auto list = iron::collector::make_list<v_t>(1, 2, 3);
		ASSERT_EQ(list, list);
		auto adapter = list.release();
		list = iron::collector::make_list<v_t>(10, 20, 30);
		const auto seq = list.seq();
		const auto list1 = iron::collector::make_list<v_t>(adapter.clone(), seq, adapter.clone(), 40, seq);
		const auto seq1 = list1.seq();
		check_seq(seq1, { 1,2,3,10,20,30,1,2,3,40,10,20,30 });
	}
	else { //make fwd list
		auto list = iron::collector::make_fwd_list<v_t>(1, 2, 3);
		ASSERT_EQ(list, list);
		list.reverse();
		auto adapter = list.release();
		list = iron::collector::make_fwd_list<v_t>(10, 20, 30);
		list.reverse();
		const auto seq = list.seq();
		auto list1 = iron::collector::make_fwd_list<v_t>(adapter.clone(), seq,adapter.clone(),40, seq);
		list1.reverse();
		const auto seq1 = list1.seq();
		check_seq(seq1, { 1,2,3,10,20,30,1,2,3,40,10,20,30 });
	}
	return true;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

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
	constexpr int get()const noexcept { return m_i; }
	constexpr not_copiable& operator++()noexcept { ++m_i; return *this; }
	constexpr bool operator==(const not_copiable& v)const noexcept { return m_i == v.m_i; }
private:
	int m_i;
};

int main() {
	f<iron::fwd_list<int>>();
	f<iron::fwd_list<not_copiable>>();
	f<iron::list<int>>();
	f<iron::list<not_copiable>>();

#if 1
	[[maybe_unused]] constexpr auto b1 = f<iron::fwd_list<int>>();
	[[maybe_unused]] constexpr auto b2 = f<iron::fwd_list<not_copiable>>();

	[[maybe_unused]] constexpr auto b3 = f<iron::list<int>>();
	[[maybe_unused]] constexpr auto b4 = f<iron::list<not_copiable>>();

#endif 
}
