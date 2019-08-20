#include <iostream>
#include <MainApplication.h>

int main()
{
    MainApplication app;
    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();
    return 0;
}
