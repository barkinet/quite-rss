#include "optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
  QDialog(parent)
{
  setWindowFlags (windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setWindowTitle(tr("Options"));

  categoriesTree = new QTreeWidget();
  categoriesTree->setObjectName("categoriesTree");
  categoriesTree->setHeaderHidden(true);
  categoriesTree->setColumnCount(3);
  categoriesTree->setColumnHidden(0, true);
  categoriesTree->header()->setStretchLastSection(false);
  categoriesTree->header()->resizeSection(2, 5);
  categoriesTree->header()->setResizeMode(1, QHeaderView::Stretch);
  categoriesTree->setMinimumWidth(150);
  QStringList treeItem;
  treeItem << "0" << tr("General");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "1" << tr("System tray");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "2" << tr("Network Connections");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "3" << tr("Browser");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "4" << tr("Feeds");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "5" << tr("Notifications");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "6" << tr("Language");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "7" << tr("Fonts");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "8" << tr("Keyboard shortcuts");
  categoriesTree->addTopLevelItem(new QTreeWidgetItem(treeItem));

  //{ general
  showSplashScreen_ = new QCheckBox(tr("Show splash screen on startup"));
  reopenFeedStartup_ = new QCheckBox(tr("Reopen last opened feeds on startup"));

  QVBoxLayout *generalLayout = new QVBoxLayout();
  generalLayout->addWidget(showSplashScreen_);
  generalLayout->addWidget(reopenFeedStartup_);
  generalLayout->addStretch();

  QFrame *generalWidget_ = new QFrame();
  generalWidget_->setFrameStyle(QFrame::Box | QFrame::Sunken);
  generalWidget_->setLayout(generalLayout);
  //} general

  //{ system tray
  showTrayIconBox_ = new QGroupBox(tr("Show system tray icon"));
  showTrayIconBox_->setCheckable(true);
  showTrayIconBox_->setChecked(false);

  startingTray_ = new QCheckBox(tr("starting QuiteRSS"));
  minimizingTray_ = new QCheckBox(tr("minimizing QuiteRSS"));
  closingTray_ = new QCheckBox(tr("closing QuiteRSS"));
  QVBoxLayout *moveTrayLayout = new QVBoxLayout();
  moveTrayLayout->setContentsMargins(15, 0, 5, 10);
  moveTrayLayout->addWidget(startingTray_);
  moveTrayLayout->addWidget(minimizingTray_);
  moveTrayLayout->addWidget(closingTray_);

  staticIconTray_ = new QRadioButton(tr("Show static icon"));
  changeIconTray_ = new QRadioButton(tr("Change icon for incoming new news"));
  newCountTray_ = new QRadioButton(tr("Show count of new news"));
  unreadCountTray_ = new QRadioButton(tr("Show count of unread news"));
  QVBoxLayout *behaviorLayout = new QVBoxLayout();
  behaviorLayout->setContentsMargins(15, 0, 5, 10);
  behaviorLayout->addWidget(staticIconTray_);
  behaviorLayout->addWidget(changeIconTray_);
  behaviorLayout->addWidget(newCountTray_);
  behaviorLayout->addWidget(unreadCountTray_);

  singleClickTray_ = new QCheckBox(
        tr("Single click instead of double click for show window"));
  clearStatusNew_ = new QCheckBox(tr("Clear status new on minimize to tray"));
  emptyWorking_ = new QCheckBox(tr("Empty working set on minimize to tray"));

  QVBoxLayout *trayLayout = new QVBoxLayout();
  trayLayout->addWidget(new QLabel(tr("Move to the system tray when:")));
  trayLayout->addLayout(moveTrayLayout);
  trayLayout->addWidget(new QLabel(tr("Tray icon behavior:")));
  trayLayout->addLayout(behaviorLayout);
  trayLayout->addWidget(singleClickTray_);
  trayLayout->addWidget(clearStatusNew_);
#if defined(Q_WS_WIN)
  trayLayout->addWidget(emptyWorking_);
#endif
  trayLayout->addStretch(1);

  showTrayIconBox_->setLayout(trayLayout);

  QVBoxLayout *boxTrayLayout = new QVBoxLayout();
  boxTrayLayout->setMargin(5);
  boxTrayLayout->addWidget(showTrayIconBox_);

  traySystemWidget_ = new QFrame();
  traySystemWidget_->setFrameStyle(QFrame::Box | QFrame::Sunken);
  traySystemWidget_->setLayout(boxTrayLayout);
  //} system tray

  //{ networkConnections
  systemProxyButton_ = new QRadioButton(
        tr("System proxy configuration (if available)"));
  //  systemProxyButton_->setEnabled(false);
  directConnectionButton_ = new QRadioButton(
        tr("Direct connection to the Internet"));
  manualProxyButton_ = new QRadioButton(tr("Manual proxy configuration:"));

  QVBoxLayout *networkConnectionsLayout = new QVBoxLayout();
  networkConnectionsLayout->addWidget(systemProxyButton_);
  networkConnectionsLayout->addWidget(directConnectionButton_);
  networkConnectionsLayout->addWidget(manualProxyButton_);

  editHost_ = new QLineEdit();
  editPort_ = new QLineEdit();
  editPort_->setFixedWidth(60);
  editUser_ = new QLineEdit();
  editPassword_ = new QLineEdit();

  QHBoxLayout *addrPortLayout = new QHBoxLayout();
  addrPortLayout->setMargin(0);
  addrPortLayout->addWidget(new QLabel(tr("Proxy server:")));
  addrPortLayout->addWidget(editHost_);
  addrPortLayout->addWidget(new QLabel(tr("Port:")));
  addrPortLayout->addWidget(editPort_);

  QWidget *addrPortWidget = new QWidget();
  addrPortWidget->setLayout(addrPortLayout);

  QGridLayout *userPasswordLayout = new QGridLayout();
  userPasswordLayout->setMargin(0);
  userPasswordLayout->addWidget(new QLabel(tr("Username:")), 0, 0);
  userPasswordLayout->addWidget(editUser_,                   0, 1);
  userPasswordLayout->addWidget(new QLabel(tr("Password:")), 1, 0);
  userPasswordLayout->addWidget(editPassword_,               1, 1);

  QWidget *userPasswordWidget = new QWidget();
  userPasswordWidget->setLayout(userPasswordLayout);

  QVBoxLayout *manualLayout = new QVBoxLayout();
  manualLayout->setMargin(0);
  manualLayout->addWidget(addrPortWidget);
  manualLayout->addWidget(userPasswordWidget);
  manualLayout->addStretch();

  manualWidget_ = new QWidget();
  manualWidget_->setEnabled(false);  // т.к. при создании соответствующая радио-кнока не выбрана
  // @TODO(arhohryakov:2011.12.08): убрать границу и заголовок группы
  manualWidget_->setLayout(manualLayout);

  networkConnectionsLayout->addWidget(manualWidget_);

  networkConnectionsWidget_ = new QFrame();
  networkConnectionsWidget_->setFrameStyle(QFrame::Box | QFrame::Sunken);
  networkConnectionsWidget_->setLayout(networkConnectionsLayout);
  //} networkConnections

  //{ browser
  javaScriptEnable_ = new QCheckBox(tr("Enable JavaScript"));
  pluginsEnable_ = new QCheckBox(tr("Enable plug-ins"));

  QVBoxLayout *embeddedBrowserLayout = new QVBoxLayout();
  embeddedBrowserLayout->addWidget(javaScriptEnable_);
  embeddedBrowserLayout->addWidget(pluginsEnable_);

  embeddedBrowserOn_ = new QGroupBox(tr("Use the embedded browser"));
  embeddedBrowserOn_->setCheckable(true);
  embeddedBrowserOn_->setLayout(embeddedBrowserLayout);

  QVBoxLayout *browserLayout = new QVBoxLayout();
  browserLayout->setMargin(5);
  browserLayout->addWidget(embeddedBrowserOn_);
  browserLayout->addStretch();

  browserWidget_ = new QFrame();
  browserWidget_->setFrameStyle(QFrame::Box | QFrame::Sunken);
  browserWidget_->setLayout(browserLayout);
  //} browser

  //{ feeds
  updateFeedsStartUp_ = new QCheckBox(
        tr("Automatically update the feeds on startup"));
  updateFeeds_ = new QCheckBox(tr("Automatically update the feeds every"));
  updateFeedsTime_ = new QSpinBox();
  updateFeedsTime_->setEnabled(false);
  updateFeedsTime_->setRange(5, 9999);
  connect(updateFeeds_, SIGNAL(toggled(bool)),
          updateFeedsTime_, SLOT(setEnabled(bool)));

  intervalTime_ = new QComboBox(this);
  intervalTime_->setEnabled(false);
  QStringList intervalList;
  intervalList << tr("minutes")  << tr("hours");
  intervalTime_->addItems(intervalList);
  connect(updateFeeds_, SIGNAL(toggled(bool)),
          intervalTime_, SLOT(setEnabled(bool)));
  connect(intervalTime_, SIGNAL(currentIndexChanged(QString)),
          this, SLOT(intervalTimeChang(QString)));

  QHBoxLayout *updateFeedsLayout1 = new QHBoxLayout();
  updateFeedsLayout1->setMargin(0);
  updateFeedsLayout1->addWidget(updateFeeds_);
  updateFeedsLayout1->addWidget(updateFeedsTime_);
  updateFeedsLayout1->addWidget(intervalTime_);
  updateFeedsLayout1->addStretch();

  QVBoxLayout *updateFeedsLayout = new QVBoxLayout();
  updateFeedsLayout->addWidget(updateFeedsStartUp_);
  updateFeedsLayout->addLayout(updateFeedsLayout1);
  updateFeedsLayout->addStretch();

  QWidget *updateFeedsWidget_ = new QWidget();
  updateFeedsWidget_->setLayout(updateFeedsLayout);
//
  markNewsReadOn_ = new QCheckBox(tr("Mark selected news as read after"));
  markNewsReadTime_ = new QSpinBox();
  markNewsReadTime_->setEnabled(false);
  markNewsReadTime_->setRange(0, 100);
  connect(markNewsReadOn_, SIGNAL(toggled(bool)),
          markNewsReadTime_, SLOT(setEnabled(bool)));

  showDescriptionNews_ = new QCheckBox(
        tr("Show news' description instead loading web page"));

  QHBoxLayout *readingFeedsLayout1 = new QHBoxLayout();
  readingFeedsLayout1->setMargin(0);
  readingFeedsLayout1->addWidget(markNewsReadOn_);
  readingFeedsLayout1->addWidget(markNewsReadTime_);
  readingFeedsLayout1->addWidget(new QLabel(tr("seconds")));
  readingFeedsLayout1->addStretch();

  QVBoxLayout *readingFeedsLayout = new QVBoxLayout();
  readingFeedsLayout->addLayout(readingFeedsLayout1);
  readingFeedsLayout->addWidget(showDescriptionNews_);
  readingFeedsLayout->addStretch();

  QWidget *readingFeedsWidget_ = new QWidget();
  readingFeedsWidget_->setLayout(readingFeedsLayout);
//
  dayCleanUpOn_ = new QCheckBox(tr("Maximum age of news in days to keep:"));
  maxDayCleanUp_ = new QSpinBox();
  maxDayCleanUp_->setEnabled(false);
  maxDayCleanUp_->setRange(0, 9999);
  connect(dayCleanUpOn_, SIGNAL(toggled(bool)),
          maxDayCleanUp_, SLOT(setEnabled(bool)));

  newsCleanUpOn_ = new QCheckBox(tr("Maximum number of news to keep:"));
  maxNewsCleanUp_ = new QSpinBox();
  maxNewsCleanUp_->setEnabled(false);
  maxNewsCleanUp_->setRange(0, 9999);
  connect(newsCleanUpOn_, SIGNAL(toggled(bool)),
          maxNewsCleanUp_, SLOT(setEnabled(bool)));

  readCleanUp_ = new QCheckBox(tr("Delete read news"));
  neverUnreadCleanUp_ = new QCheckBox(tr("Never delete unread news"));
  neverStarCleanUp_ = new QCheckBox(tr("Never delete starred news"));

  QGridLayout *cleanUpFeedsLayout1 = new QGridLayout();
  cleanUpFeedsLayout1->setMargin(0);
  cleanUpFeedsLayout1->setColumnStretch(1, 1);
  cleanUpFeedsLayout1->addWidget(dayCleanUpOn_, 0, 0, 1, 1);
  cleanUpFeedsLayout1->addWidget(maxDayCleanUp_, 0, 1, 1, 1, Qt::AlignLeft);
  cleanUpFeedsLayout1->addWidget(newsCleanUpOn_, 1, 0, 1, 1);
  cleanUpFeedsLayout1->addWidget(maxNewsCleanUp_, 1, 1, 1, 1, Qt::AlignLeft);
  cleanUpFeedsLayout1->addWidget(readCleanUp_, 2, 0, 1, 1);
  cleanUpFeedsLayout1->addWidget(neverUnreadCleanUp_, 3, 0, 1, 1);
  cleanUpFeedsLayout1->addWidget(neverStarCleanUp_, 4, 0, 1, 1);

  QVBoxLayout *cleanUpFeedsLayout = new QVBoxLayout();
  cleanUpFeedsLayout->addLayout(cleanUpFeedsLayout1);
  cleanUpFeedsLayout->addStretch();

  QWidget *cleanUpFeedsWidget_ = new QWidget();
  cleanUpFeedsWidget_->setLayout(cleanUpFeedsLayout);

  feedsWidget_ = new QTabWidget();
  feedsWidget_->addTab(updateFeedsWidget_, tr("General"));
  feedsWidget_->addTab(readingFeedsWidget_, tr("Reading"));
  feedsWidget_->addTab(cleanUpFeedsWidget_, tr("Clean Up"));
  //} feeds

  //{ notifier
  soundNewNews_ = new QCheckBox(tr("Play sound for incoming new news"));

  QVBoxLayout *notifierLayout = new QVBoxLayout();
  notifierLayout->addWidget(soundNewNews_);
  notifierLayout->addStretch();

  notifierWidget_ = new QFrame();
  notifierWidget_->setFrameStyle(QFrame::Box | QFrame::Sunken);
  notifierWidget_->setLayout(notifierLayout);
  //} notifier

  //{ language
  languageFileList_ = new QListWidget();
  languageFileList_->setObjectName("languageFileList_");

  QVBoxLayout *languageLayout = new QVBoxLayout();
  languageLayout->setMargin(0);
  languageLayout->addWidget(new QLabel(tr("Choose language:")));
  languageLayout->addWidget(languageFileList_);

  languageWidget_ = new QWidget();
  languageWidget_->setLayout(languageLayout);

  // init list
  QStringList languageList;
  languageList << QString(tr("English (%1)")).arg("en")
               << QString(tr("German (%1)")).arg("de")
               << QString(tr("Russian (%1)")).arg("ru");
  foreach (QString str, languageList) {
    new QListWidgetItem(str, languageFileList_);
  }
  //} language

  //{ fonts
  fontTree = new QTreeWidget();
  fontTree->setObjectName("fontTree");
  fontTree->setColumnCount(3);
  fontTree->setColumnHidden(0, true);
  fontTree->setColumnWidth(1, 140);

  treeItem.clear();
  treeItem << tr("Id") << tr("Type") << tr("Font");
  fontTree->setHeaderLabels(treeItem);

  treeItem.clear();
  treeItem << "0" << tr("Feeds list font");
  fontTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "1" << tr("News list font");
  fontTree->addTopLevelItem(new QTreeWidgetItem(treeItem));
  treeItem.clear();
  treeItem << "2" << tr("News font");
  fontTree->addTopLevelItem(new QTreeWidgetItem(treeItem));

  fontTree->setCurrentItem(fontTree->topLevelItem(0));
  connect(fontTree, SIGNAL(doubleClicked(QModelIndex)),
          this, SLOT(slotFontChange()));

  QPushButton *fontChange = new QPushButton(tr("Change..."));
  connect(fontChange, SIGNAL(clicked()), this, SLOT(slotFontChange()));
  QPushButton *fontReset = new QPushButton(tr("Reset"));
  connect(fontReset, SIGNAL(clicked()), this, SLOT(slotFontReset()));
  QVBoxLayout *fontsButtonLayout = new QVBoxLayout();
  fontsButtonLayout->addWidget(fontChange);
  fontsButtonLayout->addWidget(fontReset);
  fontsButtonLayout->addStretch(1);

  QHBoxLayout *fontsLayout = new QHBoxLayout();
  fontsLayout->setMargin(0);
  fontsLayout->addWidget(fontTree, 1);
  fontsLayout->addLayout(fontsButtonLayout);

  fontsWidget_ = new QWidget();
  fontsWidget_->setLayout(fontsLayout);
  //} fonts

  //{ shortcut
  shortcutTree_ = new QTreeWidget();
  shortcutTree_->setObjectName("shortcutTree");
  shortcutTree_->setSortingEnabled(true);
  shortcutTree_->setColumnCount(4);
  shortcutTree_->hideColumn(0);
  shortcutTree_->setSelectionBehavior(QAbstractItemView::SelectRows);
  shortcutTree_->header()->setStretchLastSection(false);
  shortcutTree_->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  shortcutTree_->header()->setResizeMode(2, QHeaderView::Stretch);
  shortcutTree_->header()->setResizeMode(3, QHeaderView::ResizeToContents);

  treeItem.clear();
  treeItem << "Id" << tr("Action") << tr("Description") << tr("Shortcut");
  shortcutTree_->setHeaderLabels(treeItem);

  editShortcut_ = new QLineEdit();
  QPushButton *clearShortcutButton = new QPushButton(tr("Clear"));
  QPushButton *resetShortcutButton = new QPushButton(tr("Reset"));

  QHBoxLayout *editShortcutLayout = new QHBoxLayout();
  editShortcutLayout->addWidget(new QLabel(tr("Shortcut:")));
  editShortcutLayout->addWidget(editShortcut_, 1);
  editShortcutLayout->addWidget(clearShortcutButton);
  editShortcutLayout->addWidget(resetShortcutButton);

  editShortcutBox = new QGroupBox();
  editShortcutBox->setEnabled(false);
  editShortcutBox->setLayout(editShortcutLayout);

  QVBoxLayout *shortcutLayout = new QVBoxLayout();
  shortcutLayout->setMargin(0);
  shortcutLayout->addWidget(shortcutTree_, 1);
  shortcutLayout->addWidget(editShortcutBox);

  shortcutWidget_ = new QWidget();
  shortcutWidget_->setLayout(shortcutLayout);

  connect(shortcutTree_, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
          this, SLOT(shortcutTreeClicked(QTreeWidgetItem*,int)));
  connect(this, SIGNAL(signalShortcutTreeUpDownPressed()),
          SLOT(slotShortcutTreeUpDownPressed()), Qt::QueuedConnection);
  connect(clearShortcutButton, SIGNAL(clicked()),
          this, SLOT(slotClearShortcut()));
  connect(resetShortcutButton, SIGNAL(clicked()),
          this, SLOT(slotResetShortcut()));

  shortcutTree_->installEventFilter(this);
  editShortcut_->installEventFilter(this);
  //} shortcut

  contentLabel_ = new QLabel(tr("ContentLabel"));
  contentLabel_->setObjectName("contentLabel_");
  contentLabel_->setFrameStyle(QFrame::Box | QFrame::Sunken);
  QFont fontContentLabel = contentLabel_->font();
  fontContentLabel.setBold(true);
  fontContentLabel.setPointSize(fontContentLabel.pointSize()+2);
  contentLabel_->setFont(fontContentLabel);

  contentStack_ = new QStackedWidget();
  contentStack_->setObjectName("contentStack_");
  contentStack_->addWidget(generalWidget_);
  contentStack_->addWidget(traySystemWidget_);
  contentStack_->addWidget(networkConnectionsWidget_);
  contentStack_->addWidget(browserWidget_);
  contentStack_->addWidget(feedsWidget_);
  contentStack_->addWidget(notifierWidget_);
  contentStack_->addWidget(languageWidget_);
  contentStack_->addWidget(fontsWidget_);
  contentStack_->addWidget(shortcutWidget_);

  QVBoxLayout *contentLayout = new QVBoxLayout();
  contentLayout->setMargin(0);
  contentLayout->addWidget(contentLabel_);
  contentLayout->addWidget(contentStack_);

  QWidget *contentWidget = new QWidget();
  contentWidget->setObjectName("contentWidget");
  contentWidget->setLayout(contentLayout);

  QVBoxLayout *categoriesTreeLayout = new QVBoxLayout();
  categoriesTreeLayout->setMargin(0);
  categoriesTreeLayout->addWidget(categoriesTree);
  QWidget *categoriesTreeWidget = new QWidget();
  categoriesTreeWidget->setLayout(categoriesTreeLayout);

  QSplitter *splitter = new QSplitter();
  splitter->setChildrenCollapsible(false);
  splitter->addWidget(categoriesTreeWidget);
  splitter->addWidget(contentWidget);
  QList<int> sizes;
  sizes << 150 << 600;
  splitter->setSizes(sizes);

  buttonBox_ = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->setMargin(5);
  mainLayout->addWidget(splitter);
  mainLayout->addWidget(buttonBox_);

  setLayout(mainLayout);

  connect(categoriesTree, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
          this, SLOT(slotCategoriesItemClicked(QTreeWidgetItem*,int)));
  connect(this, SIGNAL(signalCategoriesTreeKeyUpDownPressed()),
          SLOT(slotCategoriesTreeKeyUpDownPressed()), Qt::QueuedConnection);
  connect(buttonBox_, SIGNAL(accepted()), this, SLOT(acceptSlot()));
  connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
  connect(manualProxyButton_, SIGNAL(toggled(bool)),
          this, SLOT(manualProxyToggle(bool)));

  categoriesTree->installEventFilter(this);

  // не нужно, т.к. после создания окна из главного окна передаются настройки
  //  manualProxyToggle(manualProxyButton_->isChecked());
}

bool OptionsDialog::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if (obj != editShortcut_) {
      if ((keyEvent->key() == Qt::Key_Up) ||
          (keyEvent->key() == Qt::Key_Down)) {
        if (obj == categoriesTree)
          emit signalCategoriesTreeKeyUpDownPressed();
        else if (obj == shortcutTree_)
          emit signalShortcutTreeUpDownPressed();
      }
    } else {
      if ((keyEvent->key() < Qt::Key_Shift) ||
          (keyEvent->key() > Qt::Key_Alt)) {
        QString str;
        if ((keyEvent->modifiers() & Qt::ShiftModifier) ||
            (keyEvent->modifiers() & Qt::ControlModifier) ||
            (keyEvent->modifiers() & Qt::AltModifier))
          str.append(QKeySequence(keyEvent->modifiers()).toString());
        str.append(QKeySequence(keyEvent->key()).toString());
        editShortcut_->setText(str);
        shortcutTree_->currentItem()->setText(3, str);
      }
      return true;
    }
    return false;
  } else {
    return QDialog::eventFilter(obj, event);
  }
}

