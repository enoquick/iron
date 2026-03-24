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
#include "iron/memory/spin_lock.h"
#include "iron/unsafe/asserts.h"
#include <iostream>
#endif

using spl_t = iron::memory::spin_lock;
using rspl_t = iron::memory::recursive_spin_lock;

void f_no_lock(int n) {
	for (std::size_t i = 0; i < 50; ++i) {
		std::cout << n;
	}
	std::cout << std::endl;
}

template <typename LOCK>
void f_lock_guard(LOCK* m,int n) {
	ASSERT(m);
	const auto lock = iron::memory::make_lock_guard(*m);
	if constexpr (std::is_same_v<LOCK, rspl_t>) {
		const auto lock1 = iron::memory::make_lock_guard(*m);
	}
	f_no_lock(n);
}

template <typename LOCK>
void f_acquire(LOCK* lk,int n) {
	ASSERT(lk);
	lk->acquire();
	if constexpr (std::is_same_v<LOCK, rspl_t>) {
		lk->acquire();
	}
	f_no_lock(n);
	if constexpr (std::is_same_v<LOCK, rspl_t>) {
		lk->release();
	}
	lk->release();
}

template <typename LOCK>
void create_threads(void (*f)(LOCK*,int),LOCK* lk, int n) {
	std::vector<std::thread> v;
	for (int i = 0; i < n; ++i) {
		v.push_back(std::thread(f,lk, i));
	}
	for (auto& th : v) {
		th.join();
	}
}

inline void f_called(spl_t& lk,rspl_t& rlk) {
	std::cout << std::endl << "acquire" << std::endl;
	create_threads(f_acquire, &lk, 10);
	std::cout << std::endl << "recursive acquire" << std::endl;
	create_threads(f_acquire, &rlk, 10);
	std::cout << std::endl << "lock guard" << std::endl;
	create_threads(f_lock_guard,&lk, 10);
	std::cout << std::endl << "recursive lock guard" << std::endl;
	create_threads(f_lock_guard, &rlk, 10);

}

int main() {
	spl_t m;
	rspl_t rm;
	f_acquire(&m,3);
	f_acquire(&rm,3);
	f_lock_guard(&m, 4);
	f_lock_guard(&rm, 4);
	f_called(m, rm);
}