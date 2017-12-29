#pragma once

#ifndef EWIN_MENU_COLLECTION_H
#define EWIN_MENU_COLLECTION_H

#include <list>
#include <variant>
#include <functional>

#include "../common/up_down_property.h"

#include "menu_bar.h"
#include "menu_popup.h"
#include "external_menu.h"

namespace ewin::menu{
	template <class base_type>
	class collection : public base_type{
	public:
		typedef base_type base_type;

		typedef std::function<void()> no_arg_action_callback_type;
		typedef std::function<void(events::menu_object &)> arg_action_callback_type;
		typedef std::variant<no_arg_action_callback_type, arg_action_callback_type> action_callback_type;

		typedef std::function<bool(std::wstring &, action_callback_type &, std::size_t &)> item_callback_type;
		typedef std::function<bool(std::wstring &, collection<popup> &, std::size_t &)> link_callback_type;
		typedef std::function<bool(std::size_t &)> separator_callback_type;

		typedef std::shared_ptr<object> object_ptr_type;
		typedef std::list<object_ptr_type> object_list_type;

		template <typename... args_types>
		explicit collection(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			items.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				base_type::created = true;//Create if not already created

				std::wstring label;
				action_callback_type action;
				std::size_t index = -1;

				auto &info = *reinterpret_cast<std::pair<std::size_t, item_callback_type *> *>(arg);
				if (!(*info.second)(label, action, index))
					return;//Canceled

				auto item = std::make_shared<menu::item>();
				item->tree.parent = *this;
				item->label = label;
				item->tree.index = index;
				item->created = true;

				object_list_.push_back(item);
				info.first = item->tree.index;
			});

			links.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				base_type::created = true;//Create if not already created

				std::wstring label;
				auto link = std::make_shared<collection<popup>>();
				std::size_t index = -1;

				auto &info = *reinterpret_cast<std::pair<std::size_t, link_callback_type *> *>(arg);
				if (!(*info.second)(label, *link, index) || !link->created)
					return;//Canceled

				auto item = std::make_shared<menu::item>();
				item->tree.parent = *this;
				item->label = label;
				item->tree.index = index;
				item->sub_menu = *link;
				item->created = true;

				object_list_.push_back(item);
				object_list_.push_back(link);

				info.first = item->tree.index;
			});

			separators.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				base_type::created = true;//Create if not already created

				std::size_t index = -1;
				auto &info = *reinterpret_cast<std::pair<std::size_t, separator_callback_type *> *>(arg);
				if (!(*info.second)(index))
					return;//Canceled

				auto item = std::make_shared<separator>();
				item->tree.parent = *this;
				item->tree.index = index;
				item->created = true;

				object_list_.push_back(item);
				info.first = item->tree.index;
			});
		}

		virtual ~collection(){
			destruct_();
		}

		common::list_value_property<item_callback_type, void *, void *, collection, std::size_t, common::property_access::list_add> items;
		common::list_value_property<link_callback_type, void *, void *, collection, std::size_t, common::property_access::list_add> links;
		common::list_value_property<separator_callback_type, void *, void *, collection, std::size_t, common::property_access::list_add> separators;

	protected:
		object_list_type object_list_;
	};

	using bar_collection = collection<bar>;
	using popup_collection = collection<popup>;

	using external_collection = collection<external>;
	using external_bar_collection = collection<external_bar>;
	using external_popup_collection = collection<external_popup>;
}

#endif /* !EWIN_MENU_COLLECTION_H */
