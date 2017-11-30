#pragma once

#ifndef EWIN_WINDOW_OBJECT_H
#define EWIN_WINDOW_OBJECT_H

#include <list>

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/string_property.h"
#include "../common/state_property.h"
#include "../common/list_property.h"
#include "../common/extended_value_property.h"
#include "../common/size_property.h"
#include "../common/point_property.h"
#include "../common/rect_property.h"

namespace ewin::application{
	class object;
}

namespace ewin::window{
	class object{
	public:
		typedef application::object application_type;

		typedef std::list<object *> object_list_type;
		typedef object_list_type::iterator object_list_iterator_type;
		typedef object_list_type::const_iterator object_list_const_iterator_type;

		struct create_info{
			common::types::hinstance instance;
			common::types::procedure procedure;
			std::wstring name;
			std::wstring menu;
			common::types::uint style;
			common::types::hbrush background_brush;
			common::types::hicon small_icon;
			common::types::hicon icon;
			int wnd_extra;
			int cls_extra;
		};

		object()
			: auto_destroy_(true){
			bind_properties_();
		}

		virtual ~object(){
			if (auto_destroy_){
				try{
					create_(false, nullptr);
				}
				catch (...){}
			}
		}

		common::read_only_value_property<common::types::hwnd, object> handle;
		common::read_only_value_property<common::types::procedure, object> procedure;

		common::string_value_property<std::wstring, object> caption;
		common::string_value_property<std::wstring, object> menu;

		common::state_value_property<common::types::uint, object> style;
		common::state_value_property<common::types::uint, object> extended_style;

		common::size_value_property<int, object> size;
		common::size_value_property<float, object> relative_size;

		common::point_value_property<int, object> offset;
		common::point_value_property<int, object> relative_offset;

		common::rect_value_property<int, object> rect;
		common::rect_value_property<int, object> relative_rect;

		common::extended_value_property<application_type, application_type *, object> app;
		common::extended_value_property<object, object *, object> parent;

		common::extended_value_property<object, object *, object> previous_sibling;
		common::extended_value_property<object, object *, object> next_sibling;

		common::list_value_property<object, object_list_iterator_type, object_list_const_iterator_type, object> children;
		common::list_value_property<object, object_list_iterator_type, object_list_const_iterator_type, object> siblings;
		common::list_value_property<object, object_list_iterator_type, object_list_const_iterator_type, object> ancestors;

		common::boolean_value_property<object> visible;
		common::boolean_value_property<object> enabled;

		common::boolean_value_property<object> maximized;
		common::boolean_value_property<object> minimized;

		common::boolean_value_property<object> show_caption;
		common::boolean_value_property<object> show_system_menu;

		common::boolean_value_property<object> clip_children;
		common::boolean_value_property<object> clip_siblings;

		common::boolean_value_property<object> tab_stop;
		common::boolean_value_property<object> file_drop;
		common::boolean_value_property<object> mouse_activate;

		common::boolean_value_property<object> right_align;
		common::boolean_value_property<object> rtl_reading;
		common::boolean_value_property<object> rtl_layout;

		common::boolean_value_property<object> top_most;
		common::boolean_value_property<object> transparent;

		common::boolean_value_property<object> created;
		common::boolean_value_property<object> auto_destroy;

