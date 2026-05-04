#include "recordings_page.h"
#include "../history_manager.h"
#include "../editor/video_editor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QFrame>
#include <QDialog>

RecordingsPage::RecordingsPage(QWidget* parent) : QWidget(parent) {
    HistoryManager::instance().load();
    setupUI();
    loadRecordings();
}

void RecordingsPage::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    // Header
    auto* headerRow = new QHBoxLayout();
    auto* titleLabel = new QLabel("⏺  Recordings");
    QFont tf; tf.setFamily("Segoe UI"); tf.setPointSize(18); tf.setBold(true);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color: #F1F5F9;");

    m_countLabel = new QLabel("");
    m_countLabel->setStyleSheet("color: #64748B; font-size: 13px;");

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("🔍  Search recordings...");
    m_searchEdit->setFixedHeight(36);
    m_searchEdit->setFixedWidth(220);
    m_searchEdit->setStyleSheet(
        "QLineEdit { background: #12122A; border: 1px solid #2D2B55; border-radius: 8px; "
        "color: #CBD5E1; padding: 0 12px; }"
        "QLineEdit:focus { border-color: #7C3AED; }");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &RecordingsPage::loadRecordings);

    m_filterCombo = new QComboBox();
    m_filterCombo->addItems({"All", "MP4", "MKV", "AVI"});
    m_filterCombo->setFixedHeight(36);
    m_filterCombo->setStyleSheet(
        "QComboBox { background: #12122A; border: 1px solid #2D2B55; border-radius: 8px; "
        "color: #CBD5E1; padding: 0 10px; min-width: 80px; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background: #12122A; color: #CBD5E1; }");
    connect(m_filterCombo, &QComboBox::currentTextChanged, this, [this](){ loadRecordings(); });

    headerRow->addWidget(titleLabel);
    headerRow->addWidget(m_countLabel);
    headerRow->addStretch();
    headerRow->addWidget(m_searchEdit);
    headerRow->addWidget(m_filterCombo);
    layout->addLayout(headerRow);

    // Table
    m_table = new QTableWidget(0, 5);
    m_table->setHorizontalHeaderLabels({"File Name", "Date", "Duration", "Size", "Format"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->hide();
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setStyleSheet(
        "QTableWidget { background: #0D0D1A; alternate-background-color: #0F0F24; "
        "border: 1px solid #1E1B3A; border-radius: 10px; color: #CBD5E1; gridline-color: transparent; }"
        "QTableWidget::item { padding: 10px; border: none; }"
        "QTableWidget::item:selected { background: #1E1050; color: white; }"
        "QHeaderView::section { background: #12122A; color: #94A3B8; border: none; "
        "padding: 10px; font-weight: bold; font-size: 12px; border-bottom: 1px solid #1E1B3A; }");

    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, &QTableWidget::customContextMenuRequested, this, [this](const QPoint& pos){
        if (m_table->currentRow() < 0) return;
        QMenu menu;
        menu.setStyleSheet("QMenu { background: #12122A; color: #CBD5E1; border: 1px solid #2D2B55; border-radius: 8px; }"
                           "QMenu::item:selected { background: #1E1050; }");
        menu.addAction("▶  Play", this, &RecordingsPage::openSelected);
        menu.addAction("✂  Open in Editor", this, &RecordingsPage::openInEditor);
        menu.addAction("📂  Show in Explorer", this, [this](){
            auto history = HistoryManager::instance().getHistory();
            int row = m_table->currentRow();
            if (row < history.size()) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(
                    QFileInfo(history[row].filePath).absolutePath()));
            }
        });
        menu.addSeparator();
        menu.addAction("🗑  Delete", this, &RecordingsPage::deleteSelected);
        menu.exec(m_table->viewport()->mapToGlobal(pos));
    });

    connect(m_table, &QTableWidget::doubleClicked, this, &RecordingsPage::openSelected);

    m_emptyLabel = new QLabel("No recordings yet.\nStart recording to see them here.");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setStyleSheet("color: #475569; font-size: 15px; padding: 60px;");
    m_emptyLabel->hide();

    layout->addWidget(m_table, 1);
    layout->addWidget(m_emptyLabel, 1);

    // Action bar
    auto* actionRow = new QHBoxLayout();
    auto makeBtn = [](const QString& text, const QString& style) {
        auto* b = new QPushButton(text);
        b->setFixedHeight(38);
        b->setStyleSheet(style);
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };
    const QString secondary =
        "QPushButton { background: #12122A; border: 1px solid #2D2B55; border-radius: 8px; "
        "color: #CBD5E1; padding: 0 16px; } QPushButton:hover { background: #1E1050; border-color: #7C3AED; }";
    const QString danger =
        "QPushButton { background: #1A0A0A; border: 1px solid #7F1D1D; border-radius: 8px; "
        "color: #FCA5A5; padding: 0 16px; } QPushButton:hover { background: #7F1D1D; color: white; }";

    auto* openBtn   = makeBtn("▶  Play", secondary);
    auto* editorBtn = makeBtn("✂  Edit", secondary);
    auto* deleteBtn = makeBtn("🗑  Delete", danger);
    auto* clearBtn  = makeBtn("Clear History", danger);

    connect(openBtn,   &QPushButton::clicked, this, &RecordingsPage::openSelected);
    connect(editorBtn, &QPushButton::clicked, this, &RecordingsPage::openInEditor);
    connect(deleteBtn, &QPushButton::clicked, this, &RecordingsPage::deleteSelected);
    connect(clearBtn,  &QPushButton::clicked, this, [this](){
        if (QMessageBox::question(this, "FrameX", "Clear all recording history?",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            HistoryManager::instance().clearHistory();
            loadRecordings();
        }
    });

    actionRow->addWidget(openBtn);
    actionRow->addWidget(editorBtn);
    actionRow->addStretch();
    actionRow->addWidget(deleteBtn);
    actionRow->addWidget(clearBtn);
    layout->addLayout(actionRow);
}

