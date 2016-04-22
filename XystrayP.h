#ifndef _XWINTRAYP_H
#define _XWINTRAYP_H

#include "Xystray.h"
#include <X11/Xos.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

typedef struct {
	Atom selection_atom;	/* _NET_SYSTEM_TRAY_S0 */
	Atom opcode_atom;	/* _NET_SYSTEM_TRAY_OPCODE */
	Atom data_atom;		/* _NET_SYSTEM_TRAY_MESSAGE_DATA */
	Atom xembed_atom;	/* _XEMBED */
	Atom xembedi_atom;	/* _XEMBED_INFO */
	Window comm;		/* XGetSelection */
	Window* iconwindows;	/* tablica ikon, alokowana w initialize */
	Dimension iconsfree;	/* ilo¶æ dostêpnych miejsc */
	Dimension iconmax;	/* ilo¶æ */
	Dimension iconrows;	/* rzêdy */
	Dimension iconcols;	/* kolumny */
	Dimension iconpadding;	/* odstêpy */
	Dimension iconsize;	/* rozmiary */
	Boolean vertical;	/* orientacja */
	Boolean multiple;	/* oddzielne okna */
} XystrayPart;

typedef struct _XystrayRec{
	CorePart core;
	XystrayPart xystray;
} XystrayRec;

typedef struct {
	int dummy;
} XystrayClassPart;

typedef struct _XystratClassRec {
	CoreClassPart core_class;
	XystrayClassPart xystray_class;
} XystrayClassRec;

extern XystrayClassRec xystrayClassRec;

#endif /* _XWINTRAY_H */
