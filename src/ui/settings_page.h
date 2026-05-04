#pragma once
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class SettingsPage : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPage(QWidget* parent = nullptr);

private:
    void setupUI();
    void loadSettings();
    void saveSettings();

    QCheckBox* m_minimizeTrayCheck;
    QCheckBox* m_startBootCheck;
    QCheckBox* m_floatingWidgetCheck;
    QCheckBox* m_hardwareAccelCheck;
    QCheckBox* m_autoSaveCheck;
    QComboBox* m_themeCombo;
    QComboBox* m_languageCombo;
    QPushButton* m_saveBtn;
    QPushButton* m_resetBtn;
};
