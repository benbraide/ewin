#include "external_window.h"

ewin::window::external::external(common::types::hwnd value){
	auto_destroy_ = false;//Prevent destruction
	if ((handle_ = value) != nullptr){//Resolve parent
		auto parent_handle = ::GetParent(handle_);
		if (parent_handle != nullptr){//Has parent
			auto parent = application::manager::main->window_handles[parent_handle];
			if (parent == nullptr){//Search other apps
				for (application_type &app : application::manager::application_list){
					if ((parent = app.window_handles[parent_handle]) != nullptr)
						break;//Handle found
				}
			}

			if (parent == nullptr)
				tree.parent_ = std::make_shared<external>(parent_handle);
			else//Use existing window object
				tree.parent_ = parent->reflect;
		}
	}
}
