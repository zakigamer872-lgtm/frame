#include "settings_page.h"
#include "../settings_manager.h"
#include "../theme_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>
#include <QApplication>
#include <QLabel>

SettingsPage::SettingsPage(QWidget* parent) : QWidget(parent) {
    setupUI();
    loadSettings();
}

void SettingsPage::setupUI() {
    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(24, 24, 24, 24);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outerLayout->addWidget(scroll);

    auto* content = new QWidget();
    scroll->setWidget(content);
    auto* layout = new QVBoxLayout(content);
    layout->setSpacing(16);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* titleLabel = new QLabel("⚙  Settings");
    QFont tf; tf.setFamily("Segoe UI"); tf.setPointSize(18); tf.setBold(true);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color: #F1F5F9;");
    layout->addWidget(titleLabel);

    const QString grpStyle =
        "QGroupBox { border: 1px solid #1E1B3A; border-radius: 12px; margin-top: 14px; "
        "padding: 12px; color: #7C3AED; font-weight: bold; font-size: 13px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 6px; }";

    const QString chkStyle =
        "QCheckBox { color: #CBD5E1; spacing: 10px; }"
        "QCheckBox::indicator { width: 18px; height: 18px; border-radius: 4px; "
        "border: 2px solid #2D2B55; background: #12122A; }"
        "QCheckBox::indicator:checked { background: #7C3AED; border-color: #7C3AED; }";

    auto makeRow = [&](const QString& label, QWidget* widget) -> QHBoxLayout* {
        auto* row = new QHBoxLayout();
        auto* lbl = new QLabel(label);
        lbl->setStyleSheet("color: #94A3B8; min-width: 180px;");
        row->addWidget(lbl);
        row->addWidget(widget, 1);
        return row;
    };

    // --- General ---
    auto* genGrp = new QGroupBox("General", content);
    genGrp->setStyleSheet(grpStyle);
    auto* genLayout = new QVBoxLayout(genGrp);
    genLayout->setSpacing(12);

    m_themeCombo = new QComboBox();
    m_themeCombo->addItems({"Dark", "Light"});
    m_themeCombo->setStyleSheet(
        "QComboBox { background: #12122A; border: 1px solid #2D2B55; border-radius: 8px; "
        "color: #CBD5E1; padding: 6px 12px; } QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background: #12122A; color: #CBD5E1; }");

    m_languageCombo = new QComboBox();
    m_languageCombo->addItems({"English", "Arabic (عربي)"});
    m_languageCombo->setStyleSheet(m_themeCombo->styleSheet());

    genLayout->addLayout(makeRow("Theme:", m_themeCombo));
    genLayout->addLayout(makeRow("Language:", m_languageCombo));
    layout->addWidget(genGrp);

    // --- Behaviour ---
    auto* behGrp = new QGroupBox("Behaviour", content);
    behGrp->setStyleSheet(grpStyle);
    auto* behLayout = new QVBoxLayout(behGrp);
    behLayout->setSpacing(10);

    m_minimizeTrayCheck = new QCheckBox("Minimize to system tray on close");
    m_startBootCheck    = new QCheckBox("Start FrameX with Windows");
    m_floatingWidgetCheck = new QCheckBox("Show floating recording widget");
    m_hardwareAccelCheck  = new QCheckBox("Enable hardware acceleration (GPU rendering)");

    for (auto* c : {m_minimizeTrayCheck, m_startBootCheck, m_floatingWidgetCheck, m_hardwareAccelCheck})
        c->setStyleSheet(chkStyle);

    behLayout->addWidget(m_minimizeTrayCheck);
    behLayout->addWidget(m_startBootCheck);
    behLayout->addWidget(m_floatingWidgetCheck);
    behLayout->addWidget(m_hardwareAccelCheck);
    layout->addWidget(behGrp);

    // --- Notifications ---
    auto* notifGrp = new QGroupBox("Notifications", content);
    notifGrp->setStyleSheet(grpStyle);
    auto* notifLayout = new QVBoxLayout(notifGrp);

    m_autoSaveCheck = new QCheckBox("Show system notifications (recording saved, screenshot taken)");
    m_autoSaveCheck->setStyleSheet(chkStyle);
    notifLayout->addWidget(m_autoSaveCheck);
    layout->addWidget(notifGrp);

    layout->addStretch();

    // --- Buttons ---
    auto* btnRow = new QHBoxLayout();
    m_resetBtn = new QPushButton("Reset to Defaults");
    m_resetBtn->setFixedHeight(40);
    m_resetBtn->setStyleSheet(
        "QPushButton { background: #1A0A0A; border: 1px solid #7F1D1D; border-radius: 8px; "
        "color: #FCA5A5; padding: 0 20px; } QPushButton:hover { background: #7F1D1D; color: white; }");
    m_resetBtn->setCursor(Qt::PointingHandCursor);

    m_saveBtn = new QPushButton("💾  Save Settings");
    m_saveBtn->setFixedHeight(40);
    m_saveBtn->setStyleSheet(
        "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #7C3AED,stop:1 #9333EA); "
        "border: none; border-radius: 8px; color: white; font-weight: bold; padding: 0 28px; }"
        "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #6D28D9,stop:1 #7C3AED); }");
    m_saveBtn->setCursor(Qt::PointingHandCursor);

    connect(m_saveBtn,  &QPushButton::clicked, this, &SettingsPage::saveSettings);
    connect(m_resetBtn, &QPushButton::clicked, this, [this](){
        if (QMessageBox::question(this, "FrameX", "Reset all settings to defaults?",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            SettingsManager::instance().resetToDefaults();
            loadSettings();
        }
    });

    btnRow->addWidget(m_resetBtn);
    btnRow->addStretch();
    btnRow->addWidget(m_saveBtn);
    outerLayout->addLayout(btnRow);
}

void SettingsPage::loadSettings() {
    auto& s = SettingsManager::instance();
    m_themeCombo->setCurrentText(s.app.theme == "dark" ? "Dark" : "Light");
    m_minimizeTrayCheck->setChecked(s.app.minimizeToTray);
    m_startBootCheck->setChecked(s.app.startOnBoot);
    m_floatingWidgetCheck->setChecked(s.app.showFloatingWidget);
    m_hardwareAccelCheck->setChecked(s.app.hardwareAcceleration);
    m_autoSaveCheck->setChecked(s.app.showNotifications);
}

void SettingsPage::saveSettings() {
    auto& s = SettingsManager::instance();
    s.app.theme               = (m_themeCombo->currentIndex() == 0) ? "dark" : "light";
    s.app.minimizeToTray      = m_minimizeTrayCheck->isChecked();
    s.app.startOnBoot         = m_startBootCheck->isChecked();
    s.app.showFloatingWidget  = m_floatingWidgetCheck->isChecked();
    s.app.hardwareAcceleration = m_hardwareAccelCheck->isChecked();
    s.app.showNotifications   = m_autoSaveCheck->isChecked();
    s.save();

    if (s.app.theme == "dark")
        ThemeManager::instance().applyTheme(*qApp);
    else
        qApp->setStyleSheet("");

    QMessageBox::information(this, "FrameX", "Settings saved successfully.");
}
