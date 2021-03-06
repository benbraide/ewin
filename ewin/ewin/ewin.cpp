#define WIN32_LEAN_AND_MEAN

#include "common/random_number.h"
#include "common/random_bool.h"
#include "common/random_string.h"

#include "window/message_window.h"
#include "window/window_form.h"

#include "application/main_application.h"

#include "window/control/button_control.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){
	ewin::application::main_app main_app;
	ewin::window::form form;

	form.caption		= L"Window Form Example";
	form.position.x		= 50;
	form.position.y		= 50;
	form.size.width		= 600;
	form.size.height	= 400;
	form.frame.visible	= true;
	form.view.visible	= true;
	form.created		= true;

	form.system_menu->separators += [](ewin::menu::separator &item){
		return true;
	};

	form.system_menu->items += [&](ewin::menu::item &item){
		item.label = L"Appended To System Menu";
		item.events->select += [&]{
			form.view.maximized = !form.view.maximized;
		};
		return true;
	};

	//ewin::menu::bar_collection bar;
	form.menu->items += [&](ewin::menu::item &item){
		item.label = L"First Item";
		item.events->select += [&]{
			if (form.position.y == 50)
				form.transition->position.y = 250;
			else
				form.transition->position.y = 50;
		};
		return true;
	};

	form.menu->items += [&](ewin::menu::item &item){
		item.label = L"Last Item";
		item.events->select += [&]{
			if (form.position.x == 50)
				form.transition->position.x = 250;
			else
				form.transition->position.x = 50;
		};
		return true;
	};

	form.menu->items += [&](ewin::menu::item &item){
		item.label = L"Inserted Item";
		item.events->select += [&]{
			if (form.size.width == 600)
				form.transition->size.width = 900;
			else
				form.transition->size.width = 600;
		};
		item.tree.index = 1;
		return true;
	};

	form.menu->links += [](ewin::menu::item &item, ewin::menu::popup_collection &popup){
		item.label = L"Pushed Link";
		item.tree.index = 0;

		popup.items += [](ewin::menu::item &item){
			item.label = L"First Popup Item";
			item.events->select += []{

			};
			return true;
		};

		popup.items += [](ewin::menu::item &item){
			item.label = L"Second Popup Item";
			item.events->select += []{

			};
			return true;
		};

		popup.separators += [](ewin::menu::separator &item){
			return true;
		};

		popup.checks += [](ewin::menu::item &item){
			item.label = L"Check Popup Item";
			item.events->check += []{

			};
			return true;
		};

		popup.separators += [](ewin::menu::separator &item){
			return true;
		};

		popup.radios += [](ewin::menu::item &item){
			item.label = L"Radio1 Popup Item";
			return true;
		};

		popup.radios += [](ewin::menu::item &item){
			item.label = L"Radio2 Popup Item";
			item.events->check += []{

			};
			return true;
		};

		popup.separators += [](ewin::menu::separator &item){
			return true;
		};

		popup.items += [](ewin::menu::item &item){
			item.label = L"Last Popup Item";
			item.events->select += []{

			};
			return true;
		};

		return true;
	};

	ewin::window::control::button btn;
	btn.tree.parent = form;
	btn.label = L"Button";
	btn.relative_position.x = 10;
	btn.relative_position.y = 10;
	btn.created = true;

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
