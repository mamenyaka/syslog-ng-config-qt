#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include "dialog.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QKeyEvent>

MainWindow::MainWindow(const std::vector<DefaultDriver>& default_drivers, QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  widget(new Widget(drivers)),
  default_drivers(default_drivers)
{
  ui->setupUi(this);
  ui->horizontalLayout->addWidget(widget);

  connect(this, &MainWindow::add_driver, widget, &Widget::add_driver);
  connect(widget, &Widget::update_driver, [=](Driver& driver) {
    driver_form_dialog(driver);
  });

  connect(ui->actionNew, &QAction::triggered, [=]() {
    const int exec = QMessageBox::question(this, tr("New configuration"),
                                           tr("Warning: All unsaved data will be lost! Are you sure?"));

    if (exec == QMessageBox::Yes)
    {
      drivers.clear();
      emit add_driver(nullptr);
      widget->update();
    }
  });
  connect(ui->actionAbout, &QAction::triggered, [=]() {
    QMessageBox::aboutQt(this);
  });
  connect(ui->actionSource, &QAction::triggered, [=]() {
    driver_select_dialog("source");
  });
  connect(ui->actionDestination, &QAction::triggered, [=]() {
    driver_select_dialog("destination");
  });
}

MainWindow::~MainWindow()
{
  delete ui;
  delete widget;
}

void MainWindow::driver_select_dialog(const std::string type)
{
  QStringList items;
  for (const DefaultDriver& driver : default_drivers)
  {
    if (driver.get_type() == type)
    {
      items << QString::fromStdString(driver.get_name());
    }
  }

  bool ok;
  const std::string name = QInputDialog::getItem(this, QString::fromStdString("Create new " + type), tr("Select subtype"),
                                                 items, 0, false, &ok).toStdString();

  if (ok && !name.empty())
  {
    Driver new_driver = get_driver(name, type);
    new_driver.set_id(get_next_driver_id(name, type));

    const int exec = driver_form_dialog(new_driver);
    if (exec == QDialog::Accepted)
    {
      drivers.push_back(std::move(new_driver));

      emit add_driver(&drivers.back());
    }
  }
}

int MainWindow::driver_form_dialog(Driver& driver)
{
  Dialog dialog(driver, this);

  return dialog.exec();
}

const DefaultDriver& MainWindow::get_driver(const std::string& name, const std::string& type) const
{
  for (const DefaultDriver& driver : default_drivers)
  {
    if (driver.get_name() == name && driver.get_type() == type)
    {
      return driver;
    }
  }
}

int MainWindow::get_next_driver_id(const std::string& name, const std::string& type) const
{
  int count = 0;
  for (const Driver& driver : drivers)
  {
    if (driver.get_name() == name && driver.get_type() == type)
    {
      count++;
    }
  }

  return count;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
  QApplication::sendEvent(widget, event);
}
