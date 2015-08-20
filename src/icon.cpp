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

#include "icon.h"
#include "object.h"
#include "dialog.h"

#include <QMouseEvent>
#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QBoxLayout>
#include <QIcon>

#define ICON_SIZE 80

Icon::Icon(QWidget* parent) :
  QWidget(parent)
{}

void Icon::mousePressEvent(QMouseEvent *)
{
  pressed_but_not_moved = true;
}

void Icon::mouseReleaseEvent(QMouseEvent *)
{
  emit released(this);
}

void Icon::mouseMoveEvent(QMouseEvent* event)
{
  // pressed signal is only emitted after the icon has moved to avoid double click issues
  if (pressed_but_not_moved)
  {
    emit pressed(this);
    pressed_but_not_moved = false;
    return;
  }

  QPoint pos = event->globalPos() - QPoint(width()/2, height()/2);
  QPoint new_pos = parentWidget()->mapFromGlobal(pos);

  move(std::max(0, new_pos.x()), std::max(0, new_pos.y()));

  parentWidget()->update();
  parentWidget()->updateGeometry();
}


ObjectIcon::ObjectIcon(std::shared_ptr<Object>& object,
                       QWidget* parent) :
  Icon(parent),
  object(std::move(object))
{
  setFixedSize(ICON_SIZE, ICON_SIZE);
  setAutoFillBackground(true);

  QLabel* label = new QLabel(QString::fromStdString(get_object()->get_name()), this);
  label->setFont(QFont("Sans", 8, QFont::DemiBold));
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(5);
  mainLayout->addWidget(label);

  // a shape is set as the widget's background
  QPixmap pixmap(size());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(Qt::black);

  get_object()->draw(&painter, width(), height());

  painter.end();

  QPalette palette;
  palette.setBrush(QPalette::Background, QBrush(pixmap));
  setPalette(palette);
}

std::shared_ptr<Object>& ObjectIcon::get_object()
{
  return object;
}

void ObjectIcon::mouseDoubleClickEvent(QMouseEvent *)
{
  // Modify Object options
  Dialog(*object, this).exec();
}


DefaultObjectIcon::DefaultObjectIcon(std::shared_ptr<Object>& object,
                                     QWidget* parent) :
  ObjectIcon(object, parent)
{}


FilterIcon::FilterIcon(std::shared_ptr<Object>& object,
                       QWidget* parent) :
  ObjectIcon(object, parent)
{
  QCheckBox* checkBox = new QCheckBox("not", this);

  QComboBox* comboBox = new QComboBox(this);
  comboBox->addItem("");
  comboBox->addItem("and");
  comboBox->addItem("or");

  QVBoxLayout* mainLayout = static_cast<QVBoxLayout*>(layout());
  mainLayout->insertWidget(0, checkBox);
  mainLayout->addWidget(comboBox);

  connect(checkBox, &QCheckBox::stateChanged, [&](int state) {
    static_cast<Filter&>(*get_object()).set_invert(state);
  });
  connect(comboBox, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::activated), [&](const QString& text) {
    static_cast<Filter&>(*get_object()).set_next(text.toStdString());
  });
}


StatementIcon::StatementIcon(QWidget* parent) :
  Icon(parent)
{
  QLabel* label = new QLabel(this);
  label->setAlignment(Qt::AlignCenter);

  QFrame* frame = new QFrame(this);
  frame->setMinimumSize(ICON_SIZE, ICON_SIZE);
  frame->setObjectName("frame");

  QBoxLayout* frameLayout = new QBoxLayout(QBoxLayout::LeftToRight, frame);
  frameLayout->setObjectName("frameLayout");

  QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
  mainLayout->setObjectName("mainLayout");
  mainLayout->addWidget(label);
  mainLayout->addWidget(frame);
}

void StatementIcon::add_icon(Icon* icon)
{
  const int index = get_index(icon);

  QBoxLayout* frameLayout = findChild<QBoxLayout*>("frameLayout");
  frameLayout->insertWidget(index, icon);

  // spent lots of time trying to find the right combination for the widget to set its size properly
  icon->show();
  adjustSize();
}

void StatementIcon::remove_icon(Icon* icon)
{
  QBoxLayout* frameLayout = findChild<QBoxLayout*>("frameLayout");
  frameLayout->removeWidget(icon);

  // spent lots of time trying to find the right combination for the widget to set its size properly
  frameLayout->activate();
  adjustSize();
}

int StatementIcon::get_index(Icon* icon)
{
  QBoxLayout* frameLayout = findChild<QBoxLayout*>("frameLayout");
  QPoint pos = mapFrom(icon->parentWidget(), icon->pos());

  for (int i = 0; i < frameLayout->count(); i++)
  {
    QWidget* widget = frameLayout->itemAt(i)->widget();

    if ((frameLayout->direction() == QBoxLayout::LeftToRight && widget->x() > pos.x()) ||
      ((frameLayout->direction() == QBoxLayout::TopToBottom && widget->y() > pos.y())))
    {
      return frameLayout->indexOf(widget);
    }
  }

  return frameLayout->count();
}