	private:
		void bind_properties_(){
			auto handler = std::bind(&object::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

			/*id.initialize_(&id_, nullptr);
			raw_name.initialize_(&info_.lpszClassName, nullptr);

			instance.initialize_(&info_.hInstance, handler);
			procedure.initialize_(&info_.lpfnWndProc, handler);

			name.initialize_(&name_, handler);
			menu.initialize_(&menu_, handler);

			style.initialize_(&info_.style, handler);
			background_brush.initialize_(&info_.hbrBackground, handler);

			small_icon.initialize_(&info_.hIconSm, handler);
			icon.initialize_(&info_.hIcon, handler);

			wnd_extra.initialize_(&info_.cbWndExtra, handler);
			cls_extra.initialize_(&info_.cbClsExtra, handler);*/

			created.initialize_(nullptr, handler);
			auto_destroy.initialize_(&auto_destroy_, nullptr);
		}

		virtual void handle_property_(void *prop, void *arg, common::property_access access){
			/*if (prop == &instance){
				if (access == common::property_access::read)
					*static_cast<common::types::hinstance *>(arg) = info_.hInstance;
				else if (access == common::property_access::write)
					info_.hInstance = *static_cast<common::types::hinstance *>(arg);
			}
			else if (prop == &procedure){
				if (access == common::property_access::read)
					*static_cast<common::types::procedure *>(arg) = info_.lpfnWndProc;
				else if (access == common::property_access::write)
					info_.lpfnWndProc = *static_cast<common::types::procedure *>(arg);
			}
			else if (prop == &name){
				if (access == common::property_access::read)
					*static_cast<std::wstring *>(arg) = name_;
				else if (access == common::property_access::write)
					info_.lpszClassName = (name_ = *static_cast<std::wstring *>(arg)).data();
			}
			else if (prop == &menu){
				if (access == common::property_access::read)
					*static_cast<std::wstring *>(arg) = menu_;
				else if (access == common::property_access::write)
					info_.lpszMenuName = (menu_ = *static_cast<std::wstring *>(arg)).data();
			}
			else if (prop == &style){
				if (access == common::property_access::read)
					*static_cast<common::types::uint *>(arg) = info_.style;
				else if (access == common::property_access::write)
					info_.style = *static_cast<common::types::uint *>(arg);
			}
			else if (prop == &background_brush){
				if (access == common::property_access::read)
					*static_cast<common::types::hbrush *>(arg) = info_.hbrBackground;
				else if (access == common::property_access::write)
					info_.hbrBackground = *static_cast<common::types::hbrush *>(arg);
			}
			else if (prop == &small_icon){
				if (access == common::property_access::read)
					*static_cast<common::types::hicon *>(arg) = info_.hIconSm;
				else if (access == common::property_access::write)
					info_.hIconSm = *static_cast<common::types::hicon *>(arg);
			}
			else if (prop == &icon){
				if (access == common::property_access::read)
					*static_cast<common::types::hicon *>(arg) = info_.hIcon;
				else if (access == common::property_access::write)
					info_.hIcon = *static_cast<common::types::hicon *>(arg);
			}
			else if (prop == &wnd_extra){
				if (access == common::property_access::read)
					*static_cast<int *>(arg) = info_.cbWndExtra;
				else if (access == common::property_access::write)
					info_.cbWndExtra = *static_cast<int *>(arg);
			}
			else if (prop == &cls_extra){
				if (access == common::property_access::read)
					*static_cast<int *>(arg) = info_.cbClsExtra;
				else if (access == common::property_access::write)
					info_.cbClsExtra = *static_cast<int *>(arg);
			}
			else if (prop == &created){
				if (access == common::property_access::read)
					*static_cast<bool *>(arg) = (id_ != static_cast<common::types::atom>(0));
				else if (access == common::property_access::write)
					create_(*static_cast<bool *>(arg), nullptr);
			}
			else if (prop == &create){
				create_(true, static_cast<create_info *>(arg));
			}*/
		}

		void create_(bool create, const create_info *info){
			/*if (create == (id_ != static_cast<common::types::atom>(0)))
				return;//Already created or destroyed

			if (create){
				if (info != nullptr){//Copy info
					info_.hInstance = info->instance;
					info_.lpfnWndProc = info->procedure;
					info_.lpszClassName = (name_ = info->name).data();
					info_.lpszMenuName = (menu_ = info->menu).data();
					info_.style = info->style;
					info_.hbrBackground = info->background_brush;
					info_.hIconSm = info->small_icon;
					info_.hIcon = info->icon;
					info_.cbWndExtra = info->wnd_extra;
					info_.cbClsExtra = info->cls_extra;
				}

				if ((id_ = ::RegisterClassExW(&info_)) == static_cast<common::types::atom>(0))
					throw ::GetLastError();//Failed to register class
			}
			else if (::UnregisterClassW(info_.lpszClassName, info_.hInstance) == FALSE)
				throw ::GetLastError();//Failed
			else//Reset ID
				id_ = static_cast<common::types::atom>(0);*/
		}

		bool auto_destroy_;
	};
}

#endif /* !EWIN_WINDOW_OBJECT_H */
