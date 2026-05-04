#include "settings_manager.h"
#include <QStandardPaths>
#include <QDir>

void SettingsManager::initialize() {
    m_settings = new QSettings("ZAKI", "FrameX");
    recording.savePath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                         + "/FrameX Recordings";
    QDir().mkpath(recording.savePath);
    load();
}

void SettingsManager::save() {
    if (!m_settings) return;

    m_settings->beginGroup("Recording");
    m_settings->setValue("captureMode",      recording.captureMode);
    m_settings->setValue("videoQuality",     recording.videoQuality);
    m_settings->setValue("fps",              recording.fps);
    m_settings->setValue("encoder",          recording.encoder);
    m_settings->setValue("outputFormat",     recording.outputFormat);
    m_settings->setValue("savePath",         recording.savePath);
    m_settings->setValue("systemAudio",      recording.systemAudio);
    m_settings->setValue("microphone",       recording.microphone);
    m_settings->setValue("webcamEnabled",    recording.webcamEnabled);
    m_settings->setValue("webcamDevice",     recording.webcamDevice);
    m_settings->setValue("webcamSize",       recording.webcamSize);
    m_settings->setValue("webcamPosition",   recording.webcamPosition);
    m_settings->setValue("showCursor",       recording.showCursor);
    m_settings->setValue("highlightCursor",  recording.highlightCursor);
    m_settings->setValue("showClicks",       recording.showClicks);
    m_settings->setValue("gpuAcceleration",  recording.gpuAcceleration);
    m_settings->setValue("audioBitrate",     recording.audioBitrate);
    m_settings->setValue("videoBitrate",     recording.videoBitrate);
    m_settings->endGroup();

    m_settings->beginGroup("Hotkeys");
    m_settings->setValue("startStop",       hotkeys.startStop.toString());
    m_settings->setValue("pauseResume",     hotkeys.pauseResume.toString());
    m_settings->setValue("screenshot",      hotkeys.screenshot.toString());
    m_settings->setValue("cancelRecording", hotkeys.cancelRecording.toString());
    m_settings->setValue("openEditor",      hotkeys.openEditor.toString());
    m_settings->endGroup();

    m_settings->beginGroup("App");
    m_settings->setValue("theme",                app.theme);
    m_settings->setValue("minimizeToTray",       app.minimizeToTray);
    m_settings->setValue("startOnBoot",          app.startOnBoot);
    m_settings->setValue("showFloatingWidget",   app.showFloatingWidget);
    m_settings->setValue("showNotifications",    app.showNotifications);
    m_settings->setValue("autoSplitMinutes",     app.autoSplitMinutes);
    m_settings->endGroup();

    m_settings->sync();
}

void SettingsManager::load() {
    if (!m_settings) return;

    m_settings->beginGroup("Recording");
    recording.captureMode     = m_settings->value("captureMode",     recording.captureMode).toString();
    recording.videoQuality    = m_settings->value("videoQuality",    recording.videoQuality).toString();
    recording.fps             = m_settings->value("fps",             recording.fps).toInt();
    recording.encoder         = m_settings->value("encoder",         recording.encoder).toString();
    recording.outputFormat    = m_settings->value("outputFormat",    recording.outputFormat).toString();
    recording.savePath        = m_settings->value("savePath",        recording.savePath).toString();
    recording.systemAudio     = m_settings->value("systemAudio",     recording.systemAudio).toBool();
    recording.microphone      = m_settings->value("microphone",      recording.microphone).toBool();
    recording.webcamEnabled   = m_settings->value("webcamEnabled",   recording.webcamEnabled).toBool();
    recording.webcamDevice    = m_settings->value("webcamDevice",    recording.webcamDevice).toString();
    recording.webcamSize      = m_settings->value("webcamSize",      recording.webcamSize).toInt();
    recording.webcamPosition  = m_settings->value("webcamPosition",  recording.webcamPosition).toString();
    recording.showCursor      = m_settings->value("showCursor",      recording.showCursor).toBool();
    recording.highlightCursor = m_settings->value("highlightCursor", recording.highlightCursor).toBool();
    recording.showClicks      = m_settings->value("showClicks",      recording.showClicks).toBool();
    recording.gpuAcceleration = m_settings->value("gpuAcceleration", recording.gpuAcceleration).toBool();
    recording.audioBitrate    = m_settings->value("audioBitrate",    recording.audioBitrate).toInt();
    recording.videoBitrate    = m_settings->value("videoBitrate",    recording.videoBitrate).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("Hotkeys");
    hotkeys.startStop       = QKeySequence(m_settings->value("startStop",       "Ctrl+Shift+R").toString());
    hotkeys.pauseResume     = QKeySequence(m_settings->value("pauseResume",     "Ctrl+Shift+P").toString());
    hotkeys.screenshot      = QKeySequence(m_settings->value("screenshot",      "Ctrl+Shift+S").toString());
    hotkeys.cancelRecording = QKeySequence(m_settings->value("cancelRecording", "Ctrl+Shift+X").toString());
    hotkeys.openEditor      = QKeySequence(m_settings->value("openEditor",      "Ctrl+Shift+E").toString());
    m_settings->endGroup();

    m_settings->beginGroup("App");
    app.theme               = m_settings->value("theme",              app.theme).toString();
    app.minimizeToTray      = m_settings->value("minimizeToTray",     app.minimizeToTray).toBool();
    app.startOnBoot         = m_settings->value("startOnBoot",        app.startOnBoot).toBool();
    app.showFloatingWidget  = m_settings->value("showFloatingWidget",  app.showFloatingWidget).toBool();
    app.showNotifications   = m_settings->value("showNotifications",   app.showNotifications).toBool();
    app.autoSplitMinutes    = m_settings->value("autoSplitMinutes",    app.autoSplitMinutes).toInt();
    m_settings->endGroup();
}

void SettingsManager::resetToDefaults() {
    recording = RecordingSettings{};
    hotkeys   = HotkeySettings{};
    app       = AppSettings{};
    recording.savePath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                         + "/FrameX Recordings";
    save();
}
