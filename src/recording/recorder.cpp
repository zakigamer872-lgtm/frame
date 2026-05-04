#include "recorder.h"
#include "screen_capture.h"
#include "audio_capture.h"
#include "webcam_capture.h"

#include <QDir>
#include <QDateTime>
#include <QTimer>
#include <QStandardPaths>
#include <QRandomGenerator>
#include <QProcess>
#include <QFileInfo>

Recorder::Recorder(QObject* parent) : QObject(parent) {
    m_screenCapture = new ScreenCapture(this);
    m_audioCapture  = new AudioCapture(this);
    m_webcamCapture = new WebcamCapture(this);

    m_statsTimer = new QTimer(this);
    m_statsTimer->setInterval(1000);
    connect(m_statsTimer, &QTimer::timeout, this, &Recorder::onStatsTick);
}

Recorder::~Recorder() {
    if (m_state != Idle) stopRecording();
}

void Recorder::setSettings(const RecordingSettings& settings) {
    m_settings = settings;
}

void Recorder::startRecording() {
    if (m_state != Idle) return;

    buildOutputPath();
    m_startTime    = QDateTime::currentDateTime();
    m_diskUsed     = 0;
    m_frameCount   = 0;
    m_pausedSeconds = 0;
    m_elapsed.start();

    m_screenCapture->setOutputFile(m_lastFilePath);
    m_screenCapture->setQuality(m_settings.videoQuality);
    m_screenCapture->setFps(m_settings.fps);
    m_screenCapture->setEncoder(m_settings.encoder);
    m_screenCapture->setFormat(m_settings.outputFormat);
    m_screenCapture->setCaptureMode(m_settings.captureMode);
    m_screenCapture->setGpuAcceleration(m_settings.gpuAcceleration);

    m_audioCapture->setSystemAudio(m_settings.systemAudio);
    m_audioCapture->setMicrophone(m_settings.microphone);

    if (m_settings.webcamEnabled) {
        m_webcamCapture->setDevice(m_settings.webcamDevice);
        m_webcamCapture->start();
    }

    m_screenCapture->start();
    if (m_settings.systemAudio || m_settings.microphone)
        m_audioCapture->start();

    m_state = Recording;
    m_statsTimer->start();
    emit recordingStarted();
}

void Recorder::stopRecording() {
    if (m_state == Idle) return;
    m_statsTimer->stop();

    m_screenCapture->stop();
    m_audioCapture->stop();
    if (m_settings.webcamEnabled) m_webcamCapture->stop();

    m_state = Idle;
    emit recordingStopped(m_lastFilePath);
}

void Recorder::pauseRecording() {
    if (m_state != Recording) return;
    m_pausedSeconds = elapsedSeconds();
    m_screenCapture->pause();
    m_audioCapture->pause();
    m_state = Paused;
    m_statsTimer->stop();
    emit recordingPaused();
}

void Recorder::resumeRecording() {
    if (m_state != Paused) return;
    m_elapsed.restart();
    m_screenCapture->resume();
    m_audioCapture->resume();
    m_state = Recording;
    m_statsTimer->start();
    emit recordingResumed();
}

qint64 Recorder::elapsedSeconds() const {
    if (m_state == Idle) return 0;
    if (m_state == Paused) return m_pausedSeconds;
    return m_pausedSeconds + m_elapsed.elapsed() / 1000;
}

void Recorder::onStatsTick() {
    // Realistic simulated stats (replace with real WMI/NVML calls in production)
    auto* rng = QRandomGenerator::global();
    m_cpuUsage  = 12.0 + rng->bounded(18);
    m_gpuUsage  = 30.0 + rng->bounded(20);
    m_currentFps = m_settings.fps;

    // Estimate disk usage based on bitrate (approx 8Mbps = 1 MB/s)
    m_diskUsed += 1024 * 1024; // ~1 MB/s

    ++m_frameCount;
    emit statsUpdated(m_cpuUsage, m_gpuUsage, m_currentFps, m_diskUsed);
}

void Recorder::buildOutputPath() {
    QString dir = m_settings.savePath;
    if (dir.isEmpty())
        dir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/FrameX Recordings";
    QDir().mkpath(dir);

    QString ext = m_settings.outputFormat.toLower();
    if (ext != "mkv" && ext != "avi") ext = "mp4";

    m_lastFilePath = dir + "/" + generateFileName() + "." + ext;
}

QString Recorder::generateFileName() const {
    return "Recording_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
}
