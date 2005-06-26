/***************************************************************************
                          kplayer.cpp
                          -----------
    begin                : Sat Nov 16 10:12:50 EST 2002
    copyright            : (C) 2002-2004 by kiriuja
    email                : kplayer dash developer at en dash directo dot net
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kurl.h>
#include <kurldrag.h>
#include <kwin.h>
#include <qdatetime.h>
#include <qdockarea.h>
#include <qeventloop.h>
#include <qfocusdata.h>
#include <qobjectlist.h>
#include <qwhatsthis.h>

#include <kdebug.h>

#include "kplayer.h"
#include "kplayer.moc"
#include "kplayerengine.h"
#include "kplayerlogwindow.h"
#include "kplayerplaylist.h"
#include "kplayersettings.h"
#include "kplayersettingsdialog.h"
#include "kplayerslideraction.h"
#include "kplayerwidget.h"

//void KPlayerX11SetInputFocus (uint id);
void KPlayerX11DiscardConfigureEvents (uint id);
//void KPlayerX11SendConfigureEvent (uint id, int x, int y, int w, int h);

#define ID_STATUS_MSG   1
#define ID_STATE_MSG    2
#define ID_PROGRESS_MSG 3

#define DEBUG_KPLAYER_RESIZING
#define DEBUG_KPLAYER_WINDOW
//#define DEBUG_KPLAYER_NOTIFY

KPlayerApplication::KPlayerApplication (void)
  : KUniqueApplication (true, true, true)
{
}

KPlayerApplication::~KPlayerApplication()
{
}

int KPlayerApplication::newInstance (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "New instance\n";
#endif
  KPlayer* mainWindow = (KPlayer*) mainWidget();
  if ( ! mainWindow )
  {
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "Creating main window\n";
#endif
    mainWindow = new KPlayer();
    /*if ( isRestored() && KMainWindow::canBeRestored (1) )
    {
#ifdef DEBUG_KPLAYER_WINDOW
      kdDebugTime() << "Restoring main window\n";
#endif
      mainWindow -> restore (1);
    }*/
    setMainWidget (mainWindow);
  }
  mainWindow -> start();
  return 0;
}

bool KPlayerApplication::notify (QObject* object, QEvent* event)
{
  if ( kPlayerEngine() && kPlayerSettings() && event )
  {
    switch ( event -> type() )
    {
    case QEvent::AccelOverride:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      kPlayerSettings() -> setShift (((QKeyEvent*) event) -> stateAfter() & Qt::ShiftButton);
#ifdef DEBUG_KPLAYER_NOTIFY
      kdDebug() << "KPlayerApplication::notify: event type " << event -> type()
        << " spontaneous " << event -> spontaneous()
        << " receiver " << (object ? object -> className() : "<none>")
        << " shift " << kPlayerSettings() -> shift() << "\n";
#endif
      break;
    case QEvent::Accel:
      {
        QKeyEvent* kevent = (QKeyEvent*) event;
        kPlayerSettings() -> setShift (kevent -> stateAfter() & Qt::ShiftButton);
#ifdef DEBUG_KPLAYER_NOTIFY
        kdDebugTime() << "KPlayerApplication::notify: keyboard event type " << event -> type()
          << " spontaneous " << event -> spontaneous() << " receiver " << (object ? object -> className() : "<none>")
          << " key " << kevent -> key() << " ascii " << kevent -> ascii() << " text " << kevent -> text()
          << " count " << kevent -> count() << " state " << kevent -> state() << " after " << kevent -> stateAfter()
          << " accepted " << kevent -> isAccepted() << " autorepeat " << kevent -> isAutoRepeat()
          << " shift " << kPlayerSettings() -> shift() << "\n";
#endif
        if ( (kevent -> state() & (Qt::ShiftButton | Qt::ControlButton)) == (Qt::ShiftButton | Qt::ControlButton)
          && (kevent -> key() >= Qt::Key_Exclam && kevent -> key() <= Qt::Key_Ampersand
          || kevent -> key() >= Qt::Key_ParenLeft && kevent -> key() <= Qt::Key_Plus
          || kevent -> key() == Qt::Key_Colon || kevent -> key() == Qt::Key_Less
          || kevent -> key() >= Qt::Key_Greater && kevent -> key() <= Qt::Key_At
          || kevent -> key() == Qt::Key_AsciiCircum || kevent -> key() == Qt::Key_Underscore
          || kevent -> key() >= Qt::Key_BraceLeft && kevent -> key() <= Qt::Key_AsciiTilde) )
        {
          int key;
          switch ( kevent -> key() )
          {
            case Qt::Key_Exclam:
              key = Qt::Key_1;
              break;
            case Qt::Key_QuoteDbl:
              key = Qt::Key_Apostrophe;
              break;
            case Qt::Key_NumberSign:
              key = Qt::Key_3;
              break;
            case Qt::Key_Dollar:
              key = Qt::Key_4;
              break;
            case Qt::Key_Percent:
              key = Qt::Key_5;
              break;
            case Qt::Key_Ampersand:
              key = Qt::Key_7;
              break;
            case Qt::Key_ParenLeft:
              key = Qt::Key_BracketLeft;
              break;
            case Qt::Key_ParenRight:
              key = Qt::Key_BracketRight;
              break;
            case Qt::Key_Asterisk:
              key = Qt::Key_8;
              break;
            case Qt::Key_Plus:
              key = Qt::Key_Equal;
              break;
            case Qt::Key_Colon:
              key = Qt::Key_Semicolon;
              break;
            case Qt::Key_Less:
              key = Qt::Key_Comma;
              break;
            case Qt::Key_Greater:
              key = Qt::Key_Period;
              break;
            case Qt::Key_Question:
              key = Qt::Key_Slash;
              break;
            case Qt::Key_At:
              key = Qt::Key_2;
              break;
            case Qt::Key_AsciiCircum:
              key = Qt::Key_6;
              break;
            case Qt::Key_Underscore:
              key = Qt::Key_Minus;
              break;
            case Qt::Key_BraceLeft:
              key = Qt::Key_BracketLeft;
              break;
            case Qt::Key_Bar:
              key = Qt::Key_Backslash;
              break;
            case Qt::Key_BraceRight:
              key = Qt::Key_BracketRight;
              break;
            case Qt::Key_AsciiTilde:
              key = Qt::Key_QuoteLeft;
              break;
            default:
              key = kevent -> key();
          }
          QKeyEvent keyevent (kevent -> type(), key, kevent -> ascii() == kevent -> key() ? key : kevent -> ascii(),
            kevent -> state() & ~ Qt::ShiftButton, kevent -> text(), kevent -> isAutoRepeat(), kevent -> count());
          if ( kevent -> isAccepted() )
            keyevent.accept();
          else
            keyevent.ignore();
#ifdef DEBUG_KPLAYER_NOTIFY
          kdDebugTime() << "KPlayerApplication::notify: passing keyboard event " << keyevent.type()
            << " spontaneous " << keyevent.spontaneous() << " receiver " << (object ? object -> className() : "<none>")
            << " key " << keyevent.key() << " ascii " << keyevent.ascii() << " text " << keyevent.text()
            << " count " << keyevent.count() << " state " << keyevent.state() << " after " << keyevent.stateAfter()
            << " accepted " << keyevent.isAccepted() << " autorepeat " << keyevent.isAutoRepeat()
            << " shift " << kPlayerSettings() -> shift() << "\n";
#endif
          return KUniqueApplication::notify (object, &keyevent);
        }
      }
      break;
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
      kPlayerSettings() -> setShift (((QMouseEvent*) event) -> stateAfter() & Qt::ShiftButton);
#ifdef DEBUG_KPLAYER_NOTIFY
      kdDebugTime() << "KPlayerApplication::notify: event type " << event -> type()
        << " spontaneous " << event -> spontaneous()
        << " receiver " << (object ? object -> className() : "<none>")
        << " shift " << kPlayerSettings() -> shift() << "\n";
#endif
      break;
    case QEvent::Wheel:
      kPlayerSettings() -> setShift (((QWheelEvent*) event) -> state() & Qt::ShiftButton);
#ifdef DEBUG_KPLAYER_NOTIFY
      kdDebugTime() << "KPlayerApplication::notify: event type " << event -> type()
        << " spontaneous " << event -> spontaneous()
        << " receiver " << (object ? object -> className() : "<none>")
        << " shift " << kPlayerSettings() -> shift() << "\n";
#endif
      break;
    case QEvent::ContextMenu:
      kPlayerSettings() -> setShift (((QContextMenuEvent*) event) -> state() & Qt::ShiftButton);
#ifdef DEBUG_KPLAYER_NOTIFY
      kdDebugTime() << "KPlayerApplication::notify: event type " << event -> type()
        << " spontaneous " << event -> spontaneous()
        << " receiver " << (object ? object -> className() : "<none>")
        << " shift " << kPlayerSettings() -> shift() << "\n";
#endif
      break;
#ifdef DEBUG_KPLAYER_NOTIFY
    case QEvent::Resize:
      if ( object == mainWidget() )
      {
        QResizeEvent* revent = (QResizeEvent*) event;
        kdDebugTime() << "KPlayerApplication::notify: Main window resize to " << revent -> size().width() << "x" << revent -> size().height() << "\n";
      }
#endif
    default:
      break;
    }
  }
  return KUniqueApplication::notify (object, event);
}

#ifdef DEBUG_KPLAYER_WINDOW
static void dumpObject (const QObject* object, int indent, int depth = 20)
{
  QString spaces;
  for ( int i = 0; i < indent * 2; i ++ )
    spaces += " ";
  if ( object -> inherits ("QWidget") )
  {
    QWidget* widget = (QWidget*) object;
    kdDebug() << spaces << object -> className() << " " << widget -> winId() <<
      " " << widget -> minimumSize().width() << "x" << widget -> minimumSize().height() <<
      " " << widget -> minimumSizeHint().width() << "x" << widget -> minimumSizeHint().height() <<
      " " << widget -> sizeHint().width() << "x" << widget -> sizeHint().height() <<
      " " << widget -> width() << "x" << widget -> height() << "\n";
  }
  else if ( object -> inherits ("QLayout") )
  {
    QLayout* layout = (QLayout*) object;
    kdDebug() << spaces << object -> className() <<
      " " << layout -> minimumSize().width() << "x" << layout -> minimumSize().height() <<
      " " << layout -> sizeHint().width() << "x" << layout -> sizeHint().height() <<
      " " << layout -> geometry().width() << "x" << layout -> geometry().height() <<
      " " << layout -> margin() << "+" << layout -> spacing() <<
      " " << layout -> alignment() << " " << layout -> expanding() <<
      " " << layout -> hasHeightForWidth() << " " << layout -> heightForWidth (1) << "\n";
  }
  else
    kdDebug() << spaces << object -> className() << "\n";
  indent ++;
  QObjectList* objectlist = (QObjectList*) object -> children();
  if ( objectlist && depth -- > 0 )
    for ( object = objectlist -> first(); object; object = objectlist -> next() )
      if ( object )
        dumpObject (object, indent, depth);
}
#endif

