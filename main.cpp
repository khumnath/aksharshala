#include <QApplication>
#include <QWebEngineView>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QDebug>
#include <QWebEngineSettings>
#include <QDesktopServices>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QStandardPaths>
#include <QSettings>
#include <QCloseEvent>
#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QTcpServer>
#include <QSharedMemory>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
// own
#include "httpserver.h"

int findAvailablePort(int startPort, int range = 100) {
    for (int port = startPort; port < startPort + range; ++port) {
        QTcpServer server;
        if (server.listen(QHostAddress::Any, port)) {
            server.close();
            // qDebug() << "Found available port:" << port;
            return port;
        }
    }
    // qWarning() << "Could not find an available port in range.";
    return -1;
}

class CustomPage : public QWebEnginePage {
public:
    CustomPage(QWebEngineProfile *profile, QObject *parent = nullptr)
        : QWebEnginePage(profile, parent) {}
protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override {
        if ((url.scheme() == QLatin1String("http") || url.scheme() == QLatin1String("https")) && url.host() != QLatin1String("localhost")) {
            //qDebug() << "External link clicked, opening in browser:" << url;
            QDesktopServices::openUrl(url);
            return false;
        }
        Q_UNUSED(type);
        Q_UNUSED(isMainFrame);
        return true;
    }
};

class ClickableView : public QWebEngineView {
public:
    ClickableView(QWidget *parent = nullptr) : QWebEngineView(parent) {
        // Set up a persistent profile to store cache, cookies, etc.
        QWebEngineProfile *profile = new QWebEngineProfile("AksharshalaTypingTutorProfile", this);
        profile->setPersistentStoragePath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/aksharshala-profile");

        CustomPage *page = new CustomPage(profile, this);
        setPage(page);

        settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
        settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);

        setContextMenuPolicy(Qt::NoContextMenu);
    }
};

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Aksharshala Typing Tutor");
        resize(1150, 760);

        m_stack = new QStackedWidget(this);
        setCentralWidget(m_stack);

        m_webView = new ClickableView(this);
        m_stack->addWidget(m_webView);

        m_errorLabel = new QLabel(this);
        m_errorLabel->setAlignment(Qt::AlignCenter);
        m_errorLabel->setStyleSheet("font-size: 16px; color: #444;");
        m_stack->addWidget(m_errorLabel);

        QSettings settings("Aksharshala", "Aksharshala");
        if (settings.contains("geometry")) {
            restoreGeometry(settings.value("geometry").toByteArray());
        }
    }

    void showWebView(const QUrl& url) {
        m_webView->setUrl(url);
        m_stack->setCurrentWidget(m_webView);
    }

    void showError(const QString& message) {
        m_errorLabel->setText(message);
        m_stack->setCurrentWidget(m_errorLabel);
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        QSettings settings("Aksharshala", "Aksharshala");
        settings.setValue("geometry", saveGeometry());
        QMainWindow::closeEvent(event);
    }

private:
    QStackedWidget* m_stack;
    ClickableView* m_webView;
    QLabel* m_errorLabel;
};


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    const QString uniqueKey = "AksharshalaTypingTutor_UniqueInstanceKey";
    QSharedMemory sharedMemory(uniqueKey);

    if (!sharedMemory.attach()) {
        if (!sharedMemory.create(1, QSharedMemory::ReadWrite)) {
            QMessageBox::critical(nullptr, "Application Error",
                                  "Could not create shared memory segment.\n"
                                  "Please try restarting your computer.");
            // qCritical() << "Failed to create shared memory:" << sharedMemory.errorString();
            return 1;
        }
    } else {
        QLocalSocket socket;
        socket.connectToServer(uniqueKey);

        if (socket.waitForConnected(500)) {
            // qWarning() << "Application is already running. Activating the existing window.";
            socket.disconnectFromServer();
            return 0;
        } else {
            // qWarning() << "Found a stale shared memory segment. Cleaning up...";
            if (!sharedMemory.detach()) {
                // qWarning() << "Could not detach from stale shared memory. It might be locked.";
            }

            if (!sharedMemory.create(1, QSharedMemory::ReadWrite)) {
                QMessageBox::critical(nullptr, "Application Error",
                                      "Could not clean up and re-create shared memory segment.\n"
                                      "Please try restarting your computer.");
                // qCritical() << "Failed to re-create shared memory after cleanup:" << sharedMemory.errorString();
                return 1;
            }
        }
    }


    MainWindow mainWindow;
    QLocalServer localServer;
    QObject::connect(&localServer, &QLocalServer::newConnection, [&]() {
        mainWindow.setWindowState((mainWindow.windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        mainWindow.raise();
        mainWindow.activateWindow();
        if (localServer.hasPendingConnections()) {
            QLocalSocket *socket = localServer.nextPendingConnection();
            socket->disconnectFromServer();
            socket->deleteLater();
        }
    });

    QLocalServer::removeServer(uniqueKey);
    localServer.listen(uniqueKey);


    // Server Initialization
    const int httpPort = findAvailablePort(8093);
    const int wsPort = findAvailablePort(9000);

    bool serversOk = (httpPort != -1 && wsPort != -1);

    AHttpServer httpServer;
    QWebSocketServer wsServer("WebSocket Server", QWebSocketServer::NonSecureMode);

    if (serversOk) {
        if (!httpServer.listen(QHostAddress::Any, httpPort)) {
            // qCritical() << "HTTP server failed to start.";
            serversOk = false;
        }
        if (!wsServer.listen(QHostAddress::Any, wsPort)) {
            // qCritical() << "WebSocket server failed to start.";
            serversOk = false;
        }
    }

    // Final UI State
    if (serversOk) {
        QObject::connect(&wsServer, &QWebSocketServer::newConnection, [&wsServer]() {
            if (QWebSocket *socket = wsServer.nextPendingConnection()) {
                QObject::connect(socket, &QWebSocket::textMessageReceived, [socket](const QString &message) {
                    socket->sendTextMessage("Echo: " + message);
                });
                QObject::connect(socket, &QWebSocket::disconnected, socket, &QObject::deleteLater);
            }
        });

        mainWindow.showWebView(QUrl(QString("http://localhost:%1").arg(httpPort)));
    } else {
        mainWindow.showError("Error: Could not start required services.\n"
                             "Another application may be using the necessary ports.");
    }

    mainWindow.show();
    int exitCode = app.exec();

    return exitCode;
}

