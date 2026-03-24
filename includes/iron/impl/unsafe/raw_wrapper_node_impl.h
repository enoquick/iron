#ifndef _IRON_UNSAFE_RAW_WRAPPER_NODE_H
#define _IRON_UNSAFE_RAW_WRAPPER_NODE_H

#ifndef _IRON_UNSAFE_RAW_WRAPPER_ARRAY_H
#include "iron/unsafe/raw_wrapper_array.h"
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:26481)
#pragma warning(disable:26482)
#pragma warning(disable:26446)
#endif

namespace iron::unsafe {

	template <typename _T,std::size_t _DATAS_SIZE,std::size_t _CHILDS_SIZE,bool _HAS_PARENT_NODE,typename _A>
	class raw_wrapper_node  {
	public:
		using data_t = _T;
		using size_t = std::size_t;
		using allocator_t=_A;
		using error_handler_t=iron::error_handler;
		static constexpr size_t datas_max_size()noexcept { return _DATAS_SIZE;}
		static constexpr size_t childs_max_size()noexcept { return _CHILDS_SIZE;}
		static constexpr bool has_parent_node()noexcept { return _HAS_PARENT_NODE;}
		using datas_t = raw_wrapper_array<data_t,datas_max_size()>;
		using childs_t = raw_wrapper_array<raw_wrapper_node*,childs_max_size()>;
		static constexpr auto rebind(allocator_t& _a)noexcept {
			return typename std::allocator_traits<allocator_t>:: template rebind_alloc<raw_wrapper_node>(_a);
		}
		constexpr raw_wrapper_node()noexcept {}
		raw_wrapper_node(const raw_wrapper_node&) = delete;
		constexpr raw_wrapper_node(raw_wrapper_node&& _v)noexcept // too optimized?
			:_m_datas(std::move(_v._m_datas))
			, _m_childs(std::move(_v._m_childs))
		{
			if constexpr (has_parent_node()) {
				for (auto _p : _m_childs) {
					DEBUG_ASSERT(_p);
					_set_parent_node(*_p);
				}
			}
		}
		raw_wrapper_node& operator=(const raw_wrapper_node&) = delete;
		raw_wrapper_node& operator=(raw_wrapper_node&&) = delete;
		constexpr ~raw_wrapper_node() {
			DEBUG_ASSERT(_m_childs.empty()); //if failed use destroy before
			DEBUG_ASSERT(_m_datas.empty()); //if failed use destroy before
		}
		constexpr datas_t& datas()noexcept { return _m_datas;}
		constexpr const datas_t& datas()const noexcept { return _m_datas;}
		constexpr const childs_t& childs()const noexcept { return _m_childs;}
		constexpr raw_wrapper_node& child(size_t _idx)noexcept {
			auto _p=_m_childs.get(_idx);
			DEBUG_ASSERT(_p);
			return *_p;
		}
		constexpr const raw_wrapper_node& child(size_t _idx)const noexcept {
			auto _p = _m_childs.get(_idx);
			DEBUG_ASSERT(_p);
			return *_p;
		}
		constexpr static error_handler_t make_instance(raw_wrapper_node*& _p,allocator_t& _a)noexcept {
			DEBUG_ASSERT(!_p);
			auto _ra = rebind(_a);
			using _rw_t = raw_storage<raw_wrapper_node,decltype(_ra)>;
			auto _h=_rw_t::allocate(_p, 1, _ra);
			if (_h) {
				DEBUG_ASSERT(_p);
				_h = _rw_t::construct_at(_p);
				DEBUG_ASSERT(_h);
			}
			return _h;
		}
		constexpr void destroy(allocator_t& _a)noexcept {
			auto _ra = rebind(_a);
			for (auto& _ch : _m_childs) {
				auto& _pnode = _ch.unwrap();
				DEBUG_ASSERT(_pnode);
				_pnode->destroy(_a);
				_ra.deallocate(_pnode, 1);
#ifndef NDEBUG
				_pnode = {};
#endif
			}
			_m_childs.reset();
			_m_datas.reset();
		}
		constexpr raw_wrapper_node*const parent()noexcept {
			if constexpr (has_parent_node()) {
				return _m_parent._parent;
			}
			else {
				return {};
			}
		}
		constexpr const raw_wrapper_node*const parent()const noexcept {
			if constexpr (has_parent_node()) {
				return _m_parent._parent;
			}
			else {
				return {};
			}
		}
		constexpr raw_wrapper_node& insert_child(size_t _idx,raw_wrapper_node* _p)noexcept {
			DEBUG_ASSERT(_p);
			DEBUG_ASSERT(!_p->parent());
			auto& _r=_m_childs.insert(_idx,std::move(_p));
			DEBUG_ASSERT(_r);
			_set_parent_node(*_r);
			return *_r;
		}
		constexpr raw_wrapper_node& push_front_child(raw_wrapper_node* _p)noexcept {
			return insert_child(0,_p);
		}
		constexpr raw_wrapper_node& push_back_child(raw_wrapper_node* _p)noexcept {
			return insert_child(_m_childs.size(),_p);
		}
		constexpr raw_wrapper_node* detach_child(size_t _idx)noexcept {
			auto _p=_m_childs.erase(_idx);
			DEBUG_ASSERT(_p);
			_reset_parent_node(*_p);
			return _p;
		}
		constexpr raw_wrapper_node* pop_front_child()noexcept {
			return detach_child(0);
		}
		constexpr raw_wrapper_node* pop_back_child()noexcept {
			DEBUG_ASSERT(!_m_childs.empty());
			return detach_child(_m_childs.size() - 1);
		}
		constexpr size_t total_data_count()const noexcept {
			size_t _c{};
			_data_count(_c, *this);
			return _c;
		}
	private:
		typedef struct _parent_node {
			raw_wrapper_node* _parent{};
		} _parent_node_t;
		typedef struct _empty {} _empty_t;
		using _parent_t=std::conditional_t<has_parent_node(),_parent_node_t,_empty_t>;
		constexpr void _set_parent_node(raw_wrapper_node& _p)noexcept {
			DEBUG_ASSERT(&_p != this);
			if constexpr (has_parent_node()) {
				_p._m_parent._parent=this;
			}
		}
		constexpr void _reset_parent_node(raw_wrapper_node& _p)noexcept {
			if constexpr (has_parent_node()) {
				DEBUG_ASSERT_EQ(_p._m_parent._parent,this);
#ifndef NDEBUG
				_p._m_parent._parent={};
#endif
			}
		}
		constexpr static void _data_count(size_t& _c, const raw_wrapper_node& _node)noexcept {
			_c += _node.datas().size();
			for (auto& _ch : _node.childs()) {
				auto& _pnode = _ch.unwrap();
				DEBUG_ASSERT(_pnode);
				_data_count(_c, *_pnode);
			}
		}
		_parent_t _m_parent{};
		datas_t _m_datas{};
		childs_t _m_childs{};
	};

} //ns

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#endif
