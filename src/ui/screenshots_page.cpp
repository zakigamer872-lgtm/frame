#include "screenshots_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QPixmap>
#include <QPushButton>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>

ScreenshotsPage::ScreenshotsPage(QWidget* parent) : QWidget(parent) {
    setupUI();
    loadScreenshots();
}

void ScreenshotsPage::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    // Header
    auto* headerRow = new QHBoxLayout();
    auto* titleLabel = new QLabel("🖼  Screenshots");
    QFont tf; tf.setFamily("Segoe UI"); tf.setPointSize(18); tf.setBold(true);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color: #F1F5F9;");

    m_countLabel = new QLabel("");
    m_countLabel->setStyleSheet("color: #64748B; font-size: 13px;");

    auto* openFolderBtn = new QPushButton("📂  Open Folder");
    openFolderBtn->setFixedHeight(36);
    openFolderBtn->setStyleSheet(
        "QPushButton { background: #12122A; border: 1px solid #2D2B55; border-radius: 8px; "
        "color: #CBD5E1; padding: 0 14px; } QPushButton:hover { background: #1E1050; border-color: #7C3AED; }");
    openFolderBtn->setCursor(Qt::PointingHandCursor);
    connect(openFolderBtn, &QPushButton::clicked, [](){
        QString dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                      + "/FrameX Screenshots";
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
    });

    headerRow->addWidget(titleLabel);
    headerRow->addWidget(m_countLabel);
    headerRow->addStretch();
    headerRow->addWidget(openFolderBtn);
    layout->addLayout(headerRow);

    // Scroll area with grid
    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background: transparent; }");

    m_gridWidget = new QWidget();
    m_gridWidget->setStyleSheet("background: transparent;");
    m_gridLayout = new QGridLayout(m_gridWidget);
    m_gridLayout->setSpacing(12);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);

    scroll->setWidget(m_gridWidget);
    layout->addWidget(scroll, 1);

    m_emptyLabel = new QLabel("No screenshots yet.\nPress Ctrl+Shift+S to capture.");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setStyleSheet("color: #475569; font-size: 15px; padding: 60px;");
}

void ScreenshotsPage::loadScreenshots() {
    // Clear existing items
    while (QLayoutItem* item = m_gridLayout->takeAt(0)) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QString dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                  + "/FrameX Screenshots";
    QDir d(dir);
    QStringList files = d.entryList({"*.png", "*.jpg", "*.jpeg"}, QDir::Files, QDir::Time);

    if (files.isEmpty()) {
        m_gridLayout->addWidget(m_emptyLabel, 0, 0, Qt::AlignCenter);
        m_emptyLabel->show();
        m_countLabel->setText("");
        return;
    }

    m_emptyLabel->hide();
    m_countLabel->setText(QString("(%1 screenshots)").arg(files.size()));

    int col = 0, row = 0;
    const int cols = 4;
    for (auto& f : files) {
        QString path = dir + "/" + f;
        auto* card = new QFrame();
        card->setStyleSheet(
            "QFrame { background: #0F0F24; border: 1px solid #1E1B3A; border-radius: 10px; }"
            "QFrame:hover { border-color: #7C3AED; }");
        card->setCursor(Qt::PointingHandCursor);
        card->setFixedSize(190, 150);

        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(6, 6, 6, 6);
        cl->setSpacing(4);

        auto* thumb = new QLabel();
        QPixmap px(path);
        thumb->setPixmap(px.scaled(178, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        thumb->setAlignment(Qt::AlignCenter);
        thumb->setFixedHeight(110);

        auto* nameLabel = new QLabel(f);
        nameLabel->setStyleSheet("color: #64748B; font-size: 9px;");
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setElideMode(Qt::ElideMiddle);

        cl->addWidget(thumb);
        cl->addWidget(nameLabel);

        // Click to open
        card->installEventFilter(this);
        connect(new QObject(card), &QObject::destroyed, [](){}); // lifetime
        card->setProperty("path", path);
        card->setAttribute(Qt::WA_Hover, true);

        auto* openBtn = new QPushButton("▶");
        openBtn->setParent(card);
        openBtn->setFixedSize(28, 28);
        openBtn->move(155, 6);
        openBtn->setStyleSheet(
            "QPushButton { background: rgba(124,58,237,0.8); border: none; border-radius: 8px; color: white; }"
            "QPushButton:hover { background: #7C3AED; }");
        openBtn->setCursor(Qt::PointingHandCursor);
        connect(openBtn, &QPushButton::clicked, [path](){
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        });

        m_gridLayout->addWidget(card, row, col);
        if (++col >= cols) { col = 0; ++row; }
    }
    m_gridLayout->setRowStretch(row + 1, 1);
}
