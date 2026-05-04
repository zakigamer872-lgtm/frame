#include "streaming_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QCheckBox>

StreamingPage::StreamingPage(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void StreamingPage::setupUI() {
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(24, 24, 24, 24);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outerLayout->addWidget(scroll);

    auto* content = new QWidget();
    scroll->setWidget(content);
    auto* layout = new QVBoxLayout(content);
    layout->setSpacing(20);

    auto* titleLabel = new QLabel("📡  Live Streaming");
    QFont tf("Segoe UI", 18, QFont::Bold);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color: #F1F5F9;");
    layout->addWidget(titleLabel);

    // Platform selection
    auto* grp = new QGroupBox("Stream Settings");
    grp->setStyleSheet("QGroupBox { border: 1px solid #1E1B3A; border-radius: 12px; margin-top: 12px; padding: 16px; color: #7C3AED; font-weight: bold; }"
                       "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }");
    auto* grpLayout = new QVBoxLayout(grp);
    grpLayout->setSpacing(12);

    auto addRow = [&](const QString& lbl, QWidget* w) {
        auto* row = new QHBoxLayout();
        auto* lb = new QLabel(lbl);
        lb->setStyleSheet("color: #CBD5E1;");
        lb->setFixedWidth(120);
        row->addWidget(lb);
        row->addWidget(w, 1);
        grpLayout->addLayout(row);
    };

    m_platformCombo = new QComboBox();
    m_platformCombo->addItems({"YouTube Live", "Twitch", "TikTok Live", "Facebook Live", "Custom RTMP"});
    addRow("Platform:", m_platformCombo);

    m_streamKeyEdit = new QLineEdit();
    m_streamKeyEdit->setPlaceholderText("Enter your stream key...");
    m_streamKeyEdit->setEchoMode(QLineEdit::Password);
    m_streamKeyEdit->setFixedHeight(38);
    addRow("Stream Key:", m_streamKeyEdit);

    auto* showKeyCheck = new QCheckBox("Show key");
    showKeyCheck->setStyleSheet("color: #64748B;");
    connect(showKeyCheck, &QCheckBox::toggled, [this](bool show) {
        m_streamKeyEdit->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
    });
    grpLayout->addWidget(showKeyCheck);

    m_qualityCombo = new QComboBox();
    m_qualityCombo->addItems({"720p 30fps", "1080p 30fps", "1080p 60fps", "1440p 60fps"});
    addRow("Quality:", m_qualityCombo);

    auto* bitrateCombo = new QComboBox();
    bitrateCombo->addItems({"2500 Kbps", "4000 Kbps", "6000 Kbps", "8000 Kbps"});
    addRow("Bitrate:", bitrateCombo);

    layout->addWidget(grp);

    // Status
    m_statusLabel = new QLabel("● Not streaming");
    m_statusLabel->setStyleSheet("color: #64748B; font-size: 13px;");
    layout->addWidget(m_statusLabel);

    // Buttons
    auto* btnRow = new QHBoxLayout();
    m_startBtn = new QPushButton("▶  Start Streaming");
    m_startBtn->setObjectName("primaryBtn");
    m_startBtn->setFixedHeight(44);
    connect(m_startBtn, &QPushButton::clicked, this, &StreamingPage::startStream);

    m_stopBtn = new QPushButton("■  Stop Streaming");
    m_stopBtn->setFixedHeight(44);
    m_stopBtn->setEnabled(false);
    m_stopBtn->setStyleSheet("QPushButton { background: #2D1010; color: #EF4444; border: 1px solid #7F1D1D; border-radius: 8px; padding: 0 20px; }"
                             "QPushButton:hover { background: #7F1D1D; color: white; }"
                             "QPushButton:disabled { opacity: 0.4; }");
    connect(m_stopBtn, &QPushButton::clicked, this, &StreamingPage::stopStream);

    btnRow->addWidget(m_startBtn);
    btnRow->addWidget(m_stopBtn);
    btnRow->addStretch();
    layout->addLayout(btnRow);
    layout->addStretch();
}

void StreamingPage::startStream() {
    if (m_streamKeyEdit->text().isEmpty()) {
        m_statusLabel->setText("⚠ Please enter a stream key first.");
        m_statusLabel->setStyleSheet("color: #F59E0B; font-size: 13px;");
        return;
    }
    m_streaming = true;
    m_statusLabel->setText("● LIVE — Streaming to " + m_platformCombo->currentText());
    m_statusLabel->setStyleSheet("color: #EF4444; font-size: 13px; font-weight: bold;");
    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(true);
}

void StreamingPage::stopStream() {
    m_streaming = false;
    m_statusLabel->setText("● Not streaming");
    m_statusLabel->setStyleSheet("color: #64748B; font-size: 13px;");
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
}