void OptionsDialog::slotCategoriesTreeKeyUpDownPressed()
{
  slotCategoriesItemClicked(categoriesTree->currentItem(), 1);
}

void OptionsDialog::slotCategoriesItemClicked(QTreeWidgetItem* item, int column)
{
  Q_UNUSED(column)
  contentLabel_->setText(item->data(1, Qt::DisplayRole).toString());
  contentStack_->setCurrentIndex(item->data(0, Qt::DisplayRole).toInt());
}

void OptionsDialog::manualProxyToggle(bool checked)
{
  manualWidget_->setEnabled(checked);
}

QNetworkProxy OptionsDialog::proxy()
{
  return networkProxy_;
}

void OptionsDialog::setProxy(const QNetworkProxy proxy)
{
  networkProxy_ = proxy;
  updateProxy();
}

QString OptionsDialog::language()
{
  QString langFileName = languageFileList_->currentItem()->data(Qt::DisplayRole).toString();
  return langFileName.section("(", 1).replace(")", "");
}

void OptionsDialog::setLanguage(QString langFileName)
{
  // установка курсора на выбранный файл
  langFileName = QString("(%1)").arg(langFileName);

  QList<QListWidgetItem*> list =
      languageFileList_->findItems(langFileName, Qt::MatchContains);
  if (list.count()) {
    languageFileList_->setCurrentItem(list.at(0));
  } else {
    // если не удалось найти, выбираем английский
    QList<QListWidgetItem*> list =
        languageFileList_->findItems("(en)", Qt::MatchContains);
    languageFileList_->setCurrentItem(list.at(0));
  }
}

