#include <gtkmm.h>
#include <ui_main.h>

/* Launch Application */
int main (int argc, char *argv[])
{
  auto app = Gtk::Application::create(argc, argv, "me.osa2");

  UIMain main;
  app->run(main);

  return 0;
}
