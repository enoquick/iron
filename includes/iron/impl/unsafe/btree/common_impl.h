#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning (disable:26429)
#pragma warning (disable:26492)
#pragma warning (disable:26494)
#endif

namespace iron::unsafe::btree {

	template <typename _INDEX>
	class common final {
	public:
		using size_t = std::size_t;
		using index_t = _INDEX;
		static constexpr size_t min_data_size = index_t::min_data_size;
		static constexpr size_t max_data_size = index_t::max_data_size;
		static constexpr size_t max_child_size = index_t::max_child_size;
		static constexpr bool is_multi = index_t::is_multi;
		static constexpr bool has_mapped = index_t::has_mapped;
		using tree_node_t = typename index_t::tree_node_t;
		using data_iter_t = typename index_t::data_iter_t;
		using data_node_t = typename index_t::data_node_t;
		using key_less_t = typename index_t::key_less_t;
		using child_iter_t = typename tree_node_t::child_iter_t;
		using key_t = typename index_t::key_t;
		using mapped_t = typename index_t::mapped_t;
		using value_t = typename index_t::value_t;
		using exported_value_t = typename index_t::exported_value_t;
		using error_handler_t = iron::error_handler;
		typedef enum class enum_compare {
			lt
			, eq
			, gt
		} compare_t;
		template <typename _ITER>
		static constexpr void check_sequence(_ITER _b, _ITER _e)noexcept {
			if (_b != _e) {
				auto _pred = _b;
				++_b;
				while (_b != _e) {
//					auto& _vp = *_pred;
//					auto& _v = *_b;
					auto& _kp = get_key(*_pred);
					auto& _k = get_key(*_b);
					const auto _compare = compare(_kp, _k);
					if constexpr (is_multi) {
						ASSERT(_compare == compare_t::lt || _compare == compare_t::eq);
					}
					else {
						ASSERT(_compare == compare_t::lt);
					}
					_pred = _b++;
				}
			}
		}
		[[nodiscard]] constexpr static tree_node_t* first_tree_node(tree_node_t* _p)noexcept {
			DEBUG_ASSERT(_p && !_p->parent()); //_p must be root node
			return _leftmostleaf(_p);
		}
		[[nodiscard]] constexpr static tree_node_t* last_tree_node(tree_node_t* _p)noexcept {
			DEBUG_ASSERT(_p && !_p->parent()); //_p must be root node
			return _rightmostleaf(_p);
		}
		static constexpr void next(index_t& _index)noexcept { _next(_index); }
		static constexpr void prev(index_t& _index)noexcept { _prev(_index); }
		[[nodiscard]] static constexpr size_t count_data(data_node_t* _data_node)noexcept {
			size_t _c{};
			while (_data_node) {
				++_c;
				_data_node = _data_node->next();
			}
			return _c;
		}
		template <typename _A>
		[[nodiscard]] constexpr static error_handler_t split_child(
			tree_node_t& _tree_node
			, child_iter_t _child_iter
			, _A& _a
		)noexcept {
			DEBUG_ASSERT(_child_iter);
			tree_node_t* _pright{};
			auto _h = tree_node_t::from(_pright, _a); RE(_h);
			_tree_node.child_insert(_child_iter + 1, _pright);
			auto& _pleft = *_child_iter;
			if (_pleft->child_size()) {
				DEBUG_ASSERT(_pright->child_empty());
				_pright->child_append_from(*_pleft, min_data_size + 1);
			}
			_pright->data_append_from(*_pleft, min_data_size + 1);
			auto _tdata = _pleft->data_pop_back();
			auto _data_iter = _tree_node.data_begin() + (_child_iter - _tree_node.child_begin());
			_tree_node.data_insert(_data_iter, _tdata);
			return _h;
		}

		template <typename _A>
		[[nodiscard]] static constexpr data_node_t* detach_data(const index_t& _start_index, _A& _a)noexcept {
			return _detach_data(_start_index, _a);
		}
		template <typename _A>
		[[nodiscard]] static constexpr data_node_t* detach_data_single(const index_t& _start_index, _A& _a)noexcept 
		{
			if constexpr (is_multi) {
				return _detach_data_single(_start_index, _a);
			}
			else {
				return _detach_data(_start_index, _a);
			}
		}

