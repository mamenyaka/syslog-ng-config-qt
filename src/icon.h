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

#ifndef ICON_H
#define ICON_H

#include <QWidget>

#include <memory>

class Object;
class ObjectStatement;
class LogStatement;

class Icon : public QWidget
{
  Q_OBJECT

  bool pressed_but_not_moved = false;

public:
  explicit Icon(QWidget* parent = 0);
  virtual ~Icon() {}

signals:
  void pressed(Icon* icon);
  void released(Icon* icon);

protected:
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent* event);

  virtual void mouseDoubleClickEvent(QMouseEvent *) = 0;
};

class ObjectIcon : public Icon
{
  Q_OBJECT

  std::shared_ptr<Object> object;

public:
  explicit ObjectIcon(std::shared_ptr<Object>& object,
                      QWidget* parent = 0);

  std::shared_ptr<Object>& get_object();

protected:
  void mouseDoubleClickEvent(QMouseEvent *);
};

class FilterIcon : public ObjectIcon
{
public:
  explicit FilterIcon(std::shared_ptr<Object>& object,
                      QWidget* parent = 0);
};

class StatementIcon : public Icon
{
  Q_OBJECT

public:
  explicit StatementIcon(QWidget* parent = 0);

  virtual void add_icon(Icon* icon);
  virtual void remove_icon(Icon* icon);

private:
  int get_index(Icon* icon);
};

class ObjectStatementIcon : public StatementIcon
{
  Q_OBJECT

  std::shared_ptr<ObjectStatement> object_statement;

public:
  explicit ObjectStatementIcon(std::shared_ptr<ObjectStatement>& object_statement,
                               QWidget* parent = 0);

  std::shared_ptr<ObjectStatement>& get_object_statement();

  void add_icon(Icon* icon);
  void remove_icon(Icon* icon);

protected:
  void mouseDoubleClickEvent(QMouseEvent *) {}
};

class ObjectStatementIconCopy : public ObjectStatementIcon
{
public:
  explicit ObjectStatementIconCopy(std::shared_ptr<ObjectStatement>& object_statement,
                                   QWidget* parent = 0);

  void add_icon(Icon *) {}
};

class LogStatementIcon : public StatementIcon
{
  Q_OBJECT

  std::shared_ptr<LogStatement> log_statement;

public:
  explicit LogStatementIcon(std::shared_ptr<LogStatement>& log_statement,
                            QWidget* parent = 0);

  void add_icon(Icon* icon);
  void remove_icon(Icon* icon);

protected:
  void mouseDoubleClickEvent(QMouseEvent *);
};


class DeleteIcon : public QWidget
{
  Q_OBJECT

public:
  explicit DeleteIcon(QWidget* parent = 0);
};

#endif  // ICON_H
