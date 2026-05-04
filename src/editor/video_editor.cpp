#include "video_editor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>

VideoEditor::VideoEditor(QWidget* parent) : QWidget(parent) {
    m_playTimer = new QTimer(this);
    m_playTimer->setInterval(33);
    connect(m_playTimer, &QTimer::timeout, this, &VideoEditor::onPlaybackTick);
    setupUI();
}

void VideoEditor::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Title row
    auto* titleRow = new QHBoxLayout();
    auto* titleLabel = new QLabel("✂  Video Editor");
    QFont tf("Segoe UI", 16, QFont::Bold);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color: #F1F5F9;");

    auto* openBtn = new QPushButton("📂  Open File");
    openBtn->setFixedHeight(36);
    connect(openBtn, &QPushButton::clicked, [this]() {
        QString f = QFileDialog::getOpenFileName(this, "Open Video",
            QString(), "Video Files (*.mp4 *.mkv *.avi *.mov)");
        if (!f.isEmpty()) loadFile(f);
    });

    titleRow->addWidget(titleLabel);
    titleRow->addStretch();
    titleRow->addWidget(openBtn);
    layout->addLayout(titleRow);

    // Main splitter
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    // Left: Preview + controls
    auto* previewFrame = new QFrame();
    previewFrame->setStyleSheet("QFrame { background: #050508; border-radius: 12px; }");
    auto* previewLayout = new QVBoxLayout(previewFrame);
    previewLayout->setContentsMargins(0, 0, 0, 0);
    previewLayout->setSpacing(0);

    m_previewLabel = new QLabel("Open a video file to start editing");
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet("color: #475569; font-size: 14px;");
    m_previewLabel->setMinimumHeight(300);
    previewLayout->addWidget(m_previewLabel, 1);

    // Playback controls
    auto* controlsFrame = new QFrame();
    controlsFrame->setStyleSheet("QFrame { background: #0F0F1F; border-top: 1px solid #1E1B3A; "
                                 "border-radius: 0 0 12px 12px; padding: 8px; }");
    auto* ctrlLayout = new QVBoxLayout(controlsFrame);
    ctrlLayout->setSpacing(8);

    // Timeline
    auto* timeRow = new QHBoxLayout();
    m_positionLabel = new QLabel("00:00:00");
    m_positionLabel->setStyleSheet("color: #94A3B8; font-size: 11px; font-family: monospace;");
    m_timelineSlider = new QSlider(Qt::Horizontal);
    m_timelineSlider->setRange(0, 1000);
    m_durationLabel = new QLabel("00:00:00");
    m_durationLabel->setStyleSheet("color: #94A3B8; font-size: 11px; font-family: monospace;");
    connect(m_timelineSlider, &QSlider::valueChanged, this, &VideoEditor::onSeek);
    timeRow->addWidget(m_positionLabel);
    timeRow->addWidget(m_timelineSlider, 1);
    timeRow->addWidget(m_durationLabel);
    ctrlLayout->addLayout(timeRow);

    // Buttons
    auto* btnRow = new QHBoxLayout();
    btnRow->setSpacing(8);

    auto makeCtrlBtn = [](const QString& sym, const QString& tip) {
        auto* b = new QPushButton(sym);
        b->setFixedSize(36, 36);
        b->setToolTip(tip);
        b->setStyleSheet("QPushButton { background: #1A1A2E; border: 1px solid #2D2B55; border-radius: 8px; color: #CBD5E1; font-size: 14px; }"
                         "QPushButton:hover { background: #7C3AED; border-color: #9333EA; color: white; }");
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };

    auto* rewindBtn = makeCtrlBtn("⏮", "Rewind 10s");
    m_playBtn = makeCtrlBtn("▶", "Play/Pause");
    m_playBtn->setStyleSheet("QPushButton { background: #7C3AED; border: none; border-radius: 8px; color: white; font-size: 14px; }"
                             "QPushButton:hover { background: #6D28D9; }");
    auto* forwardBtn = makeCtrlBtn("⏭", "Forward 10s");

    connect(m_playBtn, &QPushButton::clicked, this, &VideoEditor::onPlayPause);
    connect(rewindBtn, &QPushButton::clicked, [this]() { m_position = qMax(0LL, m_position - 10000); });
    connect(forwardBtn, &QPushButton::clicked, [this]() { m_position = qMin(m_duration, m_position + 10000); });

    // Speed selector
    auto* speedCombo = new QComboBox();
    speedCombo->addItems({"0.25x", "0.5x", "0.75x", "1x", "1.25x", "1.5x", "2x"});
    speedCombo->setCurrentIndex(3);
    speedCombo->setFixedWidth(70);
    speedCombo->setFixedHeight(32);
    connect(speedCombo, &QComboBox::currentTextChanged, [this](const QString& t) {
        m_playbackSpeed = t.chopped(1).toDouble();
    });

    auto* volIcon = new QLabel("🔊");
    auto* volSlider = new QSlider(Qt::Horizontal);
    volSlider->setRange(0, 100);
    volSlider->setValue(100);
    volSlider->setFixedWidth(80);

    btnRow->addWidget(rewindBtn);
    btnRow->addWidget(m_playBtn);
    btnRow->addWidget(forwardBtn);
    btnRow->addStretch();
    btnRow->addWidget(speedCombo);
    btnRow->addWidget(volIcon);
    btnRow->addWidget(volSlider);
    ctrlLayout->addLayout(btnRow);

    previewLayout->addWidget(controlsFrame);
    splitter->addWidget(previewFrame);

    // Right: Tools panel
    auto* toolsScroll = new QScrollArea();
    toolsScroll->setWidgetResizable(true);
    toolsScroll->setFrameShape(QFrame::NoFrame);
    toolsScroll->setFixedWidth(200);

    auto* toolsContent = new QWidget();
    auto* toolsLayout = new QVBoxLayout(toolsContent);
    toolsLayout->setSpacing(8);
    toolsLayout->setContentsMargins(8, 0, 8, 0);

    auto* toolsTitle = new QLabel("Editing Tools");
    QFont ttf("Segoe UI", 12, QFont::Bold);
    toolsTitle->setFont(ttf);
    toolsTitle->setStyleSheet("color: #94A3B8;");
    toolsLayout->addWidget(toolsTitle);

    struct EdTool { QString icon, name; };
    QList<EdTool> edTools = {
        {"✂", "Trim"}, {"⊸", "Split"}, {"⊕", "Merge"},
        {"T", "Add Text"}, {"🎵", "Add Music"}, {"🖼", "Add Image"},
        {"⚡", "Speed"}, {"🎨", "Color"}, {"◎", "Blur"},
        {"🔊", "Audio Mix"}, {"📐", "Crop"}, {"↔", "Resize"}
    };

    for (auto& t : edTools) {
        auto* btn = new QPushButton(t.icon + "  " + t.name);
        btn->setFixedHeight(38);
        btn->setStyleSheet("QPushButton { background: #12122A; border: 1px solid #1E1B3A; border-radius: 8px; color: #CBD5E1; text-align: left; padding: 0 10px; }"
                           "QPushButton:hover { background: #1E1050; border-color: #7C3AED; color: #C084FC; }");
        btn->setCursor(Qt::PointingHandCursor);
        toolsLayout->addWidget(btn);
    }
    toolsLayout->addStretch();
    toolsScroll->setWidget(toolsContent);
    splitter->addWidget(toolsScroll);

    splitter->setSizes({650, 200});
    layout->addWidget(splitter, 1);

    // Bottom toolbar: Export
    auto* exportRow = new QHBoxLayout();
    m_trimBtn = new QPushButton("✂  Trim");
    m_splitBtn = new QPushButton("⊸  Split");
    m_exportBtn = new QPushButton("📤  Export Video");
    m_exportBtn->setObjectName("primaryBtn");

    for (auto* b : {m_trimBtn, m_splitBtn}) {
        b->setFixedHeight(40);
        b->setStyleSheet("QPushButton { background: #1A1A2E; border: 1px solid #2D2B55; border-radius: 8px; color: #CBD5E1; padding: 0 16px; }"
                         "QPushButton:hover { background: #2D2B55; }");
    }
    m_exportBtn->setFixedHeight(40);

    connect(m_trimBtn, &QPushButton::clicked, this, &VideoEditor::onTrim);
    connect(m_splitBtn, &QPushButton::clicked, this, &VideoEditor::onSplit);
    connect(m_exportBtn, &QPushButton::clicked, this, &VideoEditor::onExport);

    exportRow->addWidget(m_trimBtn);
    exportRow->addWidget(m_splitBtn);
    exportRow->addStretch();
    exportRow->addWidget(m_exportBtn);
    layout->addLayout(exportRow);
}

