/***************************************************************************
                          x11.cpp
                          -------
    begin                : Wed Feb 26 2003
    copyright            : (C) 2003-2004 by kiriuja
    email                : kplayer dash developer at en dash directo dot net
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <X11/Xlib.h>
#include <kdebug.h>

//#define DEBUG_KPLAYER_X11
//#define DEBUG_KPLAYER_FOCUS
//#define DEBUG_KPLAYER_KEY
//#define DEBUG_KPLAYER_RESIZE

#ifdef NDEBUG
#define kdDebugTime kndDebug
#else
kdbgstream kdDebugTime (void);
#endif

/*bool KPlayerX11TestGrab (Display* display, int winid)
{
  if ( int status = XGrabPointer (display, winid, False,
        (uint)(ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask),
        GrabModeAsync, GrabModeAsync, None, None, CurrentTime) )
  {
    const char *s =
      status == GrabNotViewable ? "GrabNotViewable" :
      status == AlreadyGrabbed  ? "AlreadyGrabbed" :
      status == GrabFrozen      ? "GrabFrozen" :
      status == GrabInvalidTime ? "GrabInvalidTime" : "Unknown";
    kdDebugTime() << "Grab failed, status: " << s << "\n";
    return false;
  }
  kdDebugTime() << "Grab succeeded\n";
  XUngrabPointer (display, CurrentTime);
  return true;
}*/

extern void KPlayerWidgetResizeHandler (bool);
extern void KPlayerWidgetMapHandler (uint);
extern void KPlayerWidgetUnmapHandler (uint);

typedef int (*QX11EventFilter) (XEvent*);

static QX11EventFilter PreviousX11EventFilter = 0;

#ifdef DEBUG_KPLAYER_RESIZE

const char* KPlayerX11EventTypeNames [LASTEvent] = {
  "EVENT0",
  "EVENT1",
  "KeyPress",
  "KeyRelease",
  "ButtonPress",
  "ButtonRelease",
  "MotionNotify",
  "EnterNotify",
  "LeaveNotify",
  "FocusIn",
  "FocusOut",
  "KeymapNotify",
  "Expose",
  "GraphicsExpose",
  "NoExpose",
  "VisibilityNotify",
  "CreateNotify",
  "DestroyNotify",
  "UnmapNotify",
  "MapNotify",
  "MapRequest",
  "ReparentNotify",
  "ConfigureNotify",
  "ConfigureRequest",
  "GravityNotify",
  "ResizeRequest",
  "CirculateNotify",
  "CirculateRequest",
  "PropertyNotify",
  "SelectionClear",
  "SelectionRequest",
  "SelectionNotify",
  "ColormapNotify",
  "ClientMessage",
  "MappingNotify"
};

const char* KPlayerX11EventModeNames [4] = {
  "NotifyNormal",
  "NotifyGrab",
  "NotifyUngrab",
  "NotifyWhileGrabbed"
};

const char* KPlayerX11EventDetailNames [8] = {
  "NotifyAncestor",
  "NotifyVirtual",
  "NotifyInferior",
  "NotifyNonlinear",
  "NotifyNonlinearVirtual",
  "NotifyPointer",
  "NotifyPointerRoot",
  "NotifyDetailNone"
};

#endif

int KPlayerX11EventFilter (XEvent* event)
{
#ifdef DEBUG_KPLAYER_X11
  XAnyEvent* anyev = (XAnyEvent*) event;
  kdDebugTime() << "X11 event " << KPlayerX11EventTypeNames [event -> type] << " " << anyev -> window
    << " " << anyev -> send_event << " " << anyev -> serial << "\n";
#endif
  if ( event -> type == FocusIn || event -> type == FocusOut )
  {
    XFocusChangeEvent* ev = (XFocusChangeEvent*) event;
#ifdef DEBUG_KPLAYER_FOCUS
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type] << " " << ev -> window << " " 
      << ev -> send_event << " " << ev -> serial << " mode " << KPlayerX11EventModeNames [ev -> mode]
      << " detail " << KPlayerX11EventDetailNames [ev -> detail] << "\n";
#endif
    if ( event -> type == FocusIn && ev -> mode == NotifyUngrab
        || event -> type == FocusOut && ev -> mode == NotifyGrab && ev -> detail == NotifyAncestor )
    {
      kdDebugTime() << "Calling KPlayerWidgetResizeHandler (" << (ev -> mode == NotifyGrab) << ")\n";
      KPlayerWidgetResizeHandler (ev -> mode == NotifyGrab);
    }
  }
