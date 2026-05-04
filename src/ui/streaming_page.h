#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class StreamingPage : public QWidget {
    Q_OBJECT
public:
    explicit StreamingPage(QWidget* parent = nullptr);

private:
    void setupUI();
    void startStream();
    void stopStream();

    QComboBox* m_platformCombo;
    QLineEdit* m_streamKeyEdit;
    QComboBox* m_qualityCombo;
    QPushButton* m_startBtn;
    QPushButton* m_stopBtn;
    QLabel* m_statusLabel;
    bool m_streaming = false;
};
