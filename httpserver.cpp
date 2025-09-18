#include <QFile>
#include <QMimeDatabase>
#include <QDebug>
//own
#include "httpserver.h"

AHttpServer::AHttpServer(QObject *parent)
    : QTcpServer(parent)
{
}

void AHttpServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
        handleRequest(socket);
    });
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    socket->setSocketDescriptor(socketDescriptor);
}

void AHttpServer::handleRequest(QTcpSocket *socket) {
    QByteArray request = socket->readAll();
    QStringList tokens = QString(request).split(QRegularExpression("[ \r\n][ \r\n]*"));

    if (tokens.size() < 2) return;
    QString method = tokens[0];
    QString path = tokens[1];

    if (path == "/" || path == "/index.html") {
        serveFile(socket, ":/web_content/index.html", "text/html");
    } else {
        serveFile(socket, ":/web_content" + path, "");
    }
}

void AHttpServer::serveFile(QTcpSocket *socket, const QString &filePath, const QString &contentType) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray content = file.readAll();

        QMimeDatabase mimeDb;
        QString mimeType = contentType.isEmpty() ?
                               mimeDb.mimeTypeForFile(filePath).name() : contentType;

        QByteArray response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + mimeType.toUtf8() + "\r\n";
        response += "Content-Length: " + QByteArray::number(content.size()) + "\r\n";
        response += "Connection: close\r\n\r\n";
        response += content;

        socket->write(response);
    } else {
        QByteArray response = "HTTP/1.1 404 Not Found\r\n"
                              "Content-Type: text/plain\r\n"
                              "Connection: close\r\n\r\n"
                              "File not found: " + filePath.toUtf8();
        socket->write(response);
    }
    socket->disconnectFromHost();
}
