#pragma once

#ifndef EWIN_WRITING_GDI_FONT_H
#define EWIN_WRITING_GDI_FONT_H

#include <functional>

#include "../common/error_target.h"
#include "../common/numeric_property.h"
#include "../common/boolean_property.h"
#include "../common/string_property.h"

#include "writing_type_aliases.h"

namespace ewin::window::control{
	class object;
}

namespace ewin::writing{
	class gdi_font : public common::error_target<>{
	public:
		typedef std::function<void(gdi_font &)> callback_type;

		gdi_font();

		explicit gdi_font(callback_type callback);

		virtual ~gdi_font();

		gdi_font(const gdi_font &) = delete;
		
		gdi_font &operator =(const gdi_font &) = delete;

		common::value_property<types::gdi_font, gdi_font> handle;

		common::value_property<types::gdi_font_info, gdi_font> info;
		common::string_value_property<std::wstring, gdi_font> face_name;

		common::numeric_value_property<int, gdi_font> height;
		common::numeric_value_property<int, gdi_font> weight;

		common::numeric_value_property<int, gdi_font> escapement;
		common::numeric_value_property<int, gdi_font> orientation;

		common::boolean_value_property<gdi_font> italic;
		common::boolean_value_property<gdi_font> underline;
		common::boolean_value_property<gdi_font> strike_out;

		common::boolean_value_property<gdi_font> created;
		common::boolean_value_property<gdi_font> auto_destroy;

	protected:
		friend class window::control::object;

		void bind_properties_();

		void handle_property_(void *prop, void *arg, common::property_access access);

		void destroy_();

		types::gdi_font get_handle_();

		callback_type callback_;
		types::gdi_font_info info_;
		types::gdi_font handle_;
		std::wstring face_name_;
		bool auto_destroy_;
	};
}

#endif /* !EWIN_WRITING_GDI_FONT_H */
