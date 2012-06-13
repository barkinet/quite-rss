#include "filterrulesdialog.h"
#include "newsfiltersdialog.h"
#include "rsslisting.h"

NewsFiltersDialog::NewsFiltersDialog(QWidget *parent, QSettings *settings)
  : QDialog(parent),
    settings_(settings)
{
  setWindowFlags (windowFlags() & ~Qt::WindowContextHelpButtonHint);
  setWindowTitle(tr("News filters"));
  setMinimumWidth(500);
  setMinimumHeight(250);

  filtersTree = new QTreeWidget(this);
  filtersTree->setObjectName("filtersTree");
  filtersTree->setColumnCount(3);
  filtersTree->setColumnHidden(0, true);
  filtersTree->header()->resizeSection(1, 150);
  filtersTree->header()->resizeSection(2, 350);
  filtersTree->header()->setMovable(false);

  QStringList treeItem;
  treeItem << "Id" << tr("Name filter") << tr("Feeds");
  filtersTree->setHeaderLabels(treeItem);

  RSSListing *rssl_ = qobject_cast<RSSListing*>(parentWidget());
  QSqlQuery q(rssl_->db_);
  QString qStr = QString("SELECT id, name, feeds FROM filters");
  q.exec(qStr);
  while (q.next()) {
    QSqlQuery q1(rssl_->db_);
    QString strNameFeeds;
    QStringList strIdFeeds = q.value(2).toString().split(",");
    foreach (QString strIdFeed, strIdFeeds) {
      if (!strNameFeeds.isNull()) strNameFeeds.append(", ");
      qStr = QString("SELECT text FROM feeds WHERE id==%1").
          arg(strIdFeed);
      q1.exec(qStr);
      if (q1.next()) strNameFeeds.append(q1.value(0).toString());
    }

    treeItem.clear();
    treeItem << q.value(0).toString()
             << q.value(1).toString()
             << strNameFeeds;
    QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem(treeItem);
    treeWidgetItem->setCheckState(1, Qt::Checked);
    filtersTree->addTopLevelItem(treeWidgetItem);
  }

  QPushButton *newButton = new QPushButton(tr("New..."), this);
  connect(newButton, SIGNAL(clicked()), this, SLOT(newFilter()));
  editButton = new QPushButton(tr("Edit..."), this);
  editButton->setEnabled(false);
  connect(editButton, SIGNAL(clicked()), this, SLOT(editFilter()));
  deleteButton = new QPushButton(tr("Delete..."), this);
  deleteButton->setEnabled(false);
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteFilter()));

  moveUpButton = new QPushButton(tr("Move up"), this);
  moveUpButton->setEnabled(false);
  moveUpButton->setVisible(false);
  connect(moveUpButton, SIGNAL(clicked()), this, SLOT(moveUpFilter()));
  moveDownButton = new QPushButton(tr("Move down"), this);
  moveDownButton->setEnabled(false);
  moveDownButton->setVisible(false);
  connect(moveDownButton, SIGNAL(clicked()), this, SLOT(moveDownFilter()));

  QVBoxLayout *buttonsLayout = new QVBoxLayout();
  buttonsLayout->addWidget(newButton);
  buttonsLayout->addWidget(editButton);
  buttonsLayout->addWidget(deleteButton);
  buttonsLayout->addSpacing(20);
  buttonsLayout->addWidget(moveUpButton);
  buttonsLayout->addWidget(moveDownButton);
  buttonsLayout->addStretch();

  QHBoxLayout *layoutH1 = new QHBoxLayout();
  layoutH1->setMargin(0);
  layoutH1->addWidget(filtersTree);
  layoutH1->addLayout(buttonsLayout);

  QPushButton *closeButton = new QPushButton(tr("&Close"), this);
  closeButton->setDefault(true);
  closeButton->setFocus(Qt::OtherFocusReason);
  connect(closeButton, SIGNAL(clicked()), SLOT(close()));
  QHBoxLayout *closeLayout = new QHBoxLayout();
  closeLayout->setAlignment(Qt::AlignRight);
  closeLayout->addWidget(closeButton);

  QVBoxLayout *mainlayout = new QVBoxLayout();
  mainlayout->setMargin(5);
  mainlayout->addLayout(layoutH1);
  mainlayout->addLayout(closeLayout);
  setLayout(mainlayout);

  connect(filtersTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
          this, SLOT(slotCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
  connect(filtersTree, SIGNAL(doubleClicked(QModelIndex)),
          this, SLOT(editFilter()));
  connect(this, SIGNAL(finished(int)), this, SLOT(closeDialog()));

  restoreGeometry(settings_->value("newsFiltersDlg/geometry").toByteArray());
}

void NewsFiltersDialog::closeDialog()
{
  settings_->setValue("newsFiltersDlg/geometry", saveGeometry());
}

void NewsFiltersDialog::newFilter()
{
  FilterRulesDialog *filterRulesDialog = new FilterRulesDialog(
        parentWidget(), -1);

  int result = filterRulesDialog->exec();
  if (result == QDialog::Rejected) {
    delete filterRulesDialog;
    return;
  }

  int filterId = filterRulesDialog->filterId_;
  delete filterRulesDialog;

  RSSListing *rssl_ = qobject_cast<RSSListing*>(parentWidget());
  QSqlQuery q(rssl_->db_);
  QString qStr = QString("SELECT name, feeds FROM filters WHERE id=='%1'").
      arg(filterId);
  q.exec(qStr);
  if (q.next()) {
    QSqlQuery q1(rssl_->db_);
    QString strNameFeeds;
    QStringList strIdFeeds = q.value(1).toString().split(",");
    foreach (QString strIdFeed, strIdFeeds) {
      if (!strNameFeeds.isNull()) strNameFeeds.append(", ");
      qStr = QString("SELECT text FROM feeds WHERE id==%1").
          arg(strIdFeed);
      q1.exec(qStr);
      if (q1.next()) strNameFeeds.append(q1.value(0).toString());
    }

    QStringList treeItem;
    treeItem << QString::number(filterId)
             << q.value(0).toString()
             << strNameFeeds;
    QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem(treeItem);
    treeWidgetItem->setCheckState(1, Qt::Checked);
    filtersTree->addTopLevelItem(treeWidgetItem);
  }

  if (((filtersTree->currentIndex().row() != (filtersTree->topLevelItemCount()-1))) &&
      filtersTree->currentIndex().isValid())
    moveDownButton->setEnabled(true);
}

void NewsFiltersDialog::editFilter()
{
  int filterRow = filtersTree->currentIndex().row();
  int filterId = filtersTree->topLevelItem(filterRow)->text(0).toInt();

  FilterRulesDialog *filterRulesDialog = new FilterRulesDialog(
        parentWidget(), filterId);

  int result = filterRulesDialog->exec();
  if (result == QDialog::Rejected) {
    delete filterRulesDialog;
    return;
  }

  delete filterRulesDialog;

  RSSListing *rssl_ = qobject_cast<RSSListing*>(parentWidget());
  QSqlQuery q(rssl_->db_);
  QString qStr = QString("SELECT name, feeds FROM filters WHERE id=='%1'").
      arg(filterId);
  q.exec(qStr);
  if (q.next()) {
    QSqlQuery q1(rssl_->db_);
    QString strNameFeeds;
    QStringList strIdFeeds = q.value(1).toString().split(",");
    foreach (QString strIdFeed, strIdFeeds) {
      if (!strNameFeeds.isNull()) strNameFeeds.append(", ");
      qStr = QString("SELECT text FROM feeds WHERE id==%1").
          arg(strIdFeed);
      q1.exec(qStr);
      if (q1.next()) strNameFeeds.append(q1.value(0).toString());
    }

    filtersTree->topLevelItem(filterRow)->setText(0, QString::number(filterId));
    filtersTree->topLevelItem(filterRow)->setText(1, q.value(0).toString());
    filtersTree->topLevelItem(filterRow)->setText(2, strNameFeeds);
    filtersTree->topLevelItem(filterRow)->setCheckState(1, Qt::Checked);
  }
}

void NewsFiltersDialog::deleteFilter()
{
  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setWindowTitle(tr("Delete filter"));
  msgBox.setText(QString(tr("Are you sure to delete the filter '%1'?")).
                 arg(filtersTree->currentItem()->text(1)));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);

  if (msgBox.exec() == QMessageBox::No) return;

  int filterRow = filtersTree->currentIndex().row();
  int filterId = filtersTree->topLevelItem(filterRow)->text(0).toInt();

  RSSListing *rssl_ = qobject_cast<RSSListing*>(parentWidget());
  QSqlQuery q(rssl_->db_);
  q.exec(QString("DELETE FROM filters WHERE id='%1'").arg(filterId));
  q.exec(QString("DELETE FROM filterConditions WHERE idFilter='%1'").arg(filterId));
  q.exec(QString("DELETE FROM filterActions WHERE idFilter='%1'").arg(filterId));
  q.exec("VACUUM");
  q.finish();

  filtersTree->takeTopLevelItem(filterRow);

  if (filtersTree->currentIndex().row() == 0)
    moveUpButton->setEnabled(false);
  if (filtersTree->currentIndex().row() == (filtersTree->topLevelItemCount()-1))
    moveDownButton->setEnabled(false);
}