void RecordingsPage::loadRecordings() {
    m_table->setRowCount(0);
    auto history = HistoryManager::instance().getHistory();
    QString filter = m_filterCombo ? m_filterCombo->currentText() : "All";
    QString search = m_searchEdit  ? m_searchEdit->text().toLower() : "";

    int count = 0;
    for (auto& entry : history) {
        if (filter != "All" && entry.format != filter) continue;
        if (!search.isEmpty() && !entry.fileName.toLower().contains(search)) continue;

        int row = m_table->rowCount();
        m_table->insertRow(row);

        auto item = [](const QString& t) {
            auto* i = new QTableWidgetItem(t);
            i->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            return i;
        };

        qint64 h = entry.duration / 3600, m = (entry.duration % 3600) / 60, s = entry.duration % 60;
        double sizeMB = entry.fileSize / 1048576.0;

        m_table->setItem(row, 0, item(entry.fileName));
        m_table->setItem(row, 1, item(entry.startTime.toString("yyyy-MM-dd  HH:mm")));
        m_table->setItem(row, 2, item(QString("%1:%2:%3")
            .arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0'))));
        m_table->setItem(row, 3, item(sizeMB > 1 ?
            QString("%1 MB").arg(sizeMB, 0, 'f', 1) : QString("%1 KB").arg(entry.fileSize/1024)));
        m_table->setItem(row, 4, item(entry.format));
        m_table->setRowHeight(row, 44);
        ++count;
    }

    bool empty = (count == 0);
    m_table->setVisible(!empty);
    m_emptyLabel->setVisible(empty);
    if (m_countLabel)
        m_countLabel->setText(empty ? "" : QString("(%1 recordings)").arg(count));
}

void RecordingsPage::deleteSelected() {
    int row = m_table->currentRow();
    if (row < 0) return;
    if (QMessageBox::question(this, "FrameX", "Delete this recording from history?",
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        HistoryManager::instance().removeEntry(row);
        loadRecordings();
    }
}

void RecordingsPage::openSelected() {
    int row = m_table->currentRow();
    if (row < 0) return;
    auto history = HistoryManager::instance().getHistory();
    if (row < history.size())
        QDesktopServices::openUrl(QUrl::fromLocalFile(history[row].filePath));
}

void RecordingsPage::openInEditor() {
    int row = m_table->currentRow();
    if (row < 0) return;
    auto history = HistoryManager::instance().getHistory();
    if (row >= history.size()) return;

    auto* dlg = new QDialog(this);
    dlg->setWindowTitle("FrameX — Video Editor");
    dlg->resize(900, 620);
    dlg->setStyleSheet("background: #0D0D1A;");
    auto* layout = new QVBoxLayout(dlg);
    layout->setContentsMargins(0, 0, 0, 0);
    auto* editor = new VideoEditor(dlg);
    editor->loadFile(history[row].filePath);
    layout->addWidget(editor);
    dlg->exec();
}
