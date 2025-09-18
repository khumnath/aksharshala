#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QRegularExpression>

class AHttpServer : public QTcpServer {
    Q_OBJECT
public:
    AHttpServer(QObject *parent = nullptr);
    void incomingConnection(qintptr socketDescriptor) override;

private:
    void handleRequest(QTcpSocket *socket);
    void serveFile(QTcpSocket *socket, const QString &filePath, const QString &contentType);
};

#endif // HTTPSERVER_H