		template <typename _K>
		[[nodiscard]] static constexpr bool search(data_iter_t& _b, const data_iter_t& _e, const _K& _key)noexcept {
			while (_b != _e) {
				auto& _k = get_key((*_b)->value());
				if (key_less_t{}(_k, _key)) {
					++_b;
					continue;
				}
				if (key_less_t{}(_key, _k)) { //not found
					break;
				}
				return true; //key found
			}
			return {}; //key not found
		}
		template <typename _K>
		[[nodiscard]] static constexpr index_t recursive_search(tree_node_t& _tree_node, const _K& _key)noexcept {
			return _recursive_search(&_tree_node, _key);
		}
		template <typename ITER>
		[[nodiscard]] static constexpr index_t& index(ITER& _iter) { return _iter._m_index; }

		template <typename _K1, typename _K2>
		[[nodiscard]] static constexpr compare_t compare(const _K1& _k1, const _K2& _k2)noexcept {
			if (key_less_t{}(_k1, _k2)) {
				return compare_t::lt;
			}
			if (key_less_t{}(_k2, _k1)) {
				return compare_t::gt;
			}
			return compare_t::eq;
		}
		template <typename _ITER1,typename _ITER2>
		[[nodiscard]] static constexpr bool eq(_ITER1 _b1, _ITER1 _e1, _ITER2 _b2)noexcept {
			while (_b1 != _e1) {
//				auto& _v1 = *_b1++;
//				auto& _v2 = *_b2++;
				auto& _k1 = get_key(*_b1);
				auto& _k2 = get_key(*_b2);
				if constexpr (
					std::is_same_v<key_less_t, iron::comparables::less_to<key_t>>
					&& iron::concepts::nothrow_equality_comparable<key_t>	
				) 
				{
					if (!(_k1 == _k2)) {
						return {};
					}
				}
				else {
					if (compare(_k1, _k2) != compare_t::eq) {
						return {};
					}
				}
				if constexpr (has_mapped) {
					if (!((*_b1).second == (*_b2).second)) {
						return {};
					}
				}
				++_b1;
				++_b2;
			}
			return true;
		}
		template <typename _ITER1, typename _ITER2>
		[[nodiscard]] static constexpr compare_t compare_seq(_ITER1 _b1, _ITER1 _e1, _ITER2 _b2,_ITER2 _e2)noexcept {
			while (_b1 != _e1 && _b2 != _e2) {
				auto& _k1 = get_key(*_b1);
				auto& _k2 = get_key(*_b2);
				switch (const auto _t=compare(_k1, _k2)) {
					case compare_t::lt:
					case compare_t::gt: return _t;
					case compare_t::eq: { break; }
				}
				if constexpr (has_mapped) {
					const auto _t = (*_b1).second <=> (*_b2).second;
					if (_t < 0) return compare_t::lt;
					if (_t > 0) return compare_t::gt;
				}
				++_b1;
				++_b2;
			}
			const auto _r1 = _b1 == _e1;
			const auto _r2 = _b2 == _e2;
			return _r1 == _r2 ? compare_t::eq : _r1 ? compare_t::lt : compare_t::gt;
		}

		template <typename _T>
		[[nodiscard]] static constexpr auto& get_key(const _T& _v)noexcept {
			if constexpr (has_mapped) {
				return _v.first;
			}
			else {
				return _v;
			}
		}

#if 0
		[[nodiscard]] static constexpr auto& get_key(const value_t& _v)noexcept {
			if constexpr (has_mapped) {
				return _v.first;
			}
			else {
				return  _v;
			}
		}
#endif 
		static constexpr void check_node(const tree_node_t& _p)noexcept {
			if (_p.parent()) {
				ASSERT(_p.data_size() >= min_data_size);
			}
			else {
				ASSERT(_p.data_size());
			}
			ASSERT(_p.child_empty() || _p.child_size() == _p.data_size() + 1);
		}
		static constexpr void check_recursive_node(const tree_node_t& _p)noexcept {
			check_node(_p);
			auto _b = _p.child_begin();
			const auto _e = _p.child_end();
			while (_b != _e) {
				auto _ch = *_b++;
				ASSERT(_ch);
				check_recursive_node(*_ch);
			}
		}

