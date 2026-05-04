#include "hotkeys_page.h"
#include "../settings_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

HotkeysPage::HotkeysPage(QWidget* parent) : QWidget(parent) {
    setupUI();
    loadHotkeys();
}

void HotkeysPage::setupUI() {
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

    auto* titleLabel = new QLabel("⌨  Hotkeys");
    QFont tf; tf.setFamily("Segoe UI"); tf.setPointSize(18); tf.setBold(true);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color: #F1F5F9;");
    layout->addWidget(titleLabel);

    auto* infoLabel = new QLabel("Click a hotkey field and press your desired key combination.");
    infoLabel->setStyleSheet("color: #64748B; font-size: 12px;");
    layout->addWidget(infoLabel);

    auto* grp = new QGroupBox("Recording Hotkeys", content);
    grp->setStyleSheet(
        "QGroupBox { border: 1px solid #1E1B3A; border-radius: 12px; margin-top: 14px; "
        "padding: 14px; color: #7C3AED; font-weight: bold; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; }");
    auto* gl = new QVBoxLayout(grp);
    gl->setSpacing(14);

    const QString editStyle =
        "QKeySequenceEdit { background: #12122A; border: 1px solid #2D2B55; border-radius: 8px; "
        "color: #C084FC; padding: 6px 12px; font-size: 13px; font-weight: bold; }"
        "QKeySequenceEdit:focus { border-color: #7C3AED; }";

    auto addRow = [&](const QString& label, const QString& desc, QKeySequenceEdit*& edit) {
        auto* row = new QHBoxLayout();
        auto* col = new QVBoxLayout();
        auto* lbl = new QLabel(label);
        lbl->setStyleSheet("color: #E2E8F0; font-weight: bold;");
        auto* dlbl = new QLabel(desc);
        dlbl->setStyleSheet("color: #475569; font-size: 11px;");
        col->addWidget(lbl);
        col->addWidget(dlbl);
        col->setSpacing(2);
        edit = new QKeySequenceEdit();
        edit->setStyleSheet(editStyle);
        edit->setFixedWidth(220);
        row->addLayout(col, 1);
        row->addWidget(edit);
        gl->addLayout(row);
    };

    addRow("Start / Stop Recording", "Begin or end recording", m_startStopEdit);
    addRow("Pause / Resume",         "Pause or resume active recording", m_pauseEdit);
    addRow("Screenshot",             "Capture current screen", m_screenshotEdit);
    addRow("Cancel Recording",       "Stop and discard current recording", m_cancelEdit);
    addRow("Open Editor",            "Open the video editor", m_editorEdit);

    layout->addWidget(grp);
    layout->addStretch();

    // Save button
    auto* saveBtn = new QPushButton("💾  Save Hotkeys");
    saveBtn->setFixedHeight(42);
    saveBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #7C3AED,stop:1 #9333EA); "
        "border: none; border-radius: 10px; color: white; font-weight: bold; font-size: 14px; padding: 0 32px; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #6D28D9,stop:1 #7C3AED); }");
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &HotkeysPage::saveHotkeys);

    outerLayout->addWidget(saveBtn, 0, Qt::AlignRight);
}

void HotkeysPage::loadHotkeys() {
    auto& hk = SettingsManager::instance().hotkeys;
    m_startStopEdit->setKeySequence(hk.startStop);
    m_pauseEdit->setKeySequence(hk.pauseResume);
    m_screenshotEdit->setKeySequence(hk.screenshot);
    m_cancelEdit->setKeySequence(hk.cancelRecording);
    m_editorEdit->setKeySequence(hk.openEditor);
}

void HotkeysPage::saveHotkeys() {
    auto& hk = SettingsManager::instance().hotkeys;
    hk.startStop       = m_startStopEdit->keySequence();
    hk.pauseResume     = m_pauseEdit->keySequence();
    hk.screenshot      = m_screenshotEdit->keySequence();
    hk.cancelRecording = m_cancelEdit->keySequence();
    hk.openEditor      = m_editorEdit->keySequence();
    SettingsManager::instance().save();
    QMessageBox::information(this, "FrameX", "Hotkeys saved. Restart required to apply changes.");
}