#ifdef DEBUG_KPLAYER_KEY
  else if ( event -> type == KeyPress || event -> type == KeyRelease )
  {
    XKeyEvent* ev = (XKeyEvent*) event;
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type] << " " << ev -> window << " " 
      << ev -> send_event << " " << ev -> serial << " root " << ev -> root << " subwindow "
      << ev -> subwindow << " " << ev -> x << "x" << ev -> y << " " << ev -> x_root << "x" << ev -> y_root
      << " keycode " << ev -> keycode << " state " << ev -> state << " same " << ev -> same_screen << "\n";
  }
#endif
#ifdef DEBUG_KPLAYER_RESIZE
  else if ( event -> type == ConfigureNotify )
  {
    XConfigureEvent* ev = (XConfigureEvent*) event;
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type] << " " << ev -> event
      << " " << ev -> window << " " << ev -> send_event << " " << ev -> serial
      << " " << ev -> x << "x" << ev -> y << " " << ev -> width << "x" << ev -> height
      << " border " << ev -> border_width << " above " << ev -> above << " override " << ev -> override_redirect << "\n";
  }
  else if ( event -> type == ConfigureRequest )
  {
    XConfigureRequestEvent* ev = (XConfigureRequestEvent*) event;
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type] << " " << ev -> parent
      << " " << ev -> window << " " << ev -> send_event << " " << ev -> serial
      << " " << ev -> x << "x" << ev -> y << " " << ev -> width << "x" << ev -> height
      << " border " << ev -> border_width << " above " << ev -> above
      << " detail " << ev -> detail << " mask " << ev -> value_mask << "\n";
  }
  else if ( event -> type == ResizeRequest )
  {
    XResizeRequestEvent* ev = (XResizeRequestEvent*) event;
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type]
      << " " << ev -> window << " " << ev -> send_event << " " << ev -> serial
      << " " << ev -> width << "x" << ev -> height << "\n";
  }
  else if ( event -> type == ReparentNotify )
  {
    XReparentEvent* ev = (XReparentEvent*) event;
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type] << " " << ev -> event
      << " " << ev -> window << " " << ev -> parent << " " << ev -> send_event << " " << ev -> serial
      << " " << ev -> x << "x" << ev -> y << " override " << ev -> override_redirect << "\n";
  }
  else if ( event -> type == MapNotify )
  {
    XMapEvent* ev = (XMapEvent*) event;
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type] << " " << ev -> event << " " << ev -> window
      << " " << ev -> send_event << " " << ev -> serial << " override " << ev -> override_redirect << "\n";
  }
#endif
  else if ( event -> type == MapRequest )
  {
    XMapRequestEvent* ev = (XMapRequestEvent*) event;
#ifdef DEBUG_KPLAYER_RESIZE
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type] << " " << ev -> parent
      << " " << ev -> window << " " << ev -> send_event << " " << ev -> serial << "\n";
#endif
    KPlayerWidgetMapHandler (ev -> window);
  }
  else if ( event -> type == UnmapNotify )
  {
    XUnmapEvent* ev = (XUnmapEvent*) event;
#ifdef DEBUG_KPLAYER_RESIZE
    kdDebugTime() << "X11 " << KPlayerX11EventTypeNames [event -> type] << " " << ev -> event << " " << ev -> window
      << " " << ev -> send_event << " " << ev -> serial << " from configure " << ev -> from_configure << "\n";
#endif
    KPlayerWidgetUnmapHandler (ev -> window);
  }
