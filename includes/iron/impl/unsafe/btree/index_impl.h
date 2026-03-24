namespace iron::unsafe::btree {

	template <typename _KEY, typename _MAPPED,typename _LT,std::size_t _ORDER,bool _MULTI,bool _HAS_MAPPED>
	struct index final {
		using size_t = std::size_t;
		using key_t = _KEY;
		using mapped_t = _MAPPED;
		using key_less_t = _LT;
		static constexpr size_t order=_ORDER;
		static constexpr size_t min_data_size = order - 1;
		static constexpr size_t max_data_size = order * 2 - 1;
		static constexpr size_t max_child_size =  max_data_size + 1;
		static constexpr bool is_multi = _MULTI;
		static constexpr bool has_mapped = _HAS_MAPPED;
		using value_t = std::conditional_t<has_mapped, std::pair<key_t, mapped_t>,key_t>;
		using exported_value_t = std::conditional_t<has_mapped, std::pair<key_t, mapped_t>, key_t>;
		using tree_node_t = iron::unsafe::btree::node<value_t, max_data_size, max_child_size>;
		using data_iter_t = typename tree_node_t::data_iter_t;
		using data_node_t= typename tree_node_t::data_node_t;
		tree_node_t* tree_node{};
		data_iter_t  data_iter{};
		data_node_t*  data_node{};
		[[nodiscard]] constexpr bool operator==(const index& _v)const noexcept {
			return tree_node == _v.tree_node && data_iter == _v.data_iter && data_node == _v.data_node;
		}
	};
}

