#include "scene.h"
#include "dialog.h"
#include "icon.h"
#include "config.h"

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

void Scene::add_driver(Driver& driver, const QPoint& pos)
{
  DriverIcon* icon = new DriverIcon(driver, this);
  icon->move(pos.x() - icon->width()/2, pos.y() - icon->height()/2);
  icon->raise();
  icon->show();

  updateGeometry();
}

void Scene::add_log(Log& log)
{
  LogIcon* icon = new LogIcon(log, this);
  const unsigned int x = 100 + (findChildren<LogIcon*>().size() - 1)*(icon->width() + 50);
  icon->move(x, 10);
  icon->lower();
  icon->show();

  updateGeometry();
}

void Scene::move_driver(const QPoint& pos)
{
  const int x = pos.x() - selected_icon->width()/2;
  const int y = pos.y() - selected_icon->height()/2;
  selected_icon->move(x, y);

  update();
  updateGeometry();
}

void Scene::delete_driver()
{
  Driver& driver = selected_icon->get_driver();
  const std::string id_name = driver.get_id_name();

  if (get_driver_count(id_name) == 1)
  {
    config.delete_driver(driver);
  }

  delete selected_icon;
}

void Scene::clear()
{
  selected_icon = nullptr;

  deleteLabel->hide();
  updateGeometry();
}

void Scene::reset()
{
  for (DriverIcon* icon : findChildren<DriverIcon*>())
  {
    delete icon;
  }

  for (LogIcon* icon : findChildren<LogIcon*>())
  {
    delete icon;
  }

  config.erase_all();

  clear();
}

void Scene::mousePressEvent(QMouseEvent* event)
{
  selected_icon = select_nearest_driver();

  if (!selected_icon)
  {
    return;
  }

  deleteLabel->show();

  if (copy_icon)
  {
    add_driver(selected_icon->get_driver(), event->pos());
    selected_icon = dynamic_cast<DriverIcon*>(children().last());
  }
}

void Scene::mouseReleaseEvent(QMouseEvent* event)
{
  if (!selected_icon || selected_icon->parentWidget() != this)
  {
    clear();
    return;
  }

  const QRect geom = selected_icon->geometry();
  LogIcon* icon = select_nearest_log(event->pos());

  if (icon && icon->geometry().intersects(geom))
  {
    icon->add_driver(*selected_icon);
  }

  if (deleteLabel->geometry().intersects(geom))
  {
    delete_driver();
  }

  clear();
}

void Scene::mouseMoveEvent(QMouseEvent* event)
{
  if (!selected_icon)
  {
    return;
  }

  if (selected_icon->parentWidget() != this)
  {
    LogIcon* icon = dynamic_cast<LogIcon*>(selected_icon->parentWidget()->parentWidget());
    if (icon)
    {
      icon->remove_driver(*selected_icon);
      selected_icon->setParent(this);
      selected_icon->show();
    }
  }

  if (event->x() > 0 && event->y() > 0)
  {
    move_driver(event->pos());
  }
}

void Scene::mouseDoubleClickEvent(QMouseEvent *)
{
  DriverIcon* icon = select_nearest_driver();

  if (!icon)
  {
    return;
  }

  Dialog(icon->get_driver(), this).exec();
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

void Scene::leaveEvent(QEvent*)
{
  clear();
  copy_icon = false;
}

void Scene::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->source() == window() &&
    event->mimeData()->hasFormat("drivericon"))
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
  QByteArray itemData = event->mimeData()->data("drivericon");
  QDataStream dataStream(&itemData, QIODevice::ReadOnly);

  QString name;
  int type;
  dataStream >> name >> type;

  const Driver& default_driver = config.get_default_driver(name.toStdString(), (DriverType) type);
  Driver new_driver(default_driver);

  if (Dialog(new_driver, this).exec() == QDialog::Accepted)
  {
    Driver& driver = config.add_driver(new_driver);
    add_driver(driver, event->pos());
  }
}

QSize Scene::sizeHint() const
{
  return childrenRegion().united(QRect(0, 0, 1, 1)).boundingRect().size();
}

DriverIcon* Scene::select_nearest_driver() const
{
  for (DriverIcon* icon : findChildren<DriverIcon*>())
  {
    if (icon->underMouse())
    {
      return icon;
    }
  }

  return nullptr;
}

LogIcon* Scene::select_nearest_log(const QPoint& pos) const
{
  for (LogIcon* icon : findChildren<LogIcon*>())
  {
    if (icon->geometry().contains(pos))
    {
      return icon;
    }
  }

  return nullptr;
}

unsigned int Scene::get_driver_count(const std::string& id_name) const
{
  QList<DriverIcon*> icons = findChildren<DriverIcon*>();

  return std::count_if(icons.begin(), icons.end(),
                       [&id_name](DriverIcon* icon)->bool {
                         Driver& driver = icon->get_driver();
                         return driver.get_id_name() == id_name;
                       });
}
