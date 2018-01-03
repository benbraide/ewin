#define WIN32_LEAN_AND_MEAN

#include "common/random_number.h"
#include "common/random_bool.h"
#include "common/random_string.h"

#include "window/message_window.h"
#include "window/window_form.h"

#include "application/main_application.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){
	ewin::application::main_app main_app;
	ewin::window::form form;

	form.caption		= L"Window Form Example";
	form.position.x		= CW_USEDEFAULT;
	form.position.y		= CW_USEDEFAULT;
	form.size.width		= 600;
	form.size.height	= 400;
	form.frame.visible	= true;
	form.view.visible	= true;
	form.created		= true;

	form.system_menu->separators += [](std::size_t &index){
		return true;
	};

	form.system_menu->items += [&](ewin::menu::item &item, std::size_t &index){
		item.label = L"Appended To System Menu";
		item.events->select += [&]{
			form.view.maximized = true;
		};
		return true;
	};

	//ewin::menu::bar_collection bar;
	form.menu->items += [](ewin::menu::item &item, std::size_t &index){
		item.label = L"First Item";
		item.events->select += []{

		};
		return true;
	};

	form.menu->items += [](ewin::menu::item &item, std::size_t &index){
		item.label = L"Last Item";
		item.events->select += []{

		};
		return true;
	};

	form.menu->items += [](ewin::menu::item &item, std::size_t &index){
		item.label = L"Inserted Item";
		item.events->select += []{

		};
		index = 1;
		return true;
	};

	form.menu->links += [](ewin::menu::item &item, ewin::menu::popup_collection &popup, std::size_t &index){
		item.label = L"Pushed Link";
		index = 0;

		popup.items += [](ewin::menu::item &item, std::size_t &index){
			item.label = L"First Popup Item";
			item.events->select += []{

			};
			return true;
		};

		popup.items += [](ewin::menu::item &item, std::size_t &index){
			item.label = L"Second Popup Item";
			item.events->select += []{

			};
			return true;
		};

		popup.separators += [](std::size_t &index){
			return true;
		};

		popup.checks += [](ewin::menu::item &item, std::size_t &index){
			item.label = L"Check Popup Item";
			item.events->check += []{

			};
			return true;
		};

		popup.separators += [](std::size_t &index){
			return true;
		};

		popup.radios += [](ewin::menu::item &item, std::size_t &index){
			item.label = L"Radio1 Popup Item";
			return true;
		};

		popup.radios += [](ewin::menu::item &item, std::size_t &index){
			item.label = L"Radio2 Popup Item";
			item.events->check += []{

			};
			return true;
		};

		popup.separators += [](std::size_t &index){
			return true;
		};

		popup.items += [](ewin::menu::item &item, std::size_t &index){
			item.label = L"Last Popup Item";
			item.events->select += []{

			};
			return true;
		};

		return true;
	};

	/*ewin::menu::bar bar;
	bar.created = true;

	ewin::menu::item item1;
	item1.tree.parent = bar;
	item1.label = L"First Item";
	item1.created = true;

	ewin::menu::item item2;
	bar.tree.children += item2;
	item2.label = L"Last Item";
	item2.created = true;

	ewin::menu::item item3;
	item3.tree.previous_sibling = item1;
	item3.label = L"Inserted Item";
	item3.created = true;*/

	//bar.owner = form;
	return main_app.run;
}
