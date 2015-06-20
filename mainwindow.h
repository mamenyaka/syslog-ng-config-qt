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

  std::vector<DefaultDriver> default_drivers;
  std::vector<Driver> drivers;

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

signals:
  void create_shape(const std::string& id);

private:
  void parse_yaml();

  void driver_select_dialog(const std::string& type);
  int form_dialog(Driver& driver);

  std::vector<DefaultDriver>::const_iterator get_default_driver_iterator(const std::string& name, const std::string& type) const;
  std::vector<Driver>::iterator get_driver_iterator(const std::string& id);
  int get_count(const std::string& name, const std::string& type) const;
};

#endif // MAINWINDOW_H
