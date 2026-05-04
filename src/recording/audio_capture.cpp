#include "audio_capture.h"
#include <QTimer>
#include <QRandomGenerator>

AudioCapture::AudioCapture(QObject* parent) : QObject(parent) {
    m_levelTimer = new QTimer(this);
    m_levelTimer->setInterval(100);
    connect(m_levelTimer, &QTimer::timeout, [this]() {
        if (!m_running) return;
        m_systemLevel = m_systemAudio ? (float)QRandomGenerator::global()->bounded(60) / 100.0f : 0.0f;
        m_micLevel = m_microphone ? (float)QRandomGenerator::global()->bounded(70) / 100.0f : 0.0f;
        emit levelChanged(m_systemLevel, m_micLevel);
    });
}

AudioCapture::~AudioCapture() { stop(); }

void AudioCapture::start() {
    if (m_running) return;
    m_running = true;
    m_levelTimer->start();
}

void AudioCapture::stop() {
    if (!m_running) return;
    m_running = false;
    m_levelTimer->stop();
}

void AudioCapture::pause() { m_levelTimer->stop(); }
void AudioCapture::resume() { if (m_running) m_levelTimer->start(); }

QStringList AudioCapture::availableDevices() const {
    return {"Default Microphone", "Headset Microphone", "Blue Yeti", "Logitech Headset"};
}
