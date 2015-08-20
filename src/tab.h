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

#ifndef TAB_H
#define TAB_H

#include <QWidget>

#include <memory>

class Object;
class Icon;

/*
 * A widget for holding icons for the default syslog-ng Objects.
 * The icons can be dragged.
 */
class Tab : public QWidget
{
  Q_OBJECT

public:
  explicit Tab(QWidget* parent = 0);

  /*
   * Create and insert DefaultObjectIcons into the widget's grid layout.
   */
  void setupObjects(const std::string& object_type, const std::vector< std::unique_ptr<const Object> >& default_objects);

private:
  void drag(Icon* icon);
};

#endif  // TAB_H
