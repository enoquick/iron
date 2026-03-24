#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26472)
#endif

namespace iron::unsafe::btree {

	template <typename _T,std::size_t _DATA_SIZE,std::size_t _CHILD_SIZE>
	class node final {
	public:
		using value_t = _T;
		using size_t = std::size_t;
		using raw_storage_node_t = iron::unsafe::raw_storage<node>;
		static constexpr size_t max_data_size = _DATA_SIZE;
		static constexpr size_t max_child_size = _CHILD_SIZE;
		using data_node_t = iron::unsafe::list_node<value_t, false>;
		using data_array_t = iron::unsafe::raw_array_list<data_node_t, max_data_size>;
		using child_array_t = iron::unsafe::raw_array_list<node, max_child_size>;
		using data_iter_t = typename data_array_t::iter_t;
		using const_data_iter_t = typename data_array_t::const_iter_t;
		using child_iter_t = typename child_array_t::iter_t;
		using const_child_iter_t = typename child_array_t::const_iter_t;
		using error_handler_t=iron::error_handler;
		struct btree_node_tag { btree_node_tag() = delete; };
		constexpr node()noexcept
		: _m_parent()
		,_m_data_array()
		,_m_child_array()
		{
		}
		node(const node&) = delete;
		node& operator=(const node&) = delete;
		node(node&&) = delete;
		node& operator=(node&&) = delete;
		constexpr ~node(){
			DEBUG_ASSERT(empty()); //use destroy/deallocate before
		}
		template <typename _A>
		static constexpr void destroy(node* _p,_A& _a)noexcept
		requires (iron::concepts::allocator<_A>)
		{
			DEBUG_ASSERT(_p && !_p->child_size());
			data_array_t::destroy(_p->_m_data_array,_a);
//			child_array_t::destroy(_p->_m_child_array, _a);
			raw_storage_node_t::destroy_at(_p);
		}
		template <typename _A>
		static constexpr void deallocate(node* _p, _A& _a)noexcept
		requires (iron::concepts::allocator<_A>)
		{
			DEBUG_ASSERT(_p);
			raw_storage_node_t::deallocate(_p, 1, _a);
		}
		template <typename _A>
		[[nodiscard]] static constexpr error_handler_t from(node*& _p, _A _a)noexcept
			requires (iron::concepts::allocator<_A>)
		{
			DEBUG_ASSERT(!_p);
			auto _h = raw_storage_node_t::allocate(_p, 1, _a); RE(_h);
			std::construct_at(_p);
			return {};
		}

