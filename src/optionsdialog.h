#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QtGui>
#include <QNetworkProxy>

class OptionsDialog : public QDialog
{
  Q_OBJECT
public:
  explicit OptionsDialog(QWidget *parent = 0);
  QNetworkProxy proxy();
  void setProxy(const QNetworkProxy proxy);
  QString language();
  void setLanguage(QString langFileName);
  int currentIndex();
  void setCurrentItem(int index);
  void setBehaviourTray(int behaviour);
  int behaviourTray();

  // systemTray
  QCheckBox *startingTray_;
  QCheckBox *minimizingTray_;
  QCheckBox *closingTray_;
  QCheckBox *singleClickTray_;
  QCheckBox *emptyWorking_;
  QRadioButton *noNotificationTray_;
  QRadioButton *notificationTray_;
  QRadioButton *countNewTray_;
  QRadioButton *countUnreadTray_;

  // feeds
  QCheckBox *updateFeedsStartUp_;
  QCheckBox *updateFeeds_;
  QSpinBox *updateFeedsTime_;

  QCheckBox *markNewsReadOn_;
  QSpinBox *markNewsReadTime_;

  // fonts
  QTreeWidget *fontTree;

private slots:
  void slotCategoriesItemCLicked(QTreeWidgetItem* item, int column);
  void manualProxyToggle(bool checked);
  void updateProxy();
  void applyProxy();
  void acceptSlot();
  void slotFontChange();
  void slotFontReset();

signals:

private:
  QLabel *contentLabel_;
  QStackedWidget *contentStack_;
  QTreeWidget *categoriesTree;

  //stack widgets
  QWidget *generalWidget_;
  QWidget *networkConnectionsWidget_;
  QTabWidget *feedsWidget_;
  QWidget *languageWidget_;
  QWidget *fontsWidget_;

  QDialogButtonBox *buttonBox_;

  // network connection
  QRadioButton *systemProxyButton_;
  QRadioButton *directConnectionButton_;
  QRadioButton *manualProxyButton_;
  QWidget *manualWidget_;
  QLineEdit *editHost_;
  QLineEdit *editPort_;
  QLineEdit *editUser_;
  QLineEdit *editPassword_;

  // language
  QListWidget *languageFileList_;  

  // internal variables for options
  QNetworkProxy networkProxy_;
};

#endif // OPTIONSDIALOG_H
