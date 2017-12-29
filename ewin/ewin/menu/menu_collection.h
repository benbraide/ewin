#pragma once

#ifndef EWIN_MENU_COLLECTION_H
#define EWIN_MENU_COLLECTION_H

#include <list>
#include <variant>
#include <functional>

#include "menu_bar.h"
#include "menu_popup.h"

namespace ewin::menu{
	template <class base_type>
	class collection : public base_type{
	public:
		typedef base_type base_type;

		typedef std::function<void()> no_arg_action_callback_type;
		typedef std::function<void(events::menu_object &)> action_callback_type;

		typedef std::function<bool(std::wstring &, std::variant<no_arg_action_callback_type, action_callback_type> &)> item_callback_type;
		typedef std::function<bool(std::wstring &, collection<popup> &)> link_callback_type;

		typedef std::shared_ptr<object> object_ptr_type;
		typedef std::list<object_ptr_type> object_list_type;

		template <typename... args_types>
		explicit collection(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			items.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				base_type::created = true;//Create if not already created

				std::wstring label;
				std::variant<no_arg_action_callback_type, action_callback_type> action;

				if (!(*static_cast<item_callback_type *>(arg))(label, action))
					return;//Canceled

				std::shared_ptr<item> item;
				item->tree.parent = *this;
				item->label = label;
				item->created = true;

				object_list_.push_back(item);
			});

			links.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				base_type::created = true;//Create if not already created

				std::wstring label;
				std::shared_ptr<collection<popup>> link;

				if (!(*static_cast<link_callback_type *>(arg))(label, *link) || !link->created)
					return;//Canceled

				std::shared_ptr<item> item;
				item->tree.parent = *this;
				item->label = label;
				item->created = true;
				item->sub_menu = *link;

				object_list_.push_back(item);
				object_list_.push_back(link);
			});
		}

		virtual ~collection() = default;

		common::list_value_property<item_callback_type, void *, void *, collection, int, common::property_access::list_add> items;
		common::list_value_property<link_callback_type, void *, void *, collection, int, common::property_access::list_add> links;

	protected:
		object_list_type object_list_;
	};
}

#endif /* !EWIN_MENU_COLLECTION_H */
