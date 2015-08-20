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

/*
 * Base abstract class for all icons, the graphical building blocks for the syslog-ng config.
 */
class Icon : public QWidget
{
  Q_OBJECT

  // controls when the pressed signal is emitted
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

/*
 * Icon for Objects.
 * Different types represented by different shapes.
 * Created with drag&drop from the Tab widget.
 */
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

/*
 * Tab widget icons with double click disabled.
 */
class DefaultObjectIcon : public ObjectIcon
{
public:
  explicit DefaultObjectIcon(std::shared_ptr<Object>& object,
                             QWidget* parent = 0);

protected:
  void mouseDoubleClickEvent(QMouseEvent *) {}
};

/*
 * Special icon for Filters.
 */
class FilterIcon : public ObjectIcon
{
public:
  explicit FilterIcon(std::shared_ptr<Object>& object,
                      QWidget* parent = 0);
};

/*
 * Base class for statement icons (Object or Log)
 */
class StatementIcon : public Icon
{
  Q_OBJECT

public:
  explicit StatementIcon(QWidget* parent = 0);

  virtual void add_icon(Icon* icon);
  virtual void remove_icon(Icon* icon);

private:
  /*
   * Calculate where to insert the new icon based on its position relative to the others.
   */
  int get_index(Icon* icon);
};

/*
 * Icon for ObjectStatements.
 * Holds ObjectIcons.
 */
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

/*
 * Special icon created when an ObjectStatementIcon is copied.
 */
class ObjectStatementIconCopy : public ObjectStatementIcon
{
public:
  explicit ObjectStatementIconCopy(std::shared_ptr<ObjectStatement>& object_statement,
                                   QWidget* parent = 0);

  void add_icon(Icon *) {}
};

/*
 * Icon for LogStatements.
 * Holds ObjectStatementIcons.
 */
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


/*
 * Special icon used in the Scene widget for deleting icons.
 */
class DeleteIcon : public QWidget
{
  Q_OBJECT

public:
  explicit DeleteIcon(QWidget* parent = 0);
};

#endif  // ICON_H
