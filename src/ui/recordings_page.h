#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class RecordingsPage : public QWidget {
    Q_OBJECT
public:
    explicit RecordingsPage(QWidget* parent = nullptr);
    void loadRecordings();

private:
    void setupUI();
    void deleteSelected();
    void openSelected();
    void openInEditor();

    QLineEdit*    m_searchEdit;
    QComboBox*    m_filterCombo;
    QTableWidget* m_table;
    QLabel*       m_emptyLabel;
    QLabel*       m_countLabel;
};
