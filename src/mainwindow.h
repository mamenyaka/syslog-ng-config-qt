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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"

#include <QMainWindow>

namespace Ui {
  class MainWindow;
}
class Scene;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Ui::MainWindow* ui;
  Scene* scene;

  Config config;

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent* event);

private:
  void setupConnections();

  // non copyable
  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;

  // used at application exit to warn the user if there are changes to the last saved config
  QString last_saved_config;
};

#endif // MAINWINDOW_H
