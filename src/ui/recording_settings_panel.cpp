#include "recording_settings_panel.h"
#include "../settings_manager.h"
#include <QFileDialog>
#include <QScrollArea>
#include <QFrame>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QSpacerItem>
#include <QPainter>

RecordingSettingsPanel::RecordingSettingsPanel(QWidget* parent) : QWidget(parent) {
    setupUI();
    loadSettings();
}

void RecordingSettingsPanel::setupUI() {
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scroll);

    auto* content = new QWidget();
    scroll->setWidget(content);

    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(14);

    // Header
    auto* header = new QHBoxLayout();
    auto* titleLabel = new QLabel("⚙  Recording Settings");
    QFont tf("Segoe UI", 13, QFont::Bold);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color: #E2E8F0;");
    auto* closeBtn = new QPushButton("✕");
    closeBtn->setFixedSize(24, 24);
    closeBtn->setStyleSheet("QPushButton { background: transparent; border: none; color: #64748B; font-size: 14px; }"
                            "QPushButton:hover { color: #EF4444; }");
    header->addWidget(titleLabel);
    header->addStretch();
    header->addWidget(closeBtn);
    layout->addLayout(header);

    auto* divider = new QFrame(); divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("color: #1E1B3A;");
    layout->addWidget(divider);

    // Section helper
    auto addSection = [&](const QString& title) {
        auto* lbl = new QLabel(title);
        lbl->setStyleSheet("color: #475569; font-size: 11px; font-weight: bold;");
        layout->addWidget(lbl);
    };

    // Video Quality
    addSection("VIDEO QUALITY");
    m_qualityCombo = new QComboBox();
    m_qualityCombo->addItems({"720p (HD)", "1080p (Full HD)", "2K (QHD)", "4K (Ultra HD)"});
    m_qualityCombo->setCurrentIndex(1);
    layout->addWidget(m_qualityCombo);

    // FPS
    addSection("FPS");
    auto* fpsRow = new QHBoxLayout();
    m_fps30Btn = new QPushButton("30");
    m_fps60Btn = new QPushButton("60");
    m_fps120Btn = new QPushButton("120");
    for (auto* b : {m_fps30Btn, m_fps60Btn, m_fps120Btn}) {
        b->setCheckable(true);
        b->setFixedHeight(32);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(
            "QPushButton { background: #12122A; border: 1px solid #2D2B55; border-radius: 6px; color: #94A3B8; }"
            "QPushButton:checked { background: #7C3AED; border-color: #9333EA; color: white; font-weight: bold; }");
        fpsRow->addWidget(b);
    }
    m_fps60Btn->setChecked(true);

    auto* fpsBtnGroup = new QButtonGroup(this);
    fpsBtnGroup->setExclusive(true);
    fpsBtnGroup->addButton(m_fps30Btn, 30);
    fpsBtnGroup->addButton(m_fps60Btn, 60);
    fpsBtnGroup->addButton(m_fps120Btn, 120);
    connect(fpsBtnGroup, &QButtonGroup::idClicked, [](int fps) {
        SettingsManager::instance().recording.fps = fps;
    });
    layout->addLayout(fpsRow);

    // Encoder
    addSection("VIDEO ENCODER");
    m_encoderCombo = new QComboBox();
    m_encoderCombo->addItems({
        "H.264 (NVIDIA NVENC)",
        "H.265 (NVIDIA NVENC)",
        "H.264 (AMD AMF)",
        "H.265 (AMD AMF)",
        "H.264 (Intel QSV)",
        "H.264 (Software)",
        "H.265 (Software)"
    });
    layout->addWidget(m_encoderCombo);

    auto* divider2 = new QFrame(); divider2->setFrameShape(QFrame::HLine);
    divider2->setStyleSheet("color: #1E1B3A;");
    layout->addWidget(divider2);

    // Audio Sources
    addSection("AUDIO SOURCES");

    auto addAudioRow = [&](const QString& icon, const QString& name, QCheckBox*& chk, QSlider*& slider) {
        auto* row = new QHBoxLayout();
        chk = new QCheckBox(icon + "  " + name);
        chk->setChecked(true);
        chk->setStyleSheet("color: #CBD5E1;");
        auto* toggle = new QPushButton("●");
        toggle->setFixedSize(36, 20);
        toggle->setStyleSheet(
            "QPushButton { background: #7C3AED; border-radius: 10px; color: white; font-size: 10px; border: none; }"
            "QPushButton:checked { background: #2D2B55; }");
        row->addWidget(chk);
        row->addStretch();
        row->addWidget(toggle);
        layout->addLayout(row);

        slider = new QSlider(Qt::Horizontal);
        slider->setRange(0, 100);
        slider->setValue(80);
        layout->addWidget(slider);
    };

    addAudioRow("🔊", "System Audio", m_sysAudioCheck, m_sysAudioSlider);
    addAudioRow("🎙", "Microphone", m_micCheck, m_micSlider);
    // Mic device label
    auto* micDevLabel = new QLabel("Blue Yeti");
    micDevLabel->setStyleSheet("color: #7C3AED; font-size: 11px;");
    layout->addWidget(micDevLabel);

    auto* divider3 = new QFrame(); divider3->setFrameShape(QFrame::HLine);
    divider3->setStyleSheet("color: #1E1B3A;");
    layout->addWidget(divider3);

    // Webcam
    addSection("WEBCAM");
    auto* webcamRow = new QHBoxLayout();
    m_webcamCombo = new QComboBox();
    m_webcamCombo->addItems({"Logitech C922", "Logitech C920", "Built-in Camera"});
    auto* webcamSettingsBtn = new QPushButton("⚙");
    webcamSettingsBtn->setFixedSize(32, 32);
    webcamSettingsBtn->setStyleSheet("QPushButton { background: #12122A; border: 1px solid #2D2B55;"
                                     "border-radius: 6px; color: #94A3B8; }"
                                     "QPushButton:hover { background: #1E1E3A; }");
    webcamRow->addWidget(m_webcamCombo);
    webcamRow->addWidget(webcamSettingsBtn);
    layout->addLayout(webcamRow);

    auto* divider4 = new QFrame(); divider4->setFrameShape(QFrame::HLine);
    divider4->setStyleSheet("color: #1E1B3A;");
    layout->addWidget(divider4);

    // Output Format
    addSection("OUTPUT FORMAT");
    auto* fmtRow = new QHBoxLayout();
    m_mp4Btn = new QPushButton("MP4");
    m_mkvBtn = new QPushButton("MKV");
    m_aviBtn = new QPushButton("AVI");
    auto* fmtGroup = new QButtonGroup(this);
    fmtGroup->setExclusive(true);
    for (auto* b : {m_mp4Btn, m_mkvBtn, m_aviBtn}) {
        b->setCheckable(true);
        b->setFixedHeight(32);
        b->setCursor(Qt::PointingHandCursor);
        b->setStyleSheet(
            "QPushButton { background: #12122A; border: 1px solid #2D2B55; border-radius: 6px; color: #94A3B8; }"
            "QPushButton:checked { background: #7C3AED; border-color: #9333EA; color: white; font-weight: bold; }");
        fmtRow->addWidget(b);
        fmtGroup->addButton(b);
    }
    m_mp4Btn->setChecked(true);
    layout->addLayout(fmtRow);

    // Save Location
    addSection("SAVE LOCATION");
    auto* saveRow = new QHBoxLayout();
    m_savePathLabel = new QLabel("D:\\Videos\\FrameX Recordings");
    m_savePathLabel->setStyleSheet("color: #64748B; font-size: 11px;");
    m_savePathLabel->setWordWrap(true);
    m_browseBtn = new QPushButton("📁");
    m_browseBtn->setFixedSize(32, 32);
    m_browseBtn->setStyleSheet("QPushButton { background: #12122A; border: 1px solid #2D2B55;"
                               "border-radius: 6px; color: #94A3B8; }"
                               "QPushButton:hover { background: #1E1E3A; }");
    connect(m_browseBtn, &QPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Save Folder",
            SettingsManager::instance().recording.savePath);
        if (!dir.isEmpty()) {
            SettingsManager::instance().recording.savePath = dir;
            m_savePathLabel->setText(dir);
        }
    });
    saveRow->addWidget(m_savePathLabel, 1);
    saveRow->addWidget(m_browseBtn);
    layout->addLayout(saveRow);

    auto* divider5 = new QFrame(); divider5->setFrameShape(QFrame::HLine);
    divider5->setStyleSheet("color: #1E1B3A;");
    layout->addWidget(divider5);

    // Quick Tools
    addSection("QUICK TOOLS");
    auto* quickGrid = new QGridLayout();
    quickGrid->setSpacing(8);

    struct QuickTool { QString icon; QString name; };
    QList<QuickTool> tools = {
        {"✂", "Trim Video"}, {"T", "Add Text"}, {"♪", "Add Music"},
        {"◎", "Blur Area"}, {"⇄", "Converter"}, {"AI", "Voice to Text"}
    };

    for (int i = 0; i < tools.size(); ++i) {
        auto* tb = new QPushButton(tools[i].icon + "\n" + tools[i].name);
        tb->setFixedSize(76, 60);
        tb->setStyleSheet("QPushButton { background: #12122A; border: 1px solid #2D2B55;"
                          "border-radius: 8px; color: #94A3B8; font-size: 11px; }"
                          "QPushButton:hover { background: #1E1E3A; color: #C084FC; border-color: #7C3AED; }");
        tb->setCursor(Qt::PointingHandCursor);
        quickGrid->addWidget(tb, i / 3, i % 3);
    }
    layout->addLayout(quickGrid);
    layout->addStretch();

    connect(m_qualityCombo, &QComboBox::currentTextChanged, [](const QString& t) {
        auto& s = SettingsManager::instance().recording;
        if (t.contains("720p")) s.videoQuality = "720p";
        else if (t.contains("1080p")) s.videoQuality = "1080p";
        else if (t.contains("2K")) s.videoQuality = "2K";
        else s.videoQuality = "4K";
    });
}

void RecordingSettingsPanel::loadSettings() {
    auto& s = SettingsManager::instance().recording;
    m_savePathLabel->setText(s.savePath);
}

void RecordingSettingsPanel::saveSettings() {
    SettingsManager::instance().save();
}