		template <typename _T>
		[[nodiscard]] static constexpr auto move(_T& _v)noexcept
		{
			if constexpr (has_mapped) {
				return std::make_pair(std::move(_v.first), std::move(_v.second));
			}
			else {
				return std::move(_v);
			}
		}
		template <typename _T>
		[[nodiscard]] static constexpr iron::result<exported_value_t, error_handler_t> clone(const _T& _v)noexcept {
			if constexpr (has_mapped) {
				auto _rf = iron::clone::try_clone_from_to<key_t>(_v.first); RR(_rf);
				auto _rv = iron::clone::try_clone_from<mapped_t>(_v.second); RR(_rv);
				return iron::ok(std::make_pair(std::move(_rf.unchecked_unwrap()), std::move(_rv.unchecked_unwrap())));
			}
			else {
				return iron::clone::try_clone_from_to<key_t>(_v);
			}
		}
	private:
		template <typename _K>
		[[nodiscard]] static constexpr index_t _recursive_search(tree_node_t* _tree_node, const _K& _key)noexcept {
			while (true) {
				DEBUG_ASSERT(_tree_node);
				auto _data_iter = _tree_node->data_begin();
				if (search(_data_iter, _tree_node->data_end(), _key)) {
					DEBUG_ASSERT(compare(get_key((*_data_iter)->value()), _key) == compare_t::eq);
					return index_t{ _tree_node,_data_iter,*_data_iter };
				}
				if (_tree_node->child_empty()) { //not found
					return {};
				}
				const auto _idx = _tree_node->data_index(_data_iter);
				_tree_node = _tree_node->child_nth(_idx);
			}
		}
		static constexpr void _next(index_t& _index)noexcept
		{
			auto& _tree_node = _index.tree_node;
			auto& _data_iter = _index.data_iter;
			auto& _data_node = _index.data_node;
			DEBUG_ASSERT(_tree_node && _data_iter && _data_node);
			if (_go_list_next_node(_data_node)) {
				return;
			}
			size_t _idx;
			if (_tree_node->child_empty()) {
				_idx = _tree_node->data_index(++_data_iter);
				while (_tree_node->parent() && _idx == _tree_node->data_size()) {
					_idx = _tree_node->parent_child_index();
					_tree_node = _tree_node->parent();
				}
				DEBUG_ASSERT(_tree_node && _idx <= _tree_node->data_size());
				_data_iter = _tree_node->data_begin() + _idx;
				if (_data_iter == _tree_node->data_end()) { //eoi
					DEBUG_ASSERT(!_tree_node->parent());
					_data_node = {};
					return;
				}
				_data_node = *_data_iter;
				return;
			}
			_idx = _tree_node->data_index(_data_iter) + 1;
			_tree_node = _tree_node->child_nth(_idx);
			_tree_node = _leftmostleaf(_tree_node);
			_data_iter = _tree_node->data_begin();
			_data_node = *_data_iter;
		}
		static constexpr void _prev(index_t& _index)noexcept
		{
			auto& _tree_node = _index.tree_node;
			auto& _data_iter = _index.data_iter;
			DEBUG_ASSERT(_data_iter);
			auto& _data_node = _index.data_node;
			DEBUG_ASSERT(_tree_node);
			if (!_data_node) { //point to last
				DEBUG_ASSERT(_data_iter == _tree_node->data_end());
				_tree_node = last_tree_node(_tree_node);
				DEBUG_ASSERT(_tree_node && _tree_node->data_size());
				_data_iter = _tree_node->data_begin() + (_tree_node->data_size() - 1);
				_data_node = *_data_iter;
				_go_list_last_node(_data_node);
				return;
			}
			if constexpr (is_multi) {
				auto _p = *_data_iter;
				DEBUG_ASSERT(_p);
				if (_data_node != _p) { //position to prev list_node
					while (_p->next() != _data_node) {
						_p = _p->next();
						DEBUG_ASSERT(_p);
					}
					_data_node = _p;
					return;
				}
			}
			if (_tree_node->child_empty()) {
				if (_data_iter != _tree_node->data_begin()) {
					--_data_iter;
					_data_node = *_data_iter;
					_go_list_last_node(_data_node);
					return;
				}
				while (_tree_node->parent() && !_tree_node->parent_child_index()) {
					_tree_node = _tree_node->parent();
				}
				DEBUG_ASSERT(_tree_node && _tree_node->parent());
				auto _idx = _tree_node->parent_child_index();
				DEBUG_ASSERT(_idx);
				--_idx;
				_tree_node = _tree_node->parent();
				DEBUG_ASSERT(_idx < _tree_node->data_size());
				_data_iter = _tree_node->data_begin() + _idx;
				_data_node = *_data_iter;
				_go_list_last_node(_data_node);
				return;
			} //if child_empty()
			const auto _idx = _tree_node->data_index(_data_iter);
			_tree_node = _tree_node->child_nth(_idx);
			DEBUG_ASSERT(_tree_node->data_size());
			_tree_node = _rightmostleaf(_tree_node);
			DEBUG_ASSERT(_tree_node->data_size());
			_data_iter = _tree_node->data_begin() + (_tree_node->data_size() - 1);
			_data_node = *_data_iter;
			_go_list_last_node(_data_node);
		}
		[[nodiscard]] constexpr static tree_node_t* _leftmostleaf(tree_node_t* _p)noexcept {
			DEBUG_ASSERT(_p);
			while (_p->child_size()) {
				constexpr size_t _child_idx = 0;
				_p = _p->child_nth(_child_idx);
			}
			DEBUG_ASSERT(_p->data_size());
			return _p;
		}
		template <typename _A>
		static constexpr void _root_childs_merge(tree_node_t& _ptree, _A& _a)noexcept {
			DEBUG_ASSERT(!_ptree.parent() && _ptree.data_empty());
			DEBUG_ASSERT_EQ(_ptree.child_size(), 1);
			auto _pch = _ptree.child_pop_back();
			_ptree.data_append_from(*_pch);
			_ptree.child_append_from(*_pch);
			tree_node_t::destroy(_pch, _a);
			tree_node_t::deallocate(_pch, _a);
		}
		template <typename _A>
		[[nodiscard]] static constexpr tree_node_t* _ensure_child_detach(tree_node_t* _ptree, size_t _child_idx, _A& _a)noexcept {
			while (true) {
				DEBUG_ASSERT(_ptree);
				auto _pch = _ptree->child_nth(_child_idx);
				if (_pch->data_size() > index_t::min_data_size) { // already satisfies the condition
					return _pch;
				}
				// get siblings
				tree_node_t* _pleft = _child_idx ? _ptree->child_nth(_child_idx - 1) : nullptr;
				tree_node_t* _pright = _child_idx < _ptree->data_size() ? _ptree->child_nth(_child_idx + 1) : nullptr;
				DEBUG_ASSERT(_pleft || _pright);  // at least one sibling exists because degree >= 2
				DEBUG_ASSERT(!_pleft || _pleft->child_empty() == _pch->child_empty());  // sibling must be same type (internal/leaf) as child
				DEBUG_ASSERT(!_pright || _pright->child_empty() == _pch->child_empty()); // sibling must be same type (internal/leaf) as child
				if (_pleft && _pleft->data_size() > index_t::min_data_size) {  // steal rightmost item from left sibling
					if (_pch->child_size()) {
						auto _ch = _pleft->child_pop_back();
						_pch->child_push_front(_ch);
					}
					auto _data_iter = _ptree->data_begin() + (_child_idx - 1);
					_pch->data_push_front(*_data_iter);
					*_data_iter = _pleft->data_pop_back();
#ifdef _IRON_CHECK_BTREE
					check_node(*_pleft);
#endif 
					return {}; //retry
				}
				if (_pright && _pright->data_size() > index_t::min_data_size) { //retry
					if (_pch->child_size()) {
						auto _p = _pright->child_pop_front();
						_pch->child_push_back(_p);
					}
					auto _data_iter = _ptree->data_begin() + _child_idx;
					_pch->data_push_back(*_data_iter);
					*_data_iter = _pright->data_detach(_pright->data_begin());
#ifdef _IRON_CHECK_BTREE
					check_node(*_pright);
#endif 

					return {}; //retry
				}
				if (_pleft) {
					DEBUG_ASSERT(_child_idx);
					_merge_child(*_ptree, _child_idx - 1, _a);
					if (_ptree->parent()) {
						if (_ptree->data_size() < min_data_size) {
							_child_idx = _ptree->parent_child_index();
							_ptree = _ptree->parent();
							continue;
						}
					}
					else if (_ptree->data_empty()) IRON_UNLIKELY{ //root node
						_root_childs_merge(*_ptree,_a);
					}
					return {}; //retry
				}
				DEBUG_ASSERT(_pright);
				_merge_child(*_ptree, _child_idx, _a);
				if (_ptree->parent()) {
					if (_ptree->data_size() < min_data_size) {
						_child_idx = _ptree->parent_child_index();
						_ptree = _ptree->parent();
						continue;
					}
				}
				else if (_ptree->data_empty()) IRON_UNLIKELY{ //root node
					_root_childs_merge(*_ptree, _a);
				}
				return {}; //retry
			} //while
		}

