#include "tab.h"
#include "object.h"
#include "icon.h"

#include <QLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

Tab::Tab(QWidget* parent) :
  QWidget(parent)
{}

void Tab::setupObjects(const std::string& type, const std::vector< std::unique_ptr<const Object> >& default_objects)
{
  for (const std::unique_ptr<const Object>& default_object : default_objects)
  {
    if (default_object->get_type() == type)
    {
      std::shared_ptr<Object> object(default_object->clone());
      ObjectIcon* icon = new ObjectIcon(object);
      layout()->addWidget(icon);
    }
  }
}

void Tab::mousePressEvent(QMouseEvent *)
{
  ObjectIcon* icon = select_nearest_object();

  if (!icon)
  {
    return;
  }

  std::shared_ptr<Object>& object = icon->get_object();

  QByteArray itemData;
  QDataStream dataStream(&itemData, QIODevice::WriteOnly);
  dataStream << QString::fromStdString(object->get_name()) << QString::fromStdString(object->get_type());

  QMimeData* mimeData = new QMimeData;
  mimeData->setData("objecticon", itemData);

  const QPixmap pixmap = icon->palette().brush(QPalette::Background).texture();

  QDrag *drag = new QDrag(window());
  drag->setMimeData(mimeData);
  drag->setPixmap(pixmap);
  drag->setHotSpot(pixmap.rect().center());

  drag->exec();
}

ObjectIcon* Tab::select_nearest_object() const
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
