#include "playlistmodel.h"
#include "playlistelement.h"
#include <QHash>
#include <QtAlgorithms>
#include <QDebug>

PlayListModel::PlayListModel(QObject *parent) :
    QStringListModel(parent)
{
}

void PlayListModel::setFileInfoList(QFileInfoList &fileInfoList)
{
    QStringList fileNames;
    for(int i = 0; i < fileInfoList.size(); i++) {
        QFileInfo qfi = fileInfoList.at(i);
        PlayListElement el(qfi.fileName());
        el.setPath(qfi.absoluteFilePath());
        fileNames.append(el);
    }
    QStringListModel::setStringList(fileNames);

}

void PlayListModel::addElement(QFileInfo qfi)
{
    if(!m_pathToFileNameMap.contains(qfi.absoluteFilePath())) {
        PlayListElement el(qfi.fileName());
        el.setPath(qfi.absoluteFilePath());
        m_pathToFileNameMap.insert(qfi.fileName(), el);
        QList<PlayListElement> values = m_pathToFileNameMap.values();
        QStringList qstringList;
        foreach(PlayListElement item, values) {
            qstringList.append(item);
        }
        qSort(qstringList.begin(), qstringList.end());
        QStringListModel::setStringList(qstringList);
    }
}

PlayListElement PlayListModel::getElement(int idx) {
    QString fileName =  stringList().at(idx);
    PlayListElement element = m_pathToFileNameMap.value(fileName);
    return element;
}

