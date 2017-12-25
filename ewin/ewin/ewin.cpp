#define WIN32_LEAN_AND_MEAN

#include "common/random_number.h"
#include "common/random_bool.h"
#include "common/random_string.h"

#include "window/message_window.h"
#include "window/window_form.h"

#include "application/main_application.h"

struct base{};

struct derived : base{};

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

	return main_app.run;
}