ObjectStatementIcon::ObjectStatementIcon(std::shared_ptr<ObjectStatement>& object_statement,
                                         QWidget* parent) :
  StatementIcon(parent),
  object_statement(std::move(object_statement))
{
  findChild<QLabel*>()->setText(QString::fromStdString(get_object_statement()->get_id()));
  findChild<QFrame*>("frame")->setStyleSheet("#frame { border: 2px solid yellow; }");
}

std::shared_ptr<ObjectStatement>& ObjectStatementIcon::get_object_statement()
{
  return object_statement;
}

void ObjectStatementIcon::add_icon(Icon* icon)
{
  // only add icons of the same type
  ObjectIcon* object_icon = static_cast<ObjectIcon*>(icon);
  const std::string& type = object_statement->get_type();
  if (!type.empty() && type != object_icon->get_object()->get_type())
  {
    return;
  }

  StatementIcon::add_icon(icon);

  // if the Icon is inside a LogStatementIcon
  parentWidget()->parentWidget()->adjustSize();

  std::shared_ptr<Object>& object = object_icon->get_object();
  int index = findChild<QBoxLayout*>("frameLayout")->indexOf(icon);

  object_statement->add_object(object, index);
}

void ObjectStatementIcon::remove_icon(Icon* icon)
{
  StatementIcon::remove_icon(icon);

  // if the Icon is inside a LogStatementIcon
  parent()->parent()->findChild<QBoxLayout*>("frameLayout")->activate();
  parentWidget()->parentWidget()->adjustSize();

  ObjectIcon* object_icon = static_cast<ObjectIcon*>(icon);
  std::shared_ptr<Object>& object = object_icon->get_object();

  object_statement->remove_object(object);
}


ObjectStatementIconCopy::ObjectStatementIconCopy(std::shared_ptr<ObjectStatement>& object_statement,
                                                 QWidget* parent) :
  ObjectStatementIcon(object_statement, parent)
{
  findChild<QLabel*>()->setText(QString::fromStdString(get_object_statement()->get_id()));

  QFrame* frame = findChild<QFrame*>("frame");
  frame->setStyleSheet("#frame { border: 2px solid gray; }");

  QLabel* label = new QLabel("COPY", this);
  label->setAlignment(Qt::AlignCenter);

  frame->layout()->addWidget(label);

  // to be able to move the newly created copy right away, releaseMouse handled by Scene widget
  grabMouse();
}


LogStatementIcon::LogStatementIcon(std::shared_ptr<LogStatement>& log_statement,
                                   QWidget* parent) :
  StatementIcon(parent),
  log_statement(std::move(log_statement))
{
  findChild<QLabel*>()->setText("L\nO\nG");
  findChild<QFrame*>("frame")->setStyleSheet("#frame { border: 2px solid black; }");
  findChild<QBoxLayout*>("frameLayout")->setDirection(QBoxLayout::TopToBottom);
  findChild<QBoxLayout*>("mainLayout")->setDirection(QBoxLayout::LeftToRight);
}

void LogStatementIcon::add_icon(Icon* icon)
{
  StatementIcon::add_icon(icon);

  ObjectStatementIcon* statement_icon = static_cast<ObjectStatementIcon*>(icon);
  std::shared_ptr<ObjectStatement>& object_statement = statement_icon->get_object_statement();
  int index = findChild<QBoxLayout*>("frameLayout")->indexOf(icon);

  log_statement->add_object_statement(object_statement, index);
}

void LogStatementIcon::remove_icon(Icon* icon)
{
  StatementIcon::remove_icon(icon);

  // each ObjectStatementIcon has the same size inside the LogStatementIcon, so after it's removed it can return to its original size
  icon->adjustSize();

  ObjectStatementIcon* statement_icon = static_cast<ObjectStatementIcon*>(icon);
  std::shared_ptr<ObjectStatement>& object_statement = statement_icon->get_object_statement();

  log_statement->remove_object_statement(object_statement);
}

void LogStatementIcon::mouseDoubleClickEvent(QMouseEvent *)
{
  // modify log options
  Dialog(log_statement->get_options(), this).exec();
}


DeleteIcon::DeleteIcon(QWidget* parent) :
  QWidget(parent)
{
  QLabel* icon = new QLabel;
  icon->setPixmap(QIcon::fromTheme("user-trash-symbolic").pixmap(60, 60));

  QLabel* text = new QLabel("delete");
  text->setFont(QFont("Sans", 8, QFont::DemiBold));
  text->setAlignment(Qt::AlignCenter);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(icon);
  mainLayout->addWidget(text);
}
