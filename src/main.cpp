#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QDir>
#include <QStandardPaths>
#include <QFont>
#include <QScreen>
#include "mainwindow.h"
#include "settings_manager.h"
#include "theme_manager.h"

int main(int argc, char *argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    app.setApplicationName("FrameX");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("ZAKI");
    app.setOrganizationDomain("zg22x.framex");

    // Create directories
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    for (auto& sub : QStringList{"", "/recordings", "/screenshots", "/temp"})
        QDir().mkpath(dataPath + sub);

    SettingsManager::instance().initialize();
    ThemeManager::instance().applyTheme(app);

    // Splash screen
    QPixmap splashPix(640, 380);
    splashPix.fill(Qt::transparent);
    QPainter p(&splashPix);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient bg(0, 0, 640, 380);
    bg.setColorAt(0, QColor("#07071A"));
    bg.setColorAt(1, QColor("#130828"));
    p.fillRect(0, 0, 640, 380, bg);

    // Border
    p.setPen(QPen(QColor("#2D2B55"), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(1, 1, 638, 378, 16, 16);

    // Glow
    QRadialGradient glow(320, 180, 160);
    glow.setColorAt(0, QColor(124, 58, 237, 60));
    glow.setColorAt(1, QColor(0, 0, 0, 0));
    p.fillRect(0, 0, 640, 380, glow);

    // Logo
    p.setPen(QColor("#C084FC"));
    QFont lf; lf.setFamily("Segoe UI"); lf.setPointSize(46); lf.setBold(true);
    p.setFont(lf);
    p.drawText(QRect(0, 100, 640, 80), Qt::AlignCenter, "FrameX");

    // Tagline
    p.setPen(QColor("#94A3B8"));
    QFont sf; sf.setFamily("Segoe UI"); sf.setPointSize(13);
    p.setFont(sf);
    p.drawText(QRect(0, 188, 640, 36), Qt::AlignCenter, "Capture everything. Share anything.");

    // Version badge
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#1E1B3A"));
    QRect vBadge(270, 232, 100, 22);
    p.drawRoundedRect(vBadge, 11, 11);
    p.setPen(QColor("#7C3AED"));
    QFont vf; vf.setFamily("Segoe UI"); vf.setPointSize(9);
    p.setFont(vf);
    p.drawText(vBadge, Qt::AlignCenter, "v2.0.0");

    // Loading bar
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#12122A"));
    p.drawRoundedRect(120, 300, 400, 6, 3, 3);
    QLinearGradient bar(120, 0, 520, 0);
    bar.setColorAt(0, QColor("#7C3AED"));
    bar.setColorAt(1, QColor("#C084FC"));
    p.setBrush(bar);
    p.drawRoundedRect(120, 300, 400, 6, 3, 3);

    // Developer credit
    p.setPen(QColor("#475569"));
    QFont df; df.setFamily("Segoe UI"); df.setPointSize(9);
    p.setFont(df);
    p.drawText(QRect(0, 344, 640, 26), Qt::AlignCenter, "Developed by ZAKI  ·  Instagram & TikTok: @zg22x");
    p.end();

    QSplashScreen splash(splashPix, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    splash.setAttribute(Qt::WA_TranslucentBackground);

    // Center splash on screen
    if (auto* screen = QApplication::primaryScreen()) {
        QRect sg = screen->geometry();
        splash.move(sg.center() - QPoint(320, 190));
    }
    splash.show();
    app.processEvents();

    MainWindow* window = new MainWindow();

    QTimer::singleShot(1800, [&splash, window]() {
        splash.finish(window);
        window->show();
    });

    return app.exec();
}
