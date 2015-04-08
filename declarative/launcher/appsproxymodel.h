/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef APPSPROXYMODEL_H
#define APPSPROXYMODEL_H

#include <QtCore/QSortFilterProxyModel>

class AppsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
public:
    AppsProxyModel(QObject *parent = 0);

    QString query() const;
    void setQuery(const QString &query);

Q_SIGNALS:
    void queryChanged();

private:
    QString m_query;
};

#endif // APPSPROXYMODEL_H
