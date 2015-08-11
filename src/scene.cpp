#include "scene.h"
#include "config.h"
#include "icon.h"
#include "dialog.h"

#include <QLabel>
#include <QIcon>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

Scene::Scene(Config& config,
             QWidget* parent) :
  QWidget(parent),
  config(config),
  deleteLabel(new QLabel(this))
{
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setAcceptDrops(true);

  deleteLabel->setPixmap(QIcon::fromTheme("edit-delete-symbolic").pixmap(40, 40));
  deleteLabel->move(20, 20);
  deleteLabel->hide();
}

void Scene::add_object(std::shared_ptr<Object>& new_object, const QPoint& pos)
{
  ObjectIcon* icon = new ObjectIcon(new_object, this);
  icon->move(pos - QPoint(icon->width()/2, icon->height()/2));
  icon->raise();
  icon->show();

  updateGeometry();
}

void Scene::add_logpath(std::shared_ptr<Logpath>& new_logpath, const QPoint& pos)
{
  LogpathIcon* icon = new LogpathIcon(new_logpath, this);
  icon->move(pos - QPoint(icon->width()/2, icon->height()/2));
  icon->lower();
  icon->show();

  updateGeometry();
}

void Scene::reset()
{
  for (Icon* icon : findChildren<Icon*>())
  {
    delete icon;
  }

  config.get_global_options()->restore_default_values();

  updateGeometry();
}

void Scene::mousePressEvent(QMouseEvent* event)
{
  Icon* icon = select_nearest_icon();
  if (!icon)
  {
    return;
  }

  deleteLabel->show();
  deleteLabel->lower();

  ObjectIcon* object_icon = select_nearest_object();
  if (!object_icon)
  {
    return;
  }

  if (object_icon->parentWidget() != this)
  {
    LogpathIcon* icon = static_cast<LogpathIcon*>(object_icon->parent()->parent());
    icon->remove_object(*object_icon);
    object_icon->setParent(this);
    object_icon->show();
    object_icon->move(event->pos() - QPoint(icon->width()/2, icon->height()/2));
  }

  if (QApplication::queryKeyboardModifiers() == Qt::ControlModifier)
  {
    std::shared_ptr<Object> object = object_icon->get_object();
    add_object(object, object_icon->geometry().center());
  }

  object_icon->raise();
}

void Scene::mouseReleaseEvent(QMouseEvent* event)
{
  Icon* icon = select_nearest_icon();
  if (icon &&
    icon->geometry().intersects(deleteLabel->geometry()))
  {
    delete icon;
    update();
  }

  ObjectIcon* object_icon = select_nearest_object();
  LogpathIcon* logpath_icon = select_nearest_logpath(event->pos());
  if (object_icon && logpath_icon &&
    logpath_icon->geometry().intersects(object_icon->geometry()))
  {
    logpath_icon ->add_object(*object_icon);
  }

  deleteLabel->hide();
}

void Scene::leaveEvent(QEvent *)
{
  deleteLabel->hide();
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

Icon* Scene::select_nearest_icon() const
{
  for (Icon* icon : findChildren<Icon*>())
  {
    if (icon->underMouse())
    {
      return icon;
    }
  }

  return nullptr;
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
