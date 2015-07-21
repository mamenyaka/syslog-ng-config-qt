#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"

#include <QMainWindow>

class Scene;
namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Config config;
  Scene* scene;

  Ui::MainWindow* ui;

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent* event);

private:
  void setupConnections();
};

#endif // MAINWINDOW_H
