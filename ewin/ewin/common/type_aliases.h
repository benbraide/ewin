#pragma once

#ifndef EWIN_TYPE_ALIASES_H
#define EWIN_TYPE_ALIASES_H

#include <string>

#include "common_headers.h"

#define EWIN_SCALAR_CAST(t, v) (t)(v)
#define EWIN_OBJECT_CAST(t, v) *(const std::remove_const_t<t> *)(&(v))

#define EWIN_SCALAR_WPARAM_CAST(v) EWIN_SCALAR_CAST(ewin::common::types::wparam, v)
#define EWIN_SCALAR_LPARAM_CAST(v) EWIN_SCALAR_CAST(ewin::common::types::lparam, v)
#define EWIN_SCALAR_RESULT_CAST(v) EWIN_SCALAR_CAST(ewin::common::types::result, v)

#define EWIN_OBJECT_WPARAM_CAST(v) EWIN_OBJECT_CAST(ewin::common::types::wparam, v)
#define EWIN_OBJECT_LPARAM_CAST(v) EWIN_OBJECT_CAST(ewin::common::types::lparam, v)
#define EWIN_OBJECT_RESULT_CAST(v) EWIN_OBJECT_CAST(ewin::common::types::result, v)

#define EWIN_C_BOOL(v) ((v) ? TRUE : FALSE)
#define EWIN_CPP_BOOL(v) ((v) != FALSE)

namespace ewin::common::types{
	typedef __int64				int64;
	typedef unsigned __int64	uint64;

	typedef ::LRESULT			result;
	typedef ::WPARAM			wparam;
	typedef ::LPARAM			lparam;

	typedef ::LPVOID			pvoid;
	typedef ::LONG_PTR			ptr;
	typedef ::ULONG_PTR			uptr;

	typedef ::UINT				uint;
	typedef ::BYTE				byte;
	typedef ::WORD				word;
	typedef ::DWORD				dword;
	typedef ::ATOM				atom;

	typedef ::HANDLE			handle;
	typedef ::HINSTANCE			hinstance;

	typedef ::WNDPROC			procedure;
	typedef ::HHOOK				hook;

	typedef ::HWND				hwnd;
	typedef ::HMENU				hmenu;

	typedef ::HICON				hicon;
	typedef ::HCURSOR			hcursor;
	typedef ::HBRUSH			hbrush;

	typedef ::WNDCLASSEXW		wnd_class;
	typedef ::CREATESTRUCTW		create_struct;
	typedef ::STYLESTRUCT		style_struct;
	typedef ::MSG				msg;

	typedef ::WINDOWPLACEMENT	wnd_placement;
	typedef ::WINDOWPOS			wnd_position;

	typedef ::CBT_CREATEWND		hook_create_window_info;
	typedef ::CBTACTIVATESTRUCT	hook_activate_info;

	typedef ::POINT				point;
	typedef ::SIZE				size;
	typedef ::RECT				rect;
}

#endif /* !EWIN_TYPE_ALIASES_H */
