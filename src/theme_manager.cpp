#include "theme_manager.h"
#include "settings_manager.h"

void ThemeManager::applyTheme(QApplication& app, Theme t) {
    m_theme = t;
    QString theme = SettingsManager::instance().app.theme;
    if (theme == "light")
        app.setStyleSheet(lightStyleSheet());
    else
        app.setStyleSheet(darkStyleSheet());
}

void ThemeManager::setTheme(Theme t) { m_theme = t; }

QString ThemeManager::darkStyleSheet() {
    return R"(
QMainWindow, QDialog, QWidget {
    background-color: #0D0D1A;
    color: #E2E8F0;
    font-family: 'Segoe UI', Arial, sans-serif;
    font-size: 13px;
}
QFrame#titleBar {
    background-color: #09091A;
    border-bottom: 1px solid #1E1B3A;
}
QFrame#sidebar, QWidget#sidebar {
    background-color: #09091A;
    border-right: 1px solid #1E1B3A;
}
QFrame#settingsPanel, QWidget#settingsPanel {
    background-color: #0A0A18;
    border-left: 1px solid #1E1B3A;
}
QPushButton {
    background-color: #1A1A2E;
    color: #CBD5E1;
    border: 1px solid #2D2B55;
    border-radius: 8px;
    padding: 8px 16px;
    font-size: 13px;
}
QPushButton:hover {
    background-color: #252545;
    border-color: #7C3AED;
    color: #E2E8F0;
}
QPushButton:pressed {
    background-color: #7C3AED;
    border-color: #9333EA;
    color: white;
}
QPushButton#recordBtn {
    background: qradialgradient(cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,
        stop:0 #EF4444, stop:0.65 #DC2626, stop:1 #B91C1C);
    border: 3px solid #F87171;
    border-radius: 65px;
    color: white;
    font-size: 18px;
    font-weight: bold;
    min-width: 130px;
    min-height: 130px;
}
QPushButton#recordBtn:hover {
    background: qradialgradient(cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,
        stop:0 #F87171, stop:0.6 #EF4444, stop:1 #DC2626);
    border-color: #FCA5A5;
}
QPushButton#primaryBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #7C3AED, stop:1 #9333EA);
    color: white;
    border: none;
    border-radius: 8px;
    padding: 10px 24px;
    font-weight: bold;
}
QPushButton#primaryBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6D28D9, stop:1 #7C3AED);
}
QPushButton#captureTypeBtn {
    background-color: #12122A;
    border: 1px solid #2D2B55;
    border-radius: 10px;
    color: #94A3B8;
    padding: 12px 16px;
    text-align: left;
    font-size: 13px;
}
QPushButton#captureTypeBtn:hover {
    background-color: #1A1A3E;
    border-color: #7C3AED;
    color: #E2E8F0;
}
QPushButton#captureTypeBtn:checked {
    background-color: #1E1050;
    border-color: #7C3AED;
    border-width: 2px;
    color: #C084FC;
    font-weight: bold;
}
QPushButton#sidebarBtn {
    background: transparent;
    border: none;
    border-radius: 10px;
    color: #64748B;
    padding: 10px 16px;
    text-align: left;
    font-size: 13px;
}
QPushButton#sidebarBtn:hover {
    background-color: #14142A;
    color: #C084FC;
}
QPushButton#sidebarBtn:checked {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1E1050, stop:1 #12122A);
    color: #C084FC;
    border-left: 3px solid #7C3AED;
    font-weight: bold;
}
QLabel {
    color: #E2E8F0;
    background: transparent;
}
QLabel#sectionLabel {
    font-size: 11px;
    font-weight: bold;
    color: #475569;
}
QComboBox {
    background-color: #12122A;
    border: 1px solid #2D2B55;
    border-radius: 8px;
    padding: 8px 12px;
    color: #E2E8F0;
    font-size: 13px;
    min-height: 36px;
}
QComboBox:hover { border-color: #7C3AED; }
QComboBox:focus { border-color: #7C3AED; }
QComboBox::drop-down { border: none; width: 24px; }
QComboBox QAbstractItemView {
    background-color: #12122A;
    border: 1px solid #2D2B55;
    selection-background-color: #7C3AED;
    color: #E2E8F0;
    border-radius: 8px;
    padding: 4px;
}
QSlider::groove:horizontal {
    height: 4px;
    background: #2D2B55;
    border-radius: 2px;
}
QSlider::handle:horizontal {
    background: #7C3AED;
    border: 2px solid #9333EA;
    width: 16px;
    height: 16px;
    margin: -6px 0;
    border-radius: 8px;
}
QSlider::sub-page:horizontal {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #7C3AED, stop:1 #C084FC);
    border-radius: 2px;
}
QCheckBox { color: #CBD5E1; spacing: 8px; }
QCheckBox::indicator {
    width: 18px;
    height: 18px;
    border-radius: 4px;
    border: 2px solid #2D2B55;
    background: #12122A;
}
QCheckBox::indicator:checked {
    background: #7C3AED;
    border-color: #9333EA;
}
QLineEdit, QTextEdit, QPlainTextEdit {
    background-color: #12122A;
    border: 1px solid #2D2B55;
    border-radius: 8px;
    padding: 8px 12px;
    color: #E2E8F0;
    selection-background-color: #7C3AED;
}
QLineEdit:focus, QTextEdit:focus { border-color: #7C3AED; }
QKeySequenceEdit {
    background-color: #12122A;
    border: 1px solid #2D2B55;
    border-radius: 8px;
    padding: 8px 12px;
    color: #C084FC;
    font-weight: bold;
}
QKeySequenceEdit:focus { border-color: #7C3AED; }
QScrollBar:vertical {
    background: transparent;
    width: 5px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: #2D2B55;
    border-radius: 3px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover { background: #7C3AED; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal {
    background: transparent;
    height: 5px;
}
QScrollBar::handle:horizontal {
    background: #2D2B55;
    border-radius: 3px;
    min-width: 30px;
}
QScrollBar::handle:horizontal:hover { background: #7C3AED; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
QTabWidget::pane {
    border: 1px solid #2D2B55;
    border-radius: 10px;
    background: #0F0F1F;
}
QTabBar::tab {
    background: #12122A;
    color: #64748B;
    border: none;
    padding: 10px 20px;
    border-radius: 8px;
    margin-right: 4px;
}
QTabBar::tab:selected { background: #7C3AED; color: white; font-weight: bold; }
QTabBar::tab:hover { background: #1E1E3E; color: #C084FC; }
QProgressBar {
    background: #1A1A2E;
    border: none;
    border-radius: 4px;
    text-align: center;
}
QProgressBar::chunk {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #7C3AED, stop:1 #C084FC);
    border-radius: 4px;
}
QGroupBox {
    border: 1px solid #1E1B3A;
    border-radius: 12px;
    margin-top: 16px;
    padding: 16px;
    color: #94A3B8;
    font-size: 12px;
    font-weight: bold;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 14px;
    padding: 0 6px;
    color: #7C3AED;
}
QTableWidget {
    background: #0F0F1F;
    alternate-background-color: #0D0D18;
    border: 1px solid #1E1B3A;
    border-radius: 10px;
    gridline-color: transparent;
    color: #CBD5E1;
    selection-background-color: #1E1050;
}
QTableWidget::item { padding: 10px; border: none; }
QTableWidget::item:selected { background: #1E1050; color: #C084FC; }
QHeaderView::section {
    background: #12122A;
    color: #64748B;
    border: none;
    border-bottom: 1px solid #1E1B3A;
    padding: 10px;
    font-weight: bold;
    font-size: 11px;
}
QMenu {
    background: #12122A;
    border: 1px solid #2D2B55;
    border-radius: 10px;
    padding: 6px;
}
QMenu::item {
    padding: 8px 20px;
    border-radius: 6px;
    color: #CBD5E1;
}
QMenu::item:selected { background: #1E1050; color: #C084FC; }
QMenu::separator { height: 1px; background: #1E1B3A; margin: 4px 8px; }
QToolTip {
    background: #1E1B3A;
    color: #E2E8F0;
    border: 1px solid #2D2B55;
    border-radius: 8px;
    padding: 6px 12px;
}
QScrollArea { background: transparent; border: none; }
QSplitter::handle { background: #1E1B3A; width: 1px; height: 1px; }
QMessageBox { background: #0F0F1F; }
QMessageBox QLabel { color: #CBD5E1; }
QMessageBox QPushButton { min-width: 80px; min-height: 32px; }
)";
}

QString ThemeManager::lightStyleSheet() {
    return R"(
QMainWindow, QDialog, QWidget {
    background-color: #F8FAFC;
    color: #1E293B;
    font-family: 'Segoe UI', Arial, sans-serif;
    font-size: 13px;
}
QFrame#sidebar, QWidget#sidebar {
    background-color: #FFFFFF;
    border-right: 1px solid #E2E8F0;
}
QPushButton {
    background-color: #F1F5F9;
    color: #334155;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    padding: 8px 16px;
}
QPushButton:hover {
    background-color: #EDE9FE;
    border-color: #7C3AED;
    color: #7C3AED;
}
QPushButton#primaryBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #7C3AED, stop:1 #9333EA);
    color: white;
    border: none;
    font-weight: bold;
}
QPushButton#sidebarBtn {
    background: transparent;
    border: none;
    text-align: left;
    color: #64748B;
}
QPushButton#sidebarBtn:checked {
    background: #EDE9FE;
    color: #7C3AED;
    border-left: 3px solid #7C3AED;
    font-weight: bold;
}
QLabel { color: #1E293B; background: transparent; }
QComboBox {
    background: #F1F5F9;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    padding: 8px 12px;
    color: #1E293B;
}
QComboBox:focus { border-color: #7C3AED; }
QLineEdit, QTextEdit {
    background: #FFFFFF;
    border: 1px solid #E2E8F0;
    border-radius: 8px;
    padding: 8px 12px;
    color: #1E293B;
}
QLineEdit:focus { border-color: #7C3AED; }
QCheckBox { color: #334155; }
QCheckBox::indicator {
    width: 18px;
    height: 18px;
    border-radius: 4px;
    border: 2px solid #CBD5E1;
    background: #FFFFFF;
}
QCheckBox::indicator:checked {
    background: #7C3AED;
    border-color: #7C3AED;
}
QScrollBar:vertical { background: #F1F5F9; width: 6px; }
QScrollBar::handle:vertical { background: #CBD5E1; border-radius: 3px; }
QScrollBar::handle:vertical:hover { background: #7C3AED; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QProgressBar { background: #E2E8F0; border: none; border-radius: 4px; }
QProgressBar::chunk { background: #7C3AED; border-radius: 4px; }
QTableWidget { background: #FFFFFF; border: 1px solid #E2E8F0; color: #1E293B; }
QTableWidget::item:selected { background: #EDE9FE; color: #7C3AED; }
QHeaderView::section { background: #F8FAFC; color: #64748B; border: none; border-bottom: 1px solid #E2E8F0; padding: 10px; }
QMenu { background: #FFFFFF; border: 1px solid #E2E8F0; border-radius: 8px; }
QMenu::item:selected { background: #EDE9FE; color: #7C3AED; }
)";
}
