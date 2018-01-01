#pragma once

#ifndef EWIN_CONTROL_OBJECT_H
#define EWIN_CONTROL_OBJECT_H

#include "../window_object.h"

#include "control_initializer.h"

namespace ewin::window::control{
	struct size_f{
		float width;
		float height;
	};

	class object : public window::object{
	public:
		typedef window::object base_type;
		typedef control::id id_type;

		object();

		virtual ~object();

		common::read_only_value_property<id_type, object> id;
		common::value_property<common::types::hfont, object> font;

		common::size_value_property<size_f, object> relative_size;
		common::size_value_property<common::types::size, object> padding;

	protected:
		void bind_properties_();

		virtual void handle_property_(void *prop, void *arg, common::property_access access) override;

		virtual void resize_();

		virtual void compute_size_();

		id_type id_;
		common::types::hfont font_;
		size_f relative_size_;
		common::types::size padding_;
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
				procedure_ = initializer<control_id>::initialize();
			return control::attributes<control_id>::class_name();
		}
	};
}

#endif /* !EWIN_CONTROL_OBJECT_H */
