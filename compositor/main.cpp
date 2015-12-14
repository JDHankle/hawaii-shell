/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QLoggingCategory>
#include <QtCore/QCommandLineParser>
#include <QtCore/QStandardPaths>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtWidgets/QApplication>

#include <GreenIsland/QtWaylandCompositor/QWaylandCompositor>

#include <Hawaii/greenisland_version.h>
#include <GreenIsland/Server/HomeApplication>

#include "sigwatch/sigwatch.h"

#include "config.h"
#include "gitsha1.h"
#include "processlauncher/processlauncher.h"
#include "screensaver/screensaver.h"
#include "sessionmanager/sessioninterface.h"
#include "sessionmanager/sessionmanager.h"

#include <unistd.h>

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

using namespace GreenIsland::Server;

static bool failSafe = false;

static void setupEnvironment()
{
    // Set defaults
    if (qEnvironmentVariableIsEmpty("XDG_DATA_DIRS"))
        qputenv("XDG_DATA_DIRS", QByteArrayLiteral("/usr/local/share/:/usr/share/"));
    if (qEnvironmentVariableIsEmpty("XDG_CONFIG_DIRS"))
        qputenv("XDG_CONFIG_DIRS", QByteArrayLiteral("/etc/xdg"));

    // Environment
    qputenv("QT_QPA_PLATFORMTHEME", QByteArrayLiteral("Hawaii"));
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("Wind"));
    qputenv("XCURSOR_THEME", QByteArrayLiteral("hawaii"));
    qputenv("XCURSOR_SIZE", QByteArrayLiteral("16"));
    qputenv("XDG_MENU_PREFIX", QByteArrayLiteral("hawaii-"));
    qputenv("XDG_CURRENT_DESKTOP", QByteArrayLiteral("X-Hawaii"));
}

int main(int argc, char *argv[])
{
    // Setup the environment
    setupEnvironment();

    // Application
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Hawaii"));
    app.setApplicationVersion(HAWAII_VERSION_STRING);
    app.setOrganizationName(QStringLiteral("Hawaii"));
    app.setOrganizationDomain(QStringLiteral("hawaiios.org"));
    app.setQuitOnLastWindowClosed(false);

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Wayland compositor for the Hawaii desktop environment"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Wayland socket
    QCommandLineOption socketOption(QStringLiteral("wayland-socket-name"),
                                    TR("Wayland socket"), TR("name"));
    parser.addOption(socketOption);

    // Nested mode
    QCommandLineOption nestedOption(QStringList() << QStringLiteral("n") << QStringLiteral("nested"),
                                    TR("Nest into a compositor that supports _wl_fullscreen_shell"));
    parser.addOption(nestedOption);

    // Fake screen configuration
    QCommandLineOption fakeScreenOption(QStringLiteral("fake-screen"),
                                        TR("Use fake screen configuration"),
                                        TR("filename"));
    parser.addOption(fakeScreenOption);

#if DEVELOPMENT_BUILD
    // Load shell from an arbitrary path
    QCommandLineOption qmlOption(QStringLiteral("qml"),
                                 QStringLiteral("Load a shell main QML file"),
                                 QStringLiteral("filename"));
    parser.addOption(qmlOption);
#endif

    // Parse command line
    parser.process(app);

    // Restart with D-Bus session if necessary
    if (qEnvironmentVariableIsEmpty("DBUS_SESSION_BUS_ADDRESS")) {
        qCritical("No D-Bus session bus available, please run Hawaii with dbus-launch.");
        return 1;
    }

    // Arguments
    bool nested = parser.isSet(nestedOption);
    QString socket = parser.value(socketOption);
    QString fakeScreenData = parser.value(fakeScreenOption);

    // Nested mode requires running from Wayland and a socket name
    // and fake screen data cannot be used
    if (nested) {
        if (!QGuiApplication::platformName().startsWith(QStringLiteral("wayland"))) {
            qCritical("Nested mode only make sense when running on Wayland.\n"
                      "Please pass the \"-platform wayland\" argument.");
            return 1;
        }

        if (socket.isEmpty()) {
            qCritical("Nested mode requires you to specify a socket name.\n"
                      "Please specify it with the \"--wayland-socket-name\" argument.");
            return 1;
        }

        if (!fakeScreenData.isEmpty()) {
            qCritical("Fake screen configuration cannot be used when nested");
            return 1;
        }
    }

    // Unix signals watcher
    UnixSignalWatcher sigwatch;
    sigwatch.watchForSignal(SIGINT);
    sigwatch.watchForSignal(SIGTERM);

    // Quit when the process is killed
    QObject::connect(&sigwatch, &UnixSignalWatcher::unixSignal,
                     &app, &QApplication::quit);

    // Print version information
    qDebug("== Hawaii Compositor v%s (Green Island v%s) ==\n"
           "** http://hawaiios.org\n"
           "** Bug reports to: https://github.com/hawaii-desktop/hawaii-shell/issues\n"
           "** Build: %s-%s",
           HAWAII_VERSION_STRING, GREENISLAND_VERSION_STRING,
           HAWAII_VERSION_STRING, GIT_REV);

    // Register D-Bus service
    if (!QDBusConnection::sessionBus().registerService(QStringLiteral("org.hawaiios.Session"))) {
        qCritical("Failed to register D-Bus service: %s",
                  qPrintable(QDBusConnection::sessionBus().lastError().message()));
        return 1;
    }

    // Home application
    HomeApplication *homeApp = new HomeApplication(&app);
    homeApp->setScreenConfiguration(fakeScreenData);

    // Process launcher
    ProcessLauncher *processLauncher = new ProcessLauncher(homeApp);
    if (!ProcessLauncher::registerWithDBus(processLauncher))
        return 1;

    // Screen saver
    ScreenSaver *screenSaver = new ScreenSaver(homeApp);
    if (!ScreenSaver::registerWithDBus(screenSaver))
        return 1;

    // Session interface
    SessionManager *sessionManager = new SessionManager(homeApp);
    homeApp->setContextProperty(QStringLiteral("SessionInterface"),
                                new SessionInterface(sessionManager));

    // Fail safe mode
    QObject::connect(homeApp, &HomeApplication::objectCreated, [homeApp](QObject *object, const QUrl &) {
        if (!object) {
            if (failSafe) {
                // We give up because even the error screen has an error
                qWarning("A fatal error has occurred while running Hawaii, but the error "
                         "screen has errors too. Giving up.");
                ::exit(1);
            } else {
                // Load the error screen in case of error
                failSafe = true;
                homeApp->loadUrl(QUrl(QStringLiteral("qrc:/error/ErrorCompositor.qml")));
            }
        }
    });

    // Create the compositor and run
    bool alreadyLoaded = false;
#if DEVELOPMENT_BUILD
    if (parser.isSet(qmlOption)) {
        if (homeApp->loadUrl(QUrl::fromLocalFile(parser.value(qmlOption))))
            alreadyLoaded = true;
        else
            return 1;
    }
#endif
    if (!alreadyLoaded) {
        if (!homeApp->loadUrl(QUrl(QStringLiteral("qrc:/Compositor.qml"))))
            return 1;
    }

    QObject *rootObject = homeApp->rootObjects().at(0);
    QWaylandCompositor *compositor = qobject_cast<QWaylandCompositor *>(rootObject);
    if (compositor)
        processLauncher->setWaylandSocketName(QString::fromUtf8(compositor->socketName()));

    return app.exec();
}