KPlayer::KPlayer (QWidget *parent, const char *name) : KMainWindow (parent, name)
{
  m_status_label = m_state_label = m_progress_label = 0;
  m_menubar_normally_visible = m_statusbar_normally_visible = true;
  m_menubar_fullscreen_visible = m_statusbar_fullscreen_visible = false;
  m_messagelog_normally_visible = m_messagelog_fullscreen_visible = false;
  m_editor_normally_visible = m_editor_fullscreen_visible = false;
  m_initial_show = m_error_detected = m_maximized = false;
  m_full_screen = m_show_log = m_show_playlist = false;
  Toolbar toolbars [KPLAYER_TOOLBARS] = {
    { "mainToolBar", KStdAction::stdName (KStdAction::ShowToolbar), true, false },
    { "editorToolBar", "playlist_toolbar", true, true },
    { "playlistToolBar", "playlist_show", true, false },
    { "progressToolBar", "settings_progress_toolbar", true, false },
    { "volumeToolBar", "settings_volume_toolbar", false, false },
    { "contrastToolBar", "settings_contrast_toolbar", false, false },
    { "brightnessToolBar", "settings_brightness_toolbar", false, false },
    { "hueToolBar", "settings_hue_toolbar", false, false },
    { "saturationToolBar", "settings_saturation_toolbar", false, false }
  };
  QString captions [KPLAYER_TOOLBARS] = {
    i18n("Main Toolbar"),
    i18n("Playlist Editor"),
    i18n("Playlist"),
    i18n("Progress"),
    i18n("Volume"),
    i18n("Contrast"),
    i18n("Brightness"),
    i18n("Hue"),
    i18n("Saturation")
  };
  const char* actions [KPLAYER_TOOLBARS - FIRST_SLIDER_TOOLBAR] = {
    "player_progress",
    "audio_volume",
    "video_contrast",
    "video_brightness",
    "video_hue",
    "video_saturation"
  };
  QString whatsthis [KPLAYER_TOOLBARS] = {
    i18n("Main toolbar contains buttons for commonly used operations. Left clicking an icon will activate the corresponding action. Some of the buttons will pop up slider controls when clicked. The sliders let you change various player controls: sound volume, video contrast, brightness, hue and saturation. The video controls will only be available for video files."),
    i18n("Playlist editor toolbar contains buttons that activate commands that apply to the editor and the items selected in it."),
    i18n("Playlist toolbar shows the multimedia title currently loaded or being played, offers commands to go to the next or previous items on the playlist, and also lets you see the entire playlist that consists of items that are currently being played or have been played recently. If you select a different item from the list, KPlayer will load and start playing it. The toolbar also contains options to loop and shuffle the playlist."),
    i18n("Progress and seeking toolbar is shown when the time length of a media file is known. It displays the playback progress and also allows seeking, that is, moving the playback point back and forth within the file. To seek to a specific position, drag the slider thumb to that position with the left mouse button, or simply click at that position with the middle mouse button. To move forward or backward in steps, left click the slider to the left or right of the thumb, or click the Forward and Backward buttons."),
    i18n("Volume toolbar has a volume slider that shows the current sound volume and allows you to change it. This is the same slider you get from the volume pop-up slider button on the main toolbar, but it will not disappear when you click elsewhere. You can show or hide the toolbar using the Show Volume Toolbar option on the Settings menu. Clicking the Volume button on this toolbar will also hide it. In addition the toolbar has a Mute button that turns the sound off and back on."),
    i18n("Contrast toolbar has a contrast slider that shows the current video contrast and allows you to change it. This is the same slider you get from the contrast pop-up slider button on the main toolbar, but it will not disappear when you click elsewhere. You can show or hide the toolbar using the Show Contrast Toolbar option on the Settings menu. Clicking the Contrast button on this toolbar will also hide it."),
    i18n("Brightness toolbar has a brightness slider that shows the current video brightness and allows you to change it. This is the same slider you get from the brightness pop-up slider button on the main toolbar, but it will not disappear when you click elsewhere. You can show or hide the toolbar using the Show Brightness Toolbar option on the Settings menu. Clicking the Brightness button on this toolbar will also hide it."),
    i18n("Hue toolbar has a hue slider that shows the current video hue and allows you to change it. This is the same slider you get from the hue pop-up slider button on the main toolbar, but it will not disappear when you click elsewhere. You can show or hide the toolbar using the Show Hue Toolbar option on the Settings menu. Clicking the Hue button on this toolbar will also hide it."),
    i18n("Saturation toolbar has a saturation slider that shows the current video saturation and allows you to change it. This is the same slider you get from the saturation pop-up slider button on the main toolbar, but it will not disappear when you click elsewhere. You can show or hide the toolbar using the Show Saturation Toolbar option on the Settings menu. Clicking the Saturation button on this toolbar will also hide it.")
  };
  KPlayerEngine::initialize (actionCollection(), this, 0, kapp -> config());
  connect (kPlayerEngine(), SIGNAL (syncronize (bool)), this, SLOT (syncronize (bool)));
  connect (kPlayerEngine(), SIGNAL (zoom()), this, SLOT (zoom()));
  connect (kPlayerEngine(), SIGNAL (correctSize()), this, SLOT (correctSize()));
  connect (kPlayerEngine(), SIGNAL (initialSize()), this, SLOT (initialSize()));
  m_log = new KPlayerLogWindow (this);
  connect (m_log, SIGNAL (windowHidden()), this, SLOT (logWindowHidden()));
  m_playlist = new KPlayerPlaylistWindow (actionCollection(), this);
  connect (m_playlist, SIGNAL (windowHidden()), this, SLOT (playlistEditorHidden()));
  connect (m_playlist -> kPlayerPlaylistWidget(), SIGNAL (contextMenu (KListView*, QListViewItem*, const QPoint&)), this, SLOT (playlistContextMenu (KListView*, QListViewItem*, const QPoint&)));
  connect (m_playlist -> kPlayerPlaylistWidget(), SIGNAL (started()), this, SLOT (playlistStarted()));
  connect (m_playlist -> kPlayerPlaylistWidget(), SIGNAL (activated()), this, SLOT (playlistActivated()));
  connect (m_playlist -> kPlayerPlaylistWidget(), SIGNAL (stopped()), this, SLOT (playlistStopped()));
  connect (actionCollection(), SIGNAL (actionStatusText(const QString&)), this, SLOT (setStatusMessage(const QString&)));
  connect (actionCollection(), SIGNAL (clearStatusText()), this, SLOT (clearStatusMessage()));
  actionCollection() -> setHighlightingEnabled (true);
  connect (kPlayerProcess(), SIGNAL (stateChanged(KPlayerProcess::State, KPlayerProcess::State)), this, SLOT (playerStateChanged(KPlayerProcess::State, KPlayerProcess::State)));
  connect (kPlayerProcess(), SIGNAL (progressChanged(float, KPlayerProcess::ProgressType)), this, SLOT (playerProgressChanged(float, KPlayerProcess::ProgressType)));
  connect (kPlayerProcess(), SIGNAL (infoAvailable()), this, SLOT (playerInfoAvailable()));
  connect (kPlayerProcess(), SIGNAL (sizeAvailable()), this, SLOT (playerSizeAvailable()));
  connect (kPlayerProcess(), SIGNAL (messageReceived(QString)), this, SLOT (playerMessageReceived(QString)));
  connect (kPlayerProcess(), SIGNAL (errorDetected()), this, SLOT (playerErrorDetected()));
  connect (kPlayerSettings(), SIGNAL (refresh()), this, SLOT (refreshSettings()));
  connect (kPlayerWorkspace(), SIGNAL (contextMenu(QContextMenuEvent*)), this, SLOT (contextMenuEvent(QContextMenuEvent*)));
  setCentralWidget (kPlayerWorkspace());
  initStatusBar();
  initActions();
  uint i;
  for ( i = 0; i < menuBar() -> count(); i ++ )
    connect (popupMenu (i), SIGNAL (aboutToHide()), this, SLOT (clearStatusMessage()));
  for ( i = 0; i < KPLAYER_TOOLBARS; i ++ )
    m_toolbar[i] = toolbars[i];
/*  connect (toolBar (m_toolbar[i].name), SIGNAL (orientationChanged(Orientation)), this, SLOT (barOrientationChanged(Orientation)));
    connect (toolBar (m_toolbar[i].name), SIGNAL (placeChanged(QDockWindow::Place)), this, SLOT (barPlaceChanged(QDockWindow::Place)));
    connect (toolBar (m_toolbar[i].name), SIGNAL (visibilityChanged(bool)), this, SLOT (barVisibilityChanged(bool)));
  }*/
  setAutoSaveSettings ("MainWindow", false);
  resetAutoSaveSettings();  // saveWindowSize = false;
  readOptions();
  KToolBar* toolbar = toolBar (m_toolbar[PLAYLIST_TOOLBAR].name);
  setDockEnabled (toolbar, DockLeft, false);
  setDockEnabled (toolbar, DockRight, false);
  setDockEnabled (m_playlist, DockLeft, false);
  setDockEnabled (m_playlist, DockRight, false);
  setDockEnabled (m_log, DockLeft, false);
  setDockEnabled (m_log, DockRight, false);
  QWhatsThis::add (menuBar(), i18n("Menu bar contains names of drop-down menus. Left click a name to alternately show and hide that menu, or use Alt + the underlined letter in the name as a hot key, for example Alt+F to show the File menu."));
  QWhatsThis::add (statusBar(), i18n("Status bar shows general information about the player status and progress."));
  QWhatsThis::add (m_playlist, i18n("Playlist editor is a window where KPlayer remembers files and URLs you played and shows various pieces of information about them, lets you play existing items, rename them and set their properties, add new entries, move entries around, remove them and choose several options."));
  QWhatsThis::add (m_log, i18n("Message log is a window where KPlayer shows messages it receives from MPlayer. KPlayer can show it automatically when it detects an MPlayer error if that option is selected in KPlayer settings."));
  for ( i = 0; i < KPLAYER_TOOLBARS; i ++ )
  {
    toolbar = toolBar (m_toolbar[i].name);
    toolbar -> setCaption (captions[i]);
    QWhatsThis::add (toolbar, whatsthis[i]);
    //kdDebugTime() << "Orientation " << sliderAction (m_toolbar[i].action) -> slider() -> orientation() << "\n";
    if ( i >= FIRST_SLIDER_TOOLBAR )
      ((KPlayerSliderAction*) action (actions [i - FIRST_SLIDER_TOOLBAR])) -> slider() -> setOrientation (toolbar -> orientation());
  }
  QDockArea* area = (QDockArea*) child ("playlistleftdock");
  if ( area )
  {
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "Found dock area\n";
#endif
    toolbar = toolBar (m_toolbar[EDITOR_TOOLBAR].name);
    toolbar -> undock();
    removeDockWindow (toolbar);
    toolbar -> setOrientation (Qt::Vertical);
    area -> moveDockWindow (toolbar);
  }
  if ( kPlayerSettings() -> maximized() )
    showMaximized();
  enablePlayerActions();
  enableVideoActions();
  setEraseColor (QColor (0, 0, 0));
  setAcceptDrops (true);
  //layout() -> setResizeMode (QLayout::FreeResize);
  //KWin::setState (m_log -> winId(), NET::SkipTaskbar | NET::SkipPager);
  setFocusPolicy (QWidget::StrongFocus);
  setFocusProxy (kPlayerWorkspace() -> focusProxy());
}

