#include "mainwindow.h"
#include "ui/sidebar_widget.h"
#include "ui/dashboard_page.h"
#include "ui/recordings_page.h"
#include "ui/screenshots_page.h"
#include "ui/settings_page.h"
#include "ui/hotkeys_page.h"
#include "ui/tools_page.h"
#include "ui/streaming_page.h"
#include "ui/recording_settings_panel.h"
#include "ui/floating_widget.h"
#include "recording/recorder.h"
#include "settings_manager.h"
#include "hotkey_manager.h"
#include "history_manager.h"
#include "theme_manager.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QScreen>
#include <QShortcut>
#include <QStatusBar>
#include <QLabel>
#include <QStandardPaths>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("FrameX");
    setMinimumSize(1100, 700);
    resize(1280, 780);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);

    m_recorder = new Recorder(this);

    setupUI();
    setupTray();
    setupHotkeys();

    connect(m_recorder, &Recorder::recordingStarted,  this, [this](){
        m_isRecording = true;
        m_isPaused    = false;
        m_recordingStart = QDateTime::currentDateTime();
        m_recordingTimer->start(1000);
        m_sidebar->setRecordingState(true);
        if (SettingsManager::instance().app.showFloatingWidget)
            m_floatingWidget->show();
        m_floatingWidget->startTimer();
        m_trayIcon->showMessage("FrameX", "Recording started", QSystemTrayIcon::Information, 2000);
    });

    connect(m_recorder, &Recorder::recordingStopped, this, [this](const QString& path){
        m_isRecording = false;
        m_isPaused    = false;
        m_recordingTimer->stop();
        m_sidebar->setRecordingState(false);
        m_floatingWidget->hide();
        m_floatingWidget->stopTimer();
        HistoryManager::instance().addEntry(path, m_recordingStart, QDateTime::currentDateTime());
        m_recordingsPage->loadRecordings();
        m_trayIcon->showMessage("FrameX", "Recording saved!", QSystemTrayIcon::Information, 3000);
    });

    connect(m_recorder, &Recorder::errorOccurred, this, [this](const QString& msg){
        m_trayIcon->showMessage("FrameX Error", msg, QSystemTrayIcon::Warning, 4000);
    });

    connect(m_floatingWidget, &FloatingWidget::stopRequested,       this, &MainWindow::onStopRecording);
    connect(m_floatingWidget, &FloatingWidget::pauseRequested,      this, &MainWindow::onPauseRecording);
    connect(m_floatingWidget, &FloatingWidget::screenshotRequested, this, &MainWindow::onTakeScreenshot);
}

MainWindow::~MainWindow() {
    SettingsManager::instance().save();
}

void MainWindow::setupUI() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Sidebar
    m_sidebar = new SidebarWidget(this);
    m_sidebar->setFixedWidth(220);
    m_mainLayout->addWidget(m_sidebar);

    // Right side
    auto* rightSide = new QVBoxLayout();
    rightSide->setContentsMargins(0, 0, 0, 0);
    rightSide->setSpacing(0);

    // Custom title bar
    auto* titleBar = new QFrame();
    titleBar->setFixedHeight(44);
    titleBar->setObjectName("titleBar");
    auto* tbLayout = new QHBoxLayout(titleBar);
    tbLayout->setContentsMargins(16, 0, 8, 0);

    auto* titleLabel = new QLabel("FrameX");
    titleLabel->setStyleSheet("color:#C084FC; font-weight:bold; font-size:14px;");
    tbLayout->addWidget(titleLabel);
    tbLayout->addStretch();

    // Window control buttons
    auto makeWinBtn = [](const QString& sym, const QString& color, const QString& hover) {
        auto* b = new QPushButton(sym);
        b->setFixedSize(28, 28);
        b->setStyleSheet(QString(
            "QPushButton{background:%1;border:none;border-radius:14px;color:#fff;font-size:11px;}"
            "QPushButton:hover{background:%2;}").arg(color, hover));
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };
    auto* minBtn   = makeWinBtn("—", "#2D2B55", "#3730A3");
    auto* maxBtn   = makeWinBtn("□", "#2D2B55", "#1D4ED8");
    auto* closeBtn = makeWinBtn("✕", "#2D2B55", "#DC2626");
    connect(minBtn,   &QPushButton::clicked, this, &QMainWindow::showMinimized);
    connect(maxBtn,   &QPushButton::clicked, [this](){
        isMaximized() ? showNormal() : showMaximized();
    });
    connect(closeBtn, &QPushButton::clicked, this, &QMainWindow::close);
    tbLayout->addWidget(minBtn);
    tbLayout->addWidget(maxBtn);
    tbLayout->addWidget(closeBtn);
    rightSide->addWidget(titleBar);

    // Content row
    auto* contentRow = new QHBoxLayout();
    contentRow->setContentsMargins(0, 0, 0, 0);
    contentRow->setSpacing(0);

    m_pageStack = new QStackedWidget();

    // Pages
    m_dashboardPage  = new DashboardPage(m_recorder, this);
    m_recordingsPage = new RecordingsPage(this);
    m_streamingPage  = new StreamingPage(this);
    m_screenshotsPage = new ScreenshotsPage(this);
    m_settingsPage   = new SettingsPage(this);
    m_hotkeysPage    = new HotkeysPage(this);
    m_toolsPage      = new ToolsPage(this);

    m_pageStack->addWidget(m_dashboardPage);
    m_pageStack->addWidget(m_recordingsPage);
    m_pageStack->addWidget(m_streamingPage);
    m_pageStack->addWidget(m_screenshotsPage);
    m_pageStack->addWidget(m_settingsPage);
    m_pageStack->addWidget(m_hotkeysPage);
    m_pageStack->addWidget(m_toolsPage);

    contentRow->addWidget(m_pageStack, 1);

    // Settings panel (right panel)
    m_settingsPanel = new RecordingSettingsPanel(this);
    m_settingsPanel->setFixedWidth(260);
    contentRow->addWidget(m_settingsPanel);

    rightSide->addLayout(contentRow, 1);
    m_mainLayout->addLayout(rightSide, 1);

    // Floating widget
    m_floatingWidget = new FloatingWidget();
    m_floatingWidget->hide();

    // Recording timer
    m_recordingTimer = new QTimer(this);
    connect(m_recordingTimer, &QTimer::timeout, this, &MainWindow::onRecordingTimer);

    // Connect sidebar signals
    connect(m_sidebar, &SidebarWidget::pageSelected, this, &MainWindow::onPageSelected);

    connect(m_dashboardPage, &DashboardPage::startRecordingRequested,
            this, &MainWindow::onStartRecording);
    connect(m_dashboardPage, &DashboardPage::screenshotRequested,
            this, &MainWindow::onTakeScreenshot);
}

