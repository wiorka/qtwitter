/***************************************************************************
 *   Copyright (C) 2008-2009 by Dominik Kapusta       <d@ayoy.net>         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "mainwindow.h"
#include "statusfilter.h"
#include "tweet.h"
#include "ui_about.h"

#include <QMenu>
#include <QScrollBar>
#include <QMessageBox>
#include <QIcon>
#include <QMovie>
#include <QPalette>
#include <QShortcut>
#include <QDesktopWidget>
#include <QSignalMapper>

const QString MainWindow::APP_VERSION = "0.5.1_yammer";

MainWindow::MainWindow( QWidget *parent ) :
    QWidget( parent ),
    resetUiWhenFinished( false )
{
  ui.setupUi( this );
  progressIcon = new QMovie( ":/icons/progress.gif", "gif", this );
  ui.countdownLabel->setMovie( progressIcon );

  ui.countdownLabel->setToolTip( ui.countdownLabel->text() + " " + tr( "characters left" ) );
  StatusFilter *filter = new StatusFilter( this );
  ui.statusEdit->installEventFilter( filter );

  connect( ui.updateButton, SIGNAL( clicked() ), this, SIGNAL( updateTweets() ) );
  connect( ui.settingsButton, SIGNAL( clicked() ), this, SIGNAL(settingsDialogRequested()) );
  connect( ui.statusEdit, SIGNAL( textChanged( QString ) ), this, SLOT( changeLabel() ) );
  connect( ui.statusEdit, SIGNAL( editingFinished() ), this, SLOT( resetStatus() ) );
  connect( ui.statusEdit, SIGNAL(errorMessage(QString)), this, SLOT(popupError(QString)) );
  connect( filter, SIGNAL( enterPressed() ), this, SLOT( sendStatus() ) );
  connect( filter, SIGNAL( escPressed() ), ui.statusEdit, SLOT( cancelEditing() ) );
  connect( this, SIGNAL(addReplyString(QString,int)), ui.statusEdit, SLOT(addReplyString(QString,int)) );
  connect( this, SIGNAL(addRetweetString(QString)), ui.statusEdit, SLOT(addRetweetString(QString)) );

  QShortcut *hideShortcut = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_H ), this );
  connect( hideShortcut, SIGNAL(activated()), this, SLOT(hide()) );
  QShortcut *quitShortcut = new QShortcut( QKeySequence( Qt::CTRL + Qt::Key_Q ), this );
  connect( quitShortcut, SIGNAL(activated()), qApp, SLOT(quit()) );
#ifdef Q_WS_MAC
  ui.settingsButton->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Comma ) );
#else
  ui.settingsButton->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ) );
#endif
  ui.updateButton->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_R ) );

  buttonMenu = new QMenu( this );
  newtweetAction = new QAction( tr( "New tweet" ), buttonMenu );
  newtwitpicAction = new QAction( tr( "Upload a photo to TwitPic" ), buttonMenu );
  gototwitterAction = new QAction( tr( "Go to Twitter" ), buttonMenu );
  gototwitpicAction = new QAction( tr( "Go to TwitPic" ), buttonMenu );
  newtweetAction->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_N ) );
  newtwitpicAction->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_N ) );
  gototwitterAction->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_G ) );
  gototwitpicAction->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_G ) );

  QSignalMapper *mapper = new QSignalMapper( this );
  mapper->setMapping( gototwitterAction, "http://twitter.com/home" );
  mapper->setMapping( gototwitpicAction, "http://twitpic.com" );

  connect( newtweetAction, SIGNAL(triggered()), ui.statusEdit, SLOT(setFocus()) );
  connect( newtwitpicAction, SIGNAL(triggered()), this, SIGNAL(openTwitPicDialog()) );
  connect( gototwitterAction, SIGNAL(triggered()), mapper, SLOT(map()) );
  connect( gototwitpicAction, SIGNAL(triggered()), mapper, SLOT(map()) );
  connect( mapper, SIGNAL(mapped(QString)), this, SLOT(emitOpenBrowser(QString)) );

  buttonMenu->addAction( newtweetAction );
  buttonMenu->addAction( newtwitpicAction );
  buttonMenu->addSeparator();
  buttonMenu->addAction( gototwitterAction );
  buttonMenu->addAction( gototwitpicAction );
  ui.moreButton->setMenu( buttonMenu );

  trayIcon = new QSystemTrayIcon( this );
  trayIcon->setIcon( QIcon( ":/icons/twitter_48.png" ) );

  connect( trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)) );
  connect( trayIcon, SIGNAL(messageClicked()), this, SLOT(show()) );
#ifndef Q_WS_MAC
  QMenu *trayMenu = new QMenu( this );
  trayMenu = new QMenu( this );
  QAction *quitaction = new QAction( tr( "Quit" ), trayMenu);
  QAction *settingsaction = new QAction( tr( "Settings" ), trayMenu);
  settingsaction->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_S ) );
  quitaction->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Q ) );

  connect( quitaction, SIGNAL(triggered()), qApp, SLOT(quit()) );
  connect( settingsaction, SIGNAL(triggered()), this, SIGNAL(settingsDialogRequested()) );
  connect( settingsaction, SIGNAL(triggered()), this, SLOT(show()) );

  trayMenu->addAction(settingsaction);
  trayMenu->addSeparator();
  trayMenu->addAction(quitaction);
  trayIcon->setContextMenu( trayMenu );

  trayIcon->setToolTip( "qTwitter" );
#endif
  trayIcon->show();
}

MainWindow::~MainWindow() {}

StatusList* MainWindow::getListView()
{
  return ui.statusListView;
}

int MainWindow::getScrollBarWidth()
{
  return ui.statusListView->verticalScrollBar()->size().width();
}

void MainWindow::setVisibleList( int visibleList )
{
  if ( visibleList == MainWindow::LIST_TWITTER ) {
    ui.twitterButton->setChecked( true );
    ui.statusListView->setShown( true );
    ui.yammerListView->setShown( false );
  } else if ( visibleList == MainWindow::LIST_YAMMER ) {
    ui.yammerButton->setChecked( true );
    ui.yammerListView->setShown( true );
    ui.statusListView->setShown( false );
  }
}

int MainWindow::getVisibleList()
{
  if ( ui.statusListView->isVisible() )
    return MainWindow::LIST_TWITTER;
  else if ( ui.yammerListView->isVisible() )
    return MainWindow::LIST_YAMMER;
}

void MainWindow::setListViewModel( TweetModel *model )
{
  ui.statusListView->setModel( model );
}

void MainWindow::closeEvent( QCloseEvent *e )
{
  if ( trayIcon->isVisible()) {
    hide();
    e->ignore();
    return;
  }
  QWidget::closeEvent( e );
}

void MainWindow::iconActivated( QSystemTrayIcon::ActivationReason reason )
{
  switch ( reason ) {
    case QSystemTrayIcon::Trigger:
#ifdef Q_WS_WIN
    if ( !isVisible() ) {
#else
    if ( !isVisible() || !QApplication::activeWindow() ) {
#endif
      show();
        raise();
        activateWindow();
      } else {
        hide();
      }
      break;
    default:
      break;
  }
}

void MainWindow::changeLabel()
{
  ui.countdownLabel->setText( ui.statusEdit->isStatusClean() ? QString::number( StatusEdit::STATUS_MAX_LENGTH ) : QString::number( StatusEdit::STATUS_MAX_LENGTH - ui.statusEdit->text().length() ) );
  ui.countdownLabel->setToolTip( ui.countdownLabel->text() + " " + tr( "characters left" ) );
}

void MainWindow::sendStatus()
{
  resetUiWhenFinished = true;
  emit post( ui.statusEdit->text().toUtf8(), ui.statusEdit->getInReplyTo() );
  showProgressIcon();
}

void MainWindow::resetStatusEdit()
{
  if ( resetUiWhenFinished ) {
    resetUiWhenFinished = false;
    ui.statusEdit->cancelEditing();
  }
  progressIcon->stop();
  changeLabel();
}

void MainWindow::showProgressIcon()
{
  ui.countdownLabel->clear();
  ui.countdownLabel->setMovie( progressIcon );
  progressIcon->start();
}

void MainWindow::resetStatus()
{
  if ( ui.statusEdit->isStatusClean() ) {
    changeLabel();
  }
}

void MainWindow::resizeEvent( QResizeEvent *event )
{
  emit resizeView( event->size().width(), event->oldSize().width() );
}

void MainWindow::popupMessage( int statusesCount, QStringList namesForStatuses, int messagesCount, QStringList namesForMessages )
{
  QRegExp rx( ", " );
  QString message;
#ifdef Q_WS_MAC
  QString title;
  if ( !namesForStatuses.isEmpty() ) {
    title.append( QString::number( statusesCount ) + " " );
    statusesCount == 1 ? title.append( tr( "new tweet" ) ) : title.append( tr( "new tweets" ) );
    message.append( tr( "from" ) + " " + namesForStatuses.join( ", " ) + "." );
    message.replace( rx.lastIndexIn( message ), rx.pattern().length(), " " + tr( "and" ) + " " );
    trayIcon->showMessage( title, message, QSystemTrayIcon::Information );
  }
  if ( !namesForMessages.isEmpty() ) {
    message.clear();
    title.clear();
    title.append( QString::number( messagesCount ) + " " );
    messagesCount == 1 ? title.append( tr( "new message" ) ) : title.append( tr( "new messages" ) );
    message.append( tr( "from" ) + " " + namesForMessages.join(", ") + "." );
    message.replace( rx.lastIndexIn( message ), rx.pattern().length(), " " + tr( "and" ) + " " );
    trayIcon->showMessage( title, message, QSystemTrayIcon::Information );
  }
#else
  if ( !namesForStatuses.isEmpty() ) {
    message.append( QString::number( statusesCount ) + " " );
    statusesCount == 1 ? message.append( tr( "new tweet from" ) ) : message.append( tr( "new tweets from" ) );
    message.append( " " + namesForStatuses.join( ", " ) + "." );
    message.replace( rx.lastIndexIn( message ), rx.pattern().length(), " " + tr( "and" ) + " " );
  }
  if ( !namesForMessages.isEmpty() ) {
    if ( !namesForStatuses.isEmpty() ) {
      message.append( "\n\n" );
    }
    message.append( QString::number( messagesCount ) + " " );
    messagesCount == 1 ? message.append( tr( "New message from" ) ) : message.append( tr( "New messages from" ) );
    message.append( " " + namesForMessages.join( ", " ) + "." );
    message.replace( rx.lastIndexIn( message ), rx.pattern().length(), " " + tr( "and" ) + " " );
  }
  if ( !namesForMessages.isEmpty() || !namesForStatuses.isEmpty() ) {
    trayIcon->showMessage( tr( "News from qTwitter" ), message, QSystemTrayIcon::Information );
  }
#endif
}

void MainWindow::popupError( const QString &message )
{
  QMessageBox::information( this, tr("Error"), message );
}

void MainWindow::emitOpenBrowser( QString address )
{
  emit openBrowser( QUrl( address ) );
}

void MainWindow::changeListBackgroundColor(const QColor &newColor )
{
  QPalette palette( ui.statusListView->palette() );
  palette.setColor( QPalette::Base, newColor );
  ui.statusListView->setPalette( palette );
  ui.statusListView->update();
}

void MainWindow::about()
{
  QDialog *dlg = new QDialog( this );
  Ui::AboutDialog aboutUi;
  aboutUi.setupUi( dlg );
  dlg->adjustSize();
  aboutUi.textBrowser->setHtml( aboutUi.textBrowser->toHtml().arg( APP_VERSION ) );
  dlg->exec();
  dlg->deleteLater();
}

void MainWindow::retranslateUi()
{
  ui.moreButton->setToolTip( tr("More...") );
  ui.settingsButton->setToolTip( tr("Settings") );
  ui.updateButton->setToolTip( tr("Update tweets") );
  if ( ui.statusEdit->isStatusClean() ) {
    ui.statusEdit->initialize();
  }
  ui.statusEdit->setText( tr("What are you doing?") );
  newtweetAction->setText( tr( "New tweet" ) );
  newtwitpicAction->setText( tr( "Upload a photo to TwitPic" ) );
  gototwitterAction->setText( tr( "Go to Twitter" ) );
  gototwitpicAction->setText( tr( "Go to TwitPic" ) );
}
