
template <typename T>
struct basic_struct_data {
	using value_t = T;
	std::uint32_t cat{}; //bit mask from unicode::attr_t
	value_t lcases[4]{}; //lower cases - term with \0
	value_t ucases[4]{}; //upper cases - term with \0
};

using struct_data = basic_struct_data<std::uint32_t>;

template <typename T>
struct basic_struct_section {
	using value_t = T;
	value_t from{},to{};
	const struct_data* begin{}; //point to range_table[0]
	bool shared{}; //if true the attributes are shared into the same range with index 0
};

using struct_section = basic_struct_section<std::uint32_t>;

