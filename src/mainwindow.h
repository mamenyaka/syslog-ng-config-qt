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

  QString last_saved;

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent* event);

private:
  void setupConnections();

  // non copyable
  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;
};

#endif // MAINWINDOW_H
