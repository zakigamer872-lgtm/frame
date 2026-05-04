#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QTimer>
#include <QScrollArea>
#include "../recording/recorder.h"

class CpuGpuGraph;

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(Recorder* recorder, QWidget* parent = nullptr);

    void updateRecordingTime(qint64 secs);

public slots:
    void onSettingsChanged();
    void onRecordingStats(double cpu, double gpu, int fps, qint64 disk);

signals:
    void startRecordingRequested();
    void screenshotRequested();

private:
    void setupUI();
    void setupCaptureButtons();
    void setupRecordButton();
    void setupAudioWidgets();
    void setupRecentRecordings();
    void setupStatsBar();
    void updateRecordButton();
    QString formatTime(qint64 secs) const;
    QString formatSize(qint64 bytes) const;

    Recorder* m_recorder;

    // Capture mode buttons
    QPushButton* m_fullscreenBtn;
    QPushButton* m_windowBtn;
    QPushButton* m_customAreaBtn;
    QButtonGroup* m_captureGroup;

    // Audio toggles
    QPushButton* m_micBtn;
    QPushButton* m_sysAudioBtn;
    QPushButton* m_webcamBtn;

    // Record button
    QPushButton* m_recordBtn;
    QLabel* m_recStatusLabel;
    QLabel* m_recTimeLabel;

    // Schedule / Editor buttons
    QPushButton* m_scheduleBtn;
    QPushButton* m_editorBtn;

    // Stats
    QLabel* m_cpuLabel;
    QLabel* m_gpuLabel;
    QLabel* m_fpsLabel;
    QLabel* m_timeLabel;
    QLabel* m_dropLabel;
    QLabel* m_diskLabel;

    // Recent recordings area
    QWidget* m_recentContainer;
    QHBoxLayout* m_recentLayout;

    // Glow animation
    QTimer* m_glowTimer;
    int m_glowValue = 0;
    bool m_glowUp = true;
    bool m_isRecording = false;
};
