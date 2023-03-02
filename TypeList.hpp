#pragma once

#include <optional>

namespace Martian {

	template<typename... Types>
	struct TypeList {
		TypeList () = delete;
		~TypeList () = delete;
		TypeList<Types...> & operator = (const TypeList<Types...> &) = delete;

	private:

		// NODE DEFINITIONS

		struct FieldListEnd {};

		template<typename Type = FieldListEnd, typename... Other>
		struct field_list_node {
			using type = Type;
			using next_node = field_list_node<Other...>;
		};

		// ROOT NODE

		using root_node = field_list_node<Types..., FieldListEnd>;

		// LIST SIZE

		template<typename Node, typename RecurType>
		class GetSize {
			using NextNode = typename Node::next_node;
			using NextType = typename NextNode::type;
			using Getter = GetSize<NextNode, NextType>;
		public:
			static constexpr std::size_t value { Getter::value + 1 };
		};

		template<typename Node>
		struct GetSize<Node, FieldListEnd> {
			static constexpr std::size_t value { 0 };
		};

		// LIST TYPE INDEX

		template<typename Node, typename RecurType,
				typename SearchType, std::size_t index>
		class GetIndex {
			using NextNode = typename Node::next_node;
			using NextType = typename NextNode::type;
			using Getter = GetIndex<NextNode, NextType, SearchType, index + 1>;
		public:
			static constexpr std::optional<std::size_t> value {
					std::is_same_v<RecurType, SearchType> ? index : Getter::value };
		};

		template<typename Node, typename SearchType, std::size_t index>
		struct GetIndex<Node, FieldListEnd, SearchType, index> {
			static constexpr std::optional<std::size_t> value { std::nullopt };
		};

		// GET TYPE AT INDEX

		template<typename Node, std::size_t index>
		struct IndexType {
			using type = IndexType<typename Node::next_node, index - 1>::type;
		};

		template<typename Node>
		struct IndexType<Node, 0> {
			using type = typename Node::type;
		};

	public:

		// LIST METHODS

		[[nodiscard, maybe_unused]] static consteval auto size ()
		noexcept -> std::size_t {
			using root_type = typename root_node::type;
			return GetSize<root_node, root_type>::value;
		}

		template<typename Type>
		[[nodiscard, maybe_unused]] static consteval auto index_of_type ()
		noexcept -> std::optional<std::size_t> {
			using root_type = typename root_node::type;
			return GetIndex<root_node, root_type, Type, 0>::value;
		}

		// LIST TYPES

		template<std::size_t index>
		using type_at_index = IndexType<root_node, index>::type;

	};

}