#ifdef DEBUG_KPLAYER_X11
  else if ( event -> type == PropertyNotify )
  {
    XPropertyEvent* ev = (XPropertyEvent*) event;
    kdDebugTime() << "  Property " << ev -> atom << " " << ev -> time << " " << ev -> state << "\n";
  }
#endif
  if ( PreviousX11EventFilter )
    return PreviousX11EventFilter (event);
  return 0;
}

Display* qt_xdisplay (void);
extern Time qt_x_time;

void KPlayerX11SetInputFocus (uint id)
{
  XSetInputFocus (qt_xdisplay(), id, RevertToNone, qt_x_time);
  XFlush (qt_xdisplay());
}

void KPlayerX11MapWindow (uint id)
{
  XMapWindow (qt_xdisplay(), id);
  XFlush (qt_xdisplay());
}

void KPlayerX11UnmapWindow (uint id)
{
  XUnmapWindow (qt_xdisplay(), id);
  XFlush (qt_xdisplay());
}

void KPlayerX11ClearExposeWindow (uint id)
{
  XClearArea (qt_xdisplay(), id, 0, 0, 0, 0, True);
  XFlush (qt_xdisplay());
}

void KPlayerX11SendConfigureEvent (uint id, int w, int h)
{
#ifdef DEBUG_KPLAYER_RESIZE
  kdDebugTime() << "KPlayerX11SendConfigureEvent " << id << " " << w << "x" << h << "\n";
#endif
  XConfigureEvent event = { ConfigureNotify, 0, True, qt_xdisplay(), id, id, 0, 0, w, h, 0, None, False };
  XSendEvent (qt_xdisplay(), event.event, True, StructureNotifyMask, (XEvent*) &event);
  XFlush (qt_xdisplay());
}

void KPlayerX11SendConfigureEvent (uint id, int x, int y, int w, int h)
{
#ifdef DEBUG_KPLAYER_RESIZE
  kdDebugTime() << "KPlayerX11SendConfigureEvent " << id << " " << x << "x" << y << " " << w << "x" << h << "\n";
#endif
  XConfigureEvent event = { ConfigureNotify, 0, True, qt_xdisplay(), id, id, x, y, w, h, 0, None, False };
  XSendEvent (qt_xdisplay(), event.event, True, StructureNotifyMask, (XEvent*) &event);
  XFlush (qt_xdisplay());
}

void KPlayerX11DiscardConfigureEvents (uint id)
{
  XEvent event;
  while ( XCheckTypedWindowEvent (qt_xdisplay(), id, ConfigureNotify, &event) )
#ifdef DEBUG_KPLAYER_RESIZE
    kdDebugTime() << "Discarded ConfigureEvent " << event.xconfigure.event << " " << event.xconfigure.window
      << " " << event.xconfigure.send_event << " " << event.xconfigure.serial
      << " " << event.xconfigure.x << "x" << event.xconfigure.y
      << " " << event.xconfigure.width << "x" << event.xconfigure.height
      << " border " << event.xconfigure.border_width << " above " << event.xconfigure.above
      << " override " << event.xconfigure.override_redirect << "\n"
#endif
  ;
}

extern QX11EventFilter qt_set_x11_event_filter (QX11EventFilter);

static int KPlayerX11EventFilterCount = 0;

void KPlayerSetX11EventFilter (void)
{
  if ( ! KPlayerX11EventFilterCount ++ )
    PreviousX11EventFilter = qt_set_x11_event_filter (KPlayerX11EventFilter);
}

void KPlayerResetX11EventFilter (void)
{
  if ( -- KPlayerX11EventFilterCount )
    return;
  qt_set_x11_event_filter (PreviousX11EventFilter);
  PreviousX11EventFilter = 0;
}
