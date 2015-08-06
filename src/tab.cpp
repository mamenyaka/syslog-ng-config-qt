#include "tab.h"
#include "driver.h"
#include "icon.h"

#include <QLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

Tab::Tab(QWidget* parent) :
  QWidget(parent)
{}

void Tab::setupDrivers(const std::string& type, const std::vector< std::unique_ptr< const Driver > >& default_drivers)
{
  for (const std::unique_ptr<const Driver>& default_driver : default_drivers)
  {
    if (default_driver->get_type() == type)
    {
      std::shared_ptr<Driver> driver(default_driver->clone());
      DriverIcon* icon = new DriverIcon(driver);
      layout()->addWidget(icon);
    }
  }
}

void Tab::mousePressEvent(QMouseEvent *)
{
  DriverIcon* icon = select_nearest_driver();

  if (!icon)
  {
    return;
  }

  std::shared_ptr<Driver>& driver = icon->get_driver();

  QByteArray itemData;
  QDataStream dataStream(&itemData, QIODevice::WriteOnly);
  dataStream << QString::fromStdString(driver->get_name()) << QString::fromStdString(driver->get_type());

  QMimeData* mimeData = new QMimeData;
  mimeData->setData("drivericon", itemData);

  const QPixmap& pixmap = icon->get_pixmap();

  QDrag *drag = new QDrag(window());
  drag->setMimeData(mimeData);
  drag->setPixmap(pixmap);
  drag->setHotSpot(pixmap.rect().center());

  drag->exec();
}

DriverIcon* Tab::select_nearest_driver() const
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
