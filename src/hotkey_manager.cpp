#include "hotkey_manager.h"
#include <QShortcut>

void HotkeyManager::registerHotkey(const QString& id, const QKeySequence& key, std::function<void()> callback) {
    HotkeyEntry entry;
    entry.key = key;
    entry.callback = callback;
    m_hotkeys[id] = entry;
}

void HotkeyManager::unregisterHotkey(const QString& id) {
    m_hotkeys.remove(id);
}

void HotkeyManager::updateHotkey(const QString& id, const QKeySequence& key) {
    if (m_hotkeys.contains(id)) {
        m_hotkeys[id].key = key;
    }
}

QKeySequence HotkeyManager::getHotkey(const QString& id) const {
    return m_hotkeys.value(id).key;
}
