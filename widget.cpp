#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QTime>
#include <QTcpServer>
#include <QDataStream>
#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>


Widget::Widget(int nPort, QWidget *parent) : QWidget(parent), m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, static_cast<quint16>(nPort)))
    {
        errorMessage();
    }
    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);
}

void Widget::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
}

void Widget::slotReadClient()
{
    QByteArray block;
    QTcpSocket* pClientSocket = static_cast<QTcpSocket*>(sender());
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_8);
    if (!m_nNextBlockSize)
    {
        in >> m_nNextBlockSize;
    }
    QString str;
    in >> str;
    QString strMessage = str;
    m_ptxt->append(strMessage);
    m_nNextBlockSize = 0;
    writeToJsonScoreFile(str);
    sendToClient(pClientSocket, readFromJsonScoreFile());
    qDebug() << readFromJsonScoreFile();
}

void Widget::writeToJsonScoreFile(QString str)
{
    QFile fileScore(pathFileScore);
    if (!fileScore.exists())
    {
        return;
    }
    if (fileScore.open(QIODevice::ReadWrite)) {
        QJsonDocument docStr = QJsonDocument::fromJson(str.toUtf8());
        QJsonDocument docFile = QJsonDocument::fromJson(readFromJsonScoreFile().toUtf8());
        QJsonObject objDocStr = docStr.object();
        QJsonObject objDocFile = docFile.object();
        if(objDocStr.value(objDocStr.keys().at(0)).toString() == "onlyRead")
        {
            return;
        }
        objDocFile.insert(objDocStr.keys().at(0) , QJsonValue::fromVariant(objDocStr.value(objDocStr.keys().at(0)).toString()));
        docFile.setObject(objDocFile);
        fileScore.write(docFile.toJson());
    }
    fileScore.close();
}

QString Widget::readFromJsonScoreFile()
{
    QFile fileScore(pathFileScore);
    if (!fileScore.exists())
    {
        return "File is not exists!";
    }
    if(fileScore.open(QIODevice::ReadOnly))
    {
        QString arrayFromScoreFile(fileScore.readAll());
        fileScore.close();
        return arrayFromScoreFile;
    }
    fileScore.close();
    return "null";
}

void Widget::sendToClient(QTcpSocket *pSocket, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_8);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(static_cast<quint16>(arrBlock.size()) - sizeof(quint16));
    pSocket->write(arrBlock);
}

void Widget::errorMessage()
{
    QMessageBox::critical(nullptr, "Server Error", "Unable to start the server:" + m_ptcpServer->errorString());
    m_ptcpServer->close();
    return;
}
