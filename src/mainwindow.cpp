#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "scene.h"
#include "dialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>

#include <fstream>

#define WARNING "Warning: All unsaved data will be lost! Are you sure?"

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  config(new Config),
  scene(new Scene(*config))
{
  ui->setupUi(this);

  ui->sourceWidget->setupObjects("source", config->get_default_objects());
  ui->destinationWidget->setupObjects("destination", config->get_default_objects());
  ui->filterWidget->setupObjects("filter", config->get_default_objects());
  ui->templateWidget->setupObjects("template", config->get_default_objects());
  ui->rewriteWidget->setupObjects("rewrite", config->get_default_objects());
  ui->parserWidget->setupObjects("parser", config->get_default_objects());

  ui->sceneScrollArea->setWidget(scene);

  installEventFilter(this);

  setupConnections();
}

MainWindow::~MainWindow()
{
  delete scene;
  delete config;
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
    out << *config;
  });

  connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);


  connect(ui->actionOptions, &QAction::triggered, [&]() {
    Dialog(config->get_global_options()->get_options(), this).exec();
  });

  connect(ui->actionLogpath, &QAction::triggered, [&]() {
    const Options& log_options = static_cast<const Options&>(config->get_default_object("log", "options"));
    Logpath* new_logpath = new Logpath(log_options);

    std::shared_ptr<Logpath> logpath = config->add_logpath(new_logpath);
    scene->add_logpath(logpath, QPoint(100, 100));
  });


  connect(ui->actionAbout, &QAction::triggered, [&]() {
    QMessageBox::aboutQt(this);
  });
}
