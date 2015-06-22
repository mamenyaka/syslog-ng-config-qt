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
    const auto cit = std::find_if(default_drivers.cbegin(), default_drivers.cend(),
                                  [&name, &type](const DefaultDriver& driver)->bool {
                                    return driver.get_name() == name && driver.get_type() == type;
                                  });

    const auto crit = std::find_if(drivers.crbegin(), drivers.crend(),
                                   [&name, &type](const Driver& driver)->bool {
                                     return driver.get_name() == name && driver.get_type() == type;
                                   });
    const int id = crit != drivers.crend() ? crit->get_id() + 1 : 0;

    Driver new_driver(*cit, id);

    if (driver_form_dialog(new_driver) == QDialog::Accepted)
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

void MainWindow::keyPressEvent(QKeyEvent* event)
{
  QApplication::sendEvent(widget, event);
}
