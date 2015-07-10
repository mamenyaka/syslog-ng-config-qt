#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class QWebView;
class Widget;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Ui::MainWindow* ui;
  QWebView* webView;
  Widget* widget;
  Config config;

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

protected:
  bool eventFilter(QObject *, QEvent* event);
  void closeEvent(QCloseEvent* event);

private:
  void driver_select_dialog(const std::string& type);
  void create_new_driver(const std::string& name, const std::string& type);
};

#endif // MAINWINDOW_H
