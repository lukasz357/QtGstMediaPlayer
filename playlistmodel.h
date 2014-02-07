#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include "playlistelement.h"
#include <QStringListModel>
#include <QFileInfoList>
#include <QHash>

class PlayListModel : public QStringListModel
{
    Q_OBJECT
public:
    explicit PlayListModel(QObject *parent = 0);

    void setFileInfoList(QFileInfoList &fileInfoList);

    void addElement(QFileInfo qfi);

    PlayListElement getElement(int idx);
    
signals:
    
public slots:
    
private:

    QHash<QString, PlayListElement> m_pathToFileNameMap;
};

#endif // PLAYLISTMODEL_H
