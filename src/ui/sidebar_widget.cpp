#include "sidebar_widget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QPixmap>

SidebarWidget::SidebarWidget(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void SidebarWidget::setupUI() {
    setObjectName("sidebar");
    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(10, 16, 10, 16);

    // Logo area
    auto* logoWidget = new QWidget(this);
    logoWidget->setFixedHeight(60);
    auto* logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(8, 0, 8, 0);

    auto* logoIcon = new QLabel("🎬");
    logoIcon->setFixedSize(32, 32);
    QFont iconFont; iconFont.setPointSize(20);
    logoIcon->setFont(iconFont);

    auto* logoText = new QLabel("FrameX");
    QFont lf("Segoe UI", 16, QFont::Bold);
    logoText->setFont(lf);
    logoText->setStyleSheet("color: #C084FC;");

    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(logoText);
    logoLayout->addStretch();

    layout->addWidget(logoWidget);

    // Navigation divider label
    auto* navLabel = new QLabel("NAVIGATION");
    navLabel->setObjectName("sectionLabel");
    navLabel->setContentsMargins(12, 8, 0, 4);
    layout->addWidget(navLabel);

    // Nav buttons
    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->setExclusive(true);

    struct NavItem { QString icon; QString label; int idx; };
    QList<NavItem> items = {
        {"⊞", "Dashboard", 0},
        {"⏺", "Recordings", 1},
        {"📡", "Streaming", 2},
        {"🖼", "Screenshots", 3},
        {"⚙", "Settings", 4},
        {"⌨", "Hotkeys", 5},
        {"🔧", "Tools", 6},
    };

    for (auto& item : items) {
        QPushButton* btn = makeNavButton(item.icon, item.label, item.idx);
        layout->addWidget(btn);
        m_btnGroup->addButton(btn, item.idx);
        connect(btn, &QPushButton::clicked, [this, item]() {
            emit pageSelected(item.idx);
        });
        if (item.idx == 0) btn->setChecked(true);
    }

    layout->addStretch();

    // Recording indicator
    m_recordingIndicator = new QLabel("● RECORDING");
    m_recordingIndicator->setAlignment(Qt::AlignCenter);
    m_recordingIndicator->setStyleSheet("color: #EF4444; font-weight: bold; font-size: 12px; padding: 6px;");
    m_recordingIndicator->hide();
    layout->addWidget(m_recordingIndicator);

    // Storage info
    auto* storageFrame = new QFrame(this);
    storageFrame->setStyleSheet("background: #12122A; border-radius: 10px; padding: 8px;");
    auto* storageLayout = new QVBoxLayout(storageFrame);
    storageLayout->setSpacing(4);
    storageLayout->setContentsMargins(10, 10, 10, 10);

    auto* storageTitleRow = new QHBoxLayout();
    auto* storageLabel = new QLabel("Storage");
    storageLabel->setStyleSheet("color: #94A3B8; font-size: 12px;");
    m_storageUsedLabel = new QLabel("-- GB / -- GB");
    m_storageUsedLabel->setStyleSheet("color: #CBD5E1; font-size: 12px;");
    storageTitleRow->addWidget(storageLabel);
    storageTitleRow->addStretch();
    storageTitleRow->addWidget(m_storageUsedLabel);

    m_storageBar = new QProgressBar(this);
    m_storageBar->setFixedHeight(4);
    m_storageBar->setRange(0, 100);
    m_storageBar->setValue(39);
    m_storageBar->setTextVisible(false);

    storageLayout->addLayout(storageTitleRow);
    storageLayout->addWidget(m_storageBar);
    layout->addWidget(storageFrame);

    // Separator
    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #1E1B3A;");
    layout->addWidget(sep);

    // User profile
    auto* profileWidget = new QWidget(this);
    auto* profileLayout = new QHBoxLayout(profileWidget);
    profileLayout->setContentsMargins(8, 4, 8, 4);

    auto* avatarLabel = new QLabel("Z");
    avatarLabel->setFixedSize(36, 36);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #7C3AED,stop:1 #C084FC);"
        "border-radius: 18px; color: white; font-weight: bold; font-size: 16px;");

    auto* userInfoLayout = new QVBoxLayout();
    userInfoLayout->setSpacing(0);
    m_userLabel = new QLabel("FrameX User");
    m_userLabel->setStyleSheet("color: #E2E8F0; font-size: 12px; font-weight: bold;");
    auto* devLabel = new QLabel("By ZAKI | @zg22x");
    devLabel->setStyleSheet("color: #475569; font-size: 10px;");
    userInfoLayout->addWidget(m_userLabel);
    userInfoLayout->addWidget(devLabel);

    // PRO badge
    auto* proBadge = new QLabel("PRO");
    proBadge->setFixedSize(32, 18);
    proBadge->setAlignment(Qt::AlignCenter);
    proBadge->setStyleSheet(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #7C3AED,stop:1 #C084FC);"
        "border-radius: 4px; color: white; font-size: 9px; font-weight: bold;");

    profileLayout->addWidget(avatarLabel);
    profileLayout->addLayout(userInfoLayout);
    profileLayout->addStretch();
    profileLayout->addWidget(proBadge);
    layout->addWidget(profileWidget);

    // Update storage info
    QStorageInfo storageInfo(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));
    if (storageInfo.isValid()) {
        double totalGB = storageInfo.bytesTotal() / 1073741824.0;
        double usedGB = (storageInfo.bytesTotal() - storageInfo.bytesFree()) / 1073741824.0;
        m_storageUsedLabel->setText(QString("%1 GB / %2 GB")
            .arg(usedGB, 0, 'f', 1).arg(totalGB, 0, 'f', 0));
        m_storageBar->setValue((int)(usedGB / totalGB * 100));
    }
}

QPushButton* SidebarWidget::makeNavButton(const QString& icon, const QString& text, int index) {
    Q_UNUSED(index)
    auto* btn = new QPushButton(this);
    btn->setObjectName("sidebarBtn");
    btn->setText("  " + icon + "  " + text);
    btn->setCheckable(true);
    btn->setFixedHeight(44);
    btn->setCursor(Qt::PointingHandCursor);
    QFont f("Segoe UI", 13);
    btn->setFont(f);
    return btn;
}

void SidebarWidget::setRecordingState(bool recording) {
    m_isRecording = recording;
    m_recordingIndicator->setVisible(recording);
}