void KPlayer::initActions (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Creating actions\n";
#endif
  //kPlayerEngine() -> setActionCollection (actionCollection());
  //kPlayerEngine() -> setupActions();
  //m_playlist -> kPlayerPlaylistWidget() -> setActionCollection (actionCollection());
  //m_playlist -> kPlayerPlaylistWidget() -> setupActions();
  KAction* action = KStdAction::quit (this, SLOT (fileQuit()), actionCollection());
  action -> setStatusText (i18n("Quits KPlayer"));
  action -> setWhatsThis (i18n("Quit command saves the playlist and all settings, stops playing and closes KPlayer."));
  action = new KAction (i18n("&Clear Log"), 0, 0, this, SLOT (fileClearLog()), actionCollection(), "file_clear_log");
  action -> setStatusText (i18n("Clears all messages from the message log"));
  action -> setWhatsThis (i18n("Clear Log command removes all messages from the message log."));
  action = new KToggleAction (i18n("&Show Playlist"), 0, CTRL + Key_P, this, SLOT (playlistShow()), actionCollection(), "playlist_show");
  action -> setStatusText (i18n("Shows/hides the playlist"));
  action -> setWhatsThis (i18n("Show Playlist command shows or hides the playlist toolbar."));
  action = new KToggleAction (i18n("Show &Editor"), 0, CTRL + Key_E, this, SLOT (playlistEditor()), actionCollection(), "playlist_editor");
  action -> setStatusText (i18n("Shows/hides the playlist editor"));
  action -> setWhatsThis (i18n("Show Editor command shows or hides the playlist editor."));
  action = new KToggleAction (i18n("Show &Toolbar"), 0, 0, this, SLOT (playlistToolbar()), actionCollection(), "playlist_toolbar");
  action -> setStatusText (i18n("Shows/hides the playlist editor toolbar"));
  action -> setWhatsThis (i18n("Show Toolbar command shows or hides the playlist editor toolbar."));
  action = KStdAction::showMenubar (this, SLOT (viewMenubar()), actionCollection());
  action -> setStatusText (i18n("Shows/hides the menu bar"));
  action -> setWhatsThis (i18n("Show Menubar command shows or hides the menu bar."));
  action = KStdAction::showStatusbar (this, SLOT (viewStatusbar()), actionCollection());
  action -> setStatusText (i18n("Shows/hides the status bar"));
  action -> setWhatsThis (i18n("Show Statusbar command shows or hides the status bar."));
  action = new KToggleAction (i18n("Show Message &Log"), 0, CTRL + Key_L, this, SLOT (viewMessageLog()), actionCollection(), "options_show_log");
  action -> setStatusText (i18n("Shows/hides the message log"));
  action -> setWhatsThis (i18n("Show Message Log command shows or hides the message log."));
  action = KStdAction::showToolbar (this, SLOT (viewMainToolbar()), actionCollection());
  action -> setText (i18n("Show Main &Toolbar"));
  action -> setStatusText (i18n("Shows/hides the main toolbar"));
  action -> setWhatsThis (i18n("Show Main Toolbar command shows or hides the main toolbar."));
  action = new KToggleAction (i18n("Show &Progress Toolbar"), 0, 0, this, SLOT (viewProgressToolbar()), actionCollection(), "settings_progress_toolbar");
  action -> setStatusText (i18n("Shows/hides the progress toolbar"));
  action -> setWhatsThis (i18n("Show Progress Toolbar command shows or hides the progress toolbar. This command is available when the time length of the current file is known."));
  action = new KToggleAction (i18n("Show &Volume Toolbar"), "volume", 0, this, SLOT (viewVolumeToolbar()), actionCollection(), "settings_volume_toolbar");
  action -> setStatusText (i18n("Shows/hides the volume toolbar"));
  action -> setWhatsThis (i18n("Show Volume Toolbar command shows or hides the volume toolbar."));
  action = new KToggleAction (i18n("Show C&ontrast Toolbar"), "contrast", 0, this, SLOT (viewContrastToolbar()), actionCollection(), "settings_contrast_toolbar");
  action -> setStatusText (i18n("Shows/hides the contrast toolbar"));
  action -> setWhatsThis (i18n("Show Contrast Toolbar command shows or hides the contrast toolbar. This command is available for video files."));
  action = new KToggleAction (i18n("Show &Brightness Toolbar"), "brightness", 0, this, SLOT (viewBrightnessToolbar()), actionCollection(), "settings_brightness_toolbar");
  action -> setStatusText (i18n("Shows/hides the brightness toolbar"));
  action -> setWhatsThis (i18n("Show Brightness Toolbar command shows or hides the brightness toolbar. This command is available for video files."));
  action = new KToggleAction (i18n("Show H&ue Toolbar"), "hue", 0, this, SLOT (viewHueToolbar()), actionCollection(), "settings_hue_toolbar");
  action -> setStatusText (i18n("Shows/hides the hue toolbar"));
  action -> setWhatsThis (i18n("Show Hue Toolbar command shows or hides the hue toolbar. This command is available for video files."));
  action = new KToggleAction (i18n("Show &Saturation Toolbar"), "saturation", 0, this, SLOT (viewSaturationToolbar()), actionCollection(), "settings_saturation_toolbar");
  action -> setStatusText (i18n("Shows/hides the saturation toolbar"));
  action -> setWhatsThis (i18n("Show Saturation Toolbar command shows or hides the saturation toolbar. This command is available for video files."));
  action = KStdAction::keyBindings (this, SLOT (settingsKeyBindings()), actionCollection());
  action -> setStatusText (i18n("Opens the KPlayer key bindings dialog"));
  action -> setWhatsThis (i18n("Configure Shortcuts command opens a dialog that lets you see and change KPlayer's shortcut key bindings, or associations between actions and the corresponding keys or combinations of keys that activate them. If you change the bindings, make sure not to duplicate an existing shortcut and also not to use the Shift key for your new shortcuts, because the Shift key has a special function in KPlayer."));
  action = KStdAction::preferences (this, SLOT (settingsConfigure()), actionCollection());
  action -> setStatusText (i18n("Opens the KPlayer configuration dialog"));
  action -> setWhatsThis (i18n("Configure KPlayer command opens a dialog that lets you configure the program, modifying various aspects of its functionality, user interface and interaction with MPlayer. For more information see the Configuration dialog chapter and the Advanced configuration micro-HOWTO in the user manual."));
  createGUI();
  action = actionCollection() -> action (KStdAction::stdName (KStdAction::ShowMenubar));
  if ( action )
  {
    action -> setStatusText (i18n("Shows/hides the menu bar"));
    action -> setWhatsThis (i18n("Show Menubar command shows or hides the menu bar."));
  }
  action = actionCollection() -> action (KStdAction::stdName (KStdAction::ShowStatusbar));
  if ( action )
  {
    action -> setStatusText (i18n("Shows/hides the status bar"));
    action -> setWhatsThis (i18n("Show Statusbar command shows or hides the status bar."));
  }
  action = actionCollection() -> action ("help_contents");
  if ( action )
  {
    action -> setStatusText (i18n("Opens the KPlayer user manual"));
    action -> setWhatsThis (i18n("KPlayer Handbook command opens and displays the KPlayer user manual."));
  }
  action = actionCollection() -> action ("help_whats_this");
  if ( action )
  {
    action -> setStatusText (i18n("Lets you click any part of KPlayer to get its description"));
    action -> setWhatsThis (i18n("What's This command changes the mouse pointer to a question mark and lets you click a KPlayer interface element to get a quick description of its purpose and functionality."));
  }
  action = actionCollection() -> action ("help_report_bug");
  if ( action )
  {
    action -> setStatusText (i18n("Opens the bug report window"));
    action -> setWhatsThis (i18n("Report Bug command opens a dialog that allows you to send a bug report to the KPlayer developer. However, the recommended way to report a bug is to collect information using this dialog and then submit it following the instructions in the Bug reporting micro-HOWTO."));
  }
  action = actionCollection() -> action ("help_about_app");
  if ( action )
  {
    action -> setStatusText (i18n("Shows information about this version of KPlayer"));
    action -> setWhatsThis (i18n("About KPlayer command displays some brief information about KPlayer's version number, authors and license agreement."));
  }
  action = actionCollection() -> action ("help_about_kde");
  if ( action )
  {
    action -> setStatusText (i18n("Shows information about your KDE version"));
    action -> setWhatsThis (i18n("About KDE command shows some information about the version of KDE that you are running."));
  }
}

void KPlayer::initStatusBar (void)
{
  statusBar() -> insertItem (i18n("Ready"), ID_STATUS_MSG, 1);
  statusBar() -> insertItem (i18n("Idle"), ID_STATE_MSG, 1, true);
  statusBar() -> insertItem ("", ID_PROGRESS_MSG, 1, true);
  QObjectList* children = (QObjectList*) statusBar() -> children();
  for ( uint i = 0; i < children -> count(); i ++ )
  {
    QObject* child = children -> at (i);
    if ( child -> inherits ("KStatusBarLabel") )
    {
      KStatusBarLabel* label = (KStatusBarLabel*) child;
      if ( label -> text() == i18n("Ready") )
        m_status_label = label;
      else if ( label -> text() == i18n("Idle") )
        m_state_label = label;
      else if ( label -> text().isEmpty() )
        m_progress_label = label;
    }
  }
  statusBar() -> setMinimumHeight (statusBar() -> layout() -> minimumSize().height());
  if ( m_status_label )
  {
    connect (m_status_label, SIGNAL (itemPressed (int)), this, SLOT (statusPressed()));
    QWhatsThis::add (m_status_label, i18n("Status area of the status bar tells you if there have been playback errors."));
  }
  if ( m_state_label )
    QWhatsThis::add (m_state_label, i18n("State area of the status bar displays the current player state."));
  if ( m_progress_label )
    QWhatsThis::add (m_progress_label, i18n("Progress area of the status bar shows playback progress and the total length if known."));
}

void KPlayer::refreshSettings (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::refreshSettings\n";
#endif
  QApplication::postEvent (this, new QEvent (QEvent::LayoutHint));
//activateLayout();
  if ( ! kPlayerSettings() -> url().isEmpty() )
    setCaption (kPlayerSettings() -> caption());
}