void VideoEditor::loadFile(const QString& filePath) {
    m_filePath = filePath;
    m_position = 0;
    m_duration = 600000; // Placeholder: 10 minutes
    m_previewLabel->setText("▶ " + QFileInfo(filePath).fileName());
    m_previewLabel->setStyleSheet("color: #C084FC; font-size: 13px;");
    m_durationLabel->setText(formatTime(m_duration));
}

void VideoEditor::onPlayPause() {
    m_playing = !m_playing;
    if (m_playing) { m_playTimer->start(); m_playBtn->setText("⏸"); }
    else { m_playTimer->stop(); m_playBtn->setText("▶"); }
}

void VideoEditor::onPlaybackTick() {
    m_position += (qint64)(33 * m_playbackSpeed);
    if (m_position >= m_duration) { m_position = 0; m_playing = false; m_playTimer->stop(); m_playBtn->setText("▶"); }
    m_timelineSlider->blockSignals(true);
    m_timelineSlider->setValue((int)(m_position * 1000 / qMax(1LL, m_duration)));
    m_timelineSlider->blockSignals(false);
    m_positionLabel->setText(formatTime(m_position));
}

void VideoEditor::onSeek(int value) {
    m_position = (qint64)value * m_duration / 1000;
    m_positionLabel->setText(formatTime(m_position));
}

void VideoEditor::onTrim() {}
void VideoEditor::onSplit() {}
void VideoEditor::onAddText() {}
void VideoEditor::onAddMusic() {}
void VideoEditor::onSpeedChange(double speed) { m_playbackSpeed = speed; }

void VideoEditor::onExport() {
    QString out = QFileDialog::getSaveFileName(this, "Export Video",
        QFileInfo(m_filePath).baseName() + "_edited",
        "Video Files (*.mp4 *.mkv *.avi)");
    if (!out.isEmpty()) emit exportRequested(out);
}

QString VideoEditor::formatTime(qint64 ms) const {
    qint64 s = ms / 1000, h = s / 3600, m = (s % 3600) / 60;
    return QString("%1:%2:%3").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s%60,2,10,QChar('0'));
}
