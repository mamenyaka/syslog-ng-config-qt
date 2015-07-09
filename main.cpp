#include "mainwindow.h"
#include "config.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  Config config("drivers.yml");

  QApplication a(argc, argv);
  MainWindow w(config);
  w.show();

  return a.exec();
}
