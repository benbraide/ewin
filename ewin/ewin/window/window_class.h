#pragma once

#ifndef EWIN_WINDOW_CLASS_H
#define EWIN_WINDOW_CLASS_H

#include "../common/type_aliases.h"
#include "../common/boolean_property.h"
#include "../common/string_property.h"
#include "../common/state_property.h"

namespace ewin::window{
	class wnd_class{
	public:
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

		wnd_class()
			: id_(static_cast<common::types::atom>(0)), auto_destroy_(true){
			info_ = common::types::wnd_class{ sizeof(common::types::wnd_class) };
			bind_properties_();
		}

		explicit wnd_class(const create_info &info)
			: id_(static_cast<common::types::atom>(0)), auto_destroy_(true){
			info_ = common::types::wnd_class{ sizeof(common::types::wnd_class) };
			bind_properties_();
			try{
				create_(true, &info);
			}
			catch (...){}
		}

		~wnd_class(){
			if (auto_destroy_){
				try{
					create_(false, nullptr);
				}
				catch (...){}
			}
		}

		common::read_only_value_property<common::types::atom, wnd_class> id;
		common::read_only_value_property<const wchar_t *, wnd_class> raw_name;

		common::value_property<common::types::hinstance, wnd_class> instance;
		common::value_property<common::types::procedure, wnd_class> procedure;

		common::string_value_property<std::wstring, wnd_class> name;
		common::string_value_property<std::wstring, wnd_class> menu;

		common::state_value_property<common::types::uint, wnd_class> style;
		common::value_property<common::types::hbrush, wnd_class> background_brush;

		common::value_property<common::types::hicon, wnd_class> small_icon;
		common::value_property<common::types::hicon, wnd_class> icon;

		common::numeric_value_property<int, wnd_class> wnd_extra;
		common::numeric_value_property<int, wnd_class> cls_extra;

		common::boolean_value_property<wnd_class> created;
		common::write_only_value_property<create_info, wnd_class> create;
		common::boolean_value_property<wnd_class> auto_destroy;

	private:
		void bind_properties_(){
			auto handler = std::bind(&wnd_class::handle_property_, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

			id.initialize_(&id_, nullptr);
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
			cls_extra.initialize_(&info_.cbClsExtra, handler);

			created.initialize_(nullptr, handler);
			create.initialize_(nullptr, handler);
			auto_destroy.initialize_(&auto_destroy_, nullptr);
		}

		void handle_property_(void *prop, void *arg, common::property_access access){
			if (prop == &instance){
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
			}
		}

		void create_(bool create, const create_info *info){
			if (create == (id_ != static_cast<common::types::atom>(0)))
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
				id_ = static_cast<common::types::atom>(0);
		}

		common::types::atom id_;
		bool auto_destroy_;
		common::types::wnd_class info_;
		std::wstring name_, menu_;
	};
}

#endif /* !EWIN_WINDOW_CLASS_H */
