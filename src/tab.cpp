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
