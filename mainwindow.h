#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "driver.h"

#include <QMainWindow>

#include <vector>

namespace Ui {
class MainWindow;
}
class Widget;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Ui::MainWindow* ui;
  Widget* widget;
  const std::vector<DefaultDriver>& default_drivers;

  std::vector<Driver> drivers;

public:
  explicit MainWindow(const std::vector<DefaultDriver>& default_drivers, QWidget* parent = 0);
  ~MainWindow();

signals:
  void add_driver(Driver* driver);

protected:
  void keyPressEvent(QKeyEvent* event);

private:
  void driver_select_dialog(const std::string type);
  int driver_form_dialog(Driver& driver);

  const DefaultDriver& get_driver(const std::string& name, const std::string& type) const;

  int get_next_driver_id(const std::string& name, const std::string& type) const;
};

#endif // MAINWINDOW_H