void KPlayer::saveOptions (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Saving options\n";
#endif
  KConfig* config = kPlayerConfig();
  saveMainWindowSettings (config, "General Options");
  config -> setGroup ("General Options");
  Qt::Dock dock = DockTornOff;
  int index = 0, offset = 0;
  bool newline = true;
  bool docked = getLocation (m_log, dock, index, newline, offset) &&
    (dock == DockTop || dock == DockBottom || dock == DockRight || dock == DockLeft);
  if ( docked )
  {
    config -> writeEntry ("Message Log Dock", dock);
    config -> writeEntry ("Message Log Index", index);
    config -> writeEntry ("Message Log New Line", newline);
    config -> writeEntry ("Message Log Offset", offset);
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "Log dock " << dock << " " << index << " " << newline << " " << offset << " " << m_log -> x() << "x" << m_log -> y() << " " << m_log -> width() << "x" << m_log -> height() << "\n";
#endif
  }
  else
  {
    config -> deleteEntry ("Message Log Dock");
    config -> deleteEntry ("Message Log Index");
    config -> deleteEntry ("Message Log New Line");
    config -> deleteEntry ("Message Log Offset");
  }
  config -> writeEntry ("Message Log Left", m_log -> x());
  config -> writeEntry ("Message Log Top", m_log -> y());
  config -> writeEntry ("Message Log Width", m_log -> width());
  config -> writeEntry ("Message Log Height", m_log -> height());
  config -> writeEntry ("Message Log Docked", docked);
  dock = DockTornOff;
  index = offset = 0;
  newline = true;
  docked = getLocation (m_playlist, dock, index, newline, offset) &&
    (dock == DockTop || dock == DockBottom || dock == DockRight || dock == DockLeft);
  if ( docked )
  {
    config -> writeEntry ("Playlist Dock", dock);
    config -> writeEntry ("Playlist Index", index);
    config -> writeEntry ("Playlist New Line", newline);
    config -> writeEntry ("Playlist Offset", offset);
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "Playlist dock " << dock << " " << index << " " << newline << " " << offset << " " << m_playlist -> x() << "x" << m_playlist -> y() << " " << m_playlist -> width() << "x" << m_playlist -> height() << "\n";
#endif
  }
  else
  {
    config -> deleteEntry ("Playlist Dock");
    config -> deleteEntry ("Playlist Index");
    config -> deleteEntry ("Playlist New Line");
    config -> deleteEntry ("Playlist Offset");
  }
  config -> writeEntry ("Playlist Left", m_playlist -> x());
  config -> writeEntry ("Playlist Top", m_playlist -> y());
  config -> writeEntry ("Playlist Width", m_playlist -> width());
  config -> writeEntry ("Playlist Height", m_playlist -> height());
  config -> writeEntry ("Playlist Docked", docked);
  if ( ! m_full_screen && ! isMaximized() )
  {
    config -> writeEntry ("Main Window Left", x());
    config -> writeEntry ("Main Window Top", y());
    config -> writeEntry ("Main Window Width", width());
    config -> writeEntry ("Main Window Height", height());
  }
  else
  {
    config -> writeEntry ("Main Window Left", m_normal_geometry.x());
    config -> writeEntry ("Main Window Top", m_normal_geometry.y());
    config -> writeEntry ("Main Window Width", m_normal_geometry.width());
    config -> writeEntry ("Main Window Height", m_normal_geometry.height());
  }
  config -> writeEntry ("Menu Bar Normally Visible", m_menubar_normally_visible);
  config -> writeEntry ("Menu Bar FullScreen Visible", m_menubar_fullscreen_visible);
  config -> writeEntry ("Status Bar Normally Visible", m_statusbar_normally_visible);
  config -> writeEntry ("Status Bar FullScreen Visible", m_statusbar_fullscreen_visible);
  config -> writeEntry ("Message Log Normally Visible", m_messagelog_normally_visible);
  config -> writeEntry ("Message Log FullScreen Visible", m_messagelog_fullscreen_visible);
  config -> writeEntry ("Playlist Editor Normally Visible", m_editor_normally_visible);
  config -> writeEntry ("Playlist Editor FullScreen Visible", m_editor_fullscreen_visible);
  for ( int i = 0; i < KPLAYER_TOOLBARS; i ++ )
  {
    config -> writeEntry (QString (m_toolbar[i].name) + " Normally Visible", m_toolbar[i].normally_visible);
    config -> writeEntry (QString (m_toolbar[i].name) + " FullScreen Visible", m_toolbar[i].fullscreen_visible);
  }
}

void KPlayer::readOptions (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Reading options\n";
#endif
  KConfig* config = kPlayerConfig();
  applyMainWindowSettings (config, "General Options");
  config -> setGroup ("General Options");
  int width = config -> readNumEntry ("Main Window Width", 500);
  if ( width > QApplication::desktop() -> availableGeometry().width() )
    width = QApplication::desktop() -> availableGeometry().width();
  int height = config -> readNumEntry ("Main Window Height", 350);
  if ( height > QApplication::desktop() -> availableGeometry().height() )
    height = QApplication::desktop() -> availableGeometry().height();
  int x = config -> readNumEntry ("Main Window Left", -1);
  if ( x + width > QApplication::desktop() -> availableGeometry().width() )
    x = QApplication::desktop() -> availableGeometry().width() - width;
  int y = config -> readNumEntry ("Main Window Top", -1);
  if ( y + height > QApplication::desktop() -> availableGeometry().height() )
    y = QApplication::desktop() -> availableGeometry().height() - height;
  if ( x >= 0 && y >= 0 )
    move (x, y);
  if ( width >= minimumWidth() && height >= minimumHeight() )
    resize (width, height);
  m_normal_geometry.setRect (x, y, width, height);
  Qt::Dock dock = (Qt::Dock) config -> readNumEntry ("Message Log Dock", Qt::DockBottom);
  bool docked = config -> readBoolEntry ("Message Log Docked", true);
  bool newline = config -> readBoolEntry ("Message Log New Line", true);
  int index = config -> readNumEntry ("Message Log Index", 0);
  int offset = config -> readNumEntry ("Message Log Offset", 0);
  int left = config -> readNumEntry ("Message Log Left", 0);
  int top = config -> readNumEntry ("Message Log Top", 0);
  width = config -> readNumEntry ("Message Log Width", 0);
  height = config -> readNumEntry ("Message Log Height", 0);
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Log dock " << dock << " " << index << " " << newline << " " << offset << " " << left << "x" << top << " " << width << "x" << height << "\n";
#endif
  if ( width > 0 && height > 0 )
  {
    m_log -> setFixedExtentWidth (width);
    m_log -> setFixedExtentHeight (height);
  }
  moveDockWindow (m_log, dock, newline, index, offset);
  if ( ! docked )
  {
    m_log -> undock();
    if ( width > 0 && height > 0 )
      m_log -> setGeometry (left, top, width, height);
  }
  dock = (Qt::Dock) config -> readNumEntry ("Playlist Dock", Qt::DockBottom);
  docked = config -> readBoolEntry ("Playlist Docked", true);
  newline = config -> readBoolEntry ("Playlist New Line", true);
  index = config -> readNumEntry ("Playlist Index", 0);
  offset = config -> readNumEntry ("Playlist Offset", 0);
  left = config -> readNumEntry ("Playlist Left", 0);
  top = config -> readNumEntry ("Playlist Top", 0);
  width = config -> readNumEntry ("Playlist Width", 0);
  height = config -> readNumEntry ("Playlist Height", 0);
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Playlist dock " << dock << " " << index << " " << newline << " " << offset << " " << left << "x" << top << " " << width << "x" << height << "\n";
#endif
  if ( width > 0 && height > 0 )
  {
    m_playlist -> setFixedExtentWidth (width);
    m_playlist -> setFixedExtentHeight (height);
  }
  moveDockWindow (m_playlist, dock, newline, index, offset);
  if ( ! docked )
  {
    m_playlist -> undock();
    if ( width > 0 && height > 0 )
      m_playlist -> setGeometry (left, top, width, height);
  }
  m_menubar_normally_visible = config -> readBoolEntry ("Menu Bar Normally Visible", m_menubar_normally_visible);
  m_menubar_fullscreen_visible = config -> readBoolEntry ("Menu Bar FullScreen Visible", m_menubar_fullscreen_visible);
  showMenubar();
  m_statusbar_normally_visible = config -> readBoolEntry ("Status Bar Normally Visible", m_statusbar_normally_visible);
  m_statusbar_fullscreen_visible = config -> readBoolEntry ("Status Bar FullScreen Visible", m_statusbar_fullscreen_visible);
  showStatusbar();
  for ( int i = 0; i < KPLAYER_TOOLBARS; i ++ )
  {
    m_toolbar[i].normally_visible = config -> readBoolEntry (QString (m_toolbar[i].name) + " Normally Visible", m_toolbar[i].normally_visible);
    m_toolbar[i].fullscreen_visible = config -> readBoolEntry (QString (m_toolbar[i].name) + " FullScreen Visible", m_toolbar[i].fullscreen_visible);
    //kdDebugTime() << "Show " << m_toolbar[i].name << " " << m_toolbar[i].action << "\n";
    showToolbar (i);
  }
  m_messagelog_normally_visible = config -> readBoolEntry ("Message Log Normally Visible", m_messagelog_normally_visible);
  m_messagelog_fullscreen_visible = config -> readBoolEntry ("Message Log FullScreen Visible", m_messagelog_fullscreen_visible);
  showMessageLog();
  m_editor_normally_visible = config -> readBoolEntry ("Playlist Editor Normally Visible", m_editor_normally_visible);
  m_editor_fullscreen_visible = config -> readBoolEntry ("Playlist Editor FullScreen Visible", m_editor_fullscreen_visible);
  showPlaylistEditor();
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Done reading options\n";
#endif
}

/*void KPlayer::saveProperties (KConfig* config) const
{
  config -> writeEntry ("File URL", kPlayerSettings() -> url().url());
}

void KPlayer::readProperties (KConfig* config)
{
  m_playlist -> kPlayerPlaylistWidget() -> load (config -> readEntry ("File URL"), false);
}*/

void KPlayer::enableToolbar (int index)
{
  Toolbar& toolbar = m_toolbar [index];
  KToggleAction* action = toggleAction (toolbar.action);
//if ( action -> isEnabled() )
//  return;
  action -> setEnabled (true);
  showToolbar (index);
}

void KPlayer::disableToolbar (int index)
{
  Toolbar& toolbar = m_toolbar [index];
  KToggleAction* action = toggleAction (toolbar.action);
//if ( ! action -> isEnabled() )
//  return;
  if ( action -> isChecked() )
  {
    action -> setChecked (false);
    toolBar (toolbar.name) -> hide();
  }
  action -> setEnabled (false);
}

void KPlayer::enablePlayerActions (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer: Enabling player actions\n";
#endif
  if ( kPlayerProcess() -> state() == KPlayerProcess::Idle || kPlayerProcess() -> isInfoAvailable() || kPlayerSettings() -> hasLength() )
  {
    if ( kPlayerSettings() -> hasLength() )
      enableToolbar (PROGRESS_TOOLBAR);
    else
      disableToolbar (PROGRESS_TOOLBAR);
  }
  enableSubmenu (2, 3, kPlayerProcess() -> state() != KPlayerProcess::Paused);
}

void KPlayer::enableVideoActions (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer: Enabling video actions\n";
#endif
  bool video = kPlayerSettings() -> hasVideo();
  bool unpaused = video && kPlayerProcess() -> state() != KPlayerProcess::Paused;
  if ( kPlayerProcess() -> state() != KPlayerProcess::Running )
  {
    for ( int i = FIRST_VIDEO_TOOLBAR; i < KPLAYER_TOOLBARS; i ++ )
      if ( video )
        enableToolbar (i);
      else
        disableToolbar (i);
  }
  enableSubtitleActions();
  enableSubmenu (2, 2, unpaused);
}

QPopupMenu* KPlayer::popupMenu (int index)
{
  QMenuData* data = menuBar();
  if ( ! data )
    return 0;
  int id = data -> idAt (index);
  if ( id == -1 )
    return 0;
  QMenuItem* item = data -> findItem (id);
  if ( ! item )
    return 0;
  return item -> popup();
}

void KPlayer::enableSubmenu (int index, int subindex, bool enable)
{
  QMenuData* data = popupMenu (index);
  if ( ! data )
    return;
  int id = data -> idAt (data -> count() - subindex);
  if ( id == -1 )
    return;
  data -> setItemEnabled (id, enable);
}

void KPlayer::enableSubtitleActions (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer: Enabling subtitle actions\n";
#endif
  enableSubmenu (2, 1, kPlayerSettings() -> hasSubtitles() && kPlayerProcess() -> state() == KPlayerProcess::Playing);
}

/*void KPlayer::checkBarActions (void)
{
  toggleAction (KStdAction::stdName (KStdAction::ShowMenubar)) -> setChecked (menuBar() -> isVisible());
  for ( int i = 0; i < KPLAYER_TOOLBARS; i ++ )
    toggleAction (m_toolbar[i].action) -> setChecked (toolBar (m_toolbar[i].name) -> isVisible());
  toggleAction (KStdAction::stdName (KStdAction::ShowStatusbar)) -> setChecked (statusBar() -> isVisible());
}*/

void KPlayer::dragEnterEvent (QDragEnterEvent *event)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Drag enter event\n";
#endif
  event -> accept (KURLDrag::canDecode (event));
}