void MainWindow::setupTray() {
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    m_trayIcon->setToolTip("FrameX - Screen Recorder");

    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction("Show FrameX",     this, [this](){ show(); raise(); activateWindow(); });
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("Start Recording", this, &MainWindow::onStartRecording);
    m_trayMenu->addAction("Screenshot",      this, &MainWindow::onTakeScreenshot);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("Quit",            qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);
}

void MainWindow::setupHotkeys() {
    auto& hk = SettingsManager::instance().hotkeys;

    auto makeShortcut = [this](const QKeySequence& seq, auto slot) {
        auto* sc = new QShortcut(seq, this);
        sc->setContext(Qt::ApplicationShortcut);
        connect(sc, &QShortcut::activated, this, slot);
    };

    makeShortcut(hk.startStop,  [this](){
        m_isRecording ? onStopRecording() : onStartRecording();
    });
    makeShortcut(hk.pauseResume, &MainWindow::onPauseRecording);
    makeShortcut(hk.screenshot,  &MainWindow::onTakeScreenshot);
}

void MainWindow::onPageSelected(int index) {
    m_pageStack->setCurrentIndex(index);
}

void MainWindow::onStartRecording() {
    if (m_isRecording) return;
    auto settings = SettingsManager::instance().recording;
    m_recorder->setSettings(settings);
    m_recorder->startRecording();
}

void MainWindow::onStopRecording() {
    if (!m_isRecording) return;
    m_recorder->stopRecording();
}

void MainWindow::onPauseRecording() {
    if (!m_isRecording) return;
    if (!m_isPaused) {
        m_recorder->pauseRecording();
        m_floatingWidget->setPaused(true);
        m_recordingTimer->stop();
    } else {
        m_recorder->resumeRecording();
        m_floatingWidget->setPaused(false);
        m_recordingTimer->start(1000);
    }
    m_isPaused = !m_isPaused;
}

void MainWindow::onTakeScreenshot() {
    QString ssDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                    + "/FrameX Screenshots";
    QDir().mkpath(ssDir);
    QString filename = ssDir + "/Screenshot_" +
                       QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") + ".png";

    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QPixmap px = screen->grabWindow(0);
        if (px.save(filename, "PNG")) {
            m_trayIcon->showMessage("FrameX", "Screenshot saved!", QSystemTrayIcon::Information, 2000);
            m_screenshotsPage->loadScreenshots();
        }
    }
}

void MainWindow::onRecordingTimer() {
    qint64 secs = m_recordingStart.secsTo(QDateTime::currentDateTime());
    m_floatingWidget->updateTime(secs);
    m_dashboardPage->updateRecordingTime(secs);
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick) {
        show(); raise(); activateWindow();
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (SettingsManager::instance().app.minimizeToTray && !m_forceQuit) {
        hide();
        event->ignore();
        m_trayIcon->showMessage("FrameX", "Running in system tray", QSystemTrayIcon::Information, 2000);
    } else {
        if (m_isRecording) {
            auto reply = QMessageBox::question(this, "FrameX",
                "Recording is in progress. Stop and quit?",
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) { event->ignore(); return; }
            m_recorder->stopRecording();
        }
        SettingsManager::instance().save();
        qApp->quit();
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && event->pos().y() < 44) {
        m_dragging = true;
        m_dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragStartPos);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    QMainWindow::mouseReleaseEvent(event);
}
