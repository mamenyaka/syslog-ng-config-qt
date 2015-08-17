#include "tab.h"
#include "object.h"
#include "icon.h"

#include <QGridLayout>
#include <QDrag>
#include <QMimeData>

Tab::Tab(QWidget* parent) :
  QWidget(parent)
{}

void Tab::setupObjects(const std::string& type, const std::vector< std::unique_ptr<const Object> >& default_objects)
{
  QGridLayout* mainLayout = static_cast<QGridLayout*>(layout());
  mainLayout->setSpacing(10);

  int row = 0, col = 0;
  for (const std::unique_ptr<const Object>& default_object : default_objects)
  {
    if (default_object->get_type() == type)
    {
      std::shared_ptr<Object> object(const_cast<Object*>(default_object.get()), [](const Object *){});
      ObjectIcon* icon = new ObjectIcon(object);
      mainLayout->addWidget(icon, row, col++);

      connect(icon, &Icon::pressed, this, &Tab::drag);

      if (col == 2)
      {
        col = 0;
        row++;
      }
    }
  }
}

void Tab::drag(Icon* icon)
{
  std::shared_ptr<Object>& object = static_cast<ObjectIcon*>(icon)->get_object();

  QByteArray itemData;
  QDataStream dataStream(&itemData, QIODevice::WriteOnly);
  dataStream << QString::fromStdString(object->get_name()) << QString::fromStdString(object->get_type());

  QMimeData* mimeData = new QMimeData;
  mimeData->setData("objecticon", itemData);

  QPixmap pixmap(icon->size());
  pixmap.fill(Qt::transparent);
  icon->render(&pixmap);

  QDrag *drag = new QDrag(window());
  drag->setMimeData(mimeData);
  drag->setPixmap(pixmap);
  drag->setHotSpot(pixmap.rect().center());

  drag->exec();
}