void KPlayer::dropEvent (QDropEvent* event)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Drop event\n";
#endif
  KURL::List urls;
  if ( ! KURLDrag::decode (event, urls) )
    return;
  m_playlist -> kPlayerPlaylistWidget() -> openUrls (urls);
}

bool KPlayer::isMaximized (void) const
{
  // KDE 3.2:
  //return (KWin::windowInfo (winId(), NET::WMState).state() & NET::Max) == NET::Max;
  return (KWin::info (winId()).state & NET::Max) == NET::Max;
}

void KPlayer::showMaximized (void)
{
  bool maximized = isMaximized();
  KMainWindow::showMaximized();
  KWin::clearState (winId(), 1 << 9); // KDE 3.2 FullScreen
  KWin::setState (winId(), NET::Max);
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::showMaximized " << maximized << " -> " << isMaximized() << "\n";
#endif
  //syncronizeEvents();
  activateLayout();
}

void KPlayer::showNormal (void)
{
  bool maximized = isMaximized();
  KMainWindow::showNormal();
  //KMainWindow::showNormal();
  KWin::clearState (winId(), NET::Max | (1 << 9)); // KDE 3.2 FullScreen
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::showNormal " << maximized << " -> " << isMaximized() << "\n";
#endif
  //syncronizeEvents();
  activateLayout();
}

void KPlayer::start (void)
{
  if ( isMinimized() )
  {
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "Restoring minimized window\n";
#endif
    clearWState (WState_Visible); // or showNormal won't work
    showNormal();
  }
  else
    show();
  raise();
  setActiveWindow(); // doesn't work after restoring from minimized state
  KWin::setActiveWindow (winId());
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Main window minimum size " << minimumWidth() << "x" << minimumHeight()
    << ", maximum size " << maximumWidth() << "x" << maximumHeight()
    << ", size " << width() << "x" << height() << "\n";
#endif
  KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
  if ( args -> count() > 0 )
  {
    KURL::List urls;
    for ( int i = 0; i < args -> count(); i ++ )
      urls << args -> url (i);
    m_playlist -> kPlayerPlaylistWidget() -> openUrls (urls);
    //openUrl (KCmdLineArgs::makeURL (args -> arg (args -> count() - 1)));
  }
  else if ( kPlayerSettings() -> showOpenDialog() )
    m_playlist -> kPlayerPlaylistWidget() -> fileOpen();
  args -> clear();
}

//bool KPlayerX11TestGrab (Display*, int);

/*bool KPlayer::event (QEvent* ev)
{
  kdDebugTime() << "KPlayer event: type " << ev -> type()
    << " spontaneous " << ev -> spontaneous() << "\n";
//if ( ev -> type() == QEvent::LayoutHint && isFullScreen() )
//  return false;
  return KMainWindow::event (ev);
}*/

void KPlayer::contextMenuEvent (QContextMenuEvent* event)
{
  KMainWindow::contextMenuEvent (event);
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Main " << winId() << " wspace " << kPlayerWorkspace() -> winId()
    << " widget " << kPlayerWidget() -> winId() << "\n";
  dumpObject (this, 0);
#endif
  QPopupMenu* popup = (QPopupMenu*) factory() -> container ("player_popup", this);
  popup -> popup (event -> globalPos());
  event -> accept();
}

void KPlayer::playlistStarted (void)
{
  setStatusText (i18n("Ready"));
  m_log -> setError (false);
  if ( m_status_label )
    m_status_label -> unsetCursor();
}

void KPlayer::playlistActivated (void)
{
  setCaption (kPlayerSettings() -> caption());
  clearStatusMessage();
  if ( m_progress_label )
    m_progress_label -> setText ("");
  connect (kPlayerSettings() -> properties(), SIGNAL (refresh()), this, SLOT (refreshProperties()));
}

void KPlayer::playlistStopped (void)
{
  if ( m_error_detected && ! m_log -> isEmpty() )
    m_log -> setError (true);
  m_error_detected = false;
  playerProgressChanged (kPlayerProcess() -> position(), KPlayerProcess::Position);
}

void KPlayer::statusPressed (void)
{
  if ( m_log -> hasError() || m_error_detected )
    showMessageLog (true);
}

void KPlayer::refreshProperties (void)
{
  setCaption (kPlayerSettings() -> caption());
}

void KPlayer::playlistContextMenu (KListView*, QListViewItem*, const QPoint& point)
{
  QPopupMenu* popup = (QPopupMenu*) factory() -> container ("playlist_popup", this);
  popup -> popup (point);
}

void KPlayer::closeEvent (QCloseEvent* event)
{
  disconnect (m_log, SIGNAL (windowHidden()), this, SLOT (logWindowHidden()));
  disconnect (m_playlist, SIGNAL (windowHidden()), this, SLOT (playlistEditorHidden()));
  if ( kPlayerSettings() -> properties() )
    disconnect (kPlayerSettings() -> properties(), SIGNAL (refresh()), this, SLOT (refreshProperties()));
  disconnect (kPlayerSettings(), SIGNAL (refresh()), this, SLOT (refreshSettings()));
  saveOptions();
  m_playlist -> kPlayerPlaylistWidget() -> terminate();
  KPlayerEngine::terminate();
  KMainWindow::closeEvent (event);
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::closeEvent\n";
#endif
}

void KPlayer::fileClearLog (void)
{
  m_log -> clear();
  setStatusText (i18n("Ready"));
  if ( m_status_label )
    m_status_label -> unsetCursor();
}

void KPlayer::fileQuit (void)
{
  close();
}

void KPlayer::playlistShow (void)
{
  showToolbar (PLAYLIST_TOOLBAR, toggleAction (m_toolbar[PLAYLIST_TOOLBAR].action) -> isChecked());
}

void KPlayer::playlistToolbar (void)
{
  showToolbar (EDITOR_TOOLBAR, toggleAction (m_toolbar[EDITOR_TOOLBAR].action) -> isChecked());
}

void KPlayer::playlistEditor (void)
{
  showPlaylistEditor (toggleAction ("playlist_editor") -> isChecked());
}

void KPlayer::showPlaylistEditor (bool show)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "showPlaylistEditor " << show << " => " << kPlayerSettings() -> fullScreen() << "\n";
#endif
  if ( kPlayerSettings() -> fullScreen() )
    m_editor_fullscreen_visible = show;
  else
    m_editor_normally_visible = show;
  showPlaylistEditor();
}

void KPlayer::showPlaylistEditor (void)
{
  bool show = kPlayerSettings() -> fullScreen() ? m_editor_fullscreen_visible : m_editor_normally_visible;
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "showPlaylistEditor " << show << " <= " << kPlayerSettings() -> fullScreen() << "\n";
#endif
  toggleAction ("playlist_editor") -> setChecked (show);
  KToggleAction* toggle = toggleAction (m_toolbar[EDITOR_TOOLBAR].action);
  toggle -> setEnabled (show);
  m_show_playlist = show;
  if ( show )
  {
    m_playlist -> show();
    showToolbar (EDITOR_TOOLBAR);
  }
  else
  {
    m_playlist -> hide();
    toggle -> setChecked (false);
  }
}

void KPlayer::showMenubar (void)
{
  bool show = kPlayerSettings() -> fullScreen() ? m_menubar_fullscreen_visible : m_menubar_normally_visible;
  toggleAction (KStdAction::stdName (KStdAction::ShowMenubar)) -> setChecked (show);
  toggleAction (KStdAction::stdName (KStdAction::ShowMenubar)) -> setStatusText (i18n("Shows/hides the menu bar"));
  if ( show )
    menuBar() -> show();
  else
    menuBar() -> hide();
}

void KPlayer::viewMenubar (void)
{
  bool show = toggleAction (KStdAction::stdName (KStdAction::ShowMenubar)) -> isChecked();
  if ( kPlayerSettings() -> fullScreen() )
    m_menubar_fullscreen_visible = show;
  else
    m_menubar_normally_visible = show;
  if ( show )
    menuBar() -> show();
  else
    menuBar() -> hide();
}

void KPlayer::showStatusbar (void)
{
  bool show = kPlayerSettings() -> fullScreen() ? m_statusbar_fullscreen_visible : m_statusbar_normally_visible;
  toggleAction (KStdAction::stdName (KStdAction::ShowStatusbar)) -> setChecked (show);
  toggleAction (KStdAction::stdName (KStdAction::ShowStatusbar)) -> setStatusText (i18n("Shows/hides the status bar"));
  if ( show )
    statusBar() -> show();
  else
    statusBar() -> hide();
}

void KPlayer::viewStatusbar (void)
{
  bool show = toggleAction (KStdAction::stdName (KStdAction::ShowStatusbar)) -> isChecked();
  if ( kPlayerSettings() -> fullScreen() )
    m_statusbar_fullscreen_visible = show;
  else
    m_statusbar_normally_visible = show;
  if ( show )
    statusBar() -> show();
  else
    statusBar() -> hide();
}

void KPlayer::showMessageLog (void)
{
  bool show = kPlayerSettings() -> fullScreen() ? m_messagelog_fullscreen_visible : m_messagelog_normally_visible;
  toggleAction ("options_show_log") -> setChecked (show);
  m_show_log = show;
  if ( show )
    m_log -> show();
  else
    m_log -> hide();
}

void KPlayer::showMessageLog (bool show)
{
  if ( kPlayerSettings() -> fullScreen() )
    m_messagelog_fullscreen_visible = show;
  else
    m_messagelog_normally_visible = show;
  showMessageLog();
  setStatusText (i18n("Ready"));
  m_log -> setError (false);
  if ( m_status_label )
    m_status_label -> unsetCursor();
}

void KPlayer::viewMessageLog (void)
{
  showMessageLog (toggleAction ("options_show_log") -> isChecked());
}

void KPlayer::showToolbar (int index)
{
  bool show = kPlayerSettings() -> fullScreen() ? m_toolbar[index].fullscreen_visible : m_toolbar[index].normally_visible;
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "showToolbar '" << m_toolbar[index].name << "' " << show << " <= " << kPlayerSettings() -> fullScreen() << "\n";
#endif
  toggleAction (m_toolbar[index].action) -> setChecked (show);
  if ( show )
  {
    KPlayerSettings* settings = kPlayerSettings();
    KToolBar* toolbar = toolBar (m_toolbar[index].name);
    toolbar -> show();
    int hint = toolbar -> minimumSizeHint().height();
    if ( toolbar -> orientation() == Qt::Vertical && kPlayerWorkspace() -> height() < hint
      && ! settings -> fullScreen() && ! settings -> maximized() )
    {
      settings -> setDisplaySize (QSize (settings -> displaySize().width(), hint));
      zoom();
    }
  }
  else
    toolBar (m_toolbar[index].name) -> hide();
}

void KPlayer::showToolbar (int index, bool show)
{
  if ( kPlayerSettings() -> fullScreen() )
    m_toolbar[index].fullscreen_visible = show;
  else
    m_toolbar[index].normally_visible = show;
  showToolbar (index);
}

void KPlayer::viewMainToolbar (void)
{
  showToolbar (MAIN_TOOLBAR, toggleAction (m_toolbar[MAIN_TOOLBAR].action) -> isChecked());
}

void KPlayer::viewProgressToolbar (void)
{
  showToolbar (PROGRESS_TOOLBAR, toggleAction (m_toolbar[PROGRESS_TOOLBAR].action) -> isChecked());
}

void KPlayer::viewVolumeToolbar (void)
{
  showToolbar (VOLUME_TOOLBAR, toggleAction (m_toolbar[VOLUME_TOOLBAR].action) -> isChecked());
}

