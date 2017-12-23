# ewin
Modern Property Based C++ Win32 Framework

Example Usage:
==============
```c++
#include "window/window_form.h"  
#include "application/main_application.h"  

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){
    ewin::application::main_app main_app;       //Main App instance - required for all applications
    ewin::window::form form;                    //Window Form instance - Omiting target app uses Main App

    //Set properties of the window
    form.caption          = L"Window Form Example";
    form.position.x       = CW_USEDEFAULT;
    form.position.y       = CW_USEDEFAULT;
    form.size.width       = 600;
    form.size.height      = 400;
    form.frame.visible    = true;               //Show frame around window - Caption, Borders, etc
    form.view.visible     = true;               //Make window visible after creation
    form.created          = true;               //Create the window(form)

    return main_app.run;                        //Run the Main App
}
```
