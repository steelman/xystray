#include <assert.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Core.h>
#include <X11/Xmd.h>
#include "XystrayP.h"

#define xwoffset(field) XtOffsetOf(XystrayRec, xystray.field)
#define offset(field) XtOffsetOf(WidgetRec, core.field)

#ifdef DEBUG
#define FDEBUG printf("%s\n", __FUNCTION__)
#else
#define FDEBUG
#endif

static XtResource resources[] = {
	/* zasoby zdefiniowane przez Xystray */
        {XtNiconmax, XtCiconmax, XtRDimension, sizeof(Dimension),
		xwoffset(iconmax), XtRImmediate, (XtPointer) 0},

        {XtNiconrows, XtCiconrows, XtRDimension, sizeof(Dimension),
		xwoffset(iconrows), XtRImmediate, (XtPointer) 2},

        {XtNiconcols, XtCiconcols, XtRDimension, sizeof(Dimension),
		xwoffset(iconcols), XtRImmediate, (XtPointer) 4},

        {XtNiconpadding, XtCiconpadding, XtRDimension, sizeof(Dimension),
		xwoffset(iconpadding), XtRImmediate, (XtPointer) 0},

        {XtNiconsize, XtCiconsize, XtRDimension, sizeof(Dimension),
		xwoffset(iconsize), XtRImmediate, (XtPointer) 22},

        {XtNmultiple, XtCmultiple, XtRBoolean, sizeof(Boolean),
		xwoffset(multiple), XtRImmediate, (XtPointer) FALSE},

        {XtNvertical, XtCvertical, XtRBoolean, sizeof(Boolean),
		xwoffset(vertical), XtRImmediate, (XtPointer) FALSE},
};

static void XWtClassInitialize(void);
static void XWtInitialize (Widget, Widget, ArgList, Cardinal*);
static void XWtRealize(Widget, XtValueMask*, XSetWindowAttributes*);
static void XWtDestroy(Widget);
static void XWtEventHandler(Widget, XPointer, XEvent*, Boolean*);
static void XWtRecvMessage(XystrayWidget xys, XClientMessageEvent* xev);
static void XWtDockIcon(XystrayWidget, Window);
static void XWtUndockIcon(XystrayWidget xys, Window icon);

/*
char* eventNames[] = { "XEvent0", "XEvent1",
"KeyPress", "KeyRelease", "ButtonPress", "ButtonRelease", "MotionNotify",
"EnterNotify", "LeaveNotify", "FocusIn", "FocusOut", "KeymapNotify",
"Expose", "GraphicsExpose", "NoExpose", "VisibilityNotify", "CreateNotify",
"DestroyNotify", "UnmapNotify", "MapNotify", "MapRequest", "ReparentNotify",
"ConfigureNotify", "ConfigureRequest", "GravityNotify", "ResizeRequest",
"CirculateNotify", "CirculateRequest", "PropertyNotify", "SelectionClear",
"SelectionRequest", "SelectionNotify", "ColormapNotify", "ClientMessage",
"MappingNotify", "LASTEvent",
};
*/

XystrayClassRec xystrayClassRec = {
    { /* core fields */
    /* superclass               */      (WidgetClass) &coreClassRec,
    /* class_name               */      "Xystray",
    /* widget_size              */      sizeof(XystrayRec),
    /* class_initialize         */      XWtClassInitialize,
    /* class_part_initialize    */      NULL,
    /* class_inited             */      FALSE,
    /* initialize               */      XWtInitialize,
    /* initialize_hook          */      NULL,
    /* realize                  */      XWtRealize,
    /* actions                  */      NULL,
    /* num_actions              */      0,
    /* resources                */      resources,
    /* resource_count           */      XtNumber(resources),
    /* xrm_class                */      NULLQUARK,          
    /* compress_motion          */      TRUE,
    /* compress_exposure        */      XtExposeCompressMaximal,
    /* compress_enterleave      */      TRUE,                   
    /* visible_interest         */      FALSE,
    /* destroy                  */      XWtDestroy,
    /* resize                   */      NULL,
    /* expose                   */      NULL,
    /* set_values               */      NULL,
    /* set_values_hook          */      NULL,
    /* set_values_almost        */      XtInheritSetValuesAlmost,
    /* get_values_hook          */      NULL,
    /* accept_focus             */      NULL,
    /* version                  */      XtVersion,
    /* callback_private         */      NULL,
    /* tm_table                 */      NULL,
    /* query_geometry           */      XtInheritQueryGeometry,
    /* display_accelerator      */      XtInheritDisplayAccelerator,
    /* extension                */      NULL
    }, { 0 }
};
    