void KPlayer::viewContrastToolbar (void)
{
  showToolbar (CONTRAST_TOOLBAR, toggleAction (m_toolbar[CONTRAST_TOOLBAR].action) -> isChecked());
}

void KPlayer::viewBrightnessToolbar (void)
{
  showToolbar (BRIGHTNESS_TOOLBAR, toggleAction (m_toolbar[BRIGHTNESS_TOOLBAR].action) -> isChecked());
}

void KPlayer::viewHueToolbar (void)
{
  showToolbar (HUE_TOOLBAR, toggleAction (m_toolbar[HUE_TOOLBAR].action) -> isChecked());
}

void KPlayer::viewSaturationToolbar (void)
{
  showToolbar (SATURATION_TOOLBAR, toggleAction (m_toolbar[SATURATION_TOOLBAR].action) -> isChecked());
}

void KPlayer::settingsKeyBindings (void)
{
  // reportedly was buggy in KDE 3.0x
  KKeyDialog::configure (actionCollection());
//KKeyDialog keydlg (true);
//keydlg.insert (actionCollection());
//keydlg.configure (true);
}

void KPlayer::settingsConfigure (void)
{
  KPlayerSettingsDialog (this).exec();
}

void KPlayer::playerStateChanged (KPlayerProcess::State state, KPlayerProcess::State previous)
{
  static const QString stateMessages [4] = { i18n("Idle"), i18n("Running"), i18n("Playing"), i18n("Paused") };
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Main window received state change: " << previous << " => " << state << "\n";
#endif
  clearStatusMessage();
  if ( state != kPlayerProcess() -> state() )
    return;
  if ( m_state_label )
    m_state_label -> setText (stateMessages [state]);
  if ( state == KPlayerProcess::Running && previous != state )
  {
    if ( kPlayerSettings() -> clearMessagesBeforePlaying() && ! m_error_detected )
      fileClearLog();
    else if ( ! m_log -> isEmpty() )
      m_log -> addLine ("------------------------------------------------------------");
    if ( kPlayerSettings() -> showMessagesBeforePlaying() )
      showMessageLog (true);
    if ( m_error_detected && ! m_log -> isEmpty() )
      m_log -> setError (true);
    m_error_detected = false;
  }
  if ( state == KPlayerProcess::Idle && previous != state )
  {
    if ( kPlayerSettings() -> showMessagesAfterPlaying() )
      showMessageLog (true);
    //playerProgressChanged (0, KPlayerProcess::Position);
    //if ( kPlayerProcess() -> state() == KPlayerProcess::Idle )
    if ( kPlayerEngine() -> stopped() )
    {
      if ( m_error_detected && ! m_log -> isEmpty() )
        m_log -> setError (true);
      m_error_detected = false;
    }
  }
  enablePlayerActions();
  enableVideoActions();
}

void KPlayer::playerProgressChanged (float progress, KPlayerProcess::ProgressType type)
{
  static const QString cacheFillMessage (i18n("Cache fill: %1%"));
  static const QString indexGenerationMessage (i18n("Generating index: %1%"));
  static const QString fileTransferMessage (i18n("Transferring file: %1%"));
  if ( ! m_progress_label )
    return;
  if ( type == KPlayerProcess::Position )
    m_progress_label -> setText (kPlayerProcess() -> positionString());
  else
  {
    int value = limit (int (progress + 0.5), 0, 100);
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "Progress " << type << " " << progress << " " << value << "\n";
#endif
    m_progress_label -> setText ((type == KPlayerProcess::CacheFill ? cacheFillMessage
      : type == KPlayerProcess::IndexGeneration ? indexGenerationMessage
      : fileTransferMessage).arg (value));
  }
}

void KPlayer::playerInfoAvailable (void)
{
  enablePlayerActions();
  if ( kPlayerSettings() -> hasLength() )
    playerProgressChanged (kPlayerProcess() -> position(), KPlayerProcess::Position);
}

void KPlayer::playerSizeAvailable (void)
{
  enableVideoActions();
}

void KPlayer::playerMessageReceived (QString message)
{
  if ( message.isEmpty() )
    return;
  log (message);
}

void KPlayer::playerErrorDetected (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Error detected\n";
#endif
  setStatusText (i18n("Error"));
  if ( m_log -> isHidden() && m_status_label )
    m_status_label -> setCursor (Qt::PointingHandCursor);
  m_error_detected = true;
  if ( kPlayerSettings() -> showMessagesOnError() )
    showMessageLog (true);
}

void KPlayer::logWindowHidden (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Log window hidden signal\n";
  if ( m_log -> isVisible() )
    kdDebug() << "  but the log window is visible\n";
  else if ( ! m_log -> isHidden() )
    kdDebug() << "  but the log window is not hidden\n";
#endif
  showMessageLog (false);
}

void KPlayer::playlistEditorHidden (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Playlist window hidden signal\n";
  if ( m_playlist -> isVisible() )
    kdDebug() << "  but the playlist editor is visible\n";
  else if ( ! m_playlist -> isHidden() )
    kdDebug() << "  but the playlist editor is not hidden\n";
#endif
  showPlaylistEditor (false);
}

void KPlayer::log (QString message)
{
  if ( message.isEmpty() )
    return;
  m_log -> addLine (message);
}

void KPlayer::setStatusText (const QString& text)
{
  statusBar() -> clear();
//statusBar() -> changeItem (text, ID_STATUS_MSG);
  if ( m_status_label )
    m_status_label -> setText (text);
}

void KPlayer::setStatusMessage (const QString& text)
{
  if ( text.isEmpty() )
  {
    clearStatusMessage();
    return;
  }
  QPopupMenu* popup = (QPopupMenu*) factory() -> container ("player_popup", this);
  if ( ! popup || ! popup -> isVisible() )
    popup = (QPopupMenu*) factory() -> container ("playlist_popup", this);
  if ( ! popup || ! popup -> isVisible() )
    for ( uint i = 0; i < menuBar() -> count(); i ++ )
      if ( (popup = popupMenu (i)) && popup -> isVisible() )
        break;
  if ( popup && popup -> isVisible() )
  {
    statusBar() -> message (text);
    m_progress_label -> hide();
    m_state_label -> hide();
  }
}

void KPlayer::clearStatusMessage (void)
{
  statusBar() -> clear();
  m_state_label -> show();
  m_progress_label -> show();
}

void KPlayer::showEvent (QShowEvent* event)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::showEvent\n";
#endif
  KMainWindow::showEvent (event);
  if ( m_initial_show )
    return;
  m_initial_show = true;
  if ( x() != m_normal_geometry.x() && m_normal_geometry.x() >= 0
    && y() != m_normal_geometry.y() && m_normal_geometry.y() >= 0 )
  {
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "KPlayer::showEvent: adjusting position to " << m_normal_geometry.x() << "x" << m_normal_geometry.y() << "\n";
#endif
    move (m_normal_geometry.x(), m_normal_geometry.y());
  }
  if ( width() != m_normal_geometry.width() && m_normal_geometry.width() >= minimumWidth()
    && height() != m_normal_geometry.height() && m_normal_geometry.height() >= minimumHeight() )
  {
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "KPlayer::showEvent: adjusting size to " << m_normal_geometry.width() << "x" << m_normal_geometry.height() << "\n";
#endif
    resize (m_normal_geometry.width(), m_normal_geometry.height());
  }
/*if ( focusProxy() )
    KPlayerX11SetInputFocus (focusProxy() -> winId());
#ifdef DEBUG_KPLAYER_WINDOW
  else
    kdDebugTime() << "KPlayer::showEvent: no focus proxy\n";
#endif*/
}

void KPlayer::windowActivationChange (bool old)
{
  KMainWindow::windowActivationChange (old);
  bool active = isActiveWindow();
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Main window activation " << old << " -> " << active << "\n";
#endif
/*if ( active && focusProxy() )
    KPlayerX11SetInputFocus (focusProxy() -> winId());
#ifdef DEBUG_KPLAYER_WINDOW
  else if ( active )
    kdDebug() << "  no focus proxy\n";
#endif*/
}

void KPlayer::focusInEvent (QFocusEvent* event)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Window focus in event\n";
#endif
  KMainWindow::focusInEvent (event);
}

void KPlayer::focusOutEvent (QFocusEvent* event)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Window focus out event\n";
#endif
  KMainWindow::focusOutEvent (event);
}

void KPlayer::moveEvent (QMoveEvent* event)
{
  bool maximized = isMaximized();
  KMainWindow::moveEvent (event);
  if ( ! m_full_screen && ! kPlayerSettings() -> maximized() && ! isMaximized() && m_initial_show )
    m_normal_geometry.setRect (x(), y(), width(), height());
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "WiMove " << event -> oldPos().x() << "x" << event -> oldPos().y()
    << " => " << event -> pos().x() << "x" << event -> pos().y() << ", "
    << maximized << " -> " << isMaximized() << "\n";
  kdDebug() << "             Normal geometry " << m_normal_geometry.x() << "x" << m_normal_geometry.y()
    << " " << m_normal_geometry.width() << "x" << m_normal_geometry.height() << "\n";
#endif
}

void KPlayer::resizeEvent (QResizeEvent* event)
{
  bool maximized = isMaximized();
  KMainWindow::resizeEvent (event);
  if ( ! m_full_screen && ! kPlayerSettings() -> maximized() && ! isMaximized() && m_initial_show )
    m_normal_geometry.setSize (QSize (width(), height()));
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "WiSize " << event -> oldSize(). width() << "x" << event -> oldSize(). height()
    << " => " << event -> size(). width() << "x" << event -> size(). height() << ", "
    << maximized << " -> " << isMaximized() << "\n";
  kdDebug() << "             Normal geometry " << m_normal_geometry.x() << "x" << m_normal_geometry.y()
    << " " << m_normal_geometry.width() << "x" << m_normal_geometry.height() << "\n";
#endif
}

void KPlayer::setMinimumSize (int w, int h)
{
  QSize prev (size()), msh (minimumSizeHint());
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Set minimum size " << w << "x" << h << " / " << msh.width() << "x" << msh.height() << "\n";
#endif
  w = msh.width();
  h = msh.height();
  KMainWindow::setMinimumSize (w, h);
#ifdef DEBUG_KPLAYER_RESIZING
  dumpObject (this, 0, 1);
  if ( prev != size() )
    kdDebug() << "             Size changed\n";
#endif
  if ( prev != size() && m_previous_size == prev )
    m_previous_size = size();
}

/*QSize KPlayer::sizeHint (void) const
{
  return minimumSizeHint() + kPlayerWidget() -> videoSize();
}*/

QSize KPlayer::minimumSizeHint (void) const
{
  KPlayer* that = (KPlayer*) this;
  QSize size (kPlayerSettings() -> minimumSliderLength(), 0);
  if ( ! that -> menuBar() -> isHidden() )
    size.setHeight (that -> menuBar() -> sizeHint().height());
  size.setHeight (size.height() + topDock() -> height());
  size.setHeight (size.height() + bottomDock() -> height());
  size.setWidth (size.width() + leftDock() -> width());
  size.setWidth (size.width() + rightDock() -> width());
  if ( ! that -> statusBar() -> isHidden() )
  {
    int x = that -> statusBar() -> minimumSizeHint().width();
    if ( x > size.width() )
      size.setWidth (x);
    size.setHeight (size.height() + that -> statusBar() -> minimumHeight());
  }
#ifdef DEBUG_KPLAYER_WINDOW
  //kdDebugTime() << "Minimum size hint " << size.width() << "x" << size.height() << "\n";
#endif
  return size.boundedTo (QApplication::desktop() -> availableGeometry().size());
}

