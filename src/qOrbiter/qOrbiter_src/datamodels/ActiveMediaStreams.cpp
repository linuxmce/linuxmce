/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
    QOrbiter is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

    This class is another specific model/item pair with the parent being parent being floorplan model.
    it is not however implmented directly as a model, but a QList<QOBject*>. This class holds very simple data pertaining to the
    images and descriptions of particular pages.

*/

#include "ActiveMediaStreams.h"
#include "DCE/Logger.h"
using namespace DCE;

ActiveMediaStreamItem::ActiveMediaStreamItem(QObject* parent) : GenericModelItem(parent)
{
        QHash<int, QByteArray> names;
        names[StreamIDRole] = "streamID";
	names[DescriptionRole] = "description";
	setRoleNames(names);
}

ActiveMediaStreamItem::ActiveMediaStreamItem(DataGridCell *pCell) : GenericModelItem()
{
        QHash<int, QByteArray> names;
        names[StreamIDRole] = "streamID";
	names[DescriptionRole] = "description";
	setRoleNames(names);
        setFromDataGridCell(pCell);
}

void ActiveMediaStreamItem::setFromDataGridCell(DataGridCell *pCell) {
    m_data.insert(StreamIDRole, pCell->GetValue());
    m_data.insert(DescriptionRole, pCell->GetText());
}