WidgetClass xystrayWidgetClass = (WidgetClass) &xystrayClassRec;

static void
XWtClassInitialize( void ) {
	FDEBUG;
}

static void
XWtInitialize (Widget request, Widget new, ArgList args, Cardinal* num) {

	Display*	d = NULL;
	XystrayWidget xys = (XystrayWidget)new;
	Dimension w;
	Dimension h;
	d = XtDisplay(xys);

	FDEBUG;

	XWtGetSize(xys, &w, &h);

	XtVaSetValues((Widget)xys, XtNwidth, w,
			XtNheight, h,
			NULL);

	XtAddEventHandler((Widget)xys,
		SubstructureNotifyMask|SubstructureRedirectMask,
		True,
		(XtEventHandler)XWtEventHandler, (XPointer) NULL);

	
	assert(d!=NULL);
	xys->xystray.selection_atom = XInternAtom(d, "_NET_SYSTEM_TRAY_S0", False);
	xys->xystray.opcode_atom = XInternAtom(d, "_NET_SYSTEM_TRAY_OPCODE", False);
	xys->xystray.data_atom = XInternAtom(d, "_NET_SYSTEM_TRAY_MESSAGE_DATA", False);
	xys->xystray.xembed_atom = XInternAtom(d, "_XEMBED", False);
	xys->xystray.xembedi_atom = XInternAtom(d, "_XEMBED_INFO", False);


	if(xys->xystray.iconmax) {

		if(xys->xystray.vertical) {
			xys->xystray.iconrows = xys->xystray.iconmax;
			xys->xystray.iconcols = 1;
		} else {
			xys->xystray.iconrows = 1;
			xys->xystray.iconcols = xys->xystray.iconmax;
		}
			
	} else {
		xys->xystray.iconmax = xys->xystray.iconrows * \
				 xys->xystray.iconcols;
	}

	if(xys->xystray.iconmax == 0) {
		xys->xystray.iconmax = 1;
	}

	if(xys->xystray.iconrows == 0) {
		xys->xystray.iconrows = 1;
	}

	if(xys->xystray.iconcols == 0) {
		xys->xystray.iconcols = 1;
	}

	/* Zaalokowaæ tablicê okienek */
	/* FIXME: przerobiæ na listê? */
	xys->xystray.iconwindows = (Window*)XtCalloc(xys->xystray.iconmax,
						sizeof(Window));
	assert( xys->xystray.iconwindows != NULL );

	xys->xystray.iconsfree = xys->xystray.iconmax;
}

