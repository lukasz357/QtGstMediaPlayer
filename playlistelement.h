#ifndef PLAYLISTELEMENT_H
#define PLAYLISTELEMENT_H

#include <QString>

class PlayListElement : public QString
{

public:
    PlayListElement(const QString & path);
    PlayListElement();

    void setPath(const QString & path);
    QString path();
    

private:
    QString m_path;
    
};

#endif // PLAYLISTELEMENT_H
