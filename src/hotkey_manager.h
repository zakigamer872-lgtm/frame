#pragma once
#include <QObject>
#include <QKeySequence>
#include <QMap>
#include <functional>

class HotkeyManager : public QObject {
    Q_OBJECT
public:
    static HotkeyManager& instance() {
        static HotkeyManager inst;
        return inst;
    }
    void registerHotkey(const QString& id, const QKeySequence& key, std::function<void()> callback);
    void unregisterHotkey(const QString& id);
    void updateHotkey(const QString& id, const QKeySequence& key);
    QKeySequence getHotkey(const QString& id) const;

private:
    HotkeyManager() {}
    struct HotkeyEntry {
        QKeySequence key;
        std::function<void()> callback;
    };
    QMap<QString, HotkeyEntry> m_hotkeys;
};
