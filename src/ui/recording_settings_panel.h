#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QSlider>

class RecordingSettingsPanel : public QWidget {
    Q_OBJECT
public:
    explicit RecordingSettingsPanel(QWidget* parent = nullptr);

signals:
    void settingsChanged();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();

    // Video quality
    QComboBox* m_qualityCombo;
    // FPS buttons
    QPushButton* m_fps30Btn;
    QPushButton* m_fps60Btn;
    QPushButton* m_fps120Btn;
    // Encoder
    QComboBox* m_encoderCombo;
    // Audio sources
    QCheckBox* m_sysAudioCheck;
    QSlider* m_sysAudioSlider;
    QCheckBox* m_micCheck;
    QSlider* m_micSlider;
    // Webcam
    QComboBox* m_webcamCombo;
    // Format
    QPushButton* m_mp4Btn;
    QPushButton* m_mkvBtn;
    QPushButton* m_aviBtn;
    // Save location
    QLabel* m_savePathLabel;
    QPushButton* m_browseBtn;
};
