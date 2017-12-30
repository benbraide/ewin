#pragma once

#ifndef EWIN_EXTERNAL_MENU_H
#define EWIN_EXTERNAL_MENU_H

#include <vector>
#include <memory>

#include "menu_bar.h"
#include "menu_popup.h"

namespace ewin::menu{
	class external_item : public item{
	public:
		typedef std::shared_ptr<popup> item_ptr_type;

		external_item(object &parent, std::size_t index);

		virtual ~external_item();

	protected:
		item_ptr_type sub_menu_ptr_;
	};

	template <class menu_type>
	class basic_external : public menu_type{
	public:
		typedef menu_type menu_type;

		typedef std::shared_ptr<object> item_ptr_type;
		typedef std::vector<item_ptr_type> item_list_type;

		basic_external(common::types::hmenu value, application::object &app){
			init_(value, &app);
		}

		virtual ~basic_external(){
			destruct_();
		}

	protected:
		virtual void child_removed_(object &value, std::size_t index) override{
			if (item_list_.empty())
				return;

			auto list_end = item_list_.end();
			for (auto iter = item_list_.begin(); iter != list_end; ++iter){
				if (iter->get() == &value){
					item_list_.erase(iter);
					break;
				}
			}
		}

		virtual void destruct_() override{
			menu_type::destruct_();
			if (!app_->menu_handles_.empty()){//Remove from list
				auto entry = app_->menu_handles_.find(handle_);
				if (entry != app_->menu_handles_.end())
					app_->menu_handles_.erase(entry);
			}

			if (handle_ == app_->cached_menu_handle_.first){//Reset cached
				app_->cached_menu_handle_.first = nullptr;
				app_->cached_menu_handle_.second = nullptr;
			}
		}

		void init_(common::types::hmenu value, application::object *app){
			auto_destroy_ = false;//Prevent destruction
			if (value == nullptr)
				return;

			if (app == nullptr){
				error_value_ = error_type::no_app;
				return;
			}

			handle_ = value;
			(app_ = app)->task += [this]{
				auto count = ::GetMenuItemCount(handle_);

				item_list_.reserve(count);
				for (auto index = 0; index < count; ++index)
					item_list_.push_back(std::make_shared<external_item>(*this, index));

				app_->menu_handles_[handle_] = this;
				app_->cached_menu_handle_.first = handle_;
				app_->cached_menu_handle_.second = this;

				common::types::menu_info info{
					sizeof(common::types::menu_info),
					MIM_STYLE,
					MNS_NOTIFYBYPOS
				};

				::SetMenuInfo(handle_, &info);
			};
		}

		item_list_type item_list_;
	};

	using external = basic_external<container>;
	using external_bar = basic_external<bar>;
	using external_popup = basic_external<popup>;
}

#endif /* !EWIN_EXTERNAL_MENU_H */
