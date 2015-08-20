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

#include "scene.h"
#include "config.h"
#include "icon.h"
#include "dialog.h"

#include <QLabel>
#include <QDropEvent>
#include <QMimeData>
#include <QApplication>

Scene::Scene(Config& config,
             QWidget* parent) :
  QWidget(parent),
  config(config)
{
  setObjectName("Scene");
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setAcceptDrops(true);

  DeleteIcon* deleteIcon = new DeleteIcon(this);
  deleteIcon->move(20, 20);
  deleteIcon->hide();
}

ObjectIcon* Scene::add_object(std::shared_ptr<Object>& new_object, const QPoint& pos)
{
  ObjectIcon* icon = new_object->get_type() == "filter" ?
    new FilterIcon(new_object, this) :
    new ObjectIcon(new_object, this);
  add_icon(icon, pos);

  return icon;
}

ObjectStatementIcon* Scene::add_object_statement(std::shared_ptr<ObjectStatement>& new_object_statement, const QPoint& pos)
{
  ObjectStatementIcon* icon = new ObjectStatementIcon(new_object_statement, this);
  add_icon(icon, pos);

  return icon;
}

ObjectStatementIconCopy* Scene::add_object_statement_copy(std::shared_ptr<ObjectStatement>& new_object_statement, const QPoint& pos)
{
  ObjectStatementIconCopy* icon = new ObjectStatementIconCopy(new_object_statement, this);
  add_icon(icon, pos);

  return icon;
}

LogStatementIcon* Scene::add_log_statement(std::shared_ptr<LogStatement>& new_log_statement, const QPoint& pos)
{
  LogStatementIcon* icon = new LogStatementIcon(new_log_statement, this);
  add_icon(icon, pos);

  return icon;
}

void Scene::add_icon(Icon* icon, const QPoint& pos)
{
  icon->move(pos - QPoint(icon->width()/2, icon->height()/2));
  icon->lower();
  icon->show();

  connect(icon, &Icon::pressed, this, &Scene::pressed);
  connect(icon, &Icon::released, this, &Scene::released);

  updateGeometry();
}

void Scene::reset()
{
  for (Icon* icon : findChildren<Icon*>(QString(), Qt::FindDirectChildrenOnly))
  {
    icon->deleteLater();
  }

  updateGeometry();
}

/*
 * Responsible for making ObjectStatementIcon copies and removing icons from their parent StatementIcons.
 */
void Scene::pressed(Icon* icon)
{
  DeleteIcon* deleteIcon = findChild<DeleteIcon*>();
  deleteIcon->show();
  deleteIcon->lower();

  icon->raise();

  // make copies
  ObjectStatementIcon* statement_icon = dynamic_cast<ObjectStatementIcon*>(icon);
  if (statement_icon &&
    QApplication::queryKeyboardModifiers() == Qt::ControlModifier)
  {
    QPoint pos = icon->parentWidget()->mapTo(this, icon->pos()) + QPoint(statement_icon->width()/2, 20);
    std::shared_ptr<ObjectStatement> new_object_statement = statement_icon->get_object_statement();
    add_object_statement_copy(new_object_statement, pos);
    return;
  }

  // icons not belonging to this widget are the ones in StatementIcons
  if (icon->parent() != this)
  {
    QPoint pos = icon->parentWidget()->mapTo(this, icon->pos());
    StatementIcon* statement_icon = static_cast<StatementIcon*>(icon->parent()->parent());
    statement_icon->remove_icon(icon);
    icon->setParent(this);
    icon->move(pos);
    icon->show();
    icon->grabMouse();  // workaround, because sometimes when clicking on the edge of an icon, the icon gets stuck
  }
}

/*
 * Responsible for deleting icons and also for addig icons to statement icons.
 */
void Scene::released(Icon* icon)
{
  icon->releaseMouse();  // necessary, because of the workaround

  DeleteIcon* deleteIcon = findChild<DeleteIcon*>();
  deleteIcon->hide();

  // if icons inside StatementIcons break loose (hopefully never), this prevents them from getting deleted
  if (icon->parent() != this)
  {
    return;
  }

  // delete icons
  if (icon->geometry().intersects(deleteIcon->geometry()))
  {
    ObjectStatementIcon* statement_icon = dynamic_cast<ObjectStatementIcon*>(icon);
    if (statement_icon && !dynamic_cast<ObjectStatementIconCopy*>(icon))
    {
      delete_copies(statement_icon->get_object_statement()->get_id());
    }

    icon->deleteLater();

    update();
    return;
  }

  // add icons to the corresponding StatementIcon
  QPoint pos = icon->pos() + QPoint(icon->width()/2, icon->height()/2);
  StatementIcon* statement_icon = nullptr;

  if (dynamic_cast<ObjectIcon*>(icon))
  {
    statement_icon = select_nearest_object_statement_icon(pos);
  }
  else if (dynamic_cast<ObjectStatementIcon*>(icon))
  {
    statement_icon = select_nearest_log_statement_icon(pos);
  }

  if (statement_icon)
  {
    statement_icon->add_icon(icon);
  }
}

void Scene::delete_copies(const std::string& name)
{
  for (ObjectStatementIconCopy* copy : findChildren<ObjectStatementIconCopy*>())
  {
    if (copy->get_object_statement()->get_id() != name)
    {
      continue;
    }

    if (copy->parent() != this)
    {
      LogStatementIcon* icon = static_cast<LogStatementIcon*>(copy->parent()->parent());
      icon->remove_icon(copy);
    }

    copy->deleteLater();
  }
}

void Scene::leaveEvent(QEvent *)
{
  DeleteIcon* deleteIcon = findChild<DeleteIcon*>();
  deleteIcon->hide();
}

void Scene::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->source() == window() &&
    event->mimeData()->hasFormat("objecticon"))
  {
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

void Scene::dropEvent(QDropEvent* event)
{
  QByteArray itemData = event->mimeData()->data("objecticon");
  QDataStream dataStream(&itemData, QIODevice::ReadOnly);

  QString name, type;
  dataStream >> name >> type;

  const Object& default_object = config.get_default_object(name.toStdString(), type.toStdString());
  Object* new_object = default_object.clone();

  if (Dialog(*new_object, this).exec() == QDialog::Accepted)
  {
    std::shared_ptr<Object> object(new_object);
    add_object(object, event->pos());
  }
}

/*
 * The widget is inside a QScrollArea.
 * The size is calculated based on the positions of the icons.
 */
QSize Scene::sizeHint() const
{
  return childrenRegion().united(QRect(0, 0, 1, 1)).boundingRect().size();
}

ObjectStatementIcon* Scene::select_nearest_object_statement_icon(const QPoint& pos) const
{
  for (ObjectStatementIcon* icon : findChildren<ObjectStatementIcon*>())
  {
    QPoint converted_pos = icon->parentWidget()->mapFrom(this, pos);
    if (icon->geometry().contains(converted_pos))
    {
      return icon;
    }
  }

  return nullptr;
}

LogStatementIcon* Scene::select_nearest_log_statement_icon(const QPoint& pos) const
{
  for (LogStatementIcon* icon : findChildren<LogStatementIcon*>())
  {
    QPoint converted_pos = icon->parentWidget()->mapFrom(this, pos);
    if (icon->geometry().contains(converted_pos))
    {
      return icon;
    }
  }

  return nullptr;
}