		template <typename _A,typename _K,typename _M>
		[[nodiscard]] static constexpr error_handler_t make_data(data_node_t*& _p,_A& _a, _K&& _k, _M&& _m)noexcept
		requires(
			iron::concepts::allocator<_A>
		)
		{
			if constexpr (iron::concepts::std_pair<value_t>) {
				return data_node_t::from_pair(_p, _a, std::forward<_K>(_k), std::forward<_M>(_m));
			}
			else {
				return data_node_t::from_value(_p, _a, std::forward<_K>(_k));
			}
		}
		[[nodiscard]] constexpr node* parent()noexcept { return _m_parent; }
		[[nodiscard]] constexpr const node* parent()const noexcept { return _m_parent; }
		[[nodiscard]] constexpr node* root_node()noexcept {
			auto _p = this;
			while (auto _x = _p->parent()) {
				_p = _x;
			}
			return _p;
		}
		[[nodiscard]] constexpr const node* root_node()const noexcept {
			auto _p = this;
			while (auto _x = _p->parent()) {
				_p = _x;
			}
			return _p;
		}
		[[nodiscard]] constexpr bool empty()const noexcept { return data_empty() && child_empty(); }
		//data
		[[nodiscard]] constexpr size_t data_size()const noexcept { return _m_data_array.size(); }
		[[nodiscard]] constexpr bool data_empty()const noexcept { return _m_data_array.empty(); }
		[[nodiscard]] constexpr bool data_full()const noexcept { return _m_data_array.full(); }
		[[nodiscard]] constexpr data_iter_t data_begin()noexcept { return  _m_data_array.begin(); }
		[[nodiscard]] constexpr data_iter_t data_end()noexcept { return _m_data_array.end(); }
		[[nodiscard]] constexpr const_data_iter_t data_begin()const noexcept { return _m_data_array.begin(); }
		[[nodiscard]] constexpr const_data_iter_t data_end()const noexcept { return _m_data_array.end(); }
		[[nodiscard]] constexpr const_data_iter_t cdata_begin()const noexcept { return data_begin(); }
		[[nodiscard]] constexpr const_data_iter_t cdata_end()const noexcept { return data_end(); }
		[[nodiscard]] constexpr size_t data_index(data_iter_t _iter)const noexcept { return _m_data_array.index(_iter);}
		[[nodiscard]] constexpr data_node_t* data_nth(size_t _idx)noexcept { return _m_data_array.nth(_idx); }
		[[nodiscard]] constexpr data_iter_t data_find(data_node_t* _node)noexcept {
			return _m_data_array.find(_node);
		}
		constexpr data_iter_t data_push_front(data_node_t* _node)noexcept {
			return _m_data_array.push_front(_node);
		}
		constexpr data_iter_t data_push_back(data_node_t* _node)noexcept {
			return _m_data_array.push_back(_node);
		}
		constexpr data_iter_t data_insert(data_iter_t _iter,data_node_t* _node)noexcept {
			DEBUG_ASSERT(_iter);
			if (_iter == data_end()) {
				return data_push_back(_node);
			}
			return _m_data_array.insert(_iter,_node);
		}
		[[nodiscard]] constexpr data_node_t* data_pop_back()noexcept {
			return _m_data_array.pop_back();
		}
		[[nodiscard]] constexpr data_node_t* data_detach(data_iter_t _iter)noexcept {
			return _m_data_array.detach(_iter);
		}
		constexpr void data_append_from(node& _node, size_t _start_idx, size_t _len)noexcept {
			_m_data_array.append_from(_node._m_data_array, _start_idx, _len);
		}
		constexpr void data_append_from(node& _node, size_t _start_idx)noexcept {
			DEBUG_ASSERT(_start_idx < _node.data_size());
			const auto _len = _node.data_size() - _start_idx;
			data_append_from(_node, _start_idx, _len);
		}
		constexpr void data_append_from(node& _node)noexcept {
			data_append_from(_node, 0);
		}
		//child
		[[nodiscard]] constexpr size_t child_size()const noexcept { return _m_child_array.size(); }
		[[nodiscard]] constexpr bool child_empty()const noexcept { return _m_child_array.empty(); }
		[[nodiscard]] constexpr bool child_full()const noexcept { return _m_child_array.full(); }
		[[nodiscard]] constexpr child_iter_t child_begin()noexcept { return _m_child_array.begin(); }
		[[nodiscard]] constexpr child_iter_t child_end()noexcept { return _m_child_array.end(); }
		[[nodiscard]] constexpr const_child_iter_t child_begin()const noexcept { return _m_child_array.begin(); }
		[[nodiscard]] constexpr const_child_iter_t child_end()const noexcept { return _m_child_array.end(); }
		[[nodiscard]] constexpr const_child_iter_t cchild_begin()const noexcept { return child_begin(); }
		[[nodiscard]] constexpr const_child_iter_t cchild_end()const noexcept { return child_end(); }
		[[nodiscard]] constexpr size_t child_index(child_iter_t _iter)const noexcept {return _m_child_array.index(_iter);}
		[[nodiscard]] constexpr size_t parent_child_index()const noexcept {
			auto _np = this->parent();
			DEBUG_ASSERT(_np);
			auto _b = _np->child_begin();
			const auto _e = _np->child_end();
			while (_b != _e) IRON_LIKELY {
				if (*_b == this) {
					break;
				}
				++_b;
			}
			DEBUG_ASSERT(_b != _e);
			return static_cast<size_t>(_b - _np->child_begin());
		}
		[[nodiscard]] constexpr node* child_nth(size_t _idx)noexcept {
			return _m_child_array.nth(_idx);
		}
		[[nodiscard]] constexpr child_iter_t child_find(node* _node)noexcept {
			return _m_child_array.find(_node);
		}
		constexpr child_iter_t child_push_front(node* _p)noexcept {
			DEBUG_ASSERT(_p && !_p->_m_parent);
			_p->_m_parent = this;
			return _m_child_array.push_front(_p);
		}
		constexpr child_iter_t child_push_back(node* _p)noexcept {
			DEBUG_ASSERT(_p && !_p->_m_parent);
			_p->_m_parent = this;
			return _m_child_array.push_back(_p);
		}
		constexpr child_iter_t child_insert(child_iter_t _iter,node* _p)noexcept {
			if (_iter == child_end()) IRON_UNLIKELY {
				return child_push_back(_p);
			}
			DEBUG_ASSERT(_p && !_p->_m_parent);
			_p->_m_parent = this;
			return _m_child_array.insert(_iter,_p);
		}
		[[nodiscard]] constexpr node* child_pop_back()noexcept {
			auto _p= _m_child_array.pop_back();
			DEBUG_ASSERT(_p && _p->_m_parent == this);
			_p->_m_parent = {};
			return _p;
		}
		[[nodiscard]] constexpr node* child_pop_front()noexcept {
			DEBUG_ASSERT(child_size());
			return child_detach(child_begin());
		}

		[[nodiscard]] constexpr node* child_detach(child_iter_t _iter)noexcept {
			auto _p=_m_child_array.detach(_iter);
			DEBUG_ASSERT(_p && _p->_m_parent == this);
			_p->_m_parent = {};
			return _p;
		}
		constexpr void child_append_from(node& _node, size_t _start_idx,size_t _len)noexcept {
			auto _e = child_end();
			_m_child_array.append_from(_node._m_child_array, _start_idx, _len);
			while (_len--) {
				auto _ch_node = *_e++;
				DEBUG_ASSERT(_ch_node && _ch_node->_m_parent == &_node);
				_ch_node->_m_parent = this;
			}
		}
		constexpr void child_append_from(node& _node, size_t _start_idx)noexcept {
			DEBUG_ASSERT(_start_idx < _node.child_size());
			const auto _len = _node.child_size() - _start_idx;
			child_append_from(_node, _start_idx, _len);
		}
		constexpr void child_append_from(node& _node)noexcept {
			child_append_from(_node, 0);
		}
	private:
		node* _m_parent;
		data_array_t _m_data_array;
		child_array_t _m_child_array;
	};


}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