void OptionsDialog::updateProxy()
{
  switch (networkProxy_.type()) {
  case QNetworkProxy::HttpProxy:
    manualProxyButton_->setChecked(true);
    break;
  case QNetworkProxy::NoProxy:
    directConnectionButton_->setChecked(true);
    break;
  case QNetworkProxy::DefaultProxy:
  default:
    systemProxyButton_->setChecked(true);
  }
  editHost_->setText(networkProxy_.hostName());
  editPort_->setText(QVariant(networkProxy_.port()).toString());
  editUser_->setText(networkProxy_.user());
  editPassword_->setText(networkProxy_.password());
}

void OptionsDialog::applyProxy()
{
  if (systemProxyButton_->isChecked())
    networkProxy_.setType(QNetworkProxy::DefaultProxy);
  else if (manualProxyButton_->isChecked())
    networkProxy_.setType(QNetworkProxy::HttpProxy);
  else
    networkProxy_.setType(QNetworkProxy::NoProxy);
  networkProxy_.setHostName(editHost_->text());
  networkProxy_.setPort(    editPort_->text().toInt());
  networkProxy_.setUser(    editUser_->text());
  networkProxy_.setPassword(editPassword_->text());
}

void OptionsDialog::acceptSlot()
{
  applyProxy();
  accept();
}

