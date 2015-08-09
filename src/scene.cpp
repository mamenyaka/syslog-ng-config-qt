#include "scene.h"
#include "config.h"
#include "icon.h"
#include "dialog.h"

#include <QLabel>
#include <QIcon>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

Scene::Scene(Config& config,
             QWidget* parent) :
  QWidget(parent),
  config(config)
{
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMouseTracking(true);
  setAcceptDrops(true);

  deleteLabel = new QLabel(this);
  deleteLabel->setPixmap(QIcon::fromTheme("edit-delete-symbolic").pixmap(40, 40));
  deleteLabel->move(20, 20);
  deleteLabel->hide();
}

void Scene::add_object(std::shared_ptr<Object>& new_object, const QPoint& pos)
{
  ObjectIcon* icon = new ObjectIcon(new_object, this);
  icon->move(pos.x() - icon->width()/2, pos.y() - icon->height()/2);
  icon->raise();
  icon->show();

  clear();
}

void Scene::add_logpath(std::shared_ptr<Logpath>& new_logpath, const QPoint& pos)
{
  LogpathIcon* icon = new LogpathIcon(new_logpath, this);
  icon->move(pos.x() - icon->width()/2, pos.y() - icon->height()/2);
  icon->lower();
  icon->show();

  clear();
}

void Scene::move_icon(QWidget* icon, const QPoint& pos)
{
  const int x = pos.x() - icon->width()/2;
  const int y = pos.y() - icon->height()/2;
  icon->move(x, y);

  update();
  updateGeometry();
}

void Scene::clear()
{
  selected_object_icon = nullptr;
  selected_logpath_icon = nullptr;

  deleteLabel->hide();
  updateGeometry();
  update();
}

void Scene::reset()
{
  for (ObjectIcon* icon : findChildren<ObjectIcon*>())
  {
    delete icon;
  }

  for (LogpathIcon* icon : findChildren<LogpathIcon*>())
  {
    delete icon;
  }

  config.get_global_options()->get_options().restore_default_values();

  clear();
}

void Scene::mousePressEvent(QMouseEvent* event)
{
  selected_object_icon = select_nearest_object();
  selected_logpath_icon = select_nearest_logpath(event->pos());

  deleteLabel->lower();
  deleteLabel->show();

  if (selected_object_icon)
  {
    selected_object_icon->raise();

    if (copy_icon)
    {
      std::shared_ptr<Object> object = selected_object_icon->get_object();
      add_object(object, event->pos());
      selected_object_icon = dynamic_cast<ObjectIcon*>(children().last());
    }
  }
}

void Scene::mouseReleaseEvent(QMouseEvent* event)
{
  if (selected_object_icon)
  {
    if (selected_object_icon->parentWidget() != this)
    {
      return;
    }

    const QRect geom = selected_object_icon->geometry();
    LogpathIcon* icon = select_nearest_logpath(event->pos());

    if (icon && icon->geometry().intersects(geom))
    {
      icon->add_object(*selected_object_icon);
    }

    if (deleteLabel->geometry().intersects(geom))
    {
      delete selected_object_icon;
    }
  }
  else if (selected_logpath_icon)
  {
    const QRect geom = selected_logpath_icon->geometry();

    if (deleteLabel->geometry().intersects(geom))
    {
      delete selected_logpath_icon;
    }
  }

  clear();
}

void Scene::mouseMoveEvent(QMouseEvent* event)
{
  if (selected_object_icon)
  {
    if (selected_object_icon->parentWidget() != this)
    {
      LogpathIcon* icon = dynamic_cast<LogpathIcon*>(selected_object_icon->parentWidget()->parentWidget());
      if (icon)
      {
        icon->remove_object(*selected_object_icon);
        selected_object_icon->setParent(this);
        selected_object_icon->show();
      }
    }

    if (event->x() > 0 && event->y() > 0)
    {
      move_icon(selected_object_icon, event->pos());
    }
  }
  else if (selected_logpath_icon)
  {
    if (event->x() > 0 && event->y() > 0)
    {
      move_icon(selected_logpath_icon, event->pos());
    }
  }
}

void Scene::mouseDoubleClickEvent(QMouseEvent* event)
{
  ObjectIcon* object_icon = select_nearest_object();
  LogpathIcon* logpath_icon = select_nearest_logpath(event->pos());

  if (object_icon)
  {
    Dialog(*object_icon->get_object(), this).exec();
  }
  else if (logpath_icon)
  {
    Dialog(logpath_icon->get_logpath()->get_options(), this).exec();
  }
}

void Scene::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Control)
  {
    copy_icon = true;
  }
}

void Scene::keyReleaseEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Control)
  {
    copy_icon = false;
  }
}

void Scene::leaveEvent(QEvent *)
{
  clear();
  copy_icon = false;
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
    std::shared_ptr<Object> object = config.add_object(new_object);
    add_object(object, event->pos());
  }
}

QSize Scene::sizeHint() const
{
  return childrenRegion().united(QRect(0, 0, 1, 1)).boundingRect().size();
}

ObjectIcon* Scene::select_nearest_object() const
{
  for (ObjectIcon* icon : findChildren<ObjectIcon*>())
  {
    if (icon->underMouse())
    {
      return icon;
    }
  }

  return nullptr;
}

LogpathIcon* Scene::select_nearest_logpath(const QPoint& pos) const
{
  for (LogpathIcon* icon : findChildren<LogpathIcon*>())
  {
    if (icon->geometry().contains(pos))
    {
      return icon;
    }
  }

  return nullptr;
}
