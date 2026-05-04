#include "history_manager.h"
#include <QFileInfo>

HistoryManager::HistoryManager() {
    m_settings = new QSettings("ZAKI", "FrameX_History");
    load();
}

void HistoryManager::addEntry(const QString& filePath, const QDateTime& start, const QDateTime& end) {
    RecordingEntry entry;
    entry.filePath = filePath;
    entry.fileName = QFileInfo(filePath).fileName();
    entry.startTime = start;
    entry.endTime = end;
    entry.duration = start.secsTo(end);
    QFileInfo fi(filePath);
    entry.fileSize = fi.exists() ? fi.size() : 0;
    entry.resolution = "1080p";
    entry.fps = 60;
    entry.format = QFileInfo(filePath).suffix().toUpper();
    m_history.prepend(entry);
    if (m_history.size() > 100) m_history.removeLast();
    save();
}

void HistoryManager::removeEntry(int index) {
    if (index >= 0 && index < m_history.size()) {
        m_history.removeAt(index);
        save();
    }
}

void HistoryManager::clearHistory() {
    m_history.clear();
    save();
}

void HistoryManager::save() {
    m_settings->beginWriteArray("history");
    for (int i = 0; i < m_history.size(); ++i) {
        m_settings->setArrayIndex(i);
        m_settings->setValue("filePath", m_history[i].filePath);
        m_settings->setValue("startTime", m_history[i].startTime);
        m_settings->setValue("endTime", m_history[i].endTime);
        m_settings->setValue("duration", m_history[i].duration);
        m_settings->setValue("fileSize", m_history[i].fileSize);
        m_settings->setValue("resolution", m_history[i].resolution);
        m_settings->setValue("fps", m_history[i].fps);
        m_settings->setValue("format", m_history[i].format);
    }
    m_settings->endArray();
    m_settings->sync();
}

void HistoryManager::load() {
    m_history.clear();
    int size = m_settings->beginReadArray("history");
    for (int i = 0; i < size; ++i) {
        m_settings->setArrayIndex(i);
        RecordingEntry entry;
        entry.filePath = m_settings->value("filePath").toString();
        entry.fileName = QFileInfo(entry.filePath).fileName();
        entry.startTime = m_settings->value("startTime").toDateTime();
        entry.endTime = m_settings->value("endTime").toDateTime();
        entry.duration = m_settings->value("duration").toLongLong();
        entry.fileSize = m_settings->value("fileSize").toLongLong();
        entry.resolution = m_settings->value("resolution", "1080p").toString();
        entry.fps = m_settings->value("fps", 60).toInt();
        entry.format = m_settings->value("format", "MP4").toString();
        m_history.append(entry);
    }
    m_settings->endArray();
}
