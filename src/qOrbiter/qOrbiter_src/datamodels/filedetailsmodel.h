/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
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

   The file details class is a very specfic and transient object. its purpose is to display contecxtual information about a specific
   piece of selected media. This is commonly integrated with the 'play' 'move' and 'delete' functions in the user interface, allthough
   those functions are not in the scope of this class.

  !!DEPRECIATED!!!! use contextobjects/filedetailsclass
*/

#ifndef FILEDETAILSMODEL_H
#define FILEDETAILSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>

#include <QStringList>

class FileDetailsItem;

class FileDetailsModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY (bool showDetails READ isVisible WRITE setVisible NOTIFY VisibleChanged)
    Q_PROPERTY(QString file READ getFile WRITE setFile NOTIFY FileChanged)

public:
    explicit FileDetailsModel(FileDetailsItem* prototype, QObject* parent = 0);
    ~FileDetailsModel();
#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(FileDetailsItem* item);
    void appendRows(const QList<FileDetailsItem*> &items);
    void insertRow(int row, FileDetailsItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    FileDetailsItem* takeRow(int row);
    FileDetailsItem* find(const QString &id) const;
    QModelIndex indexFromItem( const FileDetailsItem* item) const;
    FileDetailsItem* currentRow();
    void clear();
    void ReturnSelectedItems();
    void sortModel(int column, Qt::SortOrder order);

    QModelIndex lastrow;
    QString ident;

    QString file;
    inline QString getFile () {return file;}
    inline void setFile(QString incFile) { file = incFile; emit FileChanged(file);}

    bool showDetails;
    inline bool isVisible () { return showDetails; }
    inline void setVisible (bool state) { showDetails = state; emit VisibleChanged();}

    Q_INVOKABLE void setSelectionStatus(QString format);
    Q_INVOKABLE bool getSelectionStatus();

signals:
    void ItemAdded();
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const bool &state);
    void SetTypeSort(int pos, QString type);
    void VisibleChanged();
    void FileChanged(QString f);
private slots:
    void handleItemChange();

private:
    FileDetailsItem* m_prototype;
    QList<FileDetailsItem*> m_list;
};

#endif // FILEDETAILSMODEL_H
