#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
  class MainWindow;
}
class Config;
class Scene;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Ui::MainWindow* ui;

  Config* config;
  Scene* scene;

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent* event);

  bool eventFilter(QObject *, QEvent* event);

private:
  void setupConnections();
};

#endif // MAINWINDOW_H
