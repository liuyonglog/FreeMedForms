/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developpers :                                                    *
 *       Guillaume Denry <guillaume.denry@gmail.com>                       *
 *       Eric MAEKER, MD <eric.maeker@gmail.com>                           *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include <QPainter>
#include <QPixmapCache>
#include <QResizeEvent>
#include <QScrollArea>

#include "view.h"

using namespace Calendar;

void ViewWidget::setFirstDate(const QDate &firstDate) {
	if (m_firstDate == firstDate)
		return;

	m_firstDate = firstDate;
	emit firstDateChanged();

	resetItemWidgets();
	m_refreshGrid = true;
	update();
}

QPixmap ViewWidget::generatePixmap() {
	QPixmap pixmap(width(), height());
	QPainter painter(&pixmap);
        paintBody(&painter, rect());
	return pixmap;
}

void ViewWidget::paintEvent(QPaintEvent *) {
	QPixmap pixmap;
	QString key = "grid";

	if (!QPixmapCache::find(key, pixmap) || m_refreshGrid) {
		pixmap = generatePixmap();
		QPixmapCache::insert(key, pixmap);
		m_refreshGrid = false;
	}
	QPainter painter(this);
	painter.drawPixmap(0, 0, pixmap);
}

void ViewWidget::resizeEvent(QResizeEvent *event) {
	if (!masterScrollArea) {
		m_refreshGrid = true;
		refreshItemsSizesAndPositions();
	}

	QWidget::resizeEvent(event);
}

void ViewWidget::refreshItemsSizesAndPositions() {
	foreach (QObject *object, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(object);
		if (widget)
			refreshItemSizeAndPosition(widget);
	}
}

void ViewWidget::refreshCurrentDateTimeStuff() {
	m_refreshGrid = true;
	update();
}

void ViewWidget::setModel(AbstractCalendarModel *model) {
	// disconnect slots
	if (m_model){
		disconnect(m_model, SIGNAL(itemInserted(const Calendar::CalendarItem &)), this, SLOT(itemInserted(const Calendar::CalendarItem &)));
		disconnect(m_model, SIGNAL(itemModified(const Calendar::CalendarItem &, const Calendar::CalendarItem &)), this, SLOT(itemModified(const Calendar::CalendarItem &, const Calendar::CalendarItem &)));
		disconnect(m_model, SIGNAL(itemRemoved(const Calendar::CalendarItem &)), this, SLOT(itemRemoved(const Calendar::CalendarItem &)));
		disconnect(m_model, SIGNAL(reset()), this, SLOT(reset()));
	}

	m_model = model;

	if (m_model) {
		// connect slots
		connect(m_model, SIGNAL(itemInserted(const Calendar::CalendarItem &)), this, SLOT(itemInserted(const Calendar::CalendarItem &)));
		connect(m_model, SIGNAL(itemModified(const Calendar::CalendarItem &, const Calendar::CalendarItem &)), this, SLOT(itemModified(const Calendar::CalendarItem &, const Calendar::CalendarItem &)));
		connect(m_model, SIGNAL(itemRemoved(const Calendar::CalendarItem &)), this, SLOT(itemRemoved(const Calendar::CalendarItem &)));
		connect(m_model, SIGNAL(reset()), this, SLOT(reset()));
	}

	resetItemWidgets();
}

CalendarItemWidget *ViewWidget::getWidgetByUid(const QString &uid) const {
	foreach (QObject *object, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(object);
		if (widget && widget->uid() == uid)
			return widget;
	}
        return 0;
}

QList<CalendarItemWidget*> ViewWidget::getWidgetsByDate(const QDate &dayDate) const {
	QList<CalendarItemWidget*> list;
	foreach (QObject *obj, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(obj);
		if (widget && widget->beginDateTime().date() == dayDate)
			list << widget;
	}
	return list;
}

void ViewWidget::deleteAllWidgets() {
	QList<CalendarItemWidget*> list;
	foreach (QObject *obj, children()) {
		CalendarItemWidget *widget = qobject_cast<CalendarItemWidget*>(obj);
		if (widget)
			list << widget;
	}
	qDeleteAll(list);
}

void ViewWidget::setMasterScrollArea(QScrollArea *scrollArea) {
	masterScrollArea = scrollArea;
	if (masterScrollArea) {
		masterScrollArea->viewport()->installEventFilter(this);
	}
}

bool ViewWidget::eventFilter(QObject *obj, QEvent *event) {
	bool r = QWidget::eventFilter(obj, event);
	if (event->type() == QEvent::Resize) {
//		QWidget *w = qobject_cast<QWidget*>(obj);
/*		QScrollArea *w = qobject_cast<QScrollArea*>(obj);
		qDebug("resize: %d, %d", w->width(), w->viewport()->width());*/
		m_refreshGrid = true;
		refreshItemsSizesAndPositions();
	}
	return r;
}

void ViewWidget::reset() {
	resetItemWidgets();
	update();
}
