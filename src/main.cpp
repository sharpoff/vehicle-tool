#include "application.h"

int main()
{
    Application *app = new Application("Application", 1280, 720);
    app->run();

    delete app;
    return 0;
}