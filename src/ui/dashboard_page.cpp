#include "dashboard_page.h"
#include "../settings_manager.h"
#include "../history_manager.h"
#include <QScrollArea>
#include <QFrame>
#include <QGridLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <QFileInfo>
#include <QSpacerItem>
#include <QApplication>
#include <QScreen>

DashboardPage::DashboardPage(Recorder* recorder, QWidget* parent)
    : QWidget(parent), m_recorder(recorder)
{
    setupUI();

    connect(m_recorder, &Recorder::statsUpdated, this, &DashboardPage::onRecordingStats);

    m_glowTimer = new QTimer(this);
    connect(m_glowTimer, &QTimer::timeout, [this]() {
        if (m_isRecording) {
            if (m_glowUp) { m_glowValue += 5; if (m_glowValue >= 100) m_glowUp = false; }
            else { m_glowValue -= 5; if (m_glowValue <= 30) m_glowUp = true; }
            m_recordBtn->setStyleSheet(QString(
                "QPushButton#recordBtn { "
                "background: qradialgradient(cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, "
                "stop:0 #EF4444, stop:0.6 #DC2626, stop:1 #B91C1C);"
                "border: 3px solid rgba(248,113,113,%1);"
                "border-radius: 50px; color: white; font-size: 16px; font-weight: bold;"
                "min-width: 100px; min-height: 100px; }").arg(m_glowValue * 2 + 55));
        }
    });
    m_glowTimer->start(50);
}

void DashboardPage::setupUI() {
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scroll);

    auto* content = new QWidget();
    scroll->setWidget(content);

    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(20);

    // Title bar (custom window frame)
    auto* titleBar = new QWidget(content);
    titleBar->setFixedHeight(42);
    auto* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    auto* homeLabel = new QLabel("🏠  Home");
    homeLabel->setStyleSheet(
        "background: #1A1A2E; color: #CBD5E1; border-radius: 8px;"
        "padding: 6px 14px; font-size: 13px;");

    titleLayout->addWidget(homeLabel);
    titleLayout->addStretch();

    // Window controls
    auto makeWinBtn = [](const QString& sym, const QString& hover) {
        auto* b = new QPushButton(sym);
        b->setFixedSize(28, 28);
        b->setStyleSheet(QString(
            "QPushButton { background: #1A1A2E; color: #94A3B8; border: none; border-radius: 6px; font-size: 14px; }"
            "QPushButton:hover { background: %1; color: white; }").arg(hover));
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };
    auto* settingsWinBtn = makeWinBtn("⚙", "#2D2B55");
    auto* notifBtn = makeWinBtn("🔔", "#2D2B55");
    auto* minBtn = makeWinBtn("—", "#2D2B55");
    auto* maxBtn = makeWinBtn("□", "#2D2B55");
    auto* closeBtn = makeWinBtn("✕", "#EF4444");

    connect(minBtn, &QPushButton::clicked, [this]() { window()->showMinimized(); });
    connect(maxBtn, &QPushButton::clicked, [this]() {
        if (window()->isMaximized()) window()->showNormal();
        else window()->showMaximized();
    });
    connect(closeBtn, &QPushButton::clicked, [this]() { window()->close(); });

    titleLayout->addWidget(settingsWinBtn);
    titleLayout->addWidget(notifBtn);
    titleLayout->addSpacing(8);
    titleLayout->addWidget(minBtn);
    titleLayout->addWidget(maxBtn);
    titleLayout->addWidget(closeBtn);
    layout->addWidget(titleBar);

    // Welcome
    auto* welcomeLabel = new QLabel("Welcome back! 👋");
    QFont wf("Segoe UI", 20, QFont::Bold);
    welcomeLabel->setFont(wf);
    welcomeLabel->setStyleSheet("color: #F1F5F9;");

    auto* subLabel = new QLabel("Capture everything. Share anything.");
    subLabel->setStyleSheet("color: #64748B; font-size: 13px;");
    layout->addWidget(welcomeLabel);
    layout->addWidget(subLabel);

    // Main recording area
    auto* mainRow = new QHBoxLayout();
    mainRow->setSpacing(20);

    // Left: Capture type buttons
    auto* leftCol = new QVBoxLayout();
    leftCol->setSpacing(10);

    setupCaptureButtons();
    leftCol->addWidget(m_fullscreenBtn);
    leftCol->addWidget(m_windowBtn);
    leftCol->addWidget(m_customAreaBtn);
    leftCol->addStretch();

    // Center: Record button circle
    auto* centerCol = new QVBoxLayout();
    centerCol->setAlignment(Qt::AlignCenter);
    setupRecordButton();
    centerCol->addWidget(m_recordBtn, 0, Qt::AlignCenter);
    m_recStatusLabel = new QLabel("Start Recording");
    m_recStatusLabel->setAlignment(Qt::AlignCenter);
    m_recStatusLabel->setStyleSheet("color: #64748B; font-size: 13px;");
    m_recTimeLabel = new QLabel("");
    m_recTimeLabel->setAlignment(Qt::AlignCenter);
    m_recTimeLabel->setStyleSheet("color: #C084FC; font-size: 20px; font-weight: bold;");
    centerCol->addWidget(m_recStatusLabel);
    centerCol->addWidget(m_recTimeLabel);

    // Right: Audio toggles
    auto* rightCol = new QVBoxLayout();
    rightCol->setSpacing(10);
    setupAudioWidgets();
    rightCol->addWidget(m_webcamBtn);
    rightCol->addWidget(m_micBtn);
    rightCol->addWidget(m_sysAudioBtn);
    rightCol->addStretch();

    // Wrap in a styled frame with glow
    auto* recordFrame = new QFrame(content);
    recordFrame->setStyleSheet(
        "QFrame { background: #0F0F1F; border: 1px solid #1E1B3A; border-radius: 16px; padding: 20px; }");
    auto* rfLayout = new QHBoxLayout(recordFrame);
    rfLayout->setSpacing(20);
    rfLayout->addLayout(leftCol, 2);
    rfLayout->addLayout(centerCol, 3);
    rfLayout->addLayout(rightCol, 2);
    layout->addWidget(recordFrame);

    // Schedule + Editor buttons
    auto* actionRow = new QHBoxLayout();
    m_scheduleBtn = new QPushButton("🕐  Schedule Recording");
    m_scheduleBtn->setObjectName("captureTypeBtn");
    m_scheduleBtn->setFixedHeight(44);

    m_editorBtn = new QPushButton("✂  Go to Editor");
    m_editorBtn->setObjectName("primaryBtn");
    m_editorBtn->setFixedHeight(44);

    actionRow->addWidget(m_scheduleBtn);
    actionRow->addWidget(m_editorBtn);
    layout->addLayout(actionRow);

    // Recent Recordings
    auto* recentHeader = new QHBoxLayout();
    auto* recentTitle = new QLabel("Recent Recordings");
    QFont rf2("Segoe UI", 14, QFont::Bold);
    recentTitle->setFont(rf2);
    recentTitle->setStyleSheet("color: #F1F5F9;");
    auto* viewAllBtn = new QPushButton("View all");
    viewAllBtn->setStyleSheet("color: #7C3AED; background: transparent; border: none; font-size: 13px;");
    viewAllBtn->setCursor(Qt::PointingHandCursor);
    recentHeader->addWidget(recentTitle);
    recentHeader->addStretch();
    recentHeader->addWidget(viewAllBtn);
    layout->addLayout(recentHeader);

    setupRecentRecordings();
    layout->addWidget(m_recentContainer);

    // Stats bar
    setupStatsBar();
}