void OptionsDialog::slotFontChange()
{
  bool bOk;
  QFont curFont;
  curFont.setFamily(fontTree->currentItem()->text(2).section(", ", 0, 0));
  curFont.setPointSize(fontTree->currentItem()->text(2).section(", ", 1).toInt());

  QFont font = QFontDialog::getFont(&bOk, curFont);
  if (bOk) {
    QString strFont = QString("%1, %2").
        arg(font.family()).
        arg(font.pointSize());
    fontTree->currentItem()->setText(2, strFont);
  }
}

void OptionsDialog::slotFontReset()
{
  switch (fontTree->currentItem()->text(0).toInt()) {
  case 2: fontTree->currentItem()->setText(
          2, QString("%1, 12").arg(qApp->font().family()));
    break;
  default: fontTree->currentItem()->setText(
          2, QString("%1, 8").arg(qApp->font().family()));
  }
}

int OptionsDialog::currentIndex()
{
  return categoriesTree->currentItem()->text(0).toInt();
}

void OptionsDialog::setCurrentItem(int index)
{
  categoriesTree->setCurrentItem(categoriesTree->topLevelItem(index), 1);
  slotCategoriesItemClicked(categoriesTree->topLevelItem(index), 1);
}

void OptionsDialog::setBehaviorIconTray(int behavior)
{
  switch (behavior) {
  case 1: changeIconTray_->setChecked(true);  break;
  case 2: newCountTray_->setChecked(true);    break;
  case 3: unreadCountTray_->setChecked(true); break;
  default: staticIconTray_->setChecked(true);
  }
}

