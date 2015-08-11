#include "scene.h"
#include "config.h"
#include "icon.h"
#include "dialog.h"

#include <QLabel>
#include <QDropEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

Scene::Scene(Config& config,
             QWidget* parent) :
  QWidget(parent),
  config(config)
{
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setAcceptDrops(true);

  DeleteIcon* deleteIcon = new DeleteIcon(this);
  deleteIcon->move(20, 20);
  deleteIcon->hide();
}

void Scene::add_object(std::shared_ptr<Object>& new_object, const QPoint& pos)
{
  ObjectIcon* icon = new ObjectIcon(new_object, this);
  add_icon(icon, pos);
}

void Scene::add_logpath(std::shared_ptr<Logpath>& new_logpath, const QPoint& pos)
{
  LogpathIcon* icon = new LogpathIcon(new_logpath, this);
  add_icon(icon, pos);
}

void Scene::add_icon(Icon* icon, const QPoint& pos)
{
  icon->move(pos - QPoint(icon->width()/2, icon->height()/2));
  icon->lower();
  icon->show();

  updateGeometry();

  connect(icon, &Icon::pressed, this, &Scene::pressed);
  connect(icon, &Icon::released, this, &Scene::released);
}

void Scene::pressed(Icon* icon)
{
  DeleteIcon* deleteIcon = findChild<DeleteIcon*>();
  deleteIcon->show();
  deleteIcon->lower();

  ObjectIcon* object_icon = dynamic_cast<ObjectIcon*>(icon);
  if (!object_icon)
  {
    return;
  }

  if (object_icon->parentWidget() != this)
  {
    QPoint pos = object_icon->parentWidget()->mapTo(this, object_icon->pos());
    LogpathIcon* icon = static_cast<LogpathIcon*>(object_icon->parent()->parent());
    icon->remove_object(*object_icon);
    object_icon->setParent(this);
    object_icon->show();
    object_icon->move(pos);
  }

  if (QApplication::queryKeyboardModifiers() == Qt::ControlModifier)
  {
    std::shared_ptr<Object> object = object_icon->get_object();
    add_object(object, object_icon->geometry().center());
  }

  object_icon->raise();
}

void Scene::released(Icon* icon)
{
  DeleteIcon* deleteIcon = findChild<DeleteIcon*>();
  deleteIcon->hide();

  if (icon->geometry().intersects(deleteIcon->geometry()))
  {
    delete icon;

    update();
    return;
  }

  ObjectIcon* object_icon = dynamic_cast<ObjectIcon*>(icon);
  if (!object_icon)
  {
    return;
  }

  LogpathIcon* logpath_icon = select_nearest_logpath(icon->pos());
  if (!logpath_icon)
  {
    return;
  }

  if (logpath_icon->geometry().intersects(object_icon->geometry()))
  {
    logpath_icon ->add_object(*object_icon);
  }
}

void Scene::reset()
{
  for (Icon* icon : findChildren<Icon*>(QString(), Qt::FindDirectChildrenOnly))
  {
    delete icon;
  }

  config.get_global_options()->restore_default_values();

  updateGeometry();
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
    std::shared_ptr<Object> object = config.add_object(new_object);
    add_object(object, event->pos());
  }
}

QSize Scene::sizeHint() const
{
  return childrenRegion().united(QRect(0, 0, 1, 1)).boundingRect().size();
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