		template <typename _A>
		[[nodiscard]] constexpr static data_node_t* _detach_minkey(tree_node_t* _ptree, _A& _a)noexcept {
			DEBUG_ASSERT(_ptree);
			while (true) {
				DEBUG_ASSERT(_ptree->data_size() > index_t::min_data_size);
				if (_ptree->child_empty()) {
					auto _data_node = _ptree->data_detach(_ptree->data_begin());
					DEBUG_ASSERT(!_ptree->empty());
					return _data_node;
				}
				constexpr size_t _child_idx = 0;
				auto _pch = _ensure_child_detach(_ptree, _child_idx, _a);
				if (!_pch) { //structure has changed - the client must retry the detach
					return {};
				}
				_ptree = _pch;
			}
		}
		template <typename _A>
		[[nodiscard]] constexpr static data_node_t* _detach_maxkey(tree_node_t* _ptree, _A& _a)noexcept {
			DEBUG_ASSERT(_ptree);
			while (true) {
				DEBUG_ASSERT(_ptree->data_size() > index_t::min_data_size);
				if (_ptree->child_empty()) {
					auto _data_node = _ptree->data_pop_back();
					DEBUG_ASSERT(!_ptree->empty());
					return _data_node;
				}
				auto _pch = _ensure_child_detach(_ptree, _ptree->child_size() - 1, _a);
				if (!_pch) { //structure has changed - the client must retry the detach
					return {};
				}
				_ptree = _pch;
			}
		}
		[[nodiscard]] constexpr static tree_node_t* _rightmostleaf(tree_node_t* _p)noexcept {
			DEBUG_ASSERT(_p);
			while (_p->child_size()) {
				_p = _p->child_nth(_p->child_size() - 1);
			}
			DEBUG_ASSERT(_p->data_size());
			return _p;
		}
		constexpr static void _go_list_last_node(data_node_t*& _p)noexcept {
			DEBUG_ASSERT(_p);
			if constexpr (is_multi) {
				while (_p->next()) IRON_UNLIKELY{
					_p = _p->next();
				}
			}
		}
		[[nodiscard]] constexpr static bool _go_list_next_node(data_node_t*& _p)noexcept {
			DEBUG_ASSERT(_p);
			if constexpr (is_multi) {
				if (auto& _next = _p->next()) IRON_UNLIKELY{
					_p = _next;
					return true;
				}
			}
			return {};
		}
		template <typename _A>
		static constexpr void _merge_child(tree_node_t& _tree_node, size_t _idx, _A& _a)noexcept {
			auto _list = _tree_node.data_detach(_tree_node.data_begin() + _idx);
			auto _child_iter = _tree_node.child_begin() + _idx;
			DEBUG_ASSERT(_tree_node.child_index(_child_iter) >= 0);
			auto _pleft = *_child_iter++;
			DEBUG_ASSERT(_tree_node.child_index(_child_iter));
			auto _pright = *_child_iter;
			if (_pleft->child_size()) {
				_pleft->child_append_from(*_pright);
			}
			_pleft->data_push_back(_list);
			if (_pright->data_size()) {
				_pleft->data_append_from(*_pright);
			}
			DEBUG_ASSERT(*_child_iter == _pright);
			DEBUG_ASSERT(_pright->data_empty());
			DEBUG_ASSERT(_pright->child_empty());
			auto _ch = _tree_node.child_detach(_child_iter);
			tree_node_t::destroy(_ch, _a);
			tree_node_t::deallocate(_ch, _a);
		}
		template <typename _A>
		[[nodiscard]] static constexpr data_node_t* _detach_data(index_t _start_index, _A& _a)noexcept
		{
			while (true) {
				auto& _tree_node = _start_index.tree_node;
				auto& _data_iter = _start_index.data_iter;
				auto& _data_node = _start_index.data_node;
				DEBUG_ASSERT(_data_node);
#ifdef _IRON_CHECK_BTREE
				check_node(*_tree_node);
#endif 
#if 0 //only for test key 
				{
					auto& _k = get_key(_data_node->value());
					if (compare(_k, 195) == compare_t::eq) {
						TODO();
					}
				}
#endif 
				if (_tree_node->child_empty()) {
					if (_tree_node->parent() && _tree_node->data_size() == min_data_size) {
						const auto _idx = _tree_node->parent_child_index();
						auto _parent = _tree_node->parent();
						auto _root_node = _parent->root_node();
						[[maybe_unused]] const tree_node_t* _x = _ensure_child_detach(_parent, _idx, _a);
						DEBUG_ASSERT(!_x);
						auto& _k = get_key(_data_node->value());
						_start_index = _recursive_search(_root_node, _k);
						DEBUG_ASSERT_EQ(_start_index.data_node, _data_node);
						continue; //retry
					}
					return _tree_node->data_detach(_data_iter);
				}
				const auto _data_idx = _tree_node->data_index(_data_iter);
				auto _child_iter = _tree_node->child_begin() + _data_idx;
				auto& _pleft = *_child_iter;
				if (_pleft->data_size() > index_t::min_data_size) {
					auto _listnode_maxkey = _detach_maxkey(_pleft, _a);
					if (!_listnode_maxkey) {
						continue; //retry
					}
					std::swap(_listnode_maxkey, *_data_iter);
					DEBUG_ASSERT(!_pleft->empty());
					return _listnode_maxkey;
				}
				++_child_iter;
				DEBUG_ASSERT(_tree_node->child_index(_child_iter));
				auto& _pright = *_child_iter;
				if (_pright->data_size() > index_t::min_data_size) {
					auto _listnode_minkey = _detach_minkey(_pright, _a);
					if (!_listnode_minkey) {
						continue; //retry
					}
					std::swap(_listnode_minkey, *_data_iter);
					DEBUG_ASSERT(!_pright->empty());
					return _listnode_minkey;
				}
				auto _root_node = _tree_node->root_node();
				_merge_child(*_tree_node, _data_idx, _a);
				if (_tree_node->parent()) IRON_LIKELY{
					if (_tree_node->data_size() < min_data_size) {
						const auto _idx = _tree_node->parent_child_index();
						auto _parent = _tree_node->parent();
						[[maybe_unused]] const tree_node_t* _x = _ensure_child_detach(_parent, _idx, _a);
						DEBUG_ASSERT(!_x);
					}
				}
				else if (_tree_node->data_empty()) IRON_UNLIKELY{ //root node
					_root_childs_merge(*_tree_node, _a);
				}
				auto& _k = get_key(_data_node->value());
				const auto _btree_index = _recursive_search(_root_node, _k);
				DEBUG_ASSERT_EQ(_data_node, _btree_index.data_node);
				_start_index = _btree_index;
			} //while
			if (std::is_constant_evaluated()) {
				return {}; //for constexpr
			}
			else {
				DEBUG_ASSERT(0);
			}
		}
		template <typename _A>
		[[nodiscard]] static constexpr data_node_t* _detach_data_single(index_t _start_index, _A& _a)noexcept 
		requires(is_multi)
		{
			auto& _data_iter = _start_index.data_iter;
			auto& _data_node = _start_index.data_node;
			auto _list = *_data_iter;
			if (_list == _data_node && !_list->next()) { //the list contain one data
				return _detach_data(_start_index, _a);
			}
			if (_list == _data_node) { //remove head
				DEBUG_ASSERT(_list->next());
				*_data_iter = _list->next();
#ifndef NDEBUG
				_list->reset_links();
#endif 
				return _list;
			}
			while (_list) IRON_LIKELY {
				if (_list->next() == _data_node) {
					_list->next() = _data_node->next();
#ifndef NDEBUG
					_data_node->reset_links();
#endif 
					return _data_node;
				}
				_list = _list->next();
			}
			return {}; //data node not found - is an internal error (index is wrong)
		}
	};
}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif
