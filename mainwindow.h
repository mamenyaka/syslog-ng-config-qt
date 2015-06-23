#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class Widget;
class Config;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Ui::MainWindow* ui;
  Widget* widget;
  Config& config;

public:
  explicit MainWindow(Config& config, QWidget* parent = 0);
  ~MainWindow();

protected:
  bool eventFilter(QObject *, QEvent* event);

private:
  void driver_select_dialog(const std::string type);
};

#endif // MAINWINDOW_H
