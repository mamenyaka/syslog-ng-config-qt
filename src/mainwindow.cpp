#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "scene.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>

#include <fstream>

#define WARNING "Warning: All unsaved data will be lost! Are you sure?"

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  scene(new Scene(config)),
  ui(new Ui::MainWindow)
{
  installEventFilter(this);

  ui->setupUi(this);

  ui->sourceWidget->setupDrivers("source", config.get_default_drivers());
  ui->destinationWidget->setupDrivers("destination", config.get_default_drivers());
  ui->filterWidget->setupDrivers("filter", config.get_default_drivers());
  ui->templateWidget->setupDrivers("template", config.get_default_drivers());
  ui->rewriteWidget->setupDrivers("rewrite", config.get_default_drivers());
  ui->parserWidget->setupDrivers("parser", config.get_default_drivers());

  ui->sceneScrollArea->setWidget(scene);

  setupConnections();
}

MainWindow::~MainWindow()
{
  delete scene;
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (QMessageBox::question(this, "Exit", WARNING) == QMessageBox::Yes)
  {
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

bool MainWindow::eventFilter(QObject*, QEvent* event)
{
  if (event->type() == QEvent::KeyPress ||
    event->type() == QEvent::KeyRelease)
  {
    QApplication::sendEvent(scene, event);
  }

  return false;
}

void MainWindow::setupConnections()
{
  connect(ui->actionNew, &QAction::triggered, [&]() {
    if (QMessageBox::question(this, "New configuration", WARNING) == QMessageBox::Yes)
    {
      scene->reset();
    }
  });

  connect(ui->actionSave, &QAction::triggered, [&]() {
    const std::string file_name = QFileDialog::getSaveFileName(this, tr("Save config"), QDir::homePath()).toStdString();
    std::ofstream out(file_name);
    out << config;
  });

  connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);


  connect(ui->actionOptions, &QAction::triggered, [&]() {
    Dialog(config.get_global_options()->get_options(), this).exec();
  });

  connect(ui->actionLog, &QAction::triggered, [&]() {
    const Driver& log_options = config.get_default_driver("log", "options");
    std::unique_ptr<Log> new_log = std::make_unique<Log>(static_cast<const Options&>(log_options));

    std::shared_ptr<Log> log = config.add_log(new_log);
    scene->add_log(log, QPoint(100, 100));
  });


  connect(ui->actionAbout, &QAction::triggered, [&]() {
    QMessageBox::aboutQt(this);
  });
}
