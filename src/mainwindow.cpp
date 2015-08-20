/*
 * Copyright (C) 2015 Andras Mamenyak
 *
 * This file is part of syslog-ng-config-qt.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scene.h"
#include "dialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextStream>
#include <QCloseEvent>
#include <QProcess>

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  scene(new Scene(config))
{
  ui->setupUi(this);

  ui->actionNew->setShortcut(QKeySequence::New);
  ui->actionSave->setShortcut(QKeySequence::Save);
  ui->actionQuit->setShortcut(QKeySequence::Quit);

  ui->sourceWidget->setupObjects("source", config.get_default_objects());
  ui->destinationWidget->setupObjects("destination", config.get_default_objects());
  ui->filterWidget->setupObjects("filter", config.get_default_objects());
  ui->templateWidget->setupObjects("template", config.get_default_objects());
  ui->rewriteWidget->setupObjects("rewrite", config.get_default_objects());
  ui->parserWidget->setupObjects("parser", config.get_default_objects());

  ui->sceneScrollArea->setWidget(scene);

  setupConnections();

  ui->actionLogStatement->trigger();  // the Scene widget has a LogStatement by default
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  // Check if the configuration has changed since the last save
  if (last_saved_config == QString::fromStdString(config.to_string()))
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
      for (std::unique_ptr<Option>& option : config.get_global_options().get_options())
      {
        option->restore_default();
      }

      scene->reset();
      ui->actionLogStatement->trigger();
    }
  });

  connect(ui->actionSave, &QAction::triggered, [&]() {
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save syslog-ng configuration"), QDir::homePath());
    if (file_name.isEmpty())
    {
      return;
    }

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      return;
    }

    last_saved_config = QString::fromStdString(config.to_string());

    QTextStream out(&file);
    out << last_saved_config;

    file.close();

    // check config file syntax with the "syslog-ng -s -f FILE" command
    QProcess* process = new QProcess(this);
    process->start("syslog-ng", { "-s", "-f", file_name });

    connect(process, static_cast<void(QProcess::*)(int)>(&QProcess::finished), [this, process](int ) {
      QByteArray array = process->readAllStandardError();
      if (!array.isEmpty())
      {
        QMessageBox::warning(this, "Configuration file syntax", QString(array));
      }
    });
  });

  connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);


  connect(ui->actionOptions, &QAction::triggered, [&]() {
    Dialog(config.get_global_options(), this).exec();
  });

  connect(ui->actionLogStatement, &QAction::triggered, [&]() {
    const Options& log_options = static_cast<const Options&>(config.get_default_object("log", "options"));
    LogStatement* new_log_statement = new LogStatement(log_options);

    std::shared_ptr<LogStatement> log_statement = config.add_log_statement(new_log_statement);
    scene->add_log_statement(log_statement, QPoint(200, 30));
  });

  connect(ui->actionObjectStatement, &QAction::triggered, [&]() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("Object statement"), tr("ID:"), QLineEdit::Normal, QString(), &ok);
    if (!ok)
    {
      return;
    }

    if (text.isEmpty())
    {
      QMessageBox::warning(this, "Warning", "ID cannot be empty!");
      return;
    }

    std::string id = text.toStdString();
    for (const std::unique_ptr<ObjectStatement>& statement : config.get_object_statements())
    {
      if (statement->get_id() == id)
      {
        QMessageBox::warning(this, "Warning", "ID already in use!");
        return;
      }
    }

    ObjectStatement* new_object_statement = new ObjectStatement(id);

    std::shared_ptr<ObjectStatement> object_statement = config.add_object_statement(new_object_statement);
    scene->add_object_statement(object_statement, QPoint(50, 150));
  });

  connect(ui->actionAbout, &QAction::triggered, [&]() {
    QString about = ""
    "Version 1.0\n\n"
    "A standalone graphical tool for creating syslog-ng configuration files.\n\n"
    "Copyright (C) 2015\n"
    "Created by: Andras Mamenyak";
    QMessageBox::about(this, "About", about);
  });
}
