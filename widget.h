#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QTcpServer>
#include <QNetworkSession>
#include <QTextEdit>
#include <QFile>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(int nPort, QWidget* parent = nullptr);
public slots:
    virtual void slotNewConnection();
    void slotReadClient();
private:
    QTcpServer* m_ptcpServer;
    QTextEdit* m_ptxt;
    quint16 m_nNextBlockSize;
    void sendToClient(QTcpSocket* pSocket, const QString& str);
    void errorMessage();
    QString readFromJsonScoreFile();
    void roundReadClient(QTcpSocket* pClientSocket);
    bool sizeOfBytes(QTcpSocket *pClientSocket);
    bool nextBlockSize(QTcpSocket* pClientSocket);
    QString pathFileScore = "/home/dmitry/Projects/jsonFiles/jsonScore.dat";
    void writeToJsonScoreFile(QString str);
//    void checkJsonOnlyRead(QJsonObject objDocStr, QJsonObject objDocFile, QString pathFileScore, QJsonDocument docFile);
};

#endif // WIDGET_H
