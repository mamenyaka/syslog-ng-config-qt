#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include "config.h"
#include "dialog.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QKeyEvent>

#include <iostream>

#define WARNING "Warning: All unsaved data will be lost! Are you sure?"

MainWindow::MainWindow(Config& config, QWidget* parent) :
  QMainWindow(parent),
  config(config)
{
  ui = new Ui::MainWindow,
  ui->setupUi(this);

  widget = new Widget(config);
  setCentralWidget(widget);

  connect(widget, &Widget::update_driver, [=](Driver& driver) {
    Dialog(driver, this).exec();
  });
  connect(widget, &Widget::update_statusbar, [=](const std::string message) {
    ui->statusBar->showMessage(QString::fromStdString(message));
  });
  connect(widget, &Widget::clear_statusbar, [=]() {
    ui->statusBar->clearMessage();
  });

  connect(ui->actionNew, &QAction::triggered, [&]() {
    if (QMessageBox::question(this, "New configuration", WARNING) == QMessageBox::Yes)
    {
      config.erase_all();
      widget->clear();
    }
  });
  connect(ui->actionSave, &QAction::triggered, [&]() {
    for (const Log& log : config.get_logs())
    {
      std::cerr << std::endl << "log_" << log.get_id() << ":" << std::endl;
      for (Driver* const driver : log.get_drivers())
      {
        std::cerr << "  " << driver->get_type().at(0) << "_" << driver->print_id() << std::endl;
      }
    }
  });
  connect(ui->actionExit, &QAction::triggered, [&]() {
    if (QMessageBox::question(this, "Exit", WARNING) == QMessageBox::Yes)
    {
      close();
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
  connect(ui->actionLog, &QAction::triggered, [&]() {
    widget->set_selected_log(config.add_new_log());
    ui->statusBar->showMessage("Click to place log");
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
  for (const DefaultDriver& driver : config.get_default_drivers())
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
    const auto cit = std::find_if(config.get_default_drivers().cbegin(), config.get_default_drivers().cend(),
                                  [&name, &type](const DefaultDriver& driver)->bool {
                                    return driver.get_name() == name && driver.get_type() == type;
                                  });

    const auto crit = std::find_if(config.get_drivers().crbegin(), config.get_drivers().crend(),
                                   [&name, &type](const Driver& driver)->bool {
                                     return driver.get_name() == name && driver.get_type() == type;
                                   });
    const int id = crit != config.get_drivers().crend() ? crit->get_id() + 1 : 0;

    Driver new_driver(*cit, id);

    if (Dialog(new_driver, this).exec() == QDialog::Accepted)
    {
      widget->set_selected_driver(config.add_driver(new_driver));

      ui->statusBar->showMessage(QString::fromStdString("Click to place new " + type));
    }
  }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
  QApplication::sendEvent(widget, event);
}