void NewsFiltersDialog::moveUpFilter()
{
  QTreeWidgetItem *treeWidgetItem = filtersTree->takeTopLevelItem(
        filtersTree->currentIndex().row()-1);
  filtersTree->insertTopLevelItem(filtersTree->currentIndex().row()+1,
                                  treeWidgetItem);
  if (filtersTree->currentIndex().row() == 0)
    moveUpButton->setEnabled(false);
  if (filtersTree->currentIndex().row() != (filtersTree->topLevelItemCount()-1))
    moveDownButton->setEnabled(true);
}

void NewsFiltersDialog::moveDownFilter()
{
  QTreeWidgetItem *treeWidgetItem = filtersTree->takeTopLevelItem(
        filtersTree->currentIndex().row()+1);
  filtersTree->insertTopLevelItem(filtersTree->currentIndex().row(),
                                  treeWidgetItem);
  if (filtersTree->currentIndex().row() == (filtersTree->topLevelItemCount()-1))
    moveDownButton->setEnabled(false);
  if (filtersTree->currentIndex().row() != 0)
    moveUpButton->setEnabled(true);
}

void NewsFiltersDialog::slotCurrentItemChanged(QTreeWidgetItem *current,
                                               QTreeWidgetItem *)
{
  if (filtersTree->indexOfTopLevelItem(current) == 0)
    moveUpButton->setEnabled(false);
  else moveUpButton->setEnabled(true);

  if (filtersTree->indexOfTopLevelItem(current) == (filtersTree->topLevelItemCount()-1))
    moveDownButton->setEnabled(false);
  else moveDownButton->setEnabled(true);

  if (filtersTree->indexOfTopLevelItem(current) < 0) {
    editButton->setEnabled(false);
    deleteButton->setEnabled(false);
    moveUpButton->setEnabled(false);
    moveDownButton->setEnabled(false);
  } else {
    editButton->setEnabled(true);
    deleteButton->setEnabled(true);
  }
}