int OptionsDialog::behaviorIconTray()
{
  if (staticIconTray_->isChecked())       return 0;
  else if (changeIconTray_->isChecked())  return 1;
  else if (newCountTray_->isChecked())    return 2;
  else if (unreadCountTray_->isChecked()) return 3;
  else return 0;
}

void  OptionsDialog::intervalTimeChang(QString str)
{
  if (str == tr("minutes")) {
    updateFeedsTime_->setRange(5, 9999);
  } else if (str == tr("hours")) {
    updateFeedsTime_->setRange(1, 9999);
  }
}

void OptionsDialog::loadActionShortcut(QList<QAction *> actions, QStringList *list)
{
  QStringList treeItem;
  int id = 0;

  QListIterator<QAction *> iter(actions);
  while (iter.hasNext()) {
    QAction *pAction = iter.next();

    treeItem.clear();
    treeItem << QString::number(id) << pAction->text().remove("&")
             << pAction->toolTip() << pAction->shortcut();
    QTreeWidgetItem *item = new QTreeWidgetItem(treeItem);

    if (pAction->icon().isNull())
      item->setIcon(1, QIcon(":/images/images/noicon.png"));
    else
      item->setIcon(1, pAction->icon());
    shortcutTree_->addTopLevelItem(item);

    ++id;
  }

  listDefaultShortcut_ = list;
}

void OptionsDialog::saveActionShortcut(QList<QAction *> actions)
{ 
  for (int i = 0; i < shortcutTree_->topLevelItemCount(); i++) {
    int id = shortcutTree_->topLevelItem(i)->text(0).toInt();
    actions.at(id)->setShortcut(
          QKeySequence(shortcutTree_->topLevelItem(i)->text(3)));
  }
}

void OptionsDialog::slotShortcutTreeUpDownPressed()
{
  shortcutTreeClicked(shortcutTree_->currentItem(), 1);
}

void OptionsDialog::shortcutTreeClicked(QTreeWidgetItem* item, int column)
{
  Q_UNUSED(column)
  editShortcut_->setText(item->text(3));
  editShortcutBox->setEnabled(true);
}

void OptionsDialog::slotClearShortcut()
{
  editShortcut_->clear();
  shortcutTree_->currentItem()->setText(3, "");
}

void OptionsDialog::slotResetShortcut()
{
  int id = shortcutTree_->currentItem()->data(0, Qt::DisplayRole).toInt();
  QString str = listDefaultShortcut_->at(id);
  editShortcut_->setText(str);
  shortcutTree_->currentItem()->setText(3, str);
}
