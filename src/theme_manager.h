#pragma once
#include <QApplication>
#include <QString>

class ThemeManager {
public:
    static ThemeManager& instance() {
        static ThemeManager inst;
        return inst;
    }

    enum class Theme { Dark, Light };

    void applyTheme(QApplication& app, Theme t = Theme::Dark);
    void setTheme(Theme t);
    Theme currentTheme() const { return m_theme; }

private:
    ThemeManager() {}
    Theme m_theme = Theme::Dark;
    QString darkStyleSheet();
    QString lightStyleSheet();
};
