#pragma once

#ifndef EWIN_CONTROL_OBJECT_H
#define EWIN_CONTROL_OBJECT_H

#include "../../writing/writing_gdi_font.h"

#include "../window_object.h"

#include "control_initializer.h"

namespace ewin::window::control{
	class object : public window::object{
	public:
		typedef window::object base_type;
		typedef control::id id_type;

		object();

		virtual ~object();

		common::read_only_value_property<id_type, object> id;
		common::string_value_property<std::wstring, object> label;

		common::size_value_property<drawing::types::size, object> relative_size;
		common::size_value_property<common::types::size, object> padding;

		writing::gdi_font font;

		static const wchar_t *alphabets;

	protected:
		friend class application::object;

		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual void create_(bool create, const typename create_info *info) override;

		virtual void on_mouse_move_(events::mouse &e) override{
			auto i = 0;
			i = 0;
		}

		virtual void on_text_change_(events::message &e) override;

		virtual void on_font_change_(events::message &e) override;

		virtual void resize_();

		virtual void compute_size_();

		virtual void compute_additional_size_();

		virtual writing::types::text_format *get_format_();

		virtual writing::types::text_layout *get_layout_();

		virtual void free_rss_();

		virtual void font_changed_(writing::gdi_font &font);

		id_type id_;
		drawing::types::size relative_size_;
		common::types::size padding_;
		writing::types::text_format *format_;
		writing::types::text_layout *layout_;
		std::wstring label_;
		application::object *rss_app_;
	};

	template <id control_id>
	class typed_object : public object{
	public:
		typed_object(){
			procedure_ = nullptr;
			id_ = control_id;
		}

		virtual ~typed_object(){
			destruct_();
		}

	protected:
		virtual const wchar_t *class_name_() override{
			if (procedure_ == nullptr)//Initialize
				procedure_ = control::initializer::initialize<control_id>();
			return control::attributes<control_id>::class_name();
		}
	};
}

#endif /* !EWIN_CONTROL_OBJECT_H */
