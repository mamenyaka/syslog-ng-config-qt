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

#ifndef SCENE_H
#define SCENE_H

#include <QWidget>

#include <memory>

class Config;
class Object;
class ObjectStatement;
class LogStatement;
class Icon;
class ObjectIcon;
class ObjectStatementIcon;
class ObjectStatementIconCopy;
class LogStatementIcon;

/*
 * Widget for displaying all the icons that make up the config.
 */
class Scene : public QWidget
{
  Q_OBJECT

  Config& config;

public:
  explicit Scene(Config& config,
                 QWidget* parent = 0);

  ObjectIcon* add_object(std::shared_ptr<Object>& new_object, const QPoint& pos);
  ObjectStatementIcon* add_object_statement(std::shared_ptr<ObjectStatement>& new_object_statement, const QPoint& pos);
  ObjectStatementIconCopy* add_object_statement_copy(std::shared_ptr<ObjectStatement>& new_object_statement, const QPoint& pos);
  LogStatementIcon* add_log_statement(std::shared_ptr<LogStatement>& new_log_statement, const QPoint& pos);

  /*
   * Connected to File > New, this method deletes all icons.
   */
  void reset();

protected:
  void leaveEvent(QEvent *);

  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);

  QSize sizeHint() const;

private:
  /*
   * Connected to each Icon's press signal.
   */
  void pressed(Icon* icon);

  /*
   * Connected to each Icon's release signal.
   */
  void released(Icon* icon);

  /*
   * Adds the newly created icon to the widget and sets up it's connections.
   */
  void add_icon(Icon* icon, const QPoint& pos);

  /*
   * When deleting an ObjectStatementIcon, make sure that all it's copies are also deleted.
   */
  void delete_copies(const std::string& id);

  ObjectStatementIcon* select_nearest_object_statement_icon(const QPoint& pos) const;
  LogStatementIcon* select_nearest_log_statement_icon(const QPoint& pos) const;

  // non copyable
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;
};

#endif  // SCENE_H
