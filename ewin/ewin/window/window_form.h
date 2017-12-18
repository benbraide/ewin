#pragma once

#ifndef EWIN_WINDOW_FORM_H
#define EWIN_WINDOW_FORM_H

#include "dialog_window.h"

namespace ewin::window{
	template <class window_type>
	class basic_form : public window_type{
	public:
		typedef window_type window_type;

		typedef typename window_type::application_type application_type;
		typedef typename window_type::attribute_option_type attribute_option_type;

		struct create_info : public window_type::create_info{
			std::variant<std::reference_wrapper<object>, application_type *> app_or_parent;
			std::wstring caption;
			common::types::point position;
			common::types::size size;
			attribute_option_type options;
		};

		basic_form(){
			bind_properties_();
		}

		basic_form(const std::wstring &caption, const common::types::point &offset, const common::types::size &size,
			attribute_option_type options = attribute_option_type::nil){
			bind_properties_();
			create_info info{
				nullptr,
				caption,
				offset,
				size,
				options
			};

			try{
				create_(true, &info);
			}
			catch (common::error_type e){
				window_type::error_value_ = e;
			}
		}

		basic_form(object &parent, const std::wstring &caption, const common::types::point &offset, const common::types::size &size,
			attribute_option_type options = attribute_option_type::nil){
			bind_properties_();
			create_info info{
				parent,
				caption,
				offset,
				size,
				options
			};
			
			try{
				create_(true, &info);
			}
			catch (common::error_type e){
				window_type::error_value_ = e;
			}
		}

		basic_form(application_type &app, const std::wstring &caption, const common::types::point &offset, const common::types::size &size,
			attribute_option_type options = attribute_option_type::nil){
			bind_properties_();
			create_info info{
				&app,
				caption,
				offset,
				size,
				options
			};
			
			try{
				create_(true, &info);
			}
			catch (common::error_type e){
				window_type::error_value_ = e;
			}
		}

		virtual ~basic_form(){
			window_type::destruct_();
		}

		common::string_value_property<std::wstring, basic_form> caption;

	protected:
		void bind_properties_(){
			caption.initialize_(&caption_, EWIN_PROP_HANDLER(basic_form<window_type>));
		}

		virtual void handle_property_(void *prop, void *arg, common::property_access access) override{
			if (prop == &caption){
				if (access == common::property_access::write && window_type::handle_ != nullptr)
					::SetWindowTextW(window_type::handle_, caption_.data());
			}
			else//Forward
				window_type::handle_property_(prop, arg, access);
		}

		virtual void create_(bool create, const typename window_type::create_info *info) override{
			if (!create){//Forward message
				window_type::create_(create, info);
				return;
			}

			if (info == nullptr){//Use cache
				common::types::size size{};
				common::types::point position{};

				auto options = attribute_option_type::nil;
				if (cache_.position.x != 0 || cache_.position.y != 0){
					options = attribute_option_type::absolute_position;
					position = cache_.position;
				}
				else//Relative position
					position = cache_.relative_position;

				if (cache_.size.cx == 0 && cache_.size.cy == 0){
					EWIN_SET(options, attribute_option_type::client_size);
					size = cache_.client_size;
				}
				else//Full size
					size = cache_.size;

				window_type::low_level_create_(common::types::create_struct{
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					size.cy,
					size.cx,
					position.y,
					position.x,
					0,
					caption_.data(),
					nullptr,
					0u
				}, nullptr, nullptr, options);

				return;
			}

			auto converted_info = reinterpret_cast<const create_info *>(info);
			object *parent = nullptr;
			application_type *app = nullptr;

			if (std::holds_alternative<application_type *>(converted_info->app_or_parent))
				app = std::get<application_type *>(converted_info->app_or_parent);
			else//Parent
				parent = &std::get<std::reference_wrapper<object>>(converted_info->app_or_parent).get();

			window_type::low_level_create_(common::types::create_struct{
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				converted_info->size.cy,
				converted_info->size.cx,
				converted_info->position.y,
				converted_info->position.x,
				0,
				converted_info->caption.data(),
				nullptr,
				0u
			}, parent, app, converted_info->options);
		}

		std::wstring caption_;
	};

	using form			= basic_form<object>;
	using dialog_form	= basic_form<dialog>;
}

#endif /* !EWIN_WINDOW_FORM_H */