void DashboardPage::setupCaptureButtons() {
    m_captureGroup = new QButtonGroup(this);
    m_captureGroup->setExclusive(true);

    m_fullscreenBtn = new QPushButton("🖥   Full Screen");
    m_fullscreenBtn->setObjectName("captureTypeBtn");
    m_fullscreenBtn->setCheckable(true);
    m_fullscreenBtn->setChecked(true);
    m_fullscreenBtn->setFixedHeight(50);
    m_fullscreenBtn->setCursor(Qt::PointingHandCursor);

    m_windowBtn = new QPushButton("☐   Window");
    m_windowBtn->setObjectName("captureTypeBtn");
    m_windowBtn->setCheckable(true);
    m_windowBtn->setFixedHeight(50);
    m_windowBtn->setCursor(Qt::PointingHandCursor);

    m_customAreaBtn = new QPushButton("⊹   Custom Area");
    m_customAreaBtn->setObjectName("captureTypeBtn");
    m_customAreaBtn->setCheckable(true);
    m_customAreaBtn->setFixedHeight(50);
    m_customAreaBtn->setCursor(Qt::PointingHandCursor);

    m_captureGroup->addButton(m_fullscreenBtn, 0);
    m_captureGroup->addButton(m_windowBtn, 1);
    m_captureGroup->addButton(m_customAreaBtn, 2);

    connect(m_captureGroup, &QButtonGroup::idClicked, [this](int id) {
        auto& s = SettingsManager::instance().recording;
        if (id == 0) s.captureMode = "fullscreen";
        else if (id == 1) s.captureMode = "window";
        else s.captureMode = "custom";
    });
}

