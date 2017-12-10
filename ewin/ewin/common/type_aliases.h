#pragma once

#ifndef EWIN_TYPE_ALIASES_H
#define EWIN_TYPE_ALIASES_H

#define WIN32_LEAN_AND_MEAN

#include <string>

#include "common_headers.h"

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

	typedef ::HWND				hwnd;
	typedef ::HMENU				hmenu;

	typedef ::HICON				hicon;
	typedef ::HCURSOR			hcursor;
	typedef ::HBRUSH			hbrush;

	typedef ::WNDCLASSEXW		wnd_class;
	typedef ::CREATESTRUCTW		create_struct;
	typedef ::WINDOWPLACEMENT	wnd_placement;

	typedef ::POINT				point;
	typedef ::SIZE				size;
	typedef ::RECT				rect;
}

#endif /* !EWIN_TYPE_ALIASES_H */