static void
XWtRealize(Widget gw, XtValueMask *valueMask, XSetWindowAttributes *attrs)
{
	XystrayWidget	xys = (XystrayWidget) gw;
	Display*	d = XtDisplay(xys);
	int		s = DefaultScreen(d);
	Window		r = RootWindow(d, s);
	Atom atom_sn = xys->xystray.selection_atom;

	Window w;
	XClientMessageEvent xev;
	XButtonEvent xbe;

	FDEBUG;

	(*xystrayWidgetClass->core_class.superclass->core_class.realize)
		(gw, valueMask, attrs);

	w = XtWindow(xys);
	/*  ___________________________________________
	   ||                                         ||
	   ||    _____         _____         _____    ||
	   || a |     |  2a   |     |       |     |   ||
	   ||---|     |-------|     |       |     |   ||
	   ||   |_____|       |_____|       |_____|   ||
	   ||      |a                                 ||
	   ||______|__________________________________||
                                                  x,y,w,h */
	/* FIXME: odddzielne okienko do komunikacji */
	xys->xystray.comm = w;

	/* FIXME: wybór ekranu */
	assert (XGetSelectionOwner(d, atom_sn) == None);

	XSetSelectionOwner(d, atom_sn, w, CurrentTime);
	assert (XGetSelectionOwner(d, atom_sn) == w);

	xev.type = ClientMessage;
	xev.window = r;
	xev.message_type = XInternAtom (d, "MANAGER", False);
	xev.format = 32;
	xev.data.l[0] = CurrentTime;
	xev.data.l[1] = atom_sn;
	xev.data.l[2] = w;
	xev.data.l[3] = 0;
	xev.data.l[4] = 0;

	XSendEvent (d, r, False, StructureNotifyMask, (XEvent*) &xev);
}

static void
XWtDestroy(Widget gw)
{
	XystrayWidget	xys=(XystrayWidget) gw;
	Display*	d = XtDisplay(gw);
	Window 		r = RootWindow(d, 0);
	Window*		iw = xys->xystray.iconwindows;
	int i;

	FDEBUG;

	for(i=0; i < xys->xystray.iconmax; i++) {
		if(iw[i] != 0) {
			XUnmapWindow(d,iw[i]);
			XReparentWindow(d, iw[i], r, 1, 1);
			iw[i]=0;
		}
	}

	XtFree((char*)xys->xystray.iconwindows);
}

static void
XWtEventHandler(Widget gw, XPointer xptr, XEvent* xev, Boolean* cont) {
	XystrayWidget xys = (XystrayWidget) gw;
	Atom		a = xys->xystray.selection_atom;
	Window		w = XtWindow(gw);
	Display*	d = XtDisplay(gw);

	FDEBUG;

	switch (xev->type) {
	case ClientMessage:
		XWtRecvMessage(xys, (XClientMessageEvent*) xev);
		break;

	case UnmapNotify:
		XWtUndockIcon(xys, xev->xdestroywindow.window);
		break;
	case DestroyNotify:
		XWtUndockIcon(xys, xev->xdestroywindow.window);
		break;

	case SelectionClear:
		if (XGetSelectionOwner(d, a) == w);
			XSetSelectionOwner(d, a, None, CurrentTime);

		XtCloseDisplay(d);
		exit(1);
		break;
	case ConfigureNotify:
		break;
	case ConfigureRequest:
		break;
	case ReparentNotify:
		break;
	}
}

static void XWtRecvMessage(XystrayWidget xys, XClientMessageEvent* xev) {
	
	Atom selection_msg = xys->xystray.selection_atom; /*XXX: potrzebne?*/
	Atom opcode_msg = xys->xystray.opcode_atom;
	Atom data_msg = xys->xystray.data_atom;
	Display*	d = XtDisplay(xys);
	Window		w = XtWindow(xys);
	Atom a;
	char* b;

	FDEBUG;

	if(xev->type != ClientMessage) {
		return;
	}

	a = xev->message_type;

	b=XGetAtomName(d, xev->message_type);
	if (xev->message_type == opcode_msg) {
		switch (xev->data.l[1]) {
		    case SYSTEM_TRAY_REQUEST_DOCK:
			XWtDockIcon(xys, xev->data.l[2]);
			break;

		    case SYSTEM_TRAY_BEGIN_MESSAGE:
			printf("Baloon.\n");
			break;

		    case SYSTEM_TRAY_CANCEL_MESSAGE:
			printf("Bang!!!\n");
			break;
		}

	} else if ( xev->message_type == data_msg) {

		if (xev->format != 8) {
			return;
		}
		printf("Data Message: (%d)%s\n", xev->message_type,b);

	} else {
		printf("Unknown Message: (%d)%s\n", xev->message_type,b);
	}
	XFree(b);
	return;
}

