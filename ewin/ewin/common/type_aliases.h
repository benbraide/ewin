#pragma once

#ifndef EWIN_TYPE_ALIASES_H
#define EWIN_TYPE_ALIASES_H

#define WIN32_LEAN_AND_MEAN

#include <string>

#include <windows.h>

namespace ewin::types{
	typedef __int64				int64;
	typedef unsigned __int64	uint64;
	typedef std::size_t			size;

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

	typedef ::HWND				hwnd;
	typedef ::HMENU				hmenu;

	typedef ::CREATESTRUCTW		create_struct;
	typedef ::WINDOWPLACEMENT	wnd_placement;
}

#endif /* !EWIN_TYPE_ALIASES_H */