void DashboardPage::setupRecordButton() {
    m_recordBtn = new QPushButton("REC");
    m_recordBtn->setObjectName("recordBtn");
    m_recordBtn->setFixedSize(130, 130);
    m_recordBtn->setCursor(Qt::PointingHandCursor);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(m_recordBtn);
    shadow->setBlurRadius(40);
    shadow->setColor(QColor(239, 68, 68, 150));
    shadow->setOffset(0, 0);
    m_recordBtn->setGraphicsEffect(shadow);

    connect(m_recordBtn, &QPushButton::clicked, [this]() {
        if (!m_isRecording) {
            m_isRecording = true;
            m_recordBtn->setText("■");
            m_recStatusLabel->setText("Recording...");
            emit startRecordingRequested();
        } else {
            m_isRecording = false;
            m_recordBtn->setText("REC");
            m_recStatusLabel->setText("Start Recording");
            m_recTimeLabel->clear();
        }
    });
}

void DashboardPage::setupAudioWidgets() {
    auto makeToggle = [this](const QString& icon, const QString& text, bool& stateRef) {
        auto* btn = new QPushButton(icon + "  " + text + "\nOn");
        btn->setObjectName("captureTypeBtn");
        btn->setCheckable(true);
        btn->setChecked(true);
        btn->setFixedHeight(56);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::toggled, [btn, text, icon, &stateRef](bool checked) {
            stateRef = checked;
            btn->setText(icon + "  " + text + "\n" + (checked ? "On" : "Off"));
        });
        return btn;
    };

    m_webcamBtn = makeToggle("📷", "Webcam", SettingsManager::instance().recording.webcamEnabled);
    m_micBtn = makeToggle("🎙", "Microphone", SettingsManager::instance().recording.microphone);
    m_sysAudioBtn = makeToggle("🔊", "System Audio", SettingsManager::instance().recording.systemAudio);
}

void DashboardPage::setupRecentRecordings() {
    m_recentContainer = new QWidget(this);
    m_recentLayout = new QHBoxLayout(m_recentContainer);
    m_recentLayout->setSpacing(12);
    m_recentLayout->setContentsMargins(0, 0, 0, 0);

    // Placeholder recent recordings
    QList<QPair<QString, QString>> samples = {
        {"Gameplay_2024", "00:12:45"},
        {"Tutorial_video", "00:08:32"},
        {"Meeting_recording", "00:24:19"},
        {"Live_stream", "01:35:42"},
    };
    QList<QString> colors = {"#1A0A2E", "#0A1A2E", "#1A1A0A", "#2E0A0A"};

    for (int i = 0; i < samples.size(); ++i) {
        auto* card = new QFrame(m_recentContainer);
        card->setFixedSize(170, 130);
        card->setStyleSheet(QString("QFrame { background: %1; border: 1px solid #1E1B3A; "
                                   "border-radius: 10px; }").arg(colors[i]));
        card->setCursor(Qt::PointingHandCursor);

        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(10, 8, 10, 8);
        cl->setSpacing(4);

        auto* timeLabel = new QLabel(samples[i].second);
        timeLabel->setStyleSheet("color: white; font-size: 11px; font-weight: bold;"
                                 "background: rgba(0,0,0,0.6); border-radius: 4px; padding: 2px 5px;");
        timeLabel->setFixedHeight(20);

        cl->addWidget(timeLabel, 0, Qt::AlignRight | Qt::AlignTop);
        cl->addStretch();

        auto* nameLabel = new QLabel(samples[i].first + ".mp4");
        nameLabel->setStyleSheet("color: #E2E8F0; font-size: 11px; font-weight: bold;");
        nameLabel->setWordWrap(true);

        auto* dateLabel = new QLabel(QDate::currentDate().toString("dd/MM/yyyy") + " · 1080p 60fps");
        dateLabel->setStyleSheet("color: #64748B; font-size: 10px;");

        cl->addWidget(nameLabel);
        cl->addWidget(dateLabel);
        m_recentLayout->addWidget(card);
    }
    m_recentLayout->addStretch();
}

void DashboardPage::setupStatsBar() {
    // Stats bar at bottom - will be part of main window status bar
}

void DashboardPage::updateRecordingTime(qint64 secs) {
    m_recTimeLabel->setText(formatTime(secs));
}

void DashboardPage::onSettingsChanged() {
    // Refresh UI based on new settings
}

void DashboardPage::onRecordingStats(double cpu, double gpu, int fps, qint64 disk) {
    Q_UNUSED(cpu) Q_UNUSED(gpu) Q_UNUSED(fps) Q_UNUSED(disk)
}

QString DashboardPage::formatTime(qint64 secs) const {
    int h = secs / 3600;
    int m = (secs % 3600) / 60;
    int s = secs % 60;
    return QString("%1:%2:%3").arg(h, 2, 10, QChar('0')).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

QString DashboardPage::formatSize(qint64 bytes) const {
    if (bytes < 1024) return QString("%1 B").arg(bytes);
    if (bytes < 1048576) return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    if (bytes < 1073741824) return QString("%1 MB").arg(bytes / 1048576.0, 0, 'f', 1);
    return QString("%1 GB").arg(bytes / 1073741824.0, 0, 'f', 2);
}
