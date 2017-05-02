#ifndef SAVER_H
#define SAVER_H

#include <QObject>

class saver : public QObject
{
    Q_OBJECT
public:
    explicit saver(QObject *parent = 0);
    bool setDir(QString dir);
    void setText(QString text);
    void setIm(QPixmap *im);
    void save();
private:
    QString text;
    QString dir;
    QPixmap *image;





signals:

public slots:
};

#endif // SAVER_H
