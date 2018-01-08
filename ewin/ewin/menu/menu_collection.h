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

		typedef std::function<bool(item &)> item_callback_type;
		typedef std::function<bool(item &, collection<popup> &)> link_callback_type;
		typedef std::function<bool(separator &)> separator_callback_type;

		typedef std::shared_ptr<object> object_ptr_type;
		typedef std::list<object_ptr_type> object_list_type;

		template <typename... args_types>
		explicit collection(args_types &&... args)
			: base_type(std::forward<args_types>(args)...){
			items.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				add_item_<item>(*reinterpret_cast<std::pair<std::size_t, item_callback_type *> *>(arg));
			});

			links.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				base_type::created = true;//Create if not already created

				auto link = std::make_shared<collection<popup>>();
				auto item = std::make_shared<menu::item>();

				auto &info = *reinterpret_cast<std::pair<std::size_t, link_callback_type *> *>(arg);
				if (!(*info.second)(*item, *link) || !link->created)
					return;//Canceled

				item->tree.parent = *this;
				item->sub_menu = *link;

				link->created = true;
				item->created = true;

				object_list_.push_back(link);
				object_list_.push_back(item);

				info.first = item->tree.index;
			});

			checks.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				add_item_<check_item>(*reinterpret_cast<std::pair<std::size_t, item_callback_type *> *>(arg));
			});

			radios.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				add_item_<radio_item>(*reinterpret_cast<std::pair<std::size_t, item_callback_type *> *>(arg));
			});

			separators.initialize_(nullptr, [this](void *prop, void *arg, common::property_access access){
				base_type::created = true;//Create if not already created

				auto &info = *reinterpret_cast<std::pair<std::size_t, separator_callback_type *> *>(arg);
				auto item = std::make_shared<separator>();

				if (!(*info.second)(*item))
					return;//Canceled

				item->tree.parent = *this;
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

		common::list_value_property<item_callback_type, void *, void *, collection, std::size_t, common::property_access::list_add> checks;
		common::list_value_property<item_callback_type, void *, void *, collection, std::size_t, common::property_access::list_add> radios;

		common::list_value_property<separator_callback_type, void *, void *, collection, std::size_t, common::property_access::list_add> separators;

	protected:
		template <typename item_type>
		void add_item_(std::pair<std::size_t, item_callback_type *> &info){
			base_type::created = true;//Create if not already created

			auto item = std::make_shared<item_type>();
			if (!(*info.second)(*item))
				return;//Canceled

			item->tree.parent = *this;
			item->created = true;

			object_list_.push_back(item);
			info.first = item->tree.index;
		}

		object_list_type object_list_;
	};

	using bar_collection = collection<bar>;
	using popup_collection = collection<popup>;

	using external_collection = collection<external>;
	using external_bar_collection = collection<external_bar>;
	using external_popup_collection = collection<external_popup>;
}

#endif /* !EWIN_MENU_COLLECTION_H */
