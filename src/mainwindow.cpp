#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "scene.h"
#include "dialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QCloseEvent>

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

  ui->actionLogpath->trigger();
}

MainWindow::~MainWindow()
{
  delete scene;
  delete config;
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (last_saved == QString::fromStdString(config->to_string()))
  {
    event->accept();
  }
  else
  {
    if (QMessageBox::question(this, "Exit",
      "All unsaved data will be lost! Are you sure?") == QMessageBox::Yes)
    {
      event->accept();
    }
    else
    {
      event->ignore();
    }
  }
}

void MainWindow::setupConnections()
{
  connect(ui->actionNew, &QAction::triggered, [&]() {
    if (QMessageBox::question(this, "New configuration",
      "Current configuration will be lost! Are you sure?") == QMessageBox::Yes)
    {
      scene->reset();
      config->get_global_options()->restore_default_values();
      ui->actionLogpath->trigger();
    }
  });

  connect(ui->actionSave, &QAction::triggered, [&]() {
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save config"), QDir::homePath());
    QFile file(file_name);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      return;
    }

    last_saved = QString::fromStdString(config->to_string());

    QTextStream out(&file);
    out << last_saved;

    file.close();
  });

  connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);


  connect(ui->actionOptions, &QAction::triggered, [&]() {
    Dialog(config->get_global_options()->get_options(), this).exec();
  });

  connect(ui->actionLogpath, &QAction::triggered, [&]() {
    const Options& log_options = static_cast<const Options&>(config->get_default_object("log", "options"));
    Logpath* new_logpath = new Logpath(log_options);

    std::shared_ptr<Logpath> logpath = config->add_logpath(new_logpath);
    scene->add_logpath(logpath, QPoint(150, 30));
  });


  connect(ui->actionAbout, &QAction::triggered, [&]() {
    QMessageBox::aboutQt(this);
  });
}
