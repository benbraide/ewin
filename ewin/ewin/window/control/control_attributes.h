#pragma once

#ifndef EWIN_CONTROL_ATTRIBUTES_H
#define EWIN_CONTROL_ATTRIBUTES_H

#include "../../common/type_aliases.h"

namespace ewin::window::control{
	enum class id{
		nil,
		button,
		edit,
		static_,
		list_box,
		combo_box,
		combo_box_ex,
		scroll_bar,
		tab,
		tool_tip,
		animate,
		tool_bar,
		status_bar,
		track_bar,
		rebar,
		date_time_picker,
		hot_key,
		ip,
		hyperlink,
		list_view,
		tree_view,
		native_font,
		pager,
		progress,
		up_down,
		header,
	};

	template <id control_id>
	struct attributes;

	template <>
	struct attributes<id::button>{
		static const common::types::dword initializer = ICC_STANDARD_CLASSES;
		static const wchar_t *class_name(){
			return WC_BUTTONW;
		}
	};

	template <>
	struct attributes<id::edit>{
		static const common::types::dword initializer = ICC_STANDARD_CLASSES;
		static const wchar_t *class_name(){
			return WC_EDITW;
		}
	};

	template <>
	struct attributes<id::static_>{
		static const common::types::dword initializer = ICC_STANDARD_CLASSES;
		static const wchar_t *class_name(){
			return WC_STATICW;
		}
	};

	template <>
	struct attributes<id::list_box>{
		static const common::types::dword initializer = ICC_STANDARD_CLASSES;
		static const wchar_t *class_name(){
			return WC_LISTBOXW;
		}
	};

	template <>
	struct attributes<id::combo_box>{
		static const common::types::dword initializer = ICC_STANDARD_CLASSES;
		static const wchar_t *class_name(){
			return WC_COMBOBOXW;
		}
	};

	template <>
	struct attributes<id::combo_box_ex>{
		static const common::types::dword initializer = ICC_USEREX_CLASSES;
		static const wchar_t *class_name(){
			return WC_COMBOBOXEXW;
		}
	};

	template <>
	struct attributes<id::scroll_bar>{
		static const common::types::dword initializer = ICC_STANDARD_CLASSES;
		static const wchar_t *class_name(){
			return WC_SCROLLBARW;
		}
	};

	template <>
	struct attributes<id::tab>{
		static const common::types::dword initializer = ICC_TAB_CLASSES;
		static const wchar_t *class_name(){
			return WC_TABCONTROLW;
		}
	};

	template <>
	struct attributes<id::tool_tip>{
		static const common::types::dword initializer = ICC_TAB_CLASSES;
		static const wchar_t *class_name(){
			return TOOLTIPS_CLASSW;
		}
	};

	template <>
	struct attributes<id::animate>{
		static const common::types::dword initializer = ICC_ANIMATE_CLASS;
		static const wchar_t *class_name(){
			return ANIMATE_CLASSW;
		}
	};

	template <>
	struct attributes<id::tool_bar>{
		static const common::types::dword initializer = ICC_BAR_CLASSES;
		static const wchar_t *class_name(){
			return TOOLBARCLASSNAMEW;
		}
	};

	template <>
	struct attributes<id::status_bar>{
		static const common::types::dword initializer = ICC_BAR_CLASSES;
		static const wchar_t *class_name(){
			return STATUSCLASSNAMEW;
		}
	};

	template <>
	struct attributes<id::track_bar>{
		static const common::types::dword initializer = ICC_BAR_CLASSES;
		static const wchar_t *class_name(){
			return TRACKBAR_CLASSW;
		}
	};

	template <>
	struct attributes<id::rebar>{
		static const common::types::dword initializer = ICC_COOL_CLASSES;
		static const wchar_t *class_name(){
			return REBARCLASSNAMEW;
		}
	};

	template <>
	struct attributes<id::date_time_picker>{
		static const common::types::dword initializer = ICC_DATE_CLASSES;
		static const wchar_t *class_name(){
			return DATETIMEPICK_CLASSW;
		}
	};

	template <>
	struct attributes<id::hot_key>{
		static const common::types::dword initializer = ICC_HOTKEY_CLASS;
		static const wchar_t *class_name(){
			return HOTKEY_CLASSW;
		}
	};

	template <>
	struct attributes<id::ip>{
		static const common::types::dword initializer = ICC_INTERNET_CLASSES;
		static const wchar_t *class_name(){
			return WC_IPADDRESSW;
		}
	};

	template <>
	struct attributes<id::hyperlink>{
		static const common::types::dword initializer = ICC_LINK_CLASS;
		static const wchar_t *class_name(){
			return WC_LINK;
		}
	};

	template <>
	struct attributes<id::list_view>{
		static const common::types::dword initializer = ICC_LISTVIEW_CLASSES;
		static const wchar_t *class_name(){
			return WC_LISTVIEWW;
		}
	};

	template <>
	struct attributes<id::tree_view>{
		static const common::types::dword initializer = ICC_TREEVIEW_CLASSES;
		static const wchar_t *class_name(){
			return WC_TREEVIEWW;
		}
	};

	template <>
	struct attributes<id::native_font>{
		static const common::types::dword initializer = ICC_NATIVEFNTCTL_CLASS;
		static const wchar_t *class_name(){
			return WC_NATIVEFONTCTLW;
		}
	};

	template <>
	struct attributes<id::pager>{
		static const common::types::dword initializer = ICC_PAGESCROLLER_CLASS;
		static const wchar_t *class_name(){
			return WC_PAGESCROLLERW;
		}
	};

	template <>
	struct attributes<id::progress>{
		static const common::types::dword initializer = ICC_PROGRESS_CLASS;
		static const wchar_t *class_name(){
			return PROGRESS_CLASSW;
		}
	};

	template <>
	struct attributes<id::up_down>{
		static const common::types::dword initializer = ICC_UPDOWN_CLASS;
		static const wchar_t *class_name(){
			return UPDOWN_CLASSW;
		}
	};

	template <>
	struct attributes<id::header>{
		static const common::types::dword initializer = ICC_LISTVIEW_CLASSES;
		static const wchar_t *class_name(){
			return WC_HEADERW;
		}
	};
}

#endif /* !EWIN_CONTROL_ATTRIBUTES_H */
