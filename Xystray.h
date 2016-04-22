#ifndef _XYSTRAY_H
#define _XYSTRAY_H

#define XtCiconmax "Iconmax"
#define XtCiconrows "Iconrows"
#define XtCiconcols "Iconcols"
#define XtCiconpadding  "Iconpadding"
#define XtCiconsize "Iconsize"
#define XtCmultiple "Multiple"
#define XtCvertical "Vertical"

#define XtNiconmax "iconmax"
#define XtNiconrows "iconrows"
#define XtNiconcols "iconcols"
#define XtNiconpadding  "iconpadding"
#define XtNiconsize "iconsize"
#define XtNmultiple "multiple"
#define XtNvertical "vertical"

typedef struct _XystrayRec* XystrayWidget;
typedef struct _XystrayClassRec* XystrayWidgetClass;
extern WidgetClass xystrayWidgetClass;

#define SYSTEM_TRAY_REQUEST_DOCK	0
#define SYSTEM_TRAY_BEGIN_MESSAGE	1
#define SYSTEM_TRAY_CANCEL_MESSAGE	2

#define XEMBED_EMBEDDED_NOTIFY          0
#define XEMBED_WINDOW_ACTIVATE          1
#define XEMBED_WINDOW_DEACTIVATE        2
#define XEMBED_REQUEST_FOCUS            3
#define XEMBED_FOCUS_IN                 4
#define XEMBED_FOCUS_OUT                5
#define XEMBED_FOCUS_NEXT               6
#define XEMBED_FOCUS_PREV               7
/* 8-9 were used for XEMBED_GRAB_KEY/XEMBED_UNGRAB_KEY */
#define XEMBED_MODALITY_ON              10
#define XEMBED_MODALITY_OFF             11
#define XEMBED_REGISTER_ACCELERATOR     12
#define XEMBED_UNREGISTER_ACCELERATOR   13
#define XEMBED_ACTIVATE_ACCELERATOR     14

#define XEMBED_MAPPED			(1 << 0)

void XWtGetSize(XystrayWidget, Dimension*, Dimension*);
#endif /* _XYSTRAY_H */
