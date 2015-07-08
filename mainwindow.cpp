#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widget.h"
#include "config.h"
#include "dialog.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

#include <fstream>

#define WARNING "Warning: All unsaved data will be lost! Are you sure?"

std::ostream& operator<<(std::ostream& os, const Config& config);

MainWindow::MainWindow(Config& config, QWidget* parent) :
  QMainWindow(parent),
  config(config)
{
  ui = new Ui::MainWindow,
  ui->setupUi(this);

  widget = new Widget(config);
  setCentralWidget(widget);

  installEventFilter(this);

  connect(widget, &Widget::update_driver, [&](Driver& driver) {
    Dialog(driver, this).exec();
  });
  connect(widget, &Widget::update_statusbar, [&](const QString message) {
    if (!message.isEmpty())
    {
      ui->statusBar->showMessage(message);
    }
    else
    {
      ui->statusBar->clearMessage();
    }
  });

  connect(ui->actionNew, &QAction::triggered, [&]() {
    if (QMessageBox::question(this, "New configuration", WARNING) == QMessageBox::Yes)
    {
      config.erase_all();
      widget->clear();
    }
  });
  connect(ui->actionExit, &QAction::triggered, [&]() {
    if (QMessageBox::question(this, "Exit", WARNING) == QMessageBox::Yes)
    {
      close();
    }
  });
  connect(ui->actionSave, &QAction::triggered, [&]() {
    const std::string file_name = QFileDialog::getSaveFileName(this, tr("Save config"), QDir::homePath()).toStdString();
    std::ofstream out(file_name);
    out << config;
  });
  connect(ui->actionAbout, &QAction::triggered, [&]() {
    QMessageBox::aboutQt(this);
  });

  connect(ui->actionSource, &QAction::triggered, [&]() {
    driver_select_dialog("source");
  });
  connect(ui->actionDestination, &QAction::triggered, [&]() {
    driver_select_dialog("destination");
  });
  connect(ui->actionLog, &QAction::triggered, [&]() {
    config.add_log(Log());
    widget->add_log(&config.get_logs().back());
    ui->statusBar->showMessage("Click to place new log");
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
    create_new_driver(name, type);
  }
}

void MainWindow::create_new_driver(const std::string& name, const std::string& type)
{
  const auto cit = std::find_if(config.get_default_drivers().cbegin(), config.get_default_drivers().cend(),
                                [&name, &type](const DefaultDriver& driver)->bool {
                                  return driver.get_name() == name && driver.get_type() == type;
                                });

  const int id = std::count_if(config.get_drivers().cbegin(), config.get_drivers().cend(),
                               [&name, &type](const Driver& driver)->bool {
                                 return driver.get_name() == name && driver.get_type() == type;
                              });

  Driver new_driver(*cit, id);

  if (Dialog(new_driver, this).exec() == QDialog::Accepted)
  {
    config.add_driver(new_driver);
    widget->add_driver(&config.get_drivers().back());
    ui->statusBar->showMessage(QString::fromStdString("Click to place new " + type));
  }
}

bool MainWindow::eventFilter(QObject *, QEvent* event)
{
  if (event->type() == QEvent::KeyPress)
  {
    QApplication::sendEvent(widget, event);
  }

  if (event->type() == QEvent::StatusTip)
  {
    return true;
  }

  return false;
}

std::ostream& operator<<(std::ostream& os, const Config& config)
{
  for (const Driver& driver : config.get_drivers())
  {
    os << driver.to_string() << std::endl;
  }

  for (const Log& log : config.get_logs())
  {
    os << log.to_string() << std::endl;
  }

  return os;
}
