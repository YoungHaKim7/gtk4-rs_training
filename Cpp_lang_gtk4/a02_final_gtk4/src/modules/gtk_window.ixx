// module; // global module fragment for includes
#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>

export module gtk_window;

import <string>;
import <format>;
import <cstdint>;

namespace demo {

class MainWindow : public Gtk::ApplicationWindow {
public:
  MainWindow() : btn_("Clicked 0 times"), count_(0) {
    set_title("GTK4 C++ Modules Demo");
    set_default_size(420, 240);

    // simple layout
    set_child(btn_);

    // when clicked, update label
    btn_.signal_clicked().connect([this] {
      ++count_;
      btn_.set_label(
          std::format("Clicked {} {}", count_, count_ == 1 ? "time" : "times"));
    });
  }

private:
  Gtk::Button btn_;
  std::uint32_t count_;
};

} // namespace demo