void KPlayer::toFullScreen (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::toFullScreen\n";
#endif
  m_full_screen = true;
  bool active = isActiveWindow();
  if ( isMaximized() )
    m_maximized = true;
  else // if ( ! isMaximized() )
    m_normal_geometry.setRect (x(), y(), width(), height());
  setAcceptDrops (false);
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "Pre full screen: " << geometry().x() << "x" << geometry().y() << " " << width() << "x" << height() << "\n";
  kdDebug() << "                              " << frameGeometry().x() << "x" << frameGeometry().y() << " " << frameGeometry().width() << "x" << frameGeometry().height() << "\n";
  kdDebug() << "                              " << m_normal_geometry.x() << "x" << m_normal_geometry.y() << " " << m_normal_geometry.width() << "x" << m_normal_geometry.height() << "\n";
#endif
  showMenubar();
  showStatusbar();
  showMessageLog();
  for ( int i = 0; i < KPLAYER_TOOLBARS; i ++ )
    showToolbar (i);
  showPlaylistEditor();
  showFullScreen();
//syncronizeEvents();
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "Full screen: " << geometry().x() << "x" << geometry().y() << " " << width() << "x" << height() << "\n";
  kdDebug() << "                          " << frameGeometry().x() << "x" << frameGeometry().y() << " " << frameGeometry().width() << "x" << frameGeometry().height() << "\n";
#endif
  layout() -> setSpacing (-1);
  activateLayout();
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "Full screen: " << geometry().x() << "x" << geometry().y() << " " << width() << "x" << height() << "\n";
  kdDebug() << "                          " << frameGeometry().x() << "x" << frameGeometry().y() << " " << frameGeometry().width() << "x" << frameGeometry().height() << "\n";
  dumpObject (this, 0);
#endif
  if ( active )
  {
    setActiveWindow();
    KWin::setActiveWindow (winId());
  }
  setAcceptDrops (true);
  enablePlayerActions();
}

void KPlayer::toNormalScreen (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::toNormalScreen\n";
  dumpObject (this, 0);
#endif
  bool active = isActiveWindow();
  layout() -> setSpacing (0);
  setAcceptDrops (false);
  showMenubar();
  showStatusbar();
  showMessageLog();
  for ( int i = 0; i < KPLAYER_TOOLBARS; i ++ )
    showToolbar (i);
  showPlaylistEditor();
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "Full screen: " << geometry().x() << "x" << geometry().y() << " " << width() << "x" << height() << "\n";
  kdDebug() << "                          " << frameGeometry().x() << "x" << frameGeometry().y() << " " << frameGeometry().width() << "x" << frameGeometry().height() << "\n";
#endif
  if ( ! m_maximized )
  {
    //move (m_normal_geometry.x(), m_normal_geometry.y());
    //resize (m_normal_geometry.width(), m_normal_geometry.height());
    setGeometry (m_normal_geometry);
    activateLayout();
    //m_initial_show = false;
  }
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "Full screen: " << geometry().x() << "x" << geometry().y() << " " << width() << "x" << height() << "\n";
  kdDebug() << "                          " << frameGeometry().x() << "x" << frameGeometry().y() << " " << frameGeometry().width() << "x" << frameGeometry().height() << "\n";
#endif
  showNormal();
  /*if ( ! m_maximized )
  {
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "  Using normal geometry\n";
#endif
    move (m_normal_geometry.x(), m_normal_geometry.y());
    resize (m_normal_geometry.width(), m_normal_geometry.height());
  }*/
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "Normal: " << geometry().x() << "x" << geometry().y() << " " << width() << "x" << height() << "\n";
  kdDebug() << "                     " << frameGeometry().x() << "x" << frameGeometry().y() << " " << frameGeometry().width() << "x" << frameGeometry().height() << "\n";
#endif
  if ( active )
  {
    setActiveWindow();
    KWin::setActiveWindow (winId());
  }
  setAcceptDrops (true);
  m_full_screen = false;
  enablePlayerActions();
}

void KPlayer::initialSize (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::initialSize (" << isMaximized() << ")\n";
#endif
  KPlayerSettings* settings = kPlayerSettings();
  QSize size (settings -> displaySize());
  if ( settings -> constrainedSize() || settings -> hasVideo() || ! size.isEmpty() )
    return;
  for ( int i = 0; i <= VOLUME_TOOLBAR; i ++ )
    if ( i != EDITOR_TOOLBAR && toolBar (m_toolbar[i].name) -> orientation() == Qt::Vertical && m_toolbar[i].normally_visible )
//      && toggleAction (m_toolbar[i].action) -> isChecked() )
    {
      size.setHeight (kPlayerWorkspace() -> height());
      break;
    }
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebug() << "             " << m_log -> isHidden() << " " << m_log -> place() << " " << m_playlist -> isHidden() << " " << m_playlist -> place() << "\n";
#endif
  if ( m_playlist -> isShown() && m_playlist -> place() == QDockWindow::InDock
      || m_log -> isShown() && m_log -> place() == QDockWindow::InDock )
    size.setWidth (kPlayerWorkspace() -> width());
  settings -> setDisplaySize (size);
}

void KPlayer::correctSize (void)
{
  //syncronizeEvents();
  bool maximized = isMaximized();
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::correctSize (" << maximized << ")\n";
#endif
  KPlayerSettings* settings = kPlayerSettings();
  if ( settings -> fullScreen() )
    layout() -> setSpacing (-1);
  else if ( ! maximized && settings -> maximized() )
  {
    if ( m_maximized )
    {
#ifdef DEBUG_KPLAYER_WINDOW
      kdDebug() << "             Using normal geometry\n";
#endif
      //syncronizeEvents();
      move (m_normal_geometry.x(), m_normal_geometry.y());
      resize (m_normal_geometry.width(), m_normal_geometry.height());
      m_maximized = false;
      activateLayout();
    }
#ifdef DEBUG_KPLAYER_WINDOW
    kdDebugTime() << "KPlayer::correctSize: setMaximized (false)\n";
#endif
    settings -> setMaximized (maximized);
  }
  if ( settings -> constrainedSize() )
  {
    if ( maximized && ! settings -> maximized() )
    {
#ifdef DEBUG_KPLAYER_WINDOW
      kdDebugTime() << "KPlayer::correctSize: setMaximized (true)\n";
#endif
      settings -> setMaximized (maximized);
    }
    return;
  }
  if ( m_previous_size != size() )
  {
    if ( maximized )
    {
#ifdef DEBUG_KPLAYER_WINDOW
      kdDebugTime() << "KPlayer::correctSize: setMaximized (true)\n";
#endif
      settings -> setMaximized (maximized);
    }
    else
      settings -> setDisplaySize (settings -> adjustSize (kPlayerWorkspace() -> size(), m_previous_size.width() == width()));
  }
#ifdef DEBUG_KPLAYER_WINDOW
  else
    kdDebugTime() << "             Using previous size\n";
#endif
}

void KPlayer::syncronize (bool user_resize)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::syncronize (" << user_resize << ")\n";
#endif
  if ( kPlayerSettings() -> fullScreen() )
  {
    if ( ! m_full_screen && ! user_resize )
      toFullScreen();
    return;
  }
  if ( m_full_screen )
  {
    if ( user_resize )
      return;
    else
      toNormalScreen();
  }
  if ( kPlayerSettings() -> maximized() )
  {
    if ( ! isMaximized() )
      showMaximized();
    return;
  }
  if ( isMaximized() )
    showNormal();
}

//extern uint qGlobalPostedEventsCount (void);

/*void KPlayer::syncronizeEvents (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::syncronizeEvents\n";
#endif
  QApplication::syncX();
  bool rv = QApplication::eventLoop() -> processEvents (QEventLoop::ExcludeUserInput | QEventLoop::ExcludeSocketNotifiers);
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::syncronizeEvents: processEvents returned " << rv << "\n";
  //kdDebugTime() << "KPlayer::syncronizeEvents: posted event count " << qGlobalPostedEventsCount() << "\n";
#endif
  //QApplication::sendPostedEvents();
  //QApplication::sendPostedEvents (this, QEvent::Resize);
  //QApplication::sendPostedEvents (this, QEvent::Move);
  //QApplication::sendPostedEvents (this, QEvent::LayoutHint);
  //QApplication::sendPostedEvents (kPlayerWorkspace(), QEvent::Resize);
#ifdef DEBUG_KPLAYER_WINDOW
  //kdDebugTime() << "KPlayer::syncronizeEvents: posted event count " << qGlobalPostedEventsCount() << "\n";
  //kdDebugTime() << "KPlayer::syncronizeEvents done\n";
#endif
}*/

