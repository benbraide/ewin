#pragma once

#ifndef EWIN_WINDOW_TREE_H
#define EWIN_WINDOW_TREE_H

#include <list>
#include <memory>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/list_property.h"
#include "../common/object_property.h"

#include "window_iterators.h"

namespace ewin::window{
	class external;

	class wnd_tree{
	public:
		typedef std::shared_ptr<object> object_ptr_type;
		typedef std::list<object_ptr_type> object_list_type;

		typedef object_list_type::iterator object_list_iterator_type;

		enum class list_target_type{
			children,
			siblings,
			ancestors,
		};

		explicit wnd_tree(object &target);

		wnd_tree(const wnd_tree &) = delete;
		
		wnd_tree &operator =(const wnd_tree &) = delete;

		common::read_only_object_value_property<object, wnd_tree> owner;
		common::object_value_property<object, wnd_tree> parent;

		common::numeric_value_property<std::size_t, wnd_tree> index;
		common::numeric_value_property<std::size_t, wnd_tree> absolute_index;
		common::read_only_numeric_value_property<std::size_t, wnd_tree> group_index;

		common::object_value_property<object, wnd_tree> previous_sibling;
		common::object_value_property<object, wnd_tree> next_sibling;

		common::list_value_property<object, object_list_iterator_type, object_list_iterator_type, wnd_tree> children;
		common::read_only_list_value_property<object, sibling_forward_iterator, sibling_forward_iterator, wnd_tree> siblings;
		common::read_only_list_value_property<object, ancestor_forward_iterator, ancestor_forward_iterator, wnd_tree> ancestors;

	private:
		friend class object;
		friend class external;

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		void set_parent_(object *value, std::size_t index);

		std::size_t index_(bool absolute) const;

		object *sibling_(bool previous) const;

		object *ancestor_(std::size_t index) const;

		object *at_(std::size_t index, list_target_type list_target) const;

		std::size_t find_(object &target, list_target_type list_target) const;

		std::size_t ancestor_count_() const;

		std::size_t add_child_(object *value);

		void remove_child_(object *value);

		void remove_child_(std::size_t index);

		object *target_;
		object_ptr_type parent_;
		object_list_type children_;
	};
}

#endif /* !EWIN_WINDOW_TREE_H */
