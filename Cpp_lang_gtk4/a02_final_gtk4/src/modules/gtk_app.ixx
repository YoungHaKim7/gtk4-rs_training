// module; // include third-party headers before the module declaration
#include <gtkmm/application.h>

export module gtk_app;

import <string>;
import gtk_window; // our window module

namespace demo {

class App {
public:
  explicit App(const std::string &app_id = "com.example.Gtk4ModuleDemo")
      : app_(Gtk::Application::create(app_id)) {}

  int run(int argc, char **argv) {
    // gtkmm-4 provides make_window_and_run to instantiate and run a custom
    // window type
    return app_->make_window_and_run<demo::MainWindow>(argc, argv);
  }

private:
  Glib::RefPtr<Gtk::Application> app_;
};

} // namespace demo
