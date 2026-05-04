#pragma once
#include <QList>
#include <QString>
#include <QDateTime>
#include <QSettings>

struct RecordingEntry {
    QString filePath;
    QString fileName;
    QDateTime startTime;
    QDateTime endTime;
    qint64 duration; // seconds
    qint64 fileSize; // bytes
    QString resolution;
    int fps;
    QString format;
};

class HistoryManager {
public:
    static HistoryManager& instance() {
        static HistoryManager inst;
        return inst;
    }

    void addEntry(const QString& filePath, const QDateTime& start, const QDateTime& end);
    QList<RecordingEntry> getHistory() const { return m_history; }
    void removeEntry(int index);
    void clearHistory();
    void save();
    void load();

private:
    HistoryManager();
    QList<RecordingEntry> m_history;
    QSettings* m_settings = nullptr;
};
