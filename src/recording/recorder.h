#pragma once
#include <QObject>
#include <QString>
#include <QThread>
#include <QDateTime>
#include <QTimer>
#include <QElapsedTimer>
#include "../settings_manager.h"

class ScreenCapture;
class AudioCapture;
class WebcamCapture;

class Recorder : public QObject {
    Q_OBJECT
public:
    enum State { Idle, Recording, Paused };

    explicit Recorder(QObject* parent = nullptr);
    ~Recorder();

    void setSettings(const RecordingSettings& settings);
    void startRecording();
    void stopRecording();
    void pauseRecording();
    void resumeRecording();

    QString lastFilePath() const { return m_lastFilePath; }
    State state() const { return m_state; }
    qint64 elapsedSeconds() const;
    double cpuUsage() const { return m_cpuUsage; }
    double gpuUsage() const { return m_gpuUsage; }
    int currentFps() const { return m_currentFps; }
    qint64 diskUsed() const { return m_diskUsed; }

signals:
    void recordingStarted();
    void recordingStopped(const QString& filePath);
    void recordingPaused();
    void recordingResumed();
    void statsUpdated(double cpu, double gpu, int fps, qint64 disk);
    void errorOccurred(const QString& msg);

private slots:
    void onStatsTick();

private:
    void buildOutputPath();
    QString generateFileName() const;

    RecordingSettings m_settings;
    State m_state = Idle;
    QString m_lastFilePath;
    QDateTime m_startTime;
    qint64 m_pausedSeconds = 0;
    QElapsedTimer m_elapsed;

    ScreenCapture* m_screenCapture = nullptr;
    AudioCapture* m_audioCapture = nullptr;
    WebcamCapture* m_webcamCapture = nullptr;

    QTimer* m_statsTimer = nullptr;
    double m_cpuUsage = 0;
    double m_gpuUsage = 0;
    int m_currentFps = 0;
    qint64 m_diskUsed = 0;
    int m_frameCount = 0;
};