void KPlayer::zoom (void)
{
  if ( m_full_screen )
    return;
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::zoom\n";
#endif
  QSize msh (minimumSizeHint());
  KMainWindow::setMinimumSize (msh.width(), msh.height());
  do_zoom();
  Qt::Dock dock;
  int index, offset;
  bool newline;
  if ( ! m_log -> isHidden() && getLocation (m_log, dock, index, newline, offset)
    && ((dock == Qt::DockLeft || dock == Qt::DockRight)
      && frameGeometry().width() > QApplication::desktop() -> availableGeometry().width()
    || (dock == Qt::DockTop || dock == Qt::DockBottom)
      && frameGeometry().height() >= QApplication::desktop() -> availableGeometry().height()) )
      //&& frameGeometry().height() > QApplication::desktop() -> availableGeometry().height()) )
  {
#ifdef DEBUG_KPLAYER_RESIZING
    kdDebugTime() << "Log dock " << dock << " " << index << " " << newline << " " << offset << " " << m_log -> x() << "x" << m_log -> y() << " " << m_log -> width() << "x" << m_log -> height() << "\n";
    kdDebugTime() << "Minimum log size " << m_log -> minimumWidth() << "x" << m_log -> minimumHeight() << "\n";
#endif
    if ( (dock == Qt::DockLeft || dock == Qt::DockRight) && m_log -> width() - m_log -> minimumWidth()
      >= frameGeometry().width() - QApplication::desktop() -> availableGeometry().width() )
    {
#ifdef DEBUG_KPLAYER_RESIZING
      kdDebugTime() << "New log width " << (m_log -> width() - frameGeometry().width() + QApplication::desktop() -> availableGeometry().width()) << "\n";
#endif
      m_log -> setFixedExtentWidth (m_log -> width() - frameGeometry().width() + QApplication::desktop() -> availableGeometry().width());
    }
    else if ( (dock == Qt::DockTop || dock == Qt::DockBottom) && m_log -> height() - m_log -> minimumHeight()
      > frameGeometry().height() - QApplication::desktop() -> availableGeometry().height() )
      //>= frameGeometry().height() - QApplication::desktop() -> availableGeometry().height() )
    {
#ifdef DEBUG_KPLAYER_RESIZING
      kdDebugTime() << "New log height " << (m_log -> height() - frameGeometry().height() + QApplication::desktop() -> availableGeometry().height() - 1) << "\n";
#endif
      m_log -> setFixedExtentHeight (m_log -> height() - frameGeometry().height() + QApplication::desktop() -> availableGeometry().height() - 1);
      //m_log -> setFixedExtentHeight (m_log -> height() - frameGeometry().height() + QApplication::desktop() -> availableGeometry().height());
    }
    else if ( m_show_log )
      m_log -> undock();
    else
      m_log -> hide();
    do_zoom();
  }
  if ( ! m_playlist -> isHidden() && getLocation (m_playlist, dock, index, newline, offset)
    && ((dock == Qt::DockLeft || dock == Qt::DockRight)
      && frameGeometry().width() > QApplication::desktop() -> availableGeometry().width()
    || (dock == Qt::DockTop || dock == Qt::DockBottom)
      && frameGeometry().height() >= QApplication::desktop() -> availableGeometry().height()) )
      //&& frameGeometry().height() > QApplication::desktop() -> availableGeometry().height()) )
  {
#ifdef DEBUG_KPLAYER_RESIZING
    kdDebugTime() << "Playlist dock " << dock << " " << index << " " << newline << " " << offset << " " << m_playlist -> x() << "x" << m_playlist -> y() << " " << m_playlist -> width() << "x" << m_playlist -> height() << "\n";
    kdDebugTime() << "Minimum playlist size " << m_playlist -> minimumWidth() << "x" << m_playlist -> minimumHeight() << "\n";
#endif
    if ( (dock == Qt::DockLeft || dock == Qt::DockRight) && m_playlist -> width() - m_playlist -> minimumWidth()
      >= frameGeometry().width() - QApplication::desktop() -> availableGeometry().width() )
    {
#ifdef DEBUG_KPLAYER_RESIZING
      kdDebugTime() << "New playlist width " << (m_playlist -> width() - frameGeometry().width() + QApplication::desktop() -> availableGeometry().width()) << "\n";
#endif
      m_playlist -> setFixedExtentWidth (m_playlist -> width() - frameGeometry().width() + QApplication::desktop() -> availableGeometry().width());
    }
    else if ( (dock == Qt::DockTop || dock == Qt::DockBottom) && m_playlist -> height() - m_playlist -> minimumHeight()
      > frameGeometry().height() - QApplication::desktop() -> availableGeometry().height() )
      //>= frameGeometry().height() - QApplication::desktop() -> availableGeometry().height() )
    {
#ifdef DEBUG_KPLAYER_RESIZING
      kdDebugTime() << "New playlist height " << (m_playlist -> height() - frameGeometry().height() + QApplication::desktop() -> availableGeometry().height() - 1) << "\n";
#endif
      m_playlist -> setFixedExtentHeight (m_playlist -> height() - frameGeometry().height() + QApplication::desktop() -> availableGeometry().height() - 1);
      //m_playlist -> setFixedExtentHeight (m_playlist -> height() - frameGeometry().height() + QApplication::desktop() -> availableGeometry().height());
    }
    else if ( m_show_playlist )
      m_playlist -> undock();
    else
      m_playlist -> hide();
    do_zoom();
  }
  m_show_log = m_show_playlist = false;
  int xc = frameGeometry().right() - QApplication::desktop() -> availableGeometry().right();
  int yc = frameGeometry().bottom() - QApplication::desktop() -> availableGeometry().bottom();
  if ( xc > 0 || yc > 0 )
  {
    if ( xc <= 0 )
      xc = x();
    else if ( QApplication::desktop() -> availableGeometry().width() > frameGeometry().width() )
      xc = QApplication::desktop() -> availableGeometry().right() - frameGeometry().width();
    else
      xc = QApplication::desktop() -> availableGeometry().left();
    if ( yc <= 0 )
      yc = y();
    else if ( QApplication::desktop() -> availableGeometry().height() > frameGeometry().height() )
      yc = QApplication::desktop() -> availableGeometry().bottom() - frameGeometry().height();
    else
      yc = QApplication::desktop() -> availableGeometry().top();
#ifdef DEBUG_KPLAYER_RESIZING
    kdDebugTime() << "Moving to " << xc << "x" << yc << "\n";
#endif
    move (xc, yc);
  }
  m_previous_size = size();
  activateLayout();
//KPlayerX11SendConfigureEvent (winId(), geometry().x(), geometry().y(), width(), height());
//syncronizeEvents();
//layout() -> invalidate();
//QApplication::postEvent (this, new QResizeEvent (size(), size()));
//KPlayerX11SendConfigureEvent (winId(), geometry().x(), geometry().y(), width(), height());
  QApplication::syncX();
//KPlayerX11DiscardConfigureEvents (winId());
  KPlayerX11DiscardConfigureEvents (kPlayerWorkspace() -> winId());
//syncronizeEvents();
  QApplication::postEvent (this, new QEvent (QEvent::LayoutHint));
//checkBarActions();
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "Zoom done\n";
#endif
}

void KPlayer::activateLayout (void)
{
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::activateLayout\n";
#endif
  layout() -> activate();
  QApplication::sendPostedEvents (this, QEvent::Resize);
  QApplication::sendPostedEvents (this, QEvent::Move);
  QApplication::sendPostedEvents (this, QEvent::LayoutHint);
  QApplication::sendPostedEvents (kPlayerWorkspace(), QEvent::Resize);
#ifdef DEBUG_KPLAYER_WINDOW
  kdDebugTime() << "KPlayer::activateLayout done\n";
#endif
}

void KPlayer::do_zoom (void)
{
  //if ( isMaximized() )
  //  showNormal();
  activateLayout();
  QSize video (kPlayerSettings() -> displaySize().expandedTo (QSize(1, 1)));
  QSize cwsize (centralWidget() -> size());
  QSize target ((size() + video - cwsize).expandedTo (minimumSizeHint()));
#ifdef DEBUG_KPLAYER_RESIZING
  kdDebugTime() << "Display " << video.width() << "x" << video.height() << "\n";
  kdDebug() << "             Window  " << geometry().x() << "x" << geometry().y() << " " << width() << "x" << height() << "\n";
  kdDebug() << "             WFrame  " << x() << "x" << y() << " " << frameGeometry().width() << "x" << frameGeometry().height() << "\n";
  kdDebug() << "             WSpace  " << cwsize.width() << "x" << cwsize.height() << "\n";
  kdDebug() << "             Widget  " << kPlayerWidget() -> size().width() << "x" << kPlayerWidget() -> size().height() << "\n";
  kdDebug() << "             Target  " << target.width() << "x" << target.height() << "\n";
/*  << "\n Total  " << minimumSize().width() << "x" << minimumSize().height()
    << " " << minimumSizeHint().width() << "x" << minimumSizeHint().height()
    << " " << sizeHint().width() << "x" << sizeHint().height()
    << "\n Top    " << topDock() -> minimumSize().width() << "x" << topDock() -> minimumSize().height()
    << " " << topDock() -> minimumSizeHint().width() << "x" << topDock() -> minimumSizeHint().height()
    << " " << topDock() -> sizeHint().width() << "x" << topDock() -> sizeHint().height()
    << "\n Bottom " << bottomDock() -> minimumSize().width() << "x" << bottomDock() -> minimumSize().height()
    << " " << bottomDock() -> minimumSizeHint().width() << "x" << bottomDock() -> minimumSizeHint().height()
    << " " << bottomDock() -> sizeHint().width() << "x" << bottomDock() -> sizeHint().height()
    << "\n Layout " << layout() -> minimumSize().width() << "x" << layout() -> minimumSize().height()
    << " " << layout() -> sizeHint().width() << "x" << layout() -> sizeHint().height()
    << "\n        " << layout() -> minimumSize().width() << "x" << layout() -> minimumSize().height()
    << " " << layout() -> sizeHint().width() << "x" << layout() -> sizeHint().height() << "\n"; */
#endif
  int i = 0;
  while ( target != size() && i ++ < 5 )
  {
    resize (target);
    //if ( isMaximized() )
    //  showNormal();
    activateLayout();
    cwsize = centralWidget() -> size();
    target = (size() + video - cwsize).expandedTo (minimumSizeHint());
#ifdef DEBUG_KPLAYER_RESIZING
    kdDebugTime() << "Window  " << width() << "x" << height() << "\n";
    kdDebug() << "             WFrame  " << frameGeometry().width() << "x" << frameGeometry().height() << "\n";
    kdDebug() << "             WSpace  " << cwsize.width() << "x" << cwsize.height() << "\n";
    kdDebug() << "             Widget  " << kPlayerWidget() -> size().width() << "x" << kPlayerWidget() -> size().height() << "\n";
    kdDebug() << "             Target  " << target.width() << "x" << target.height() << "\n";
/*    << " Total  " << minimumSize().width() << "x" << minimumSize().height()
      << " " << minimumSizeHint().width() << "x" << minimumSizeHint().height()
      << " " << sizeHint().width() << "x" << sizeHint().height()
      << "\n Top    " << topDock() -> minimumSize().width() << "x" << topDock() -> minimumSize().height()
      << " " << topDock() -> minimumSizeHint().width() << "x" << topDock() -> minimumSizeHint().height()
      << " " << topDock() -> sizeHint().width() << "x" << topDock() -> sizeHint().height()
      << "\n Bottom " << bottomDock() -> minimumSize().width() << "x" << bottomDock() -> minimumSize().height()
      << " " << bottomDock() -> minimumSizeHint().width() << "x" << bottomDock() -> minimumSizeHint().height()
      << " " << bottomDock() -> sizeHint().width() << "x" << bottomDock() -> sizeHint().height()
      << "\n Layout " << layout() -> minimumSize().width() << "x" << layout() -> minimumSize().height()
      << " " << layout() -> sizeHint().width() << "x" << layout() -> sizeHint().height()
      << "\n        " << layout() -> minimumSize().width() << "x" << layout() -> minimumSize().height()
      << " " << layout() -> sizeHint().width() << "x" << layout() -> sizeHint().height() << "\n"; */
#endif
  }
#ifdef DEBUG_KPLAYER_RESIZING
  if ( target != size() )
    kdDebugTime() << "Zoom unsuccessful\n";
  kdDebugTime() << "Desktop " << QApplication::desktop() -> width() << "x" << QApplication::desktop() -> height() << "\n";
  kdDebugTime() << "ScreenG " << QApplication::desktop() -> screenGeometry().x()
    << "x" << QApplication::desktop() -> screenGeometry().y()
    << " " << QApplication::desktop() -> screenGeometry().width()
    << "x" << QApplication::desktop() -> screenGeometry().height() << "\n";
  kdDebugTime() << "ScreenG " << QApplication::desktop() -> availableGeometry().x()
    << "x" << QApplication::desktop() -> availableGeometry().y()
    << " " << QApplication::desktop() -> availableGeometry().width()
    << "x" << QApplication::desktop() -> availableGeometry().height() << "\n";
#endif
}

/*void KPlayer::barOrientationChanged (Orientation)
{
  kdDebugTime() << "A bar orientation has changed.\n";
  kdDebugTime() << "Central Widget Size: " << centralWidget() -> width() << "x" << centralWidget() -> height()
    << ", KPlayer Widget Size: " << kPlayerWidget() -> width() << "x" << kPlayerWidget() -> height() << ".\n";
}

void KPlayer::barPlaceChanged (QDockWindow::Place)
{
  kdDebugTime() << "A bar place has changed.\n";
  kdDebugTime() << "Central Widget Size: " << centralWidget() -> width() << "x" << centralWidget() -> height()
    << ", KPlayer Widget Size: " << kPlayerWidget() -> width() << "x" << kPlayerWidget() -> height() << ".\n";
}

void KPlayer::barVisibilityChanged (bool)
{
  kdDebugTime() << "A bar visibility has changed.\n";
  kdDebugTime() << "Central Widget Size: " << centralWidget() -> width() << "x" << centralWidget() -> height()
    << ", KPlayer Widget Size: " << kPlayerWidget() -> width() << "x" << kPlayerWidget() -> height() << ".\n";
}*/