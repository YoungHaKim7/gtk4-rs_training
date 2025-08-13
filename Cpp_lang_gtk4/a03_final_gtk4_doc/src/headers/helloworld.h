// File: helloworld.h

#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

// #include <gtkmm/button.h>
#include <gtkmm-4.0/gtkmm/window.h>

class HelloWorld : public Gtk::Window {

public:
  HelloWorld();
  ~HelloWorld() override;

protected:
  // Signal handlers:
  void on_button_clicked();

  // Member widgets:
  Gtk::Button m_button;
};

#endif