struct XembedInfo {
	CARD32 version;
	CARD32 flags;
};

static void XWtDockIcon(XystrayWidget xys, Window icon) {
	Display*	d = XtDisplay(xys);
	Window		r = RootWindow(d,0); /*FIXME: zmienny "screen" */
	Window		w = XtWindow(xys);
	Window*		iw = xys->xystray.iconwindows;
	Dimension	icon_size = xys->xystray.iconsize;
	Boolean		vertical = xys->xystray.vertical;
	Dimension	padding = xys->xystray.iconpadding;
	Atom a1,a2,a3;
	XWindowAttributes attrib;
	XClientMessageEvent xev;
	Dimension	ix,iy;
	Cardinal	ir=0,ic=0;
	struct XembedInfo xembedi;
	int i;

	FDEBUG;

	if (!xys->xystray.iconsfree) {
		printf("full!!!\n");
		return;
	}

	for(i=0; i < xys->xystray.iconmax && iw[i] != 0; i++) {
		if (++ic >=  xys->xystray.iconcols)
			ir++;

		ic = ic % xys->xystray.iconcols;
	}

	xys->xystray.iconsfree--;
	iw[i]=icon;

	ix = padding + (2*padding+icon_size)*ic;
	iy = padding + (2*padding+icon_size)*ir;

	/* XGetWindowAttributes(d, icon, &attrib); */

	XReparentWindow(d, icon, w, ix, iy);

	XResizeWindow(d, icon, icon_size, icon_size);
	XFlush(d);

	xev.window = icon;
	xev.type = ClientMessage;
	xev.message_type = XInternAtom (d, "_XEMBED", False);
	xev.format = 32;
	xev.data.l[0] = CurrentTime;
	xev.data.l[1] = XEMBED_EMBEDDED_NOTIFY;
	xev.data.l[2] = 0;
	xev.data.l[3] = w;
	xev.data.l[4] = 0;
	XSendEvent (d, r, False, NoEventMask, (XEvent*)&xev);

	/* FIXME: Make use of xembed info*/
	/* if (0) {
		int actual_format;
		unsigned long nitems;
		unsigned long bytes_after;
		unsigned char *buffer;

		a1=xys->xystray.xembedi_atom;

		XGetWindowProperty(d, icon, a1, 0, 128,
			False, a1, &a3,
			&actual_format, &nitems, &bytes_after,
			&buffer);

	} */

	XMapWindow(d, icon);
}

static void XWtUndockIcon(XystrayWidget xys, Window icon) {
	Boolean		rc = True;
	Window*		iw = xys->xystray.iconwindows;
	int i;

	FDEBUG;

	if (xys->xystray.iconsfree == xys->xystray.iconmax) {
		printf("empty!!!\n");
		return;
	}

	for(i=0; i < xys->xystray.iconmax && iw[i] != icon; i++) {
	}

	if (i == xys->xystray.iconmax) {
		printf("icon not found!!!\n");
		return;
	}

	printf("icon found!!!\n");
	rc=False;
	xys->xystray.iconsfree++;
	iw[i]=0;
}

void
XWtGetSize(XystrayWidget xys, Dimension* width, Dimension* height) {
	Dimension t;

	FDEBUG;

	/* szeroko¶æ = ile_ikon * rozmiar_ikony + (ile_ikon+1 * przerwa) */
	/* FIXME: miêdzy ikonami przerwa pojedyñcza czy podwójna? */
	*width = xys->xystray.iconcols *\
		(xys->xystray.iconsize + xys->xystray.iconpadding*2);

	*height = xys->xystray.iconrows *\
		(xys->xystray.iconsize + xys->xystray.iconpadding*2);

	/* wysoko¶æ = rozmiar_ikony + 2*przerwa */
	/* *height = xys->xystray.iconsize + 2*xys->xystray.iconpadding; */
	/*if (xys->xystray.vertical) {
		t = *width;
		*width = *height;
		*height = t;
	}*/
}
