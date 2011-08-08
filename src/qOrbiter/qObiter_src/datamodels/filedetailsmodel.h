#ifndef FILEDETAILSMODEL_H
#define FILEDETAILSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/DataModelItems/filedetailsitem.h>
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
