/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qhaikustyle_p.h"
#include "qhaikustyle_p_p.h"

#if !defined(QT_NO_STYLE_HAIKU) || defined(QT_PLUGIN)

#include "qwindowsstyle_p_p.h"

#include <qapplication.h>
#include <qcombobox.h>
#include <qdebug.h>
#include <qdialogbuttonbox.h>
#include <qdir.h>
#include <qfont.h>
#include <qgroupbox.h>
#include <qhash.h>
#include <qimage.h>
#include <qlibrary.h>
#include <qmainwindow.h>
#include <qpainter.h>
#include <qpixmapcache.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qsplitter.h>
#include <qstyleoption.h>
#include <qtoolbar.h>
#include <qwizard.h>

#include <AppKit.h>
#include <Bitmap.h>
#include <ControlLook.h>
#include <InterfaceKit.h>
#include <NodeInfo.h>
#include <StorageKit.h>
#include <View.h>

QT_BEGIN_NAMESPACE

static const bool UsePixmapCache = true;

enum Direction {
    TopDown,
    FromLeft,
    BottomUp,
    FromRight
};

// from windows style
static const int windowsItemFrame        =  2; // menu item frame width
static const int windowsSepHeight        =  6; // separator item height
static const int windowsItemHMargin      =  3; // menu item hor text margin
static const int windowsItemVMargin      =  8; // menu item ver text margin
static const int windowsArrowHMargin     =  6; // arrow horizontal margin
static const int windowsTabSpacing       = 12; // space between text and tab
static const int windowsCheckMarkHMargin =  2; // horiz. margins of check mark
static const int windowsRightBorder      = 15; // right border on windows
static const int windowsCheckMarkWidth   = 12; // checkmarks width on windows

/* XPM */
static const char * const dock_widget_close_xpm[] = {
    "11 13 7 1",
    "     c None",
    ".    c #D5CFCB",
    "+    c #8F8B88",
    "@    c #6C6A67",
    "#    c #ABA6A3",
    "$    c #B5B0AC",
    "%    c #A4A09D",
    "           ",
    " +@@@@@@@+ ",
    "+#       #+",
    "@ $@   @$ @",
    "@ @@@ @@@ @",
    "@  @@@@@  @",
    "@   @@@   @",
    "@  @@@@@  @",
    "@ @@@ @@@ @",
    "@ $@   @$ @",
    "+%       #+",
    " +@@@@@@@+ ",
    "           "};


static const char * const dock_widget_restore_xpm[] = {
    "11 13 7 1",
    "     c None",
    ".    c #D5CFCB",
    "+    c #8F8B88",
    "@    c #6C6A67",
    "#    c #ABA6A3",
    "$    c #B5B0AC",
    "%    c #A4A09D",
    "           ",
    " +@@@@@@@+ ",
    "+#       #+",
    "@   #@@@# @",
    "@   @   @ @",
    "@ #@@@# @ @",
    "@ @   @ @ @",
    "@ @   @@@ @",
    "@ @   @   @",
    "@ #@@@#   @",
    "+%       #+",
    " +@@@@@@@+ ",
    "           "};

static const char * const workspace_minimize[] = {
    "11 13 7 1",
    "     c None",
    ".    c #D5CFCB",
    "+    c #8F8B88",
    "@    c #6C6A67",
    "#    c #ABA6A3",
    "$    c #B5B0AC",
    "%    c #A4A09D",
    "           ",
    " +@@@@@@@+ ",
    "+#       #+",
    "@         @",
    "@         @",
    "@         @",
    "@ @@@@@@@ @",
    "@ @@@@@@@ @",
    "@         @",
    "@         @",
    "+%       #+",
    " +@@@@@@@+ ",
    "           "};


static const char * const qt_titlebar_context_help[] = {
    "10 10 3 1",
    "  c None",
    "# c #000000",
    "+ c #444444",
    "  +####+  ",
    " ###  ### ",
    " ##    ## ",
    "     +##+ ",
    "    +##   ",
    "    ##    ",
    "    ##    ",
    "          ",
    "    ##    ",
    "    ##    "};


static const char * const qt_scrollbar_button_arrow_left[] = {
"6 7 18 1",
"d c #303030",
"g c #333333",
"c c #3d3d3d",
"j c #4d4d4d",
"l c #626262",
"h c #656565",
"p c #8b8b8b",
"a c #919191",
"k c #9a9a9a",
"f c #9e9e9e",
"n c #9f9f9f",
"e c #a5a5a5",
"m c #b3b3b3",
"i c #b4b4b4",
"b c #bababa",
"o c #c6c6c6",
"# c #cfcfcf",
". c None",
"...#a.",
"..bcde",
".fgdh.",
"iddj..",
".kgdl.",
"..mcdn",
"...op."};

static const char * const qt_scrollbar_button_arrow_right[] = {
"6 7 19 1",
"c c #303030",
"g c #333333",
"n c #3d3d3d",
"d c #3e3e3e",
"i c #4d4d4d",
"k c #626262",
"f c #646464",
"p c #8b8b8b",
"# c #919191",
"l c #9b9b9b",
"h c #9e9e9e",
"m c #9f9f9f",
"b c #a5a5a5",
"o c #b3b3b3",
"j c #b4b4b4",
"e c #bababa",
"q c #c6c6c6",
"a c #cfcfcf",
". c None",
".#a...",
"bcde..",
".fcgh.",
"..iccj",
".kcgl.",
"mcno..",
".pq..."};

static const char * const qt_scrollbar_button_arrow_up[] = {
"7 6 17 1",
"b c #303030",
"e c #333333",
"h c #3d3d3d",
"i c #4c4c4c",
"m c #626262",
"l c #656565",
"n c #8a8a8a",
"k c #919191",
"c c #9a9a9a",
"a c #9e9e9e",
"o c #a4a4a4",
"# c #afafaf",
"f c #b3b3b3",
"d c #b9b9b9",
"j c #c5c5c5",
"g c #cecece",
". c None",
"...#...",
"..abc..",
".debef.",
"ghbibhj",
"kbl.mbn",
".o...a."};

static const char * const qt_scrollbar_button_arrow_down[] = {
"7 6 17 1",
"c c #303030",
"l c #333333",
"h c #3d3d3d",
"i c #4c4c4c",
"e c #626262",
"d c #656565",
"f c #8a8a8a",
"b c #919191",
"n c #9a9a9a",
"a c #9e9e9e",
"# c #a4a4a4",
"o c #afafaf",
"m c #b3b3b3",
"k c #b9b9b9",
"j c #c5c5c5",
"g c #cecece",
". c None",
".#...a.",
"bcd.ecf",
"ghcichj",
".klclm.",
"..acn..",
"...o..."};

static const char * const qt_scrollbar_buttons[] = {
"15 15 31 1",
"  c None",
"n c #303030",
"q c #333333",
"t c #3d3d3d",
"u c #4c4c4c",
"y c #626262",
"x c #656565",
"z c #8a8a8a",
"w c #919191",
". c #989898",
"o c #9a9a9a",
"m c #9e9e9e",
"A c #a4a4a4",
"l c #afafaf",
"r c #b3b3b3",
"p c #b9b9b9",
"v c #c5c5c5",
"k c #c6c6c6",
"s c #cecece",
"B c #d4d4d4",
"j c #dbdbdb",
"i c #dcdcdc",
"h c #dedede",
"g c #dfdfdf",
"f c #e0e0e0",
"e c #e6e6e6",
"a c #e7e7e7",
"d c #e8e8e8",
"c c #e9e9e9",
"b c #eaeaea",
"# c #f7f7f7",
"...............",
".############a.",
".#           k.",
".#           k.",
".#           k.",
".#           k.",
".#           k.",
".#           k.",
".#           k.",
".#           k.",
".#           k.",
".#           k.",
".#           k.",
".akkkkkkkkkkkk.",
"..............."};


static const char * const qt_haiku_menuitem_checkbox_checked[] = {
"9 9 31 1",
"d c #000000",
"t c #020202",
"m c #060606",
"e c #0b0b0b",
"w c #0e0e0e",
"j c #191919",
"z c #1a1a1a",
"s c #2b2b2b",
"p c #2d2d2d",
"# c #4d4d4d",
"r c #555555",
"o c #636363",
"x c #646464",
"a c #707070",
"k c #808080",
"y c #828282",
"h c #838383",
"c c #9a9a9a",
"B c #9c9c9c",
"g c #a7a7a7",
"u c #b5b5b5",
"l c #bfbfbf",
"n c #c0c0c0",
"f c #c8c8c8",
"C c #c9c9c9",
"v c #cdcdcd",
"i c #d5d5d5",
"A c #d6d6d6",
"b c #d9d9d9",
"q c #dedede",
". c None",
"......#ab",
".....cdef",
".gh.ijdk.",
"ldmnodpq.",
".rdsttu..",
".vwddx...",
"..ydzA...",
"..qsB....",
"...C....."};

class TemporarySurface
{
public:
    TemporarySurface(const BRect& bounds)
        : m_bitmap(bounds, B_BITMAP_ACCEPTS_VIEWS, B_RGBA32)
        , m_view(bounds, "Qt temporary surface", 0, 0)
        , m_image(reinterpret_cast<const uchar*>(m_bitmap.Bits()),
            bounds.IntegerWidth() + 1, bounds.IntegerHeight() + 1,
            m_bitmap.BytesPerRow(), QImage::Format_ARGB32)
    {
        m_bitmap.Lock();
        m_bitmap.AddChild(&m_view);
    }

    ~TemporarySurface()
    {
        m_bitmap.RemoveChild(&m_view);
        m_bitmap.Unlock();
    }

    BView* view()
    {
        return &m_view;
    }

    QImage& image()
    {
        if (m_view.Window())
            m_view.Sync();

        return m_image;
    }

private:
    BBitmap m_bitmap;
    BView m_view;
    QImage m_image;
};


static QColor mkQColor(rgb_color rgb)
{
    return QColor(rgb.red, rgb.green, rgb.blue);
}

static void qt_haiku_draw_button(QPainter *painter, const QRect &qrect, bool def, bool flat, bool pushed, bool focus, bool enabled)
{
    Q_UNUSED(flat);

    QRect rect = qrect;
    if (be_control_look != NULL) {
        // TODO: If this button is embedded within a different color background, it would be
        // nice to tell this function so the frame can be smoothly blended into the background.
        rgb_color background = ui_color(B_PANEL_BACKGROUND_COLOR);
        rgb_color base = background;
        uint32 flags = 0;
        if (pushed)
            flags |= BControlLook::B_ACTIVATED;
        if (focus)
            flags |= BControlLook::B_FOCUSED;
        if (def) {
            flags |= BControlLook::B_DEFAULT_BUTTON;
            rect = rect.adjusted(-2,-2,2,2);
        }
        if (!enabled)
            flags |= BControlLook::B_DISABLED;

        BRect bRect(0.0f, 0.0f, rect.width() - 1, rect.height() - 1);

        TemporarySurface surface(bRect);

        be_control_look->DrawButtonFrame(surface.view(), bRect, bRect, base, background, flags);
        be_control_look->DrawButtonBackground(surface.view(), bRect, bRect, base, flags);

        painter->drawImage(rect, surface.image());

        return;
    }
}

static void qt_haiku_draw_gradient(QPainter *painter, const QRect &rect, const QColor &gradientStart,
                                   const QColor &gradientStop, Direction direction = TopDown, QBrush bgBrush = QBrush())
{
    const int x = rect.center().x();
    const int y = rect.center().y();

    QLinearGradient *gradient;
    switch (direction) {
    case FromLeft:
        gradient = new QLinearGradient(rect.left(), y, rect.right(), y);
        break;
    case FromRight:
        gradient = new QLinearGradient(rect.right(), y, rect.left(), y);
        break;
    case BottomUp:
        gradient = new QLinearGradient(x, rect.bottom(), x, rect.top());
        break;
    case TopDown:
    default:
        gradient = new QLinearGradient(x, rect.top(), x, rect.bottom());
        break;
    }

    if (bgBrush.gradient()) {
        gradient->setStops(bgBrush.gradient()->stops());
    } else {
        gradient->setColorAt(0, gradientStart);
        gradient->setColorAt(1, gradientStop);
    }

    painter->fillRect(rect, *gradient);
    delete gradient;
}

static QString uniqueName(const QString &key, const QStyleOption *option, const QSize &size)
{
    const QStyleOptionComplex *complexOption = qstyleoption_cast<const QStyleOptionComplex *>(option);

    QString tmp;
    tmp.sprintf("%s-%d-%d-%lld-%dx%d-%d", key.toLatin1().constData(), uint(option->state),
                complexOption ? uint(complexOption->activeSubControls) : uint(0),
                option->palette.cacheKey(), size.width(), size.height(), option->direction);
#ifndef QT_NO_SPINBOX
    if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
        tmp.append(QLatin1Char('-'));
        tmp.append(QString::number(spinBox->buttonSymbols));
        tmp.append(QLatin1Char('-'));
        tmp.append(QString::number(spinBox->stepEnabled));
        tmp.append(QLatin1Char('-'));
        tmp.append(QLatin1Char(spinBox->frame ? '1' : '0'));
    }
#endif // QT_NO_SPINBOX
    return tmp;
}

static void qt_haiku_draw_mdibutton(QPainter *painter, const QStyleOptionTitleBar *option, const QRect &tmp, bool hover, bool sunken)
{
    QColor dark;
    dark.setHsv(option->palette.button().color().hue(),
                qMin(255, (int)(option->palette.button().color().saturation()*1.9)),
                qMin(255, (int)(option->palette.button().color().value()*0.7)));

    const QColor highlight = option->palette.highlight().color();

    const bool active = (option->titleBarState & QStyle::State_Active);
    QColor titleBarHighlight(255, 255, 255, 60);

    if (sunken)
        painter->fillRect(tmp.adjusted(1, 1, -1, -1), option->palette.highlight().color().darker(120));
    else if (hover)
        painter->fillRect(tmp.adjusted(1, 1, -1, -1), QColor(255, 255, 255, 20));

    QColor mdiButtonGradientStartColor;
    QColor mdiButtonGradientStopColor;

    mdiButtonGradientStartColor = QColor(0, 0, 0, 40);
    mdiButtonGradientStopColor = QColor(255, 255, 255, 60);

    if (sunken)
        titleBarHighlight = highlight.darker(130);

    QLinearGradient gradient(tmp.center().x(), tmp.top(), tmp.center().x(), tmp.bottom());
    gradient.setColorAt(0, mdiButtonGradientStartColor);
    gradient.setColorAt(1, mdiButtonGradientStopColor);

    const QColor mdiButtonBorderColor(active ? option->palette.highlight().color().darker(180): dark.darker(110));

    painter->setPen(QPen(mdiButtonBorderColor, 1));
    painter->drawLine(tmp.left() + 2, tmp.top(), tmp.right() - 2, tmp.top());
    painter->drawLine(tmp.left() + 2, tmp.bottom(), tmp.right() - 2, tmp.bottom());
    painter->drawLine(tmp.left(), tmp.top() + 2, tmp.left(), tmp.bottom() - 2);
    painter->drawLine(tmp.right(), tmp.top() + 2, tmp.right(), tmp.bottom() - 2);
    painter->drawPoint(tmp.left() + 1, tmp.top() + 1);
    painter->drawPoint(tmp.right() - 1, tmp.top() + 1);
    painter->drawPoint(tmp.left() + 1, tmp.bottom() - 1);
    painter->drawPoint(tmp.right() - 1, tmp.bottom() - 1);

    painter->setPen(titleBarHighlight);
    painter->drawLine(tmp.left() + 2, tmp.top() + 1, tmp.right() - 2, tmp.top() + 1);
    painter->drawLine(tmp.left() + 1, tmp.top() + 2, tmp.left() + 1, tmp.bottom() - 2);

    painter->setPen(QPen(gradient, 1));
    painter->drawLine(tmp.right() + 1, tmp.top() + 2, tmp.right() + 1, tmp.bottom() - 2);
    painter->drawPoint(tmp.right() , tmp.top() + 1);

    painter->drawLine(tmp.left() + 2, tmp.bottom() + 1, tmp.right() - 2, tmp.bottom() + 1);
    painter->drawPoint(tmp.left() + 1, tmp.bottom());
    painter->drawPoint(tmp.right() - 1, tmp.bottom());
    painter->drawPoint(tmp.right() , tmp.bottom() - 1);
}

/*!
    \class QHaikuStyle
    \brief The QHaikuStyle class provides a widget style similar to the
    Clearlooks style available in GNOME.
    \since 4.2

    The Haiku style provides a look and feel for widgets
    that closely resembles the Clearlooks style, introduced by Richard
    Stellingwerff and Daniel Borgmann.

    \sa {Haiku Style Widget Gallery}, QWindowsXPStyle, QMacStyle, QWindowsStyle,
        QCDEStyle, QMotifStyle, QPlastiqueStyle
*/

/*!
    Constructs a QHaikuStyle object.
*/
QHaikuStyle::QHaikuStyle()
    : QWindowsStyle(*new QHaikuStylePrivate)
{
    setObjectName(QLatin1String("Haiku"));
}

/*!
    \internal

    Constructs a QHaikuStyle object.
*/
QHaikuStyle::QHaikuStyle(QHaikuStylePrivate &dd)
    : QWindowsStyle(dd)
{
}

/*!
    Destroys the QHaikuStyle object.
*/
QHaikuStyle::~QHaikuStyle()
{
}

/*!
    \fn void QHaikuStyle::drawItemText(QPainter *painter, const QRect &rectangle, int alignment, const QPalette &palette,
                                    bool enabled, const QString& text, QPalette::ColorRole textRole) const

    Draws the given \a text in the specified \a rectangle using the
    provided \a painter and \a palette.

    Text is drawn using the painter's pen. If an explicit \a textRole
    is specified, then the text is drawn using the \a palette's color
    for the specified role.  The \a enabled value indicates whether or
    not the item is enabled; when reimplementing, this value should
    influence how the item is drawn.

    The text is aligned and wrapped according to the specified \a
    alignment.

    \sa Qt::Alignment
*/
void QHaikuStyle::drawItemText(QPainter *painter, const QRect &rect, int alignment, const QPalette &pal,
                               bool enabled, const QString& text, QPalette::ColorRole textRole) const
{
    if (text.isEmpty())
        return;

    const QPen savedPen = painter->pen();

    if (textRole != QPalette::NoRole) {
        painter->setPen(QPen(pal.brush(textRole), savedPen.widthF()));
    }

    if (!enabled) {
        QPen pen = painter->pen();
        painter->setPen(pen);
    }

    painter->drawText(rect, alignment, text);
    painter->setPen(savedPen);
}

static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50)
{
    const int maxFactor = 100;
    QColor tmp = colorA;
    tmp.setRed((tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor);
    tmp.setGreen((tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor);
    tmp.setBlue((tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor);
    return tmp;
}

/*!
    \reimp
*/
void QHaikuStyle::drawPrimitive(PrimitiveElement elem,
                                const QStyleOption *option,
                                QPainter *painter, const QWidget *widget) const
{
    Q_ASSERT(option);
    QRect rect = option->rect;
    int state = option->state;
    QColor button = option->palette.button().color();
    QColor buttonShadow = option->palette.button().color().darker(110);
    QColor buttonShadowAlpha = buttonShadow;
    buttonShadowAlpha.setAlpha(128);
    QColor darkOutline;
    QColor dark;
    darkOutline.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*3.0)),
                qMin(255, (int)(button.value()*0.6)));
    dark.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*1.9)),
                qMin(255, (int)(button.value()*0.7)));
    QColor tabFrameColor = mergedColors(option->palette.background().color(),
                                                dark.lighter(135), 60);

    switch(elem) {
#ifndef QT_NO_TABBAR
    case PE_FrameTabBarBase:
        if (const QStyleOptionTabBarBase *tbb
                = qstyleoption_cast<const QStyleOptionTabBarBase *>(option)) {
            painter->save();
            painter->setPen(QPen(darkOutline.lighter(110), 0));
            switch (tbb->shape) {
            case QTabBar::RoundedNorth: {
                QRegion region(tbb->rect);
                region -= tbb->selectedTabRect;
                painter->drawLine(tbb->rect.topLeft(), tbb->rect.topRight());
                painter->setClipRegion(region);
                painter->setPen(option->palette.light().color());
                painter->drawLine(tbb->rect.topLeft() + QPoint(0, 1),
                                  tbb->rect.topRight()  + QPoint(0, 1));
            }
                break;
            case QTabBar::RoundedWest:
                painter->drawLine(tbb->rect.left(), tbb->rect.top(), tbb->rect.left(), tbb->rect.bottom());
                break;
            case QTabBar::RoundedSouth:
                painter->drawLine(tbb->rect.left(), tbb->rect.bottom(),
                            tbb->rect.right(), tbb->rect.bottom());
                break;
            case QTabBar::RoundedEast:
                painter->drawLine(tbb->rect.topRight(), tbb->rect.bottomRight());
                break;
            case QTabBar::TriangularNorth:
            case QTabBar::TriangularEast:
            case QTabBar::TriangularWest:
            case QTabBar::TriangularSouth:
                painter->restore();
                QWindowsStyle::drawPrimitive(elem, option, painter, widget);
                return;
            }
            painter->restore();
        }
        return;
#endif // QT_NO_TABBAR
    case PE_IndicatorViewItemCheck:
        {
            QStyleOptionButton button;
            button.QStyleOption::operator=(*option);
            button.state &= ~State_MouseOver;
            drawPrimitive(PE_IndicatorCheckBox, &button, painter, widget);
        }
        return;
    case PE_IndicatorHeaderArrow:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            QRect r = header->rect;
            QImage arrow;
            if (header->sortIndicator & QStyleOptionHeader::SortUp)
                arrow = QImage(qt_scrollbar_button_arrow_up);
            else if (header->sortIndicator & QStyleOptionHeader::SortDown)
                arrow = QImage(qt_scrollbar_button_arrow_down);
            if (!arrow.isNull()) {
                r.setSize(arrow.size());
                r.moveCenter(header->rect.center());
                arrow.setColor(1, header->palette.foreground().color().rgba());
                painter->drawImage(r, arrow);
            }
        }
        break;
    case PE_IndicatorButtonDropDown:
        drawPrimitive(PE_PanelButtonCommand, option, painter, widget);
        break;
    case PE_IndicatorToolBarSeparator:
        {
            QRect rect = option->rect;
            const int margin = 6;
            if (option->state & State_Horizontal) {
                const int offset = rect.width()/2;
                painter->setPen(QPen(option->palette.background().color().darker(110)));
                painter->drawLine(rect.bottomLeft().x() + offset,
                            rect.bottomLeft().y() - margin,
                            rect.topLeft().x() + offset,
                            rect.topLeft().y() + margin);
                painter->setPen(QPen(option->palette.background().color().lighter(110)));
                painter->drawLine(rect.bottomLeft().x() + offset + 1,
                            rect.bottomLeft().y() - margin,
                            rect.topLeft().x() + offset + 1,
                            rect.topLeft().y() + margin);
            } else { //Draw vertical separator
                const int offset = rect.height()/2;
                painter->setPen(QPen(option->palette.background().color().darker(110)));
                painter->drawLine(rect.topLeft().x() + margin ,
                            rect.topLeft().y() + offset,
                            rect.topRight().x() - margin,
                            rect.topRight().y() + offset);
                painter->setPen(QPen(option->palette.background().color().lighter(110)));
                painter->drawLine(rect.topLeft().x() + margin ,
                            rect.topLeft().y() + offset + 1,
                            rect.topRight().x() - margin,
                            rect.topRight().y() + offset + 1);
            }
        }
        break;
    case PE_Frame:
        painter->save();
        painter->setPen(dark.lighter(108));
        painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
        painter->restore();
        break;
    case PE_FrameMenu:
        painter->save();
        {
            painter->setPen(QPen(darkOutline, 1));
            painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
            QColor frameLight = option->palette.background().color().lighter(160);
            QColor frameShadow = option->palette.background().color().darker(110);

            //paint beveleffect
            QRect frame = option->rect.adjusted(1, 1, -1, -1);
            painter->setPen(frameLight);
            painter->drawLine(frame.topLeft(), frame.bottomLeft());
            painter->drawLine(frame.topLeft(), frame.topRight());

            painter->setPen(frameShadow);
            painter->drawLine(frame.topRight(), frame.bottomRight());
            painter->drawLine(frame.bottomLeft(), frame.bottomRight());
        }
        painter->restore();
        break;
    case PE_FrameDockWidget:

        painter->save();
        {
            QColor softshadow = option->palette.background().color().darker(120);

            QRect rect= option->rect;
            painter->setPen(softshadow);
            painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
            painter->setPen(QPen(option->palette.light(), 0));
            painter->drawLine(QPoint(rect.left() + 1, rect.top() + 1), QPoint(rect.left() + 1, rect.bottom() - 1));
            painter->setPen(QPen(option->palette.background().color().darker(120), 0));
            painter->drawLine(QPoint(rect.left() + 1, rect.bottom() - 1), QPoint(rect.right() - 2, rect.bottom() - 1));
            painter->drawLine(QPoint(rect.right() - 1, rect.top() + 1), QPoint(rect.right() - 1, rect.bottom() - 1));

        }
        painter->restore();
        break;
    case PE_PanelButtonTool:
        painter->save();
        if ((option->state & State_Enabled || option->state & State_On) || !(option->state & State_AutoRaise)) {
            QPen oldPen = painter->pen();

            if (widget && widget->inherits("QDockWidgetTitleButton")) {
                   if (option->state & State_MouseOver)
                       drawPrimitive(PE_PanelButtonCommand, option, painter, widget);
            } else {
                drawPrimitive(PE_PanelButtonCommand, option, painter, widget);
            }
        }
        painter->restore();
        break;
    case PE_IndicatorDockWidgetResizeHandle:
        {
            QStyleOption dockWidgetHandle = *option;
            bool horizontal = option->state & State_Horizontal;
            if (horizontal)
                dockWidgetHandle.state &= ~State_Horizontal;
            else
                dockWidgetHandle.state |= State_Horizontal;
            drawControl(CE_Splitter, &dockWidgetHandle, painter, widget);
        }
        break;
    case PE_FrameWindow:
        painter->save();
        {
            QRect rect= option->rect;
            painter->setPen(QPen(dark.darker(150), 0));
            painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
            painter->setPen(QPen(option->palette.light(), 0));
            painter->drawLine(QPoint(rect.left() + 1, rect.top() + 1),
                              QPoint(rect.left() + 1, rect.bottom() - 1));
            painter->setPen(QPen(option->palette.background().color().darker(120), 0));
            painter->drawLine(QPoint(rect.left() + 1, rect.bottom() - 1),
                              QPoint(rect.right() - 2, rect.bottom() - 1));
            painter->drawLine(QPoint(rect.right() - 1, rect.top() + 1),
                              QPoint(rect.right() - 1, rect.bottom() - 1));
        }
        painter->restore();
        break;
#ifndef QT_NO_LINEDIT
    case PE_FrameLineEdit:
        // fall through
#endif // QT_NO_LINEEDIT
#ifdef QT3_SUPPORT
        if (widget && widget->inherits("Q3ToolBar")) {
            drawPrimitive(PE_Q3Separator, option, painter, widget);
            break;
        }
#endif
        {
            QPen oldPen = painter->pen();
            if (option->state & State_Enabled) {
                if(option->state & State_HasFocus)
                    painter->setPen(QPen(mkQColor(ui_color(B_NAVIGATION_BASE_COLOR)), 0));
                else
                    painter->setPen(QPen(QColor(129,129,129), 0));
                painter->drawRect(rect.adjusted(1, 1, -1, -1));
            } else {
                painter->fillRect(rect, option->palette.background());
                painter->setPen(QPen(QColor(129,129,129), 0));
                painter->drawRect(rect.adjusted(1, 1, -1, -1));
            }
        }
        break;
    case PE_IndicatorCheckBox:
        painter->save();
        if (const QStyleOptionButton *checkbox = qstyleoption_cast<const QStyleOptionButton*>(option)) {

            rect = rect.adjusted(-2, -2, 1, 1);
            BRect bRect(0.0f, 0.0f, rect.width() - 1, rect.height() - 1);
            TemporarySurface surface(bRect);
            rgb_color base = ui_color(B_PANEL_BACKGROUND_COLOR);
            uint32 flags = 0;

            if (!(state & State_Enabled))
                flags |= BControlLook::B_DISABLED;
            if (checkbox->state & State_On)
                flags |= BControlLook::B_ACTIVATED;
            if (checkbox->state & State_HasFocus)
                flags |= BControlLook::B_FOCUSED;
            if (checkbox->state & State_Sunken)
                flags |= BControlLook::B_CLICKED;
            if (checkbox->state & State_NoChange)
                flags |= BControlLook::B_DISABLED | BControlLook::B_ACTIVATED;

            be_control_look->DrawCheckBox(surface.view(), bRect, bRect, base, flags);
            painter->drawImage(rect, surface.image());
        }
        painter->restore();
        break;
    case PE_IndicatorRadioButton:
        painter->save();
        {
            painter->setRenderHint(QPainter::SmoothPixmapTransform);

            painter->setRenderHint(QPainter::Antialiasing);
            if (state & State_Sunken)
                painter->setBrush(mkQColor(ui_color(B_PANEL_BACKGROUND_COLOR)));
            else
                painter->setBrush(QColor(255,255,255));

            if(state & State_HasFocus)
                painter->setPen(QPen(mkQColor(ui_color(B_NAVIGATION_BASE_COLOR)), 0));
            else
                painter->setPen(QColor(129,129,129));
            painter->drawEllipse(rect);

            if (state & (State_On )) {
                painter->setBrush(QColor(27,82,140));
                painter->setPen(Qt::NoPen);
                painter->drawEllipse(rect.adjusted(3, 3, -3, -3));
            }
        }
        painter->restore();
    break;
    case PE_IndicatorToolBarHandle:
        painter->save();
        if (option->state & State_Horizontal) {
            for (int i = rect.height()/5; i <= 4*(rect.height()/5) ; ++i) {
                int y = rect.topLeft().y() + i + 1;
                int x1 = rect.topLeft().x() + 3;
                int x2 = rect.topRight().x() - 2;

                if (i % 2 == 0)
                    painter->setPen(QPen(option->palette.light(), 0));
                else
                    painter->setPen(QPen(dark.lighter(110), 0));
                painter->drawLine(x1, y, x2, y);
            }
        }
        else { //vertical toolbar
            for (int i = rect.width()/5; i <= 4*(rect.width()/5) ; ++i) {
                int x = rect.topLeft().x() + i + 1;
                int y1 = rect.topLeft().y() + 3;
                int y2 = rect.topLeft().y() + 5;

                if (i % 2 == 0)
                    painter->setPen(QPen(option->palette.light(), 0));
                else
                    painter->setPen(QPen(dark.lighter(110), 0));
                painter->drawLine(x, y1, x, y2);
            }
        }
        painter->restore();
        break;
    case PE_FrameDefaultButton:
        case PE_FrameFocusRect:
        if (const QStyleOptionFocusRect *focusFrame = qstyleoption_cast<const QStyleOptionFocusRect *>(option)) {
            if (!(focusFrame->state & State_KeyboardFocusChange))
                return;
            QRect rect = focusFrame->rect;
            painter->save();
            painter->setBackgroundMode(Qt::TransparentMode);
            painter->setBrush(QBrush(dark.darker(120), Qt::Dense4Pattern));
            painter->setBrushOrigin(rect.topLeft());
            painter->setPen(Qt::NoPen);
            painter->drawRect(rect.left(), rect.top(), rect.width(), 1);    // Top
            painter->drawRect(rect.left(), rect.bottom(), rect.width(), 1); // Bottom
            painter->drawRect(rect.left(), rect.top(), 1, rect.height());   // Left
            painter->drawRect(rect.right(), rect.top(), 1, rect.height());  // Right
            painter->restore();
        }
        break;
    case PE_PanelButtonCommand:
        painter->save();
        {
            if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
                QStyleOptionButton copy = *btn;

                bool enabled = option->state & State_Enabled;
                bool pushed = (option->state & State_Sunken) || (option->state & State_On);
                bool focus = option->state & State_HasFocus;
                bool flat = btn->features & QStyleOptionFrameV2::Flat;
                bool def = (btn->features & QStyleOptionButton::DefaultButton) && (btn->state & State_Enabled);

                qt_haiku_draw_button(painter, option->rect.adjusted(1,1,-1,-1), def, flat, pushed, focus, enabled);
            }
         painter->restore();
        }
        break;
#ifndef QT_NO_TABBAR
        case PE_FrameTabWidget:
            painter->save();
        {
            painter->fillRect(option->rect, tabFrameColor);
        }
#ifndef QT_NO_TABWIDGET
        if (const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option)) {
            QColor borderColor = darkOutline.lighter(110);
            QColor alphaCornerColor = mergedColors(borderColor, option->palette.background().color());

            int borderThickness = pixelMetric(PM_TabBarBaseOverlap, twf, widget);
            bool reverse = (twf->direction == Qt::RightToLeft);
            QRect tabBarRect;

            switch (twf->shape) {
            case QTabBar::RoundedNorth:
                if (reverse) {
                    tabBarRect = QRect(twf->rect.right() - twf->leftCornerWidgetSize.width()
                                       - twf->tabBarSize.width() + 1,
                                       twf->rect.top(),
                                       twf->tabBarSize.width(), borderThickness);
                } else {
                    tabBarRect = QRect(twf->rect.left() + twf->leftCornerWidgetSize.width(),
                                       twf->rect.top(),
                                       twf->tabBarSize.width(), borderThickness);
                }
                break ;
            case QTabBar::RoundedWest:
                tabBarRect = QRect(twf->rect.left(),
                                   twf->rect.top() + twf->leftCornerWidgetSize.height(),
                                   borderThickness,
                                   twf->tabBarSize.height());
                tabBarRect = tabBarRect; //adjust
                break ;
            case QTabBar::RoundedEast:
                tabBarRect = QRect(twf->rect.right() - borderThickness + 1,
                                   twf->rect.top()  + twf->leftCornerWidgetSize.height(),
                                   0,
                                   twf->tabBarSize.height());
                break ;
            case QTabBar::RoundedSouth:
                if (reverse) {
                    tabBarRect = QRect(twf->rect.right() - twf->leftCornerWidgetSize.width() - twf->tabBarSize.width() + 1,
                                       twf->rect.bottom() + 1,
                                       twf->tabBarSize.width(),
                                       borderThickness);
                } else {
                    tabBarRect = QRect(twf->rect.left() + twf->leftCornerWidgetSize.width(),
                                       twf->rect.bottom() + 1,
                                       twf->tabBarSize.width(),
                                       borderThickness);
                }
                break;
            default:
                break;
            }

            QRegion region(twf->rect);
            region -= tabBarRect;
            painter->setClipRegion(region);

            // Outer border
            QLine leftLine = QLine(twf->rect.topLeft() + QPoint(0, 2), twf->rect.bottomLeft() - QPoint(0, 2));
            QLine rightLine = QLine(twf->rect.topRight(), twf->rect.bottomRight() - QPoint(0, 2));
            QLine bottomLine = QLine(twf->rect.bottomLeft() + QPoint(2, 0), twf->rect.bottomRight() - QPoint(2, 0));
            QLine topLine = QLine(twf->rect.topLeft(), twf->rect.topRight());

            painter->setPen(borderColor);
            painter->drawLine(topLine);

            // Inner border
            QLine innerLeftLine = QLine(leftLine.p1() + QPoint(1, 0), leftLine.p2() + QPoint(1, 0));
            QLine innerRightLine = QLine(rightLine.p1() - QPoint(1, -1), rightLine.p2() - QPoint(1, 0));
            QLine innerBottomLine = QLine(bottomLine.p1() - QPoint(0, 1), bottomLine.p2() - QPoint(0, 1));
            QLine innerTopLine = QLine(topLine.p1() + QPoint(0, 1), topLine.p2() + QPoint(-1, 1));

            // Rounded Corner
            QPoint leftBottomOuterCorner = QPoint(innerLeftLine.p2() + QPoint(0, 1));
            QPoint leftBottomInnerCorner1 = QPoint(leftLine.p2() + QPoint(0, 1));
            QPoint leftBottomInnerCorner2 = QPoint(bottomLine.p1() - QPoint(1, 0));
            QPoint rightBottomOuterCorner = QPoint(innerRightLine.p2() + QPoint(0, 1));
            QPoint rightBottomInnerCorner1 = QPoint(rightLine.p2() + QPoint(0, 1));
            QPoint rightBottomInnerCorner2 = QPoint(bottomLine.p2() + QPoint(1, 0));
            QPoint leftTopOuterCorner = QPoint(innerLeftLine.p1() - QPoint(0, 1));
            QPoint leftTopInnerCorner1 = QPoint(leftLine.p1() - QPoint(0, 1));
            QPoint leftTopInnerCorner2 = QPoint(topLine.p1() - QPoint(1, 0));

            painter->setPen(borderColor);
            painter->drawLine(leftLine);
            painter->drawLine(rightLine);
            painter->drawLine(bottomLine);
            painter->drawPoint(leftBottomOuterCorner);
            painter->drawPoint(rightBottomOuterCorner);
            painter->drawPoint(leftTopOuterCorner);

            painter->setPen(option->palette.light().color());
            painter->drawLine(innerLeftLine);
            painter->drawLine(innerTopLine);

            painter->setPen(buttonShadowAlpha);
            painter->drawLine(innerRightLine);
            painter->drawLine(innerBottomLine);

            painter->setPen(alphaCornerColor);
            painter->drawPoint(leftBottomInnerCorner1);
            painter->drawPoint(leftBottomInnerCorner2);
            painter->drawPoint(rightBottomInnerCorner1);
            painter->drawPoint(rightBottomInnerCorner2);
            painter->drawPoint(leftTopInnerCorner1);
            painter->drawPoint(leftTopInnerCorner2);
        }
#endif // QT_NO_TABWIDGET
    painter->restore();
    break ;

    case PE_FrameStatusBarItem:
        break;
    case PE_IndicatorTabClose:
        {
            Q_D(const QHaikuStyle);
            if (d->tabBarcloseButtonIcon.isNull())
                d->tabBarcloseButtonIcon = standardIcon(SP_DialogCloseButton, option, widget);
            if ((option->state & State_Enabled) && (option->state & State_MouseOver))
                drawPrimitive(PE_PanelButtonCommand, option, painter, widget);
            QPixmap pixmap = d->tabBarcloseButtonIcon.pixmap(QSize(16, 16), QIcon::Normal, QIcon::On);
            drawItemPixmap(painter, option->rect, Qt::AlignCenter, pixmap);
        }
        break;

#endif // QT_NO_TABBAR
    default:
        QWindowsStyle::drawPrimitive(elem, option, painter, widget);
        break;
    }
}

/*!
  \reimp
*/
void QHaikuStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter,
                                   const QWidget *widget) const
{
    QColor button = option->palette.button().color();
    QColor dark;
    dark.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*1.9)),
                qMin(255, (int)(button.value()*0.7)));
    QColor darkOutline;
    darkOutline.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*2.0)),
                qMin(255, (int)(button.value()*0.6)));
    QRect rect = option->rect;
    QColor shadow = mergedColors(option->palette.background().color().darker(120),
                                 dark.lighter(130), 60);
    QColor tabFrameColor = mergedColors(option->palette.background().color(),
                                                dark.lighter(135), 60);

    switch(element) {
     case CE_RadioButton: //fall through
     case CE_CheckBox:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            bool hover = (btn->state & State_MouseOver && btn->state & State_Enabled);
            if (hover)
                painter->fillRect(rect, btn->palette.background().color().lighter(104));
            QStyleOptionButton copy = *btn;
            copy.rect.adjust(2, 0, -2, 0);
            QWindowsStyle::drawControl(element, &copy, painter, widget);
        }
        break;
    case CE_Splitter:
        painter->save();
        {
            orientation orient = (option->state & State_Horizontal)?B_HORIZONTAL:B_VERTICAL;

            if (be_control_look != NULL) {
                QRect r = option->rect;
                rgb_color base = ui_color(B_PANEL_BACKGROUND_COLOR);;
                uint32 flags = 0;
                BRect bRect(0.0f, 0.0f, r.width() - 1, r.height() - 1);
                TemporarySurface surface(bRect);
                be_control_look->DrawSplitter(surface.view(), bRect, bRect, base, orient, flags);
                painter->drawImage(r, surface.image());
            }
        }
        painter->restore();
        break;
    case CE_SizeGrip:
        painter->save();
           painter->fillRect(option->rect, option->palette.background().color());
        painter->restore();
        break;
#ifndef QT_NO_TOOLBAR
    case CE_ToolBar:
        painter->save();
        if (const QStyleOptionToolBar *toolbar = qstyleoption_cast<const QStyleOptionToolBar *>(option)) {
            QRect rect = option->rect;

            bool paintLeftBorder = true;
            bool paintRightBorder = true;
            bool paintBottomBorder = true;

            switch (toolbar->toolBarArea) {
            case Qt::BottomToolBarArea:
                switch(toolbar->positionOfLine) {
                case QStyleOptionToolBar::Beginning:
                case QStyleOptionToolBar::OnlyOne:
                    paintBottomBorder = false;
                default:
                    break;
                }
            case Qt::TopToolBarArea:
                switch (toolbar->positionWithinLine) {
                case QStyleOptionToolBar::Beginning:
                    paintLeftBorder = false;
                    break;
                case QStyleOptionToolBar::End:
                    paintRightBorder = false;
                    break;
                case QStyleOptionToolBar::OnlyOne:
                    paintRightBorder = false;
                    paintLeftBorder = false;
                default:
                    break;
                }
                if (QApplication::layoutDirection() == Qt::RightToLeft) { //reverse layout changes the order of Beginning/end
                    bool tmp = paintLeftBorder;
                    paintRightBorder=paintLeftBorder;
                    paintLeftBorder=tmp;
                }
                break;
            case Qt::RightToolBarArea:
                switch (toolbar->positionOfLine) {
                case QStyleOptionToolBar::Beginning:
                case QStyleOptionToolBar::OnlyOne:
                    paintRightBorder = false;
                    break;
                default:
                    break;
                }
                break;
            case Qt::LeftToolBarArea:
                switch (toolbar->positionOfLine) {
                case QStyleOptionToolBar::Beginning:
                case QStyleOptionToolBar::OnlyOne:
                    paintLeftBorder = false;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }

            QColor light = option->palette.background().color().lighter(110);

            //draw top border
            painter->setPen(QPen(light));
            painter->drawLine(rect.topLeft().x(),
                        rect.topLeft().y(),
                        rect.topRight().x(),
                        rect.topRight().y());

            if (paintLeftBorder) {
                painter->setPen(QPen(light));
                painter->drawLine(rect.topLeft().x(),
                            rect.topLeft().y(),
                            rect.bottomLeft().x(),
                            rect.bottomLeft().y());
            }

            if (paintRightBorder) {
                painter->setPen(QPen(shadow));
                painter->drawLine(rect.topRight().x(),
                            rect.topRight().y(),
                            rect.bottomRight().x(),
                            rect.bottomRight().y());
            }

            if (paintBottomBorder) {
                painter->setPen(QPen(shadow));
                painter->drawLine(rect.bottomLeft().x(),
                            rect.bottomLeft().y(),
                            rect.bottomRight().x(),
                            rect.bottomRight().y());
            }
        }
        painter->restore();
        break;
#endif // QT_NO_TOOLBAR
#ifndef QT_NO_DOCKWIDGET
    case CE_DockWidgetTitle:
        painter->save();
        if (const QStyleOptionDockWidget *dwOpt = qstyleoption_cast<const QStyleOptionDockWidget *>(option)) {
            const QStyleOptionDockWidgetV2 *v2
                = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(dwOpt);
            bool verticalTitleBar = v2 == 0 ? false : v2->verticalTitleBar;

            QRect rect = dwOpt->rect;
            QRect titleRect = subElementRect(SE_DockWidgetTitleBarText, option, widget);
            QRect r = rect.adjusted(0, 0, -1, 0);
            if (verticalTitleBar)
                r.adjust(0, 0, 0, -1);
            painter->setPen(option->palette.light().color());
            painter->drawRect(r.adjusted(1, 1, 1, 1));
            painter->setPen(shadow);
            painter->drawRect(r);

            if (verticalTitleBar) {
                QRect r = rect;
                QSize s = r.size();
                s.transpose();
                r.setSize(s);

                titleRect = QRect(r.left() + rect.bottom()
                                    - titleRect.bottom(),
                                r.top() + titleRect.left() - rect.left(),
                                titleRect.height(), titleRect.width());

                painter->translate(r.left(), r.top() + r.width());
                painter->rotate(-90);
                painter->translate(-r.left(), -r.top());

                rect = r;
            }

            if (!dwOpt->title.isEmpty()) {
                QString titleText
                    = painter->fontMetrics().elidedText(dwOpt->title,
                                            Qt::ElideRight, titleRect.width());
                drawItemText(painter,
                             titleRect,
                             Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, dwOpt->palette,
                             dwOpt->state & State_Enabled, titleText,
                             QPalette::WindowText);
                }
        }
        painter->restore();
        break;
#endif // QT_NO_DOCKWIDGET
    case CE_HeaderSection:
        painter->save();
        // Draws the header in tables.
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
            QPixmap cache;
            QString pixmapName = uniqueName(QLatin1String("headersection"), option, option->rect.size());
            pixmapName += QLatin1String("-") + QString::number(int(header->position));
            pixmapName += QLatin1String("-") + QString::number(int(header->orientation));
            QRect r = option->rect;
            QColor gradientStopColor;
            QColor gradientStartColor = option->palette.button().color();
            gradientStopColor.setHsv(gradientStartColor.hue(),
                                     qMin(255, (int)(gradientStartColor.saturation()*2)),
                                     qMin(255, (int)(gradientStartColor.value()*0.96)));
            QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
            if (option->palette.background().gradient()) {
                gradient.setStops(option->palette.background().gradient()->stops());
            } else {
                gradient.setColorAt(0, gradientStartColor);
                gradient.setColorAt(0.8, gradientStartColor);
                gradient.setColorAt(1, gradientStopColor);
            }
            painter->fillRect(r, gradient);

            if (!UsePixmapCache || !QPixmapCache::find(pixmapName, cache)) {
                cache = QPixmap(r.size());
                cache.fill(Qt::transparent);
                QRect pixmapRect(0, 0, r.width(), r.height());
                QPainter cachePainter(&cache);
                if (header->orientation == Qt::Vertical) {
                    cachePainter.setPen(QPen(dark));
                    cachePainter.drawLine(pixmapRect.topRight(), pixmapRect.bottomRight());
                    if (header->position != QStyleOptionHeader::End) {
                        cachePainter.setPen(QPen(shadow));
                        cachePainter.drawLine(pixmapRect.bottomLeft() + QPoint(3, -1), pixmapRect.bottomRight() + QPoint(-3, -1));                                cachePainter.setPen(QPen(option->palette.light().color()));
                        cachePainter.drawLine(pixmapRect.bottomLeft() + QPoint(3, 0), pixmapRect.bottomRight() + QPoint(-3, 0));                              }
                } else {
                    cachePainter.setPen(QPen(dark));
                    cachePainter.drawLine(pixmapRect.bottomLeft(), pixmapRect.bottomRight());
                    cachePainter.setPen(QPen(shadow));
                    cachePainter.drawLine(pixmapRect.topRight() + QPoint(-1, 3), pixmapRect.bottomRight() + QPoint(-1, -3));                                  cachePainter.setPen(QPen(option->palette.light().color()));
                    cachePainter.drawLine(pixmapRect.topRight() + QPoint(0, 3), pixmapRect.bottomRight() + QPoint(0, -3));                                }
                cachePainter.end();
                if (UsePixmapCache)
                    QPixmapCache::insert(pixmapName, cache);
            }
            painter->drawPixmap(r.topLeft(), cache);
        }
        painter->restore();
        break;
    case CE_ProgressBarGroove:
        painter->save();
        {
            painter->fillRect(rect, option->palette.base());
            QColor borderColor = dark.lighter(110);
            painter->setPen(QPen(borderColor, 0));
            painter->drawLine(QPoint(rect.left() + 1, rect.top()), QPoint(rect.right() - 1, rect.top()));
            painter->drawLine(QPoint(rect.left() + 1, rect.bottom()), QPoint(rect.right() - 1, rect.bottom()));
            painter->drawLine(QPoint(rect.left(), rect.top() + 1), QPoint(rect.left(), rect.bottom() - 1));
            painter->drawLine(QPoint(rect.right(), rect.top() + 1), QPoint(rect.right(), rect.bottom() - 1));
            QColor alphaCorner = mergedColors(borderColor, option->palette.background().color());
            QColor innerShadow = mergedColors(borderColor, option->palette.base().color());

            //corner smoothing
            painter->setPen(alphaCorner);
            painter->drawPoint(rect.topRight());
            painter->drawPoint(rect.topLeft());
            painter->drawPoint(rect.bottomRight());
            painter->drawPoint(rect.bottomLeft());

            //inner shadow
            painter->setPen(innerShadow);
            painter->drawLine(QPoint(rect.left() + 1, rect.top() + 1),
                              QPoint(rect.right() - 1, rect.top() + 1));
            painter->drawLine(QPoint(rect.left() + 1, rect.top() + 1),
                              QPoint(rect.left() + 1, rect.bottom() + 1));

        }
        painter->restore();
        break;
    case CE_ProgressBarContents:
        painter->save();
        if (const QStyleOptionProgressBar *bar = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {
            QRect rect = bar->rect;
            bool vertical = false;
            bool inverted = false;
            bool indeterminate = (bar->minimum == 0 && bar->maximum == 0);

            // Get extra style options if version 2
            if (const QStyleOptionProgressBarV2 *bar2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(option)) {
                vertical = (bar2->orientation == Qt::Vertical);
                inverted = bar2->invertedAppearance;
            }

            // If the orientation is vertical, we use a transform to rotate
            // the progress bar 90 degrees clockwise.  This way we can use the
            // same rendering code for both orientations.
            if (vertical) {
                rect = QRect(rect.left(), rect.top(), rect.height(), rect.width()); // flip width and height
                QTransform m;
                m.translate(rect.height()-1, -1.0);
                m.rotate(90.0);
                painter->setTransform(m, true);
            }

            int maxWidth = rect.width() - 4;
            int minWidth = 4;
            qint64 progress = (qint64)qMax(bar->progress, bar->minimum); // workaround for bug in QProgressBar
            double vc6_workaround = ((progress - qint64(bar->minimum)) / qMax(double(1.0), double(qint64(bar->maximum) - qint64(bar->minimum))) * maxWidth);
            int progressBarWidth = (int(vc6_workaround) > minWidth ) ? int(vc6_workaround) : minWidth;
            int width = indeterminate ? maxWidth : progressBarWidth;

            bool reverse = (!vertical && (bar->direction == Qt::RightToLeft)) || vertical;
            if (inverted)
                reverse = !reverse;

            QRect progressBar;
            if (!indeterminate) {
                if (!reverse) {
                    progressBar.setRect(rect.left() + 1, rect.top() + 1, width + 1, rect.height() - 3);
                } else {
                    progressBar.setRect(rect.right() - 1 - width, rect.top() + 1, width + 1, rect.height() - 3);
                }
            } else {
                Q_D(const QHaikuStyle);
                int slideWidth = ((rect.width() - 4) * 2) / 3;
                int step = 0;
                if (QProgressStyleAnimation *animation = qobject_cast<QProgressStyleAnimation*>(d->animation(option->styleObject)))
                    step = animation->progressStep(slideWidth);
                else
                    d->startAnimation(new QProgressStyleAnimation(d->animationFps, option->styleObject));

                progressBar.setRect(rect.left() + 1 + step, rect.top() + 1,
                                    slideWidth / 2, rect.height() - 3);
            }
            QColor highlight = option->palette.color(QPalette::Normal, QPalette::Highlight);
            painter->setPen(QPen(highlight.darker(140), 0));

            QColor highlightedGradientStartColor = highlight.lighter(100);
            QColor highlightedGradientStopColor  = highlight.lighter(130);

            QLinearGradient gradient(rect.topLeft(), QPoint(rect.bottomLeft().x(),
                                                            rect.bottomLeft().y()*2));

            gradient.setColorAt(0, highlightedGradientStartColor);
            gradient.setColorAt(1, highlightedGradientStopColor);

            painter->setBrush(gradient);
            painter->drawRect(progressBar);

            painter->setPen(QPen(highlight.lighter(120), 0));
            painter->drawLine(QPoint(progressBar.left() + 1, progressBar.top() + 1),
                              QPoint(progressBar.right(), progressBar.top() + 1));
            painter->drawLine(QPoint(progressBar.left() + 1, progressBar.top() + 1),
                              QPoint(progressBar.left() + 1, progressBar.bottom() - 1));

            painter->setPen(QPen(highlightedGradientStartColor, 7.0));//QPen(option->palette.highlight(), 3));

            painter->save();
            painter->setClipRect(progressBar.adjusted(2, 2, -1, -1));
            for (int x = progressBar.left() - 32; x < rect.right() ; x+=18) {
                painter->drawLine(x, progressBar.bottom() + 1, x + 23, progressBar.top() - 2);
            }
            painter->restore();

        }
        painter->restore();
        break;
    case CE_MenuBarItem:
        painter->save();
        if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(option))
        {
            QStyleOptionMenuItem item = *mbi;
            item.rect = mbi->rect.adjusted(0, 0, 0, 0);
            if (be_control_look != NULL) {
                QRect r = rect.adjusted(0,-1,0,0);
                rgb_color base = ui_color(B_MENU_BACKGROUND_COLOR);;
                uint32 flags = 0;
                BRect bRect(0.0f, 0.0f, r.width() - 1, r.height() - 1);
                TemporarySurface surface(bRect);
                be_control_look->DrawMenuBarBackground(surface.view(), bRect, bRect, base, flags, 8);
                painter->drawImage(r, surface.image());
            }

            bool act = mbi->state & State_Selected && mbi->state & State_Sunken;
            //bool dis = !(mbi->state & State_Enabled);

            QRect r = option->rect;
            if (act) {
                qt_haiku_draw_gradient(painter, r.adjusted(1, 1, -1, -1),
                                            QColor(150,150,150),
                                            QColor(168,168,168), TopDown,
                                            QColor(168,168,168));

                painter->setPen(QPen(QColor(168,168,168), 0));
                painter->drawLine(QPoint(r.left(), r.top()), QPoint(r.left(), r.bottom()));
                painter->drawLine(QPoint(r.right(), r.top()), QPoint(r.right(), r.bottom()));
                painter->drawLine(QPoint(r.left(), r.bottom()), QPoint(r.right(), r.bottom()));
                painter->drawLine(QPoint(r.left(), r.top()), QPoint(r.right(), r.top()));
            }

            QPalette::ColorRole textRole = QPalette::Text;
            uint alignment = Qt::AlignCenter  | Qt::TextHideMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
            drawItemText(painter, item.rect, alignment, mbi->palette, mbi->state & State_Enabled, mbi->text, textRole);
        }
        painter->restore();
        break;
    case CE_MenuItem:
        painter->save();
        // Draws one item in a popup menu.
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            QColor menuBackground = option->palette.background().color().lighter(104);
            QColor borderColor = option->palette.background().color().darker(160);
            QColor alphaCornerColor;

            if (widget) {
                // ### backgroundrole/foregroundrole should be part of the style option
                alphaCornerColor = mergedColors(option->palette.color(widget->backgroundRole()), borderColor);
            } else {
                alphaCornerColor = mergedColors(option->palette.background().color(), borderColor);
            }

            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
                painter->fillRect(menuItem->rect, menuBackground);
                int w = 0;
                if (!menuItem->text.isEmpty()) {
                    painter->setFont(menuItem->font);
                    drawItemText(painter, menuItem->rect.adjusted(5, 0, -5, 0), Qt::AlignLeft | Qt::AlignVCenter,
                                 menuItem->palette, menuItem->state & State_Enabled, menuItem->text,
                                 QPalette::Text);
                    w = menuItem->fontMetrics.width(menuItem->text) + 5;
                }
                painter->setPen(shadow.lighter(106));
                bool reverse = menuItem->direction == Qt::RightToLeft;
                painter->drawLine(menuItem->rect.left() + 5 + (reverse ? 0 : w), menuItem->rect.center().y(),
                                  menuItem->rect.right() - 5 - (reverse ? w : 0), menuItem->rect.center().y());
                painter->restore();
                break;
            }
            bool selected = menuItem->state & State_Selected; //&& menuItem->state & State_Enabled;
            if (selected) {
                QRect r = option->rect.adjusted(1, 0, -2, -1);
                qt_haiku_draw_gradient(painter, r, QColor(150,150,150),
                                            QColor(168,168,168), TopDown,
                                            QColor(168,168,168));
                r = r.adjusted(-1, 0, 1, 0);
                painter->setPen(QPen(QColor(168,168,168), 0));
                painter->drawLine(QPoint(r.left(), r.top()), QPoint(r.left(), r.bottom()));
                painter->drawLine(QPoint(r.right(), r.top()), QPoint(r.right(), r.bottom()));
                painter->drawLine(QPoint(r.left(), r.bottom()), QPoint(r.right(), r.bottom()));
                painter->drawLine(QPoint(r.left(), r.top()), QPoint(r.right(), r.top()));
            } else {
                painter->fillRect(option->rect, menuBackground);
            }

            bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
            bool checked = menuItem->checked;
            bool sunken = menuItem->state & State_Sunken;
            bool enabled = menuItem->state & State_Enabled;

            bool ignoreCheckMark = false;
            int checkcol = qMax(menuItem->maxIconWidth, 20);

#ifndef QT_NO_COMBOBOX
            if (qobject_cast<const QComboBox*>(widget))
                ignoreCheckMark = true; //ignore the checkmarks provided by the QComboMenuDelegate
#endif

            if (!ignoreCheckMark) {
                // Check
                QRect checkRect(option->rect.left() + 7, option->rect.center().y() - 6, 13, 13);
                checkRect = visualRect(menuItem->direction, menuItem->rect, checkRect);
                if (checkable) {
                    if (menuItem->checkType & QStyleOptionMenuItem::Exclusive) {
                        // Radio button
                        if (checked || sunken) {
                            painter->setRenderHint(QPainter::Antialiasing);
                            painter->setPen(Qt::NoPen);

                            QPalette::ColorRole textRole = !enabled ? QPalette::Text:
                                                        selected ? QPalette::HighlightedText : QPalette::ButtonText;
                            painter->setBrush(option->palette.brush( option->palette.currentColorGroup(), textRole));
                            painter->drawEllipse(checkRect.adjusted(4, 4, -4, -4));
                        }
                    } else {
                        // Check box
                        if (menuItem->icon.isNull()) {
                            if (checked || sunken) {
                                QImage image(qt_haiku_menuitem_checkbox_checked);
                                if (enabled && (menuItem->state & State_Selected)) {
                                    image.setColor(1, 0x55ffffff);
                                    image.setColor(2, 0xAAffffff);
                                    image.setColor(3, 0xBBffffff);
                                    image.setColor(4, 0xFFffffff);
                                    image.setColor(5, 0x33ffffff);
                                } else {
                                    image.setColor(1, 0x55000000);
                                    image.setColor(2, 0xAA000000);
                                    image.setColor(3, 0xBB000000);
                                    image.setColor(4, 0xFF000000);
                                    image.setColor(5, 0x33000000);
                                }
                                painter->drawImage(QPoint(checkRect.center().x() - image.width() / 2,
                                                        checkRect.center().y() - image.height() / 2), image);
                            }
                        }
                    }
                }
            } else { //ignore checkmark
                if (menuItem->icon.isNull())
                    checkcol = 0;
                else
                    checkcol = menuItem->maxIconWidth;
            }

            // Text and icon, ripped from windows style
            bool dis = !(menuItem->state & State_Enabled);
            bool act = menuItem->state & State_Selected;
            const QStyleOption *opt = option;
            const QStyleOptionMenuItem *menuitem = menuItem;

            QPainter *p = painter;
            QRect vCheckRect = visualRect(opt->direction, menuitem->rect,
                                          QRect(menuitem->rect.x(), menuitem->rect.y(),
                                                checkcol, menuitem->rect.height()));
            if (!menuItem->icon.isNull()) {
                QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
                if (act && !dis)
                    mode = QIcon::Active;
                QPixmap pixmap;

                int smallIconSize = pixelMetric(PM_SmallIconSize, option, widget);
                QSize iconSize(smallIconSize, smallIconSize);
#ifndef QT_NO_COMBOBOX
                if (const QComboBox *combo = qobject_cast<const QComboBox*>(widget))
                    iconSize = combo->iconSize();
#endif // QT_NO_COMBOBOX
                if (checked)
                    pixmap = menuItem->icon.pixmap(iconSize, mode, QIcon::On);
                else
                    pixmap = menuItem->icon.pixmap(iconSize, mode);

                int pixw = pixmap.width();
                int pixh = pixmap.height();

                QRect pmr(0, 0, pixw, pixh);
                pmr.moveCenter(vCheckRect.center());
                painter->setPen(menuItem->palette.text().color());
                if (checkable && checked) {
                    QStyleOption opt = *option;
                    if (act) {
                        QColor activeColor = mergedColors(option->palette.background().color(),
                                                        option->palette.highlight().color());
                        opt.palette.setBrush(QPalette::Button, activeColor);
                    }
                    opt.state |= State_Sunken;
                    opt.rect = vCheckRect;
                    bool enabled = opt.state & State_Enabled;
                    bool focus = opt.state & State_HasFocus;
                    qt_haiku_draw_button(painter, opt.rect, false, false, true, focus, enabled);
                }
                painter->drawPixmap(pmr.topLeft(), pixmap);
            }

            painter->setPen(menuItem->palette.text().color());

            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            int tab = menuitem->tabWidth;
            QColor discol;
            if (dis) {
                discol = menuitem->palette.text().color();
                p->setPen(discol);
            }
            int xm = windowsItemFrame + checkcol + windowsItemHMargin;
            int xpos = menuitem->rect.x() + xm;

            QRect textRect(xpos, y + windowsItemVMargin, w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
            QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);
            QString s = menuitem->text;
            if (!s.isEmpty()) {                     // draw text
                p->save();
                int t = s.indexOf(QLatin1Char('\t'));
                int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
                if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
                    text_flags |= Qt::TextHideMnemonic;
                text_flags |= Qt::AlignLeft;
                if (t >= 0) {
                    QString sText = s.mid(t + 1);
                    sText.replace(QStringLiteral("Ctrl+"), QStringLiteral("Alt+"));
                    QRect vShortcutRect = visualRect(opt->direction, menuitem->rect,
                                                     QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
                    if (dis && !act && styleHint(SH_EtchDisabledText, option, widget)) {
                        p->setPen(menuitem->palette.light().color());
                        p->drawText(vShortcutRect.adjusted(1, 1, 1, 1), text_flags, sText);
                        p->setPen(discol);
                    }
                    p->drawText(vShortcutRect, text_flags, sText);
                    s = s.left(t);
                }
                QFont font = menuitem->font;

                if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                    font.setBold(true);

                p->setFont(font);
                if (dis && !act && styleHint(SH_EtchDisabledText, option, widget)) {
                    p->setPen(menuitem->palette.light().color());
                    p->drawText(vTextRect.adjusted(1, 1, 1, 1), text_flags, s.left(t));
                    p->setPen(discol);
                }
                p->drawText(vTextRect, text_flags, s.left(t));
                p->restore();
            }

            // Arrow
            if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {// draw sub menu arrow
                int dim = (menuItem->rect.height() - 4) / 2;
                PrimitiveElement arrow;
                arrow = QApplication::isRightToLeft() ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
                int xpos = menuItem->rect.left() + menuItem->rect.width() - 3 - dim;
                QRect  vSubMenuRect = visualRect(option->direction, menuItem->rect,
                                                 QRect(xpos, menuItem->rect.top() + menuItem->rect.height() / 2 - dim / 2, dim, dim));
                QStyleOptionMenuItem newMI = *menuItem;
                newMI.rect = vSubMenuRect;
                newMI.state = !enabled ? State_None : State_Enabled;
                if (selected)
                    newMI.palette.setColor(QPalette::ButtonText,
                                           newMI.palette.highlightedText().color());
                drawPrimitive(arrow, &newMI, painter, widget);
            }
        }
        painter->restore();
        break;
    case CE_MenuHMargin:
    case CE_MenuVMargin:
        break;
    case CE_MenuEmptyArea:
        break;
    case CE_PushButtonLabel:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            bool pushed = (option->state & State_Sunken) || (option->state & State_On);

            QRect ir = button->rect;
            uint tf = Qt::AlignVCenter;
            if (styleHint(SH_UnderlineShortcut, button, widget))
                tf |= Qt::TextShowMnemonic;
            else
               tf |= Qt::TextHideMnemonic;

            if (!button->icon.isNull() && button->text.isEmpty()) {
                //Center both icon and text
                QPoint point;

                QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal
                                                              : QIcon::Disabled;
                if (mode == QIcon::Normal && button->state & State_HasFocus)
                    mode = QIcon::Active;
                QIcon::State state = QIcon::Off;
                if (button->state & State_On)
                    state = QIcon::On;

                QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
                int w = pixmap.width();
                int h = pixmap.height();

                if (!button->text.isEmpty())
                    w += button->fontMetrics.boundingRect(option->rect, tf, button->text).width() + 2;

                point = QPoint(ir.x() + ir.width() / 2 - w / 2,
                               ir.y() + ir.height() / 2 - h / 2);

                if (button->direction == Qt::RightToLeft)
                    point.rx() += pixmap.width();

                painter->drawPixmap(visualPos(button->direction, button->rect, point), pixmap);

                if (button->direction == Qt::RightToLeft)
                    ir.translate(-point.x() - 2, 0);
                else
                    ir.translate(point.x() + pixmap.width(), 0);

                // left-align text if there is
                if (!button->text.isEmpty())
                    tf |= Qt::AlignLeft;

            } else {
                tf |= Qt::AlignHCenter;
            }

            if (button->features & QStyleOptionButton::HasMenu)
                ir = ir.adjusted(0, 0, -pixelMetric(PM_MenuButtonIndicator, button, widget), 0);
            if(pushed)
                ir = ir.adjusted(1,1,0,0);
            drawItemText(painter, ir, tf, button->palette, (button->state & State_Enabled),
                         button->text, QPalette::ButtonText);
        }
        break;
    case CE_MenuBarEmptyArea:
        painter->save();
        {
            if (be_control_look != NULL) {
                QRect r = rect.adjusted(0,0,0,-1);
                rgb_color base = ui_color(B_MENU_BACKGROUND_COLOR);;
                uint32 flags = 0;
                BRect bRect(0.0f, 0.0f, r.width() - 1, r.height() - 1);
                TemporarySurface surface(bRect);
                be_control_look->DrawMenuBarBackground(surface.view(), bRect, bRect, base, flags);
                painter->drawImage(r, surface.image());
            }

               painter->setPen(QPen(QColor(152,152,152)));
            painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        }
        painter->restore();
        break;
#ifndef QT_NO_TABBAR
    case CE_TabBarTabShape:
        painter->save();
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {

            bool rtlHorTabs = (tab->direction == Qt::RightToLeft
                               && (tab->shape == QTabBar::RoundedNorth
                                   || tab->shape == QTabBar::RoundedSouth));
            bool selected = tab->state & State_Selected;
            bool lastTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::End)
                            || (rtlHorTabs
                                && tab->position == QStyleOptionTab::Beginning));
            bool onlyTab = tab->position == QStyleOptionTab::OnlyOneTab;
            bool leftCornerWidget = (tab->cornerWidgets & QStyleOptionTab::LeftCornerWidget);

            bool atBeginning = ((tab->position == (tab->direction == Qt::LeftToRight ?
                                QStyleOptionTab::Beginning : QStyleOptionTab::End)) || onlyTab);

            bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
            bool previousSelected =
                ((!rtlHorTabs
                  && tab->selectedPosition == QStyleOptionTab::PreviousIsSelected)
                 || (rtlHorTabs
                     && tab->selectedPosition == QStyleOptionTab::NextIsSelected));
            bool nextSelected =
                ((!rtlHorTabs
                  && tab->selectedPosition == QStyleOptionTab::NextIsSelected)
                 || (rtlHorTabs
                     && tab->selectedPosition
                     == QStyleOptionTab::PreviousIsSelected));
            int tabBarAlignment = styleHint(SH_TabBar_Alignment, tab, widget);
            bool leftAligned = (!rtlHorTabs && tabBarAlignment == Qt::AlignLeft)
                               || (rtlHorTabs
                                   && tabBarAlignment == Qt::AlignRight);

            bool rightAligned = (!rtlHorTabs && tabBarAlignment == Qt::AlignRight)
                                || (rtlHorTabs
                                    && tabBarAlignment == Qt::AlignLeft);

            QColor light = tab->palette.light().color();

            QColor background = tab->palette.background().color();
            int borderThinkness = pixelMetric(PM_TabBarBaseOverlap, tab, widget);
            if (selected)
                borderThinkness /= 2;
            QRect r2(option->rect);
            int x1 = r2.left();
            int x2 = r2.right();
            int y1 = r2.top()+2;
            int y2 = r2.bottom();

            QTransform rotMatrix;
            bool flip = false;
            painter->setPen(shadow);
            QColor activeHighlight = option->palette.color(QPalette::Normal, QPalette::Highlight);
            switch (tab->shape) {
            case QTabBar::RoundedNorth:
                break;
            case QTabBar::RoundedSouth:
                rotMatrix.rotate(180);
                rotMatrix.translate(0, -rect.height() + 1);
                rotMatrix.scale(-1, 1);
                painter->setTransform(rotMatrix, true);
                break;
            case QTabBar::RoundedWest:
                rotMatrix.rotate(180 + 90);
                rotMatrix.scale(-1, 1);
                flip = true;
                painter->setTransform(rotMatrix, true);
                break;
            case QTabBar::RoundedEast:
                rotMatrix.rotate(90);
                rotMatrix.translate(0, - rect.width() + 1);
                flip = true;
                painter->setTransform(rotMatrix, true);
                break;
            default:
                painter->restore();
                QWindowsStyle::drawControl(element, tab, painter, widget);
                return;
            }

            if (flip) {
                QRect tmp = rect;
                rect = QRect(tmp.y(), tmp.x(), tmp.height(), tmp.width());
                int temp = x1;
                x1 = y1;
                y1 = temp;
                temp = x2;
                x2 = y2;
                y2 = temp;
            }

            QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
            if (option->palette.button().gradient()) {
                if (selected)
                    gradient.setStops(option->palette.background().gradient()->stops());
                else
                    gradient.setStops(option->palette.background().gradient()->stops());
            }
            else if (selected) {
                gradient.setColorAt(0, option->palette.background().color().lighter(104));
                gradient.setColorAt(1, tabFrameColor);
                painter->fillRect(rect.adjusted(0, 2, 0, -2), gradient);
            } else {
                y1 += 2;
                gradient.setColorAt(0, option->palette.background().color());
                gradient.setColorAt(1, dark.lighter(120));
                painter->fillRect(rect.adjusted(0, 2, 0, -2), gradient);
            }

            // Delete border
            if (selected) {
                painter->setPen(QColor(200,200,200));
                painter->drawLine(x1 + 2, y2 - 1, x2 - 2, y2 - 1);
            } else {
                painter->setPen(dark);
                painter->drawLine(x1, y2 - 1, x2 + 2, y2 - 1 );
                if (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedWest) {
                    painter->setPen(light);
                    painter->drawLine(x1, y2 , x2, y2 );
                }
            }
            // Left
            if (atBeginning || selected ) {
                painter->setPen(light);
                painter->drawLine(x1 + 1, y1 + 2 + 1, x1 + 1, y2 - ((onlyOne || atBeginning) && selected && leftAligned ? 0 : borderThinkness) - (atBeginning && leftCornerWidget ? 1 : 0));
                //painter->drawPoint(x1 + 1, y1 + 1);
                painter->setPen(dark);
                painter->drawLine(x1, y1 + 2, x1, y2 - ((onlyOne || atBeginning)  && leftAligned ? 0 : borderThinkness) - (atBeginning && leftCornerWidget ? 1 : 0));
            }
            // Top
            {
                int beg = x1 + (previousSelected ? 0 : 1);
                int end = x2 - (nextSelected ? 0 : 2);
                //painter->setPen(light);

               // if (!selected)painter->drawLine(beg - 3, y1 + 1, end, y1 + 1);

/*                if (selected)
                      painter->setPen(QPen(activeHighlight.darker(150), 0));
                else
                    painter->setPen(darkOutline);*/

                painter->setPen(darkOutline);
                painter->drawLine(beg, y1 , end, y1);

                if (atBeginning|| selected) {
                    painter->drawPoint(beg - 1, y1 + 1);
                } else if (!atBeginning) {
                    painter->drawPoint(beg - 1, y1);
                    painter->drawPoint(beg - 2, y1);
                    if (!lastTab) {
                        painter->setPen(dark.lighter(130));
                        painter->drawPoint(end + 1, y1);
                        painter->drawPoint(end + 2 , y1);
                        painter->drawPoint(end + 3, y1);
                    }
                }
            }
            // Right
            if (lastTab || selected || onlyOne || !nextSelected) {
                painter->setPen(darkOutline);

                if (selected) {
                    painter->drawLine(x2, y1 + 2, x2, y2 - ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness));
                    painter->setPen(QPen(activeHighlight.darker(150), 0));
                    painter->drawPoint(x2 - 1, y1 + 1);
                } else {
                    painter->drawLine(x2, y1 + 1, x2, y2 - ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness));
                    painter->drawPoint(x2 - 1, y1);
                }

                if (selected) {
                    painter->setPen(background.darker(110));
                    painter->drawLine(x2 - 1, y1 + 2, x2 - 1, y2 - ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness));
                }
            }
        }
        painter->restore();
        break;

#endif // QT_NO_TABBAR
    default:
        QWindowsStyle::drawControl(element,option,painter,widget);
        break;
    }
}

/*!
  \reimp
*/

QPalette QHaikuStyle::standardPalette () const
{
    QPalette palette = QWindowsStyle::standardPalette();
    palette.setBrush(QPalette::Active, QPalette::Highlight, QColor(98, 140, 178));
    palette.setBrush(QPalette::Inactive, QPalette::Highlight, QColor(145, 141, 126));
    palette.setBrush(QPalette::Disabled, QPalette::Highlight, QColor(145, 141, 126));

    QColor backGround(mkQColor(ui_color(B_PANEL_BACKGROUND_COLOR)));

    QColor light = backGround.lighter(150);
    QColor base = Qt::white;
    QColor dark = QColor(170, 156, 143).darker(110);
    dark = backGround.darker(150);
    QColor darkDisabled = QColor(209, 200, 191).darker(110);

    //### Find the correct disabled text color
    palette.setBrush(QPalette::Disabled, QPalette::Text, QColor(190, 190, 190));

    palette.setBrush(QPalette::Window, backGround);
    palette.setBrush(QPalette::Mid, backGround.darker(130));
    palette.setBrush(QPalette::Light, light);

    palette.setBrush(QPalette::Active, QPalette::Base, base);
    palette.setBrush(QPalette::Inactive, QPalette::Base, base);
    palette.setBrush(QPalette::Disabled, QPalette::Base, backGround);

    palette.setBrush(QPalette::Midlight, palette.mid().color().lighter(110));

    palette.setBrush(QPalette::All, QPalette::Dark, dark);
    palette.setBrush(QPalette::Disabled, QPalette::Dark, darkDisabled);

    QColor button = backGround;

    palette.setBrush(QPalette::Button, button);

    QColor shadow = dark.darker(135);
    palette.setBrush(QPalette::Shadow, shadow);
    palette.setBrush(QPalette::Disabled, QPalette::Shadow, shadow.lighter(150));
    palette.setBrush(QPalette::HighlightedText, QColor(QRgb(0xffffffff)));
    return palette;
}

/*!
  \reimp
*/
void QHaikuStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                         QPainter *painter, const QWidget *widget) const
{
    QColor button = option->palette.button().color();
    QColor dark;
    QColor grooveColor;
    QColor darkOutline;
    dark.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*1.9)),
                qMin(255, (int)(button.value()*0.7)));
    grooveColor.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*2.6)),
                qMin(255, (int)(button.value()*0.9)));
    darkOutline.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*3.0)),
                qMin(255, (int)(button.value()*0.6)));

    QColor alphaCornerColor;
    if (widget) {
        // ### backgroundrole/foregroundrole should be part of the style option
        alphaCornerColor = mergedColors(option->palette.color(widget->backgroundRole()), darkOutline);
    } else {
        alphaCornerColor = mergedColors(option->palette.background().color(), darkOutline);
    }
    QColor gripShadow = grooveColor.darker(110);
    QColor buttonShadow = option->palette.button().color().darker(110);

    QColor gradientStartColor = option->palette.button().color().lighter(108);
    QColor gradientStopColor = mergedColors(option->palette.button().color().darker(108), dark.lighter(150), 70);

    //QColor highlightedGradientStartColor = option->palette.button().color();
    //QColor highlightedGradientStopColor = mergedColors(option->palette.button().color(), option->palette.highlight().color(), 85);

    //QColor highlightedDarkInnerBorderColor = mergedColors(option->palette.button().color(), option->palette.highlight().color(), 35);
    //QColor highlightedLightInnerBorderColor = mergedColors(option->palette.button().color(), option->palette.highlight().color(), 58);

    QPalette palette = option->palette;

    switch (control) {
#ifndef QT_NO_SPINBOX
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            QPixmap cache;
            QString pixmapName = uniqueName(QLatin1String("spinbox"), spinBox, spinBox->rect.size());
            if (!UsePixmapCache || !QPixmapCache::find(pixmapName, cache)) {
                cache = QPixmap(spinBox->rect.size());
                cache.fill(Qt::transparent);
                QRect pixmapRect(0, 0, spinBox->rect.width(), spinBox->rect.height());
                QPainter cachePainter(&cache);

                bool isEnabled = (spinBox->state & State_Enabled);
                //bool focus = isEnabled && (spinBox->state & State_HasFocus);
                bool hover = isEnabled && (spinBox->state & State_MouseOver);
                bool sunken = (spinBox->state & State_Sunken);
                bool upIsActive = (spinBox->activeSubControls == SC_SpinBoxUp);
                bool downIsActive = (spinBox->activeSubControls == SC_SpinBoxDown);

                QRect rect = pixmapRect;
                QStyleOptionSpinBox spinBoxCopy = *spinBox;
                spinBoxCopy.rect = pixmapRect;
                QRect upRect = subControlRect(CC_SpinBox, &spinBoxCopy, SC_SpinBoxUp, widget);
                QRect downRect = subControlRect(CC_SpinBox, &spinBoxCopy, SC_SpinBoxDown, widget);

                int fw = spinBoxCopy.frame ? pixelMetric(PM_SpinBoxFrameWidth, &spinBoxCopy, widget) : 0;
                cachePainter.fillRect(rect.adjusted(1, qMax(fw - 1, 0), -1, -fw),
                                      option->palette.base());

                QRect r = rect.adjusted(0, 1, 0, -1);
                if (spinBox->frame) {

                    QColor topShadow = darkOutline;
                    topShadow.setAlpha(60);
                    cachePainter.setPen(topShadow);

                    // antialias corners
                    cachePainter.drawPoint(QPoint(r.right(), r.top() + 1));
                    cachePainter.drawPoint(QPoint(r.right() - 1, r.top() ));
                    cachePainter.drawPoint(QPoint(r.right(), r.bottom() - 1));
                    cachePainter.drawPoint(QPoint(r.right() - 1, r.bottom() ));
                    cachePainter.drawPoint(QPoint(r.left() + 1, r.bottom()));
                    cachePainter.drawPoint(QPoint(r.left(), r.bottom() - 1));
                    cachePainter.drawPoint(QPoint(r.left() + 1, r.top()));
                    cachePainter.drawPoint(QPoint(r.left(), r.top() + 1));

                    // draw frame
                    topShadow.setAlpha(30);
                    cachePainter.setPen(topShadow);
                    cachePainter.drawLine(QPoint(r.left() + 2, r.top() - 1), QPoint(r.right() - 2, r.top() - 1));

                    cachePainter.setPen(QPen(option->palette.background().color(), 1));
                    cachePainter.drawLine(QPoint(r.left() + 2, r.top() + 1), QPoint(r.right() - 2, r.top() + 1));
                    QColor highlight = Qt::white;
                    highlight.setAlpha(130);
                    cachePainter.setPen(option->palette.base().color().darker(120));
                    cachePainter.drawLine(QPoint(r.left() + 1, r.top() + 1),
                                  QPoint(r.right() - 1, r.top() + 1));
                    cachePainter.drawLine(QPoint(r.left() + 1, r.top() + 1),
                                  QPoint(r.left() + 1, r.bottom() - 1));
                    cachePainter.setPen(option->palette.base().color());
                    cachePainter.drawLine(QPoint(r.right() - 1, r.top() + 1),
                                  QPoint(r.right() - 1, r.bottom() - 1));
                    cachePainter.drawLine(QPoint(r.left() + 1, r.bottom() - 1),
                                  QPoint(r.right() - 1, r.bottom() - 1));
                    cachePainter.setPen(highlight);
                    cachePainter.drawLine(QPoint(r.left() + 3, r.bottom() + 1),
                                  QPoint(r.right() - 3, r.bottom() + 1));

                    cachePainter.setPen(QPen(darkOutline, 1));

                    // top and bottom lines
                    cachePainter.drawLine(QPoint(r.left() + 2, r.bottom()), QPoint(r.right()- 2, r.bottom()));
                    cachePainter.drawLine(QPoint(r.left() + 2, r.top()), QPoint(r.right() - 2, r.top()));
                    cachePainter.drawLine(QPoint(r.right(), r.top() + 2), QPoint(r.right(), r.bottom() - 2));
                    cachePainter.drawLine(QPoint(r.left(), r.top() + 2), QPoint(r.left(), r.bottom() - 2));
                }

                    // gradients
                    qt_haiku_draw_gradient(&cachePainter, upRect,
                                            gradientStartColor.darker(106),
                                            gradientStopColor, TopDown, option->palette.button());
                    qt_haiku_draw_gradient(&cachePainter, downRect.adjusted(0, 0, 0, 1),
                                            gradientStartColor.darker(106),
                                            gradientStopColor, TopDown, option->palette.button());
                if (isEnabled) {
                    if(upIsActive) {
                        if (sunken) {
                            cachePainter.fillRect(upRect.adjusted(1, 0, 0, 0), gradientStopColor.darker(110));
                        } else if (hover) {
                            qt_haiku_draw_gradient(&cachePainter, upRect.adjusted(1, 0, 0, 0),
                                                    gradientStartColor.lighter(110),
                                                    gradientStopColor.lighter(110), TopDown, option->palette.button());
                        }
                    }
                    if(downIsActive) {
                        if (sunken) {
                            cachePainter.fillRect(downRect.adjusted(1, 0, 0, 1), gradientStopColor.darker(110));

                        } else if (hover) {
                                qt_haiku_draw_gradient(&cachePainter, downRect.adjusted(1, 0, 0, 1),
                                                        gradientStartColor.lighter(110),
                                                        gradientStopColor.lighter(110), TopDown, option->palette.button());
                        }
                    }
                }

                if (spinBox->frame) {
                    // rounded corners
                    cachePainter.drawPoint(QPoint(r.left() + 1, r.bottom() - 1));
                    cachePainter.drawPoint(QPoint(r.left() + 1, r.top() + 1));
                    cachePainter.drawPoint(QPoint(r.right() - 1, r.bottom() - 1));
                    cachePainter.drawPoint(QPoint(r.right() - 1, r.top() + 1));

                    if (option->state & State_HasFocus) {
                        QColor darkoutline = option->palette.highlight().color().darker(150);
                        QColor innerline = mergedColors(option->palette.highlight().color(), Qt::white);
                        cachePainter.setPen(QPen(innerline, 0));
                        if (spinBox->direction == Qt::LeftToRight) {
                            cachePainter.drawRect(rect.adjusted(1, 2, -3 -downRect.width(), -3));
                            cachePainter.setPen(QPen(darkoutline, 0));
                            cachePainter.drawLine(QPoint(r.left() + 2, r.bottom()), QPoint(r.right()- downRect.width() - 1, r.bottom()));
                            cachePainter.drawLine(QPoint(r.left() + 2, r.top()), QPoint(r.right() - downRect.width() - 1, r.top()));
                            cachePainter.drawLine(QPoint(r.right() - downRect.width() - 1, r.top() + 1), QPoint(r.right()- downRect.width() - 1, r.bottom() - 1));
                            cachePainter.drawLine(QPoint(r.left(), r.top() + 2), QPoint(r.left(), r.bottom() - 2));
                            cachePainter.drawPoint(QPoint(r.left() + 1, r.bottom() - 1));
                            cachePainter.drawPoint(QPoint(r.left() + 1, r.top() + 1));
                            cachePainter.drawLine(QPoint(r.left(), r.top() + 2), QPoint(r.left(), r.bottom() - 2));
                        } else {
                            cachePainter.drawRect(rect.adjusted(downRect.width() + 2, 2, -2, -3));
                            cachePainter.setPen(QPen(darkoutline, 0));
                            cachePainter.drawLine(QPoint(r.left() + downRect.width(), r.bottom()), QPoint(r.right()- 2 - 1, r.bottom()));
                            cachePainter.drawLine(QPoint(r.left() + downRect.width(), r.top()), QPoint(r.right() - 2 - 1, r.top()));

                            cachePainter.drawLine(QPoint(r.right(), r.top() + 2), QPoint(r.right(), r.bottom() - 2));
                            cachePainter.drawPoint(QPoint(r.right() - 1, r.bottom() - 1));
                            cachePainter.drawPoint(QPoint(r.right() - 1, r.top() + 1));
                            cachePainter.drawLine(QPoint(r.left() + downRect.width() + 1, r.top()),
                                                  QPoint(r.left() + downRect.width() + 1, r.bottom()));
                        }
                    }
                }

                // outline the up/down buttons
                cachePainter.setPen(darkOutline);
                QColor light = option->palette.light().color().lighter();

                if (spinBox->direction == Qt::RightToLeft) {
                    cachePainter.drawLine(upRect.right(), upRect.top() - 1, upRect.right(), downRect.bottom() + 1);
                    cachePainter.setPen(light);
                    cachePainter.drawLine(upRect.right() - 1, upRect.top() + 3, upRect.right() - 1, downRect.bottom() );
                } else {
                    cachePainter.drawLine(upRect.left(), upRect.top() - 1, upRect.left(), downRect.bottom() + 1);
                    cachePainter.setPen(light);
                    cachePainter.drawLine(upRect.left() + 1, upRect.top() , upRect.left() + 1, downRect.bottom() );
                }
                if (upIsActive && sunken) {
                    cachePainter.setPen(gradientStopColor.darker(130));
                    cachePainter.drawLine(upRect.left() + 1, upRect.top(), upRect.left() + 1, upRect.bottom());
                    cachePainter.drawLine(upRect.left(), upRect.top() - 1, upRect.right(), upRect.top() - 1);
                } else {
                    cachePainter.setPen(light);
                    cachePainter.drawLine(upRect.topLeft() + QPoint(1, -1), upRect.topRight() + QPoint(-1, -1));
                    cachePainter.setPen(darkOutline);
                    cachePainter.drawLine(upRect.bottomLeft(), upRect.bottomRight());
                }
                if (downIsActive && sunken) {
                    cachePainter.setPen(gradientStopColor.darker(130));
                    cachePainter.drawLine(downRect.left() + 1, downRect.top(), downRect.left() + 1, downRect.bottom() + 1);
                    cachePainter.drawLine(downRect.left(), downRect.top(), downRect.right(), downRect.top());
                    cachePainter.setPen(gradientStopColor.darker(110));
                    cachePainter.drawLine(downRect.left(), downRect.bottom() + 1, downRect.right(), downRect.bottom() + 1);
                } else {
                    cachePainter.setPen(light);
                    cachePainter.drawLine(downRect.topLeft() + QPoint(2,0), downRect.topRight());
                }

                if (spinBox->buttonSymbols == QAbstractSpinBox::PlusMinus) {
                    int centerX = upRect.center().x();
                    int centerY = upRect.center().y();
                    cachePainter.setPen(spinBox->palette.foreground().color());

                    // plus/minus
                    if (spinBox->activeSubControls == SC_SpinBoxUp && sunken) {
                        cachePainter.drawLine(1 + centerX - 2, 1 + centerY, 1 + centerX + 2, 1 + centerY);
                        cachePainter.drawLine(1 + centerX, 1 + centerY - 2, 1 + centerX, 1 + centerY + 2);
                    } else {
                        cachePainter.drawLine(centerX - 2, centerY, centerX + 2, centerY);
                        cachePainter.drawLine(centerX, centerY - 2, centerX, centerY + 2);
                    }

                    centerX = downRect.center().x();
                    centerY = downRect.center().y();
                    if (spinBox->activeSubControls == SC_SpinBoxDown && sunken) {
                        cachePainter.drawLine(1 + centerX - 2, 1 + centerY, 1 + centerX + 2, 1 + centerY);
                    } else {
                        cachePainter.drawLine(centerX - 2, centerY, centerX + 2, centerY);
                    }
                } else if (spinBox->buttonSymbols == QAbstractSpinBox::UpDownArrows){
                    // arrows
                    QImage upArrow(qt_scrollbar_button_arrow_up);
                    upArrow.setColor(1, spinBox->palette.foreground().color().rgba());

                    cachePainter.drawImage(upRect.center().x() - upArrow.width() / 2,
                                            upRect.center().y() - upArrow.height() / 2,
                                            upArrow);

                    QImage downArrow(qt_scrollbar_button_arrow_down);
                    downArrow.setColor(1, spinBox->palette.foreground().color().rgba());

                    cachePainter.drawImage(downRect.center().x() - downArrow.width() / 2,
                                            downRect.center().y() - downArrow.height() / 2 + 1,
                                            downArrow);
                }

                QColor disabledColor = option->palette.background().color();
                disabledColor.setAlpha(150);
                if (!(spinBox->stepEnabled & QAbstractSpinBox::StepUpEnabled))
                    cachePainter.fillRect(upRect.adjusted(1, 0, 0, 0), disabledColor);
                if (!(spinBox->stepEnabled & QAbstractSpinBox::StepDownEnabled)) {
                    cachePainter.fillRect(downRect.adjusted(1, 0, 0, 0), disabledColor);
                }
                cachePainter.end();
                if (UsePixmapCache)
                    QPixmapCache::insert(pixmapName, cache);
            }
            painter->drawPixmap(spinBox->rect.topLeft(), cache);
        }
        break;
#endif // QT_NO_SPINBOX
    case CC_TitleBar:
        painter->save();
        if (const QStyleOptionTitleBar *titleBar = qstyleoption_cast<const QStyleOptionTitleBar *>(option)) {
            const int buttonMargin = 5;
            bool active = (titleBar->titleBarState & State_Active);
            QRect fullRect = titleBar->rect;
            QPalette palette = option->palette;
            QColor highlight = option->palette.highlight().color();

            QColor titleBarFrameBorder(active ? highlight.darker(180): dark.darker(110));
            QColor titleBarHighlight(active ? highlight.lighter(120): palette.background().color().lighter(120));
            QColor textColor(active ? 0xffffff : 0xff000000);
            QColor textAlphaColor(active ? 0xffffff : 0xff000000 );

#ifdef  QT3_SUPPORT
            if (widget && widget->inherits("Q3DockWindowTitleBar")) {
                QStyleOptionDockWidgetV2 dockwidget;
                dockwidget.QStyleOption::operator=(*option);
                drawControl(CE_DockWidgetTitle, &dockwidget, painter, widget);
            } else
#endif // QT3_SUPPORT
            {
                // Fill title bar gradient
                QColor titlebarColor = QColor(active ? highlight: palette.background().color());
                QColor titleBarGradientStop(active ? highlight.darker(150): palette.background().color().darker(120));
                QLinearGradient gradient(option->rect.center().x(), option->rect.top(),
                                         option->rect.center().x(), option->rect.bottom());

                gradient.setColorAt(0, titlebarColor.lighter(114));
                gradient.setColorAt(0.5, titlebarColor.lighter(102));
                gradient.setColorAt(0.51, titlebarColor.darker(104));
                gradient.setColorAt(1, titlebarColor);
                painter->fillRect(option->rect.adjusted(1, 1, -1, 0), gradient);

                // Frame and rounded corners
                painter->setPen(titleBarFrameBorder);

                // top outline
                painter->drawLine(fullRect.left() + 5, fullRect.top(), fullRect.right() - 5, fullRect.top());
                painter->drawLine(fullRect.left(), fullRect.top() + 4, fullRect.left(), fullRect.bottom());
                painter->drawPoint(fullRect.left() + 4, fullRect.top() + 1);
                painter->drawPoint(fullRect.left() + 3, fullRect.top() + 1);
                painter->drawPoint(fullRect.left() + 2, fullRect.top() + 2);
                painter->drawPoint(fullRect.left() + 1, fullRect.top() + 3);
                painter->drawPoint(fullRect.left() + 1, fullRect.top() + 4);

                painter->drawLine(fullRect.right(), fullRect.top() + 4, fullRect.right(), fullRect.bottom());
                painter->drawPoint(fullRect.right() - 3, fullRect.top() + 1);
                painter->drawPoint(fullRect.right() - 4, fullRect.top() + 1);
                painter->drawPoint(fullRect.right() - 2, fullRect.top() + 2);
                painter->drawPoint(fullRect.right() - 1, fullRect.top() + 3);
                painter->drawPoint(fullRect.right() - 1, fullRect.top() + 4);

                // draw bottomline
                painter->drawLine(fullRect.right(), fullRect.bottom(), fullRect.left(), fullRect.bottom());

                // top highlight
                painter->setPen(titleBarHighlight);
                painter->drawLine(fullRect.left() + 6, fullRect.top() + 1, fullRect.right() - 6, fullRect.top() + 1);
            }
            // draw title
            QRect textRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarLabel, widget);
            QFont font = painter->font();
            font.setBold(true);
            painter->setFont(font);
            painter->setPen(active? (titleBar->palette.text().color().lighter(120)) :
                                     titleBar->palette.text().color() );
            // Note workspace also does elliding but it does not use the correct font
            QString title = QFontMetrics(font).elidedText(titleBar->text, Qt::ElideRight, textRect.width() - 14);
            painter->drawText(textRect.adjusted(1, 1, 1, 1), title, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
            painter->setPen(Qt::white);
            if (active)
                painter->drawText(textRect, title, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
            // min button
            if ((titleBar->subControls & SC_TitleBarMinButton) && (titleBar->titleBarFlags & Qt::WindowMinimizeButtonHint) &&
                !(titleBar->titleBarState& Qt::WindowMinimized)) {
                QRect minButtonRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarMinButton, widget);
                if (minButtonRect.isValid()) {
                    bool hover = (titleBar->activeSubControls & SC_TitleBarMinButton) && (titleBar->state & State_MouseOver);
                    bool sunken = (titleBar->activeSubControls & SC_TitleBarMinButton) && (titleBar->state & State_Sunken);
                    qt_haiku_draw_mdibutton(painter, titleBar, minButtonRect, hover, sunken);
                    QRect minButtonIconRect = minButtonRect.adjusted(buttonMargin ,buttonMargin , -buttonMargin, -buttonMargin);
                    painter->setPen(textColor);
                    painter->drawLine(minButtonIconRect.center().x() - 2, minButtonIconRect.center().y() + 3,
                                    minButtonIconRect.center().x() + 3, minButtonIconRect.center().y() + 3);
                    painter->drawLine(minButtonIconRect.center().x() - 2, minButtonIconRect.center().y() + 4,
                                    minButtonIconRect.center().x() + 3, minButtonIconRect.center().y() + 4);
                    painter->setPen(textAlphaColor);
                    painter->drawLine(minButtonIconRect.center().x() - 3, minButtonIconRect.center().y() + 3,
                                    minButtonIconRect.center().x() - 3, minButtonIconRect.center().y() + 4);
                    painter->drawLine(minButtonIconRect.center().x() + 4, minButtonIconRect.center().y() + 3,
                                    minButtonIconRect.center().x() + 4, minButtonIconRect.center().y() + 4);
                }
            }
            // max button
            if ((titleBar->subControls & SC_TitleBarMaxButton) && (titleBar->titleBarFlags & Qt::WindowMaximizeButtonHint) &&
                !(titleBar->titleBarState & Qt::WindowMaximized)) {
                QRect maxButtonRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarMaxButton, widget);
                if (maxButtonRect.isValid()) {
                    bool hover = (titleBar->activeSubControls & SC_TitleBarMaxButton) && (titleBar->state & State_MouseOver);
                    bool sunken = (titleBar->activeSubControls & SC_TitleBarMaxButton) && (titleBar->state & State_Sunken);
                    qt_haiku_draw_mdibutton(painter, titleBar, maxButtonRect, hover, sunken);

                    QRect maxButtonIconRect = maxButtonRect.adjusted(buttonMargin, buttonMargin, -buttonMargin, -buttonMargin);

                    painter->setPen(textColor);
                    painter->drawRect(maxButtonIconRect.adjusted(0, 0, -1, -1));
                    painter->drawLine(maxButtonIconRect.left() + 1, maxButtonIconRect.top() + 1,
                                    maxButtonIconRect.right() - 1, maxButtonIconRect.top() + 1);
                    painter->setPen(textAlphaColor);
                    painter->drawPoint(maxButtonIconRect.topLeft());
                    painter->drawPoint(maxButtonIconRect.topRight());
                    painter->drawPoint(maxButtonIconRect.bottomLeft());
                    painter->drawPoint(maxButtonIconRect.bottomRight());
                }
            }

            // close button
            if ((titleBar->subControls & SC_TitleBarCloseButton) && (titleBar->titleBarFlags & Qt::WindowSystemMenuHint)) {
                QRect closeButtonRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarCloseButton, widget);
                if (closeButtonRect.isValid()) {
                    bool hover = (titleBar->activeSubControls & SC_TitleBarCloseButton) && (titleBar->state & State_MouseOver);
                    bool sunken = (titleBar->activeSubControls & SC_TitleBarCloseButton) && (titleBar->state & State_Sunken);
                    qt_haiku_draw_mdibutton(painter, titleBar, closeButtonRect, hover, sunken);
                    QRect closeIconRect = closeButtonRect.adjusted(buttonMargin, buttonMargin, -buttonMargin, -buttonMargin);
                    painter->setPen(textAlphaColor);
                    painter->drawLine(closeIconRect.left() + 1, closeIconRect.top(),
                                    closeIconRect.right(), closeIconRect.bottom() - 1);
                    painter->drawLine(closeIconRect.left(), closeIconRect.top() + 1,
                                    closeIconRect.right() - 1, closeIconRect.bottom());
                    painter->drawLine(closeIconRect.right() - 1, closeIconRect.top(),
                                    closeIconRect.left(), closeIconRect.bottom() - 1);
                    painter->drawLine(closeIconRect.right(), closeIconRect.top() + 1,
                                    closeIconRect.left() + 1, closeIconRect.bottom());
                    painter->drawPoint(closeIconRect.topLeft());
                    painter->drawPoint(closeIconRect.topRight());
                    painter->drawPoint(closeIconRect.bottomLeft());
                    painter->drawPoint(closeIconRect.bottomRight());

                    painter->setPen(textColor);
                    painter->drawLine(closeIconRect.left() + 1, closeIconRect.top() + 1,
                                    closeIconRect.right() - 1, closeIconRect.bottom() - 1);
                    painter->drawLine(closeIconRect.left() + 1, closeIconRect.bottom() - 1,
                                    closeIconRect.right() - 1, closeIconRect.top() + 1);
                }
            }

            // normalize button
            if ((titleBar->subControls & SC_TitleBarNormalButton) &&
               (((titleBar->titleBarFlags & Qt::WindowMinimizeButtonHint) &&
               (titleBar->titleBarState & Qt::WindowMinimized)) ||
               ((titleBar->titleBarFlags & Qt::WindowMaximizeButtonHint) &&
               (titleBar->titleBarState & Qt::WindowMaximized)))) {
                QRect normalButtonRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarNormalButton, widget);
                if (normalButtonRect.isValid()) {

                    bool hover = (titleBar->activeSubControls & SC_TitleBarNormalButton) && (titleBar->state & State_MouseOver);
                    bool sunken = (titleBar->activeSubControls & SC_TitleBarNormalButton) && (titleBar->state & State_Sunken);
                    QRect normalButtonIconRect = normalButtonRect.adjusted(buttonMargin, buttonMargin, -buttonMargin, -buttonMargin);
                    qt_haiku_draw_mdibutton(painter, titleBar, normalButtonRect, hover, sunken);

                    QRect frontWindowRect = normalButtonIconRect.adjusted(0, 3, -3, 0);
                    painter->setPen(textColor);
                    painter->drawRect(frontWindowRect.adjusted(0, 0, -1, -1));
                    painter->drawLine(frontWindowRect.left() + 1, frontWindowRect.top() + 1,
                                    frontWindowRect.right() - 1, frontWindowRect.top() + 1);
                    painter->setPen(textAlphaColor);
                    painter->drawPoint(frontWindowRect.topLeft());
                    painter->drawPoint(frontWindowRect.topRight());
                    painter->drawPoint(frontWindowRect.bottomLeft());
                    painter->drawPoint(frontWindowRect.bottomRight());

                    QRect backWindowRect = normalButtonIconRect.adjusted(3, 0, 0, -3);
                    QRegion clipRegion = backWindowRect;
                    clipRegion -= frontWindowRect;
                    painter->save();
                    painter->setClipRegion(clipRegion);
                    painter->setPen(textColor);
                    painter->drawRect(backWindowRect.adjusted(0, 0, -1, -1));
                    painter->drawLine(backWindowRect.left() + 1, backWindowRect.top() + 1,
                                    backWindowRect.right() - 1, backWindowRect.top() + 1);
                    painter->setPen(textAlphaColor);
                    painter->drawPoint(backWindowRect.topLeft());
                    painter->drawPoint(backWindowRect.topRight());
                    painter->drawPoint(backWindowRect.bottomLeft());
                    painter->drawPoint(backWindowRect.bottomRight());
                    painter->restore();
                }
            }

            // context help button
            if (titleBar->subControls & SC_TitleBarContextHelpButton
                && (titleBar->titleBarFlags & Qt::WindowContextHelpButtonHint)) {
                QRect contextHelpButtonRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarContextHelpButton, widget);
                if (contextHelpButtonRect.isValid()) {
                    bool hover = (titleBar->activeSubControls & SC_TitleBarContextHelpButton) && (titleBar->state & State_MouseOver);
                    bool sunken = (titleBar->activeSubControls & SC_TitleBarContextHelpButton) && (titleBar->state & State_Sunken);
                    qt_haiku_draw_mdibutton(painter, titleBar, contextHelpButtonRect, hover, sunken);

                    QColor blend;
                    QImage image(qt_titlebar_context_help);
                    QColor alpha = textColor;
                    alpha.setAlpha(128);
                    image.setColor(1, textColor.rgba());
                    image.setColor(2, alpha.rgba());
                    painter->setRenderHint(QPainter::SmoothPixmapTransform);
                    painter->drawImage(contextHelpButtonRect.adjusted(4, 4, -4, -4), image);
                }
            }

            // shade button
            if (titleBar->subControls & SC_TitleBarShadeButton && (titleBar->titleBarFlags & Qt::WindowShadeButtonHint)) {
                QRect shadeButtonRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarShadeButton, widget);
                if (shadeButtonRect.isValid()) {
                    bool hover = (titleBar->activeSubControls & SC_TitleBarShadeButton) && (titleBar->state & State_MouseOver);
                    bool sunken = (titleBar->activeSubControls & SC_TitleBarShadeButton) && (titleBar->state & State_Sunken);
                    qt_haiku_draw_mdibutton(painter, titleBar, shadeButtonRect, hover, sunken);
                    QImage image(qt_scrollbar_button_arrow_up);
                    image.setColor(1, textColor.rgba());
                    painter->drawImage(shadeButtonRect.adjusted(5, 7, -5, -7), image);
                }
            }

            // unshade button
            if (titleBar->subControls & SC_TitleBarUnshadeButton && (titleBar->titleBarFlags & Qt::WindowShadeButtonHint)) {
                QRect unshadeButtonRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarUnshadeButton, widget);
                if (unshadeButtonRect.isValid()) {
                    bool hover = (titleBar->activeSubControls & SC_TitleBarUnshadeButton) && (titleBar->state & State_MouseOver);
                    bool sunken = (titleBar->activeSubControls & SC_TitleBarUnshadeButton) && (titleBar->state & State_Sunken);
                    qt_haiku_draw_mdibutton(painter, titleBar, unshadeButtonRect, hover, sunken);
                    QImage image(qt_scrollbar_button_arrow_down);
                    image.setColor(1, textColor.rgba());
                    painter->drawImage(unshadeButtonRect.adjusted(5, 7, -5, -7), image);
                }
            }

            if ((titleBar->subControls & SC_TitleBarSysMenu) && (titleBar->titleBarFlags & Qt::WindowSystemMenuHint)) {
                QRect iconRect = subControlRect(CC_TitleBar, titleBar, SC_TitleBarSysMenu, widget);
                if (iconRect.isValid()) {
                    if (!titleBar->icon.isNull()) {
                        titleBar->icon.paint(painter, iconRect);
                    } else {
                        QStyleOption tool(0);
                        tool.palette = titleBar->palette;
                        QPixmap pm = standardIcon(SP_TitleBarMenuButton, &tool, widget).pixmap(16, 16);
                        tool.rect = iconRect;
                        painter->save();
                        drawItemPixmap(painter, iconRect, Qt::AlignCenter, pm);
                        painter->restore();
                    }
                }
            }
        }
        painter->restore();
        break;
#ifndef QT_NO_SCROLLBAR
    case CC_ScrollBar:
        painter->save();
        if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            bool isEnabled = scrollBar->state & State_Enabled;
            bool reverse = scrollBar->direction == Qt::RightToLeft;
            bool horizontal = scrollBar->orientation == Qt::Horizontal;
            bool sunken = scrollBar->state & State_Sunken;

            painter->fillRect(option->rect, option->palette.background());

            //QRect rect = scrollBar->rect;
            QRect scrollBarSubLine = subControlRect(control, scrollBar, SC_ScrollBarSubLine, widget);
            QRect scrollBarAddLine = subControlRect(control, scrollBar, SC_ScrollBarAddLine, widget);
            QRect scrollBarSlider = subControlRect(control, scrollBar, SC_ScrollBarSlider, widget);
            QRect grooveRect = subControlRect(control, scrollBar, SC_ScrollBarGroove, widget);

            // paint groove
            if (scrollBar->subControls & SC_ScrollBarGroove) {
                painter->setBrush(grooveColor);
                painter->setPen(Qt::NoPen);
                if (horizontal) {
                    painter->drawRect(grooveRect);
                    painter->setPen(darkOutline);
                    painter->drawLine(grooveRect.topLeft(), grooveRect.topRight());
                    painter->drawLine(grooveRect.bottomLeft(), grooveRect.bottomRight());
                } else {
                    painter->drawRect(grooveRect);
                    painter->setPen(darkOutline);
                    painter->drawLine(grooveRect.topLeft(), grooveRect.bottomLeft());
                    painter->drawLine(grooveRect.topRight(), grooveRect.bottomRight());
                }
            }
            //paint slider
            if (scrollBar->subControls & SC_ScrollBarSlider) {
                QRect pixmapRect = scrollBarSlider;
                if (horizontal)
                    pixmapRect.adjust(-1, 0, 0, -1);
                else
                    pixmapRect.adjust(0, -1, -1, 0);

                if (isEnabled) {
                    QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(),
                                             pixmapRect.center().x(), pixmapRect.bottom());
                    if (!horizontal)
                        gradient = QLinearGradient(pixmapRect.left(), pixmapRect.center().y(),
                                                   pixmapRect.right(), pixmapRect.center().y());

                    if (option->palette.button().gradient()) {
                        gradient.setStops(option->palette.button().gradient()->stops());
                    } else {
                        if (sunken || (option->state & State_MouseOver &&
                            (scrollBar->activeSubControls & SC_ScrollBarSlider))) {
                            gradient.setColorAt(0, gradientStartColor.lighter(110));
                            gradient.setColorAt(1, gradientStopColor.lighter(110));
                        } else {
                            gradient.setColorAt(0, gradientStartColor);
                            gradient.setColorAt(1, gradientStopColor);
                        }
                    }
                    painter->setPen(QPen(darkOutline, 0));
                    painter->setBrush(gradient);
                    painter->drawRect(pixmapRect);


                    //calculate offsets used by highlight and shadow
                    int yoffset, xoffset;
                    if (option->state & State_Horizontal) {
                        xoffset = 0;
                        yoffset = 1;
                    } else {
                        xoffset = 1;
                        yoffset = 0;
                    }
                    //draw slider highlights
                    painter->setPen(QPen(gradientStopColor, 0));
                    painter->drawLine(scrollBarSlider.left() + xoffset,
                                      scrollBarSlider.bottom() - yoffset,
                                      scrollBarSlider.right() - xoffset,
                                      scrollBarSlider.bottom() - yoffset);
                    painter->drawLine(scrollBarSlider.right() - xoffset,
                                      scrollBarSlider.top() + yoffset,
                                      scrollBarSlider.right() - xoffset,
                                      scrollBarSlider.bottom() - yoffset);

                    //draw slider shadow
                    painter->setPen(QPen(gradientStartColor, 0));
                    painter->drawLine(scrollBarSlider.left() + xoffset,
                                      scrollBarSlider.top() + yoffset,
                                      scrollBarSlider.right() - xoffset,
                                      scrollBarSlider.top() + yoffset);
                    painter->drawLine(scrollBarSlider.left() + xoffset,
                                      scrollBarSlider.top() + yoffset,
                                      scrollBarSlider.left() + xoffset,
                                      scrollBarSlider.bottom() - yoffset);
                } else {
                    QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(),
                                             pixmapRect.center().x(), pixmapRect.bottom());
                    if (!horizontal) {
                        gradient = QLinearGradient(pixmapRect.left(), pixmapRect.center().y(),
                                                   pixmapRect.right(), pixmapRect.center().y());
                    }
                    if (sunken) {
                        gradient.setColorAt(0, gradientStartColor.lighter(110));
                        gradient.setColorAt(1, gradientStopColor.lighter(110));
                    } else {
                        gradient.setColorAt(0, gradientStartColor);
                        gradient.setColorAt(1, gradientStopColor);
                    }
                    painter->setPen(darkOutline);
                    painter->setBrush(gradient);
                    painter->drawRect(pixmapRect);
                }
                int gripMargin = 4;
                //draw grips
                if (horizontal) {
                    for (int i = -3; i< 6 ; i += 3) {
                        painter->setPen(QPen(gripShadow, 1));
                        painter->drawLine(
                            QPoint(scrollBarSlider.center().x() + i ,
                                   scrollBarSlider.top() + gripMargin),
                            QPoint(scrollBarSlider.center().x() + i,
                                   scrollBarSlider.bottom() - gripMargin));
                        painter->setPen(QPen(palette.light(), 1));
                        painter->drawLine(
                            QPoint(scrollBarSlider.center().x() + i + 1,
                                   scrollBarSlider.top() + gripMargin  ),
                            QPoint(scrollBarSlider.center().x() + i + 1,
                                   scrollBarSlider.bottom() - gripMargin));
                    }
                } else {
                    for (int i = -3; i < 6 ; i += 3) {
                        painter->setPen(QPen(gripShadow, 1));
                        painter->drawLine(
                            QPoint(scrollBarSlider.left() + gripMargin ,
                                   scrollBarSlider.center().y()+ i),
                            QPoint(scrollBarSlider.right() - gripMargin,
                                   scrollBarSlider.center().y()+ i));
                        painter->setPen(QPen(palette.light(), 1));
                        painter->drawLine(
                            QPoint(scrollBarSlider.left() + gripMargin,
                                   scrollBarSlider.center().y() + 1 + i),
                            QPoint(scrollBarSlider.right() - gripMargin,
                                   scrollBarSlider.center().y() + 1 + i));
                    }
                }
            }

            // The SubLine (up/left) buttons
            if (scrollBar->subControls & SC_ScrollBarSubLine) {
                //int scrollBarExtent = pixelMetric(PM_ScrollBarExtent, option, widget);
                QRect pixmapRect = scrollBarSubLine;
                if (isEnabled ) {
                    QRect fillRect = pixmapRect.adjusted(1, 1, -1, -1);
                    // Gradients
                    if ((scrollBar->activeSubControls & SC_ScrollBarSubLine) && sunken) {
                        qt_haiku_draw_gradient(painter,
                                                    QRect(fillRect),
                                                    gradientStopColor.darker(120),
                                                    gradientStopColor.darker(120),
                                                    horizontal ? TopDown : FromLeft, option->palette.button());
                    } else {
                        qt_haiku_draw_gradient(painter,
                                                    QRect(fillRect),
                                                    gradientStartColor.lighter(105),
                                                    gradientStopColor,
                                                    horizontal ? TopDown : FromLeft, option->palette.button());
                    }
                }
                // Details
                QImage subButton;
                QImage subButtonArrow;
                if (horizontal) {
                    subButton = QImage(reverse ? qt_scrollbar_buttons : qt_scrollbar_buttons);
                    subButtonArrow = QImage(reverse ? qt_scrollbar_button_arrow_right : qt_scrollbar_button_arrow_left);
                } else {
                    subButton = QImage(qt_scrollbar_buttons);
                    subButtonArrow = QImage(qt_scrollbar_button_arrow_up);
                }
                subButton.setColor(1, alphaCornerColor.rgba());
                subButton.setColor(2, darkOutline.rgba());
                if ((scrollBar->activeSubControls & SC_ScrollBarSubLine) && sunken) {
                    subButton.setColor(3, gradientStopColor.darker(140).rgba());
                    subButton.setColor(4, gradientStopColor.darker(120).rgba());
                } else {
                    subButton.setColor(3, gradientStartColor.lighter(105).rgba());
                    subButton.setColor(4, gradientStopColor.rgba());
                }
                subButton.setColor(5, scrollBar->palette.text().color().rgba());
                painter->drawImage(pixmapRect, subButton);

                // Arrows
                QPoint a_pos = pixmapRect.topLeft()+QPoint(4,4);
                painter->drawImage(a_pos, subButtonArrow);

                // The AddLine (down/right) button
                if (scrollBar->subControls & SC_ScrollBarAddLine) {
                    QString addLinePixmapName = uniqueName(QLatin1String("scrollbar_addline"), option, QSize(16, 16));
                    QRect pixmapRect = scrollBarAddLine;
                    if (isEnabled) {
                        QRect fillRect = pixmapRect.adjusted(1, 1, -1, -1);
                        // Gradients
                        if ((scrollBar->activeSubControls & SC_ScrollBarAddLine) && sunken) {
                            qt_haiku_draw_gradient(painter,
                                                        fillRect,
                                                        gradientStopColor.darker(120),
                                                        gradientStopColor.darker(120),
                                                        horizontal ? TopDown: FromLeft, option->palette.button());
                        } else {
                            qt_haiku_draw_gradient(painter,
                                                        fillRect,
                                                        gradientStartColor.lighter(105),
                                                        gradientStopColor,
                                                        horizontal ? TopDown : FromLeft, option->palette.button());
                        }
                    }
                    // Details
                    QImage addButton;
                    QImage subButtonArrow;
                    if (horizontal) {
                        addButton = QImage(reverse ? qt_scrollbar_buttons : qt_scrollbar_buttons);
                        subButtonArrow = QImage(reverse ? qt_scrollbar_button_arrow_left : qt_scrollbar_button_arrow_right);
                    } else {
                        addButton = QImage(qt_scrollbar_buttons);
                        subButtonArrow = QImage(qt_scrollbar_button_arrow_down);
                    }
                    addButton.setColor(1, alphaCornerColor.rgba());
                    addButton.setColor(2, darkOutline.rgba());
                    if ((scrollBar->activeSubControls & SC_ScrollBarAddLine) && sunken) {
                        addButton.setColor(3, gradientStopColor.darker(140).rgba());
                        addButton.setColor(4, gradientStopColor.darker(120).rgba());
                    } else {
                        addButton.setColor(3, gradientStartColor.lighter(105).rgba());
                        addButton.setColor(4, gradientStopColor.rgba());
                    }
                    addButton.setColor(5, scrollBar->palette.text().color().rgba());
                    painter->drawImage(pixmapRect, addButton);

                    QPoint a_pos = pixmapRect.topLeft()+QPoint(4,4);
                    painter->drawImage(a_pos, subButtonArrow);
                }
            }
        }
        painter->restore();
        break;;
#endif // QT_NO_SCROLLBAR
#ifndef QT_NO_COMBOBOX
    case CC_ComboBox:
        painter->save();
        if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            bool sunken = comboBox->state & State_On; // play dead, if combobox has no items
            bool isEnabled = (comboBox->state & State_Enabled);
            bool focus = isEnabled && (comboBox->state & State_HasFocus);
            QPixmap cache;
            QString pixmapName = uniqueName(QLatin1String("combobox"), option, comboBox->rect.size());
            if (sunken)
                pixmapName += QLatin1String("-sunken");
            if (comboBox->editable)
                pixmapName += QLatin1String("-editable");
            if (isEnabled)
                pixmapName += QLatin1String("-enabled");

            if (!UsePixmapCache || !QPixmapCache::find(pixmapName, cache)) {
                cache = QPixmap(comboBox->rect.size());
                cache.fill(Qt::transparent);
                QPainter cachePainter(&cache);
                QRect pixmapRect(0, 0, comboBox->rect.width(), comboBox->rect.height());
                QStyleOptionComboBox comboBoxCopy = *comboBox;
                comboBoxCopy.rect = pixmapRect;

                QRect rect = pixmapRect;
                QRect downArrowRect = subControlRect(CC_ComboBox, &comboBoxCopy,
                                                     SC_ComboBoxArrow, widget);
                // Draw a push button
                if (comboBox->editable) {
                    QStyleOptionFrame  buttonOption;
                    buttonOption.QStyleOption::operator=(*comboBox);
                    buttonOption.rect = rect;
                    buttonOption.state = comboBox->state & (State_Enabled | State_MouseOver);

                    if (sunken) {
                        buttonOption.state |= State_Sunken;
                        buttonOption.state &= ~State_MouseOver;
                    }

                    drawPrimitive(PE_PanelButtonCommand, &buttonOption, &cachePainter, widget);

                    if (isEnabled) {
                        if(comboBox->state & State_HasFocus)
                            cachePainter.setPen(QPen(mkQColor(ui_color(B_NAVIGATION_BASE_COLOR)), 0));
                        else
                            cachePainter.setPen(QPen(QColor(129,129,129), 0));
                        cachePainter.drawRect(rect.adjusted(2, 2, -3, -2));
                    } else {
                        QColor buttonBgColor = mkQColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_LIGHTEN_1_TINT));
                        QColor dark1BorderColor = mkQColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
                        cachePainter.fillRect(rect, option->palette.background());
                        cachePainter.fillRect(rect.adjusted(2, 2, -3, -2), buttonBgColor);
                        cachePainter.setPen(QPen(dark1BorderColor, 0));
                        cachePainter.drawRect(rect.adjusted(2, 2, -3, -2));
                    }
                } else {
                    QStyleOptionButton buttonOption;
                    buttonOption.QStyleOption::operator=(*comboBox);
                    buttonOption.rect = rect;
                    buttonOption.state = comboBox->state & (State_Enabled | State_MouseOver);
                    if (sunken) {
                        buttonOption.state |= State_Sunken;
                        buttonOption.state &= ~State_MouseOver;
                    }
                    drawPrimitive(PE_PanelButtonCommand, &buttonOption, &cachePainter, widget);

                    cachePainter.setPen(buttonShadow.darker(102));
                    int borderSize = 4;

                    if (!sunken) {
                        if (comboBox->direction == Qt::RightToLeft) {
                            cachePainter.drawLine(QPoint(downArrowRect.right() + 1, downArrowRect.top() + borderSize),
                                                  QPoint(downArrowRect.right() + 1, downArrowRect.bottom() - borderSize));
                            cachePainter.setPen(option->palette.light().color());
                            cachePainter.drawLine(QPoint(downArrowRect.right(), downArrowRect.top() + borderSize),
                                                  QPoint(downArrowRect.right(), downArrowRect.bottom() - borderSize));
                        } else {
                            cachePainter.drawLine(QPoint(downArrowRect.left() - 1, downArrowRect.top() + borderSize),
                                                  QPoint(downArrowRect.left() - 1, downArrowRect.bottom() - borderSize));
                            cachePainter.setPen(option->palette.light().color());
                            cachePainter.drawLine(QPoint(downArrowRect.left() , downArrowRect.top() + borderSize),
                                                  QPoint(downArrowRect.left() , downArrowRect.bottom() - borderSize));
                        }
                    } else {
                        cachePainter.setPen(dark.lighter(110));
                        if (comboBox->direction == Qt::RightToLeft) {
                            cachePainter.drawLine(QPoint(downArrowRect.right() + 1, downArrowRect.top() + borderSize),
                                                  QPoint(downArrowRect.right() + 1, downArrowRect.bottom() - borderSize));

                        } else {
                            cachePainter.drawLine(QPoint(downArrowRect.left() - 1, downArrowRect.top() + borderSize),
                                                  QPoint(downArrowRect.left() - 1, downArrowRect.bottom() - borderSize));
                        }
                    }
                }


                if (comboBox->subControls & SC_ComboBoxArrow) {
                    if (comboBox->editable) {
                        // Draw the down arrow
                        QImage downArrow(qt_scrollbar_button_arrow_down);
                        downArrow.setColor(1, comboBox->palette.foreground().color().rgba());
                        cachePainter.drawImage(downArrowRect.center().x() - downArrow.width() / 2,
                                               downArrowRect.center().y() - downArrow.height() / 2 + 1, downArrow);
                    } else {
                        // Draw the up/down arrow
                        QImage upArrow(qt_scrollbar_button_arrow_up);
                        upArrow.setColor(1, comboBox->palette.foreground().color().rgba());
                        QImage downArrow(qt_scrollbar_button_arrow_down);
                        downArrow.setColor(1, comboBox->palette.foreground().color().rgba());
                        cachePainter.drawImage(downArrowRect.center().x() - downArrow.width() / 2,
                                               downArrowRect.center().y() - upArrow.height() - 1 , upArrow);
                        cachePainter.drawImage(downArrowRect.center().x() - downArrow.width() / 2,
                                               downArrowRect.center().y()  + 2, downArrow);
                    }
                }
                // Draw the focus rect
                if ((focus && (option->state & State_KeyboardFocusChange)) && !comboBox->editable) {
                    QStyleOptionFocusRect focus;
                    focus.rect = subControlRect(CC_ComboBox, &comboBoxCopy, SC_ComboBoxEditField, widget)
                                 .adjusted(0, 2, option->direction == Qt::RightToLeft ? 1 : -1, -2);
                    drawPrimitive(PE_FrameFocusRect, &focus, &cachePainter, widget);
                }
                cachePainter.end();
                if (UsePixmapCache)
                    QPixmapCache::insert(pixmapName, cache);
            }
            painter->drawPixmap(comboBox->rect.topLeft(), cache);
        }
        painter->restore();
        break;
#endif // QT_NO_COMBOBOX
#ifndef QT_NO_GROUPBOX
    case CC_GroupBox:
        painter->save();
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            QRect textRect = subControlRect(CC_GroupBox, groupBox, SC_GroupBoxLabel, widget);
            QRect checkBoxRect = subControlRect(CC_GroupBox, groupBox, SC_GroupBoxCheckBox, widget);
            bool flat = groupBox->features & QStyleOptionFrameV2::Flat;

            if(!flat) {
                if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
                    QStyleOptionFrameV2 frame;
                    frame.QStyleOption::operator=(*groupBox);
                    frame.features = groupBox->features;
                    frame.lineWidth = groupBox->lineWidth;
                    frame.midLineWidth = groupBox->midLineWidth;
                    frame.rect = subControlRect(CC_GroupBox, option, SC_GroupBoxFrame, widget);

                    painter->save();
                    QRegion region(groupBox->rect);
                    bool ltr = groupBox->direction == Qt::LeftToRight;
                    region -= checkBoxRect.united(textRect).adjusted(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
                    if (!groupBox->text.isEmpty() ||  groupBox->subControls & SC_GroupBoxCheckBox)
                        painter->setClipRegion(region);
                    frame.palette.setBrush(QPalette::Dark, option->palette.mid().color().lighter(110));
                    drawPrimitive(PE_FrameGroupBox, &frame, painter);
                    painter->restore();
                }
            }
            // Draw title
            if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
                if (!groupBox->text.isEmpty()) {
                    QColor textColor = groupBox->textColor;
                    if (textColor.isValid())
                        painter->setPen(textColor);
                    int alignment = int(groupBox->textAlignment);
                    if (!styleHint(QStyle::SH_UnderlineShortcut, option, widget))
                        alignment |= Qt::TextHideMnemonic;
                    if (flat) {
                        QFont font = painter->font();
                        font.setBold(true);
                        painter->setFont(font);
                        if (groupBox->subControls & SC_GroupBoxCheckBox) {
                            textRect.adjust(checkBoxRect.right() + 4, 0, checkBoxRect.right() + 4, 0);
                        }
                    }
                    painter->drawText(textRect, Qt::TextShowMnemonic | Qt::AlignLeft| alignment, groupBox->text);
                }
            }
            if (groupBox->subControls & SC_GroupBoxCheckBox) {
                QStyleOptionButton box;
                box.QStyleOption::operator=(*groupBox);
                box.rect = checkBoxRect;
                drawPrimitive(PE_IndicatorCheckBox, &box, painter, widget);
            }
        }
        painter->restore();
        break;
#endif // QT_NO_GROUPBOX
#ifndef QT_NO_SLIDER
    case CC_Slider:
        painter->save();
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            QRect groove = subControlRect(CC_Slider, option, SC_SliderGroove, widget);
            QRect handle = subControlRect(CC_Slider, option, SC_SliderHandle, widget);
            //QRect ticks = subControlRect(CC_Slider, option, SC_SliderTickmarks, widget);

            bool ticksAbove = slider->tickPosition & QSlider::TicksAbove;
            bool ticksBelow = slider->tickPosition & QSlider::TicksBelow;

            orientation orient = slider->orientation == Qt::Horizontal?B_HORIZONTAL:B_VERTICAL;

            if (be_control_look != NULL) {
                QRect r = groove;
                rgb_color base = ui_color(B_PANEL_BACKGROUND_COLOR);
                rgb_color fill_color = ui_color(B_PANEL_BACKGROUND_COLOR);
                uint32 flags = 0;

                BRect bRect(0.0f, 0.0f, option->rect.width() - 1,  option->rect.height() - 1);
                TemporarySurface surface(bRect);

                surface.view()->SetHighColor(base);
                surface.view()->SetLowColor(base);
                surface.view()->FillRect(bRect);

                if ((option->subControls & SC_SliderGroove) && groove.isValid()) {
                    r = groove;
                    bRect = BRect(r.x(), r.y(), r.x()+r.width(), r.y()+r.height());
                    be_control_look->DrawSliderBar(surface.view(), bRect, bRect, base, fill_color, flags, orient);
                    painter->drawImage(r, surface.image());
                }

                if (option->subControls & SC_SliderTickmarks) {
                    int mlocation = B_HASH_MARKS_NONE;
                    if(ticksAbove)mlocation|=B_HASH_MARKS_TOP;
                    if(ticksBelow)mlocation|=B_HASH_MARKS_BOTTOM;
                    int interval =  slider->tickInterval<=0?1:slider->tickInterval;
                    int num = 1+((slider->maximum-slider->minimum)/interval);
                    int len = pixelMetric(PM_SliderLength, slider, widget)/2;
                    r=(orient==B_HORIZONTAL)?option->rect.adjusted(len,0,-len,0):option->rect.adjusted(0,len,0,-len);
                    bRect = BRect(r.x(), r.y(), r.x()+r.width(), r.y()+r.height());
                    be_control_look->DrawSliderHashMarks(surface.view(), bRect, bRect, base, num, (hash_mark_location)mlocation, flags, orient);
                }

                if (option->subControls & SC_SliderHandle ) {
                    r=handle.adjusted(1,1,0,0);
                    bRect = BRect(r.x(), r.y(), r.x()+r.width(), r.y()+r.height());
                    be_control_look->DrawSliderThumb(surface.view(), bRect, bRect, base, flags, orient);
                }

                painter->drawImage(slider->rect, surface.image());
            }

            painter->restore();
        }
        break;
#endif // QT_NO_SLIDER
        case CC_ToolButton:
        {
        if (const QStyleOptionToolButton *toolbutton
                = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            QRect button;
            bool flat = !(widget ? qobject_cast<QToolBar*>(widget->parentWidget()) : 0);
            bool flat2 = toolbutton->features & QStyleOptionFrameV2::Flat;
            bool enabled = toolbutton->state & State_Enabled;
            bool pushed = (toolbutton->state & State_Sunken) || (toolbutton->state & State_On);

            button = subControlRect(control, toolbutton, SC_ToolButton, widget);

            painter->save();
            if(flat || flat2 || pushed) {
                qt_haiku_draw_button(painter,button,false,false,pushed,false,enabled);
            }

            painter->restore();
            }
            QWindowsStyle::drawComplexControl(control, option, painter, widget);
        }
        break;
        default:
            QWindowsStyle::drawComplexControl(control, option, painter, widget);
        break;
    }
}

/*!
  \reimp
*/
int QHaikuStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    int ret = -1;
    switch (metric) {
    case PM_ButtonDefaultIndicator:
        ret = 0;
        break;
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
        ret = 0;
        break;
    case PM_MessageBoxIconSize:
        ret = 48;
        break;
    case PM_ListViewIconSize:
        ret = 24;
        break;
    case PM_DialogButtonsSeparator:
    case PM_SplitterWidth:
        ret = 6;
        break;
    case PM_ScrollBarSliderMin:
        ret = 26;
        break;
    case PM_MenuPanelWidth: //menu framewidth
        ret = 2;
        break;
    case PM_TitleBarHeight:
        ret = 24;
        break;
    case PM_ScrollBarExtent:
        ret = 15;
        break;
    case PM_SliderThickness:
        ret = 14;
        break;
    case PM_SliderLength:
        ret = 18;
        break;
    case PM_DockWidgetTitleMargin:
        ret = 1;
        break;
    case PM_MenuBarVMargin:
        ret = 1;
        break;
    case PM_DefaultFrameWidth:
        ret = 2;
        break;
    case PM_SpinBoxFrameWidth:
        ret = 3;
        break;
    case PM_MenuBarItemSpacing:
        ret = 6;
    case PM_MenuBarHMargin:
        ret = 0;
        break;
    case PM_ToolBarHandleExtent:
        ret = 9;
        break;
    case PM_ToolBarItemSpacing:
        ret = 2;
        break;
    case PM_ToolBarFrameWidth:
        ret = 0;
        break;
    case PM_ToolBarItemMargin:
        ret = 1;
        break;
    case PM_SmallIconSize:
        ret = 16;
        break;
    case PM_ButtonIconSize:
        ret = 24;
        break;
    case PM_MenuVMargin:
    case PM_MenuHMargin:
        ret = 0;
        break;
    case PM_DockWidgetTitleBarButtonMargin:
        ret = 4;
        break;
    case PM_MaximumDragDistance:
        return -1;
    case PM_TabCloseIndicatorWidth:
    case PM_TabCloseIndicatorHeight:
        return 20;
    default:
        break;
    }

    return ret != -1 ? ret : QWindowsStyle::pixelMetric(metric, option, widget);
}

/*!
  \reimp
*/
QSize QHaikuStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
                                        const QSize &size, const QWidget *widget) const
{
    QSize newSize = QWindowsStyle::sizeFromContents(type, option, size, widget);
    switch (type) {
    case CT_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            if (!btn->text.isEmpty() && newSize.width() < 80)
                newSize.setWidth(80);
/*            if (!btn->icon.isNull() && btn->iconSize.height() > 16)
                newSize -= QSize(0, 2);*/
        }
        if (const QPushButton *button = qobject_cast<const QPushButton *>(widget)) {
            if (qobject_cast<const QDialogButtonBox *>(button->parentWidget())) {
                if (newSize.height() < 32)
                    newSize.setHeight(32);
            }
        }
        break;
    case CT_GroupBox:
    case CT_RadioButton:
    case CT_CheckBox:
        newSize += QSize(3, 2);
        break;
    case CT_ToolButton:
#ifndef QT_NO_TOOLBAR
        if (widget && qobject_cast<QToolBar *>(widget->parentWidget()))
            newSize += QSize(4, 6);
#endif // QT_NO_TOOLBAR
        break;
    case CT_SpinBox:
        newSize += QSize(0, -2);
        break;
    case CT_ComboBox:
        newSize += QSize(1, 4);
        break;
    case CT_LineEdit:
        newSize += QSize(0, 2);
        break;
    case CT_MenuBarItem:
        newSize += QSize(0, -2);
        break;
    case CT_MenuItem:
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
                if (!menuItem->text.isEmpty()) {
                    newSize.setHeight(menuItem->fontMetrics.lineSpacing());
                }
            }
#ifndef QT_NO_COMBOBOX
            else if (!menuItem->icon.isNull()) {
                if (const QComboBox *combo = qobject_cast<const QComboBox*>(widget)) {
                    newSize.setHeight(qMax(combo->iconSize().height() + 2, newSize.height()));
                }
            }
#endif // QT_NO_COMBOBOX
        }
        break;
    case CT_MdiControls:
        if (const QStyleOptionComplex *styleOpt = qstyleoption_cast<const QStyleOptionComplex *>(option)) {
            int width = 0;
            if (styleOpt->subControls & SC_MdiMinButton)
                width += 19 + 1;
            if (styleOpt->subControls & SC_MdiNormalButton)
                width += 19 + 1;
            if (styleOpt->subControls & SC_MdiCloseButton)
                width += 19 + 1;
            newSize = QSize(width, 19);
        } else {
            newSize = QSize(60, 19);
        }
        break;
    default:
        break;
    }
    return newSize;
}

/*!
  \reimp
*/
void QHaikuStyle::polish(QApplication *app)
{
    QWindowsStyle::polish(app);
}

/*!
  \reimp
*/
void QHaikuStyle::polish(QWidget *widget)
{
    QWindowsStyle::polish(widget);
    if (qobject_cast<QAbstractButton*>(widget)
#ifndef QT_NO_COMBOBOX
        || qobject_cast<QComboBox *>(widget)
#endif
#ifndef QT_NO_PROGRESSBAR
        || qobject_cast<QProgressBar *>(widget)
#endif
#ifndef QT_NO_SCROLLBAR
        || qobject_cast<QScrollBar *>(widget)
#endif
#ifndef QT_NO_SPLITTER
        || qobject_cast<QSplitterHandle *>(widget)
#endif
        || qobject_cast<QAbstractSlider *>(widget)
#ifndef QT_NO_SPINBOX
        || qobject_cast<QAbstractSpinBox *>(widget)
#endif
        || (widget->inherits("QWorkspaceChild"))
        || (widget->inherits("QDockSeparator"))
        || (widget->inherits("QDockWidgetSeparator"))
        ) {
        widget->setAttribute(Qt::WA_Hover, true);
    }
}

/*!
  \reimp
*/
void QHaikuStyle::polish(QPalette &pal)
{
    QWindowsStyle::polish(pal);
    //this is a workaround for some themes such as Human, where the contrast
    //between text and background is too low.
    QColor highlight = pal.highlight().color();
    QColor highlightText = pal.highlightedText().color();
    if (qAbs(qGray(highlight.rgb()) - qGray(highlightText.rgb())) < 150) {
        if (qGray(highlightText.rgb()) < 128)
            pal.setBrush(QPalette::Highlight, highlight.lighter(145));
    }
}

/*!
  \reimp
*/
void QHaikuStyle::unpolish(QWidget *widget)
{
    QWindowsStyle::unpolish(widget);
    if (qobject_cast<QAbstractButton*>(widget)
#ifndef QT_NO_COMBOBOX
        || qobject_cast<QComboBox *>(widget)
#endif
#ifndef QT_NO_PROGRESSBAR
        || qobject_cast<QProgressBar *>(widget)
#endif
#ifndef QT_NO_SCROLLBAR
        || qobject_cast<QScrollBar *>(widget)
#endif
#ifndef QT_NO_SPLITTER
        || qobject_cast<QSplitterHandle *>(widget)
#endif
        || qobject_cast<QAbstractSlider *>(widget)
#ifndef QT_NO_SPINBOX
        || qobject_cast<QAbstractSpinBox *>(widget)
#endif
        || (widget->inherits("QWorkspaceChild"))
        || (widget->inherits("QDockSeparator"))
        || (widget->inherits("QDockWidgetSeparator"))
        ) {
        widget->setAttribute(Qt::WA_Hover, false);
    }
}

/*!
  \reimp
*/
void QHaikuStyle::unpolish(QApplication *app)
{
    QWindowsStyle::unpolish(app);
}

/*!
  \reimp
*/
QRect QHaikuStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                       SubControl subControl, const QWidget *widget) const
{
    QRect rect = QWindowsStyle::subControlRect(control, option, subControl, widget);

    switch (control) {
#ifndef QT_NO_SLIDER
    case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            int tickSize = pixelMetric(PM_SliderTickmarkOffset, option, widget);
            switch (subControl) {
            case SC_SliderHandle: {
                if (slider->orientation == Qt::Horizontal) {
                    rect.setHeight(pixelMetric(PM_SliderThickness));
                    rect.setWidth(pixelMetric(PM_SliderLength));
                    int centerY = slider->rect.center().y() - rect.height() / 2;
                    if (slider->tickPosition & QSlider::TicksAbove)
                        centerY += tickSize;
                    if (slider->tickPosition & QSlider::TicksBelow)
                        centerY -= tickSize;
                    rect.moveTop(centerY);
                } else {
                    rect.setWidth(pixelMetric(PM_SliderThickness));
                    rect.setHeight(pixelMetric(PM_SliderLength));
                    int centerX = slider->rect.center().x() - rect.width() / 2;
                    if (slider->tickPosition & QSlider::TicksAbove)
                        centerX += tickSize;
                    if (slider->tickPosition & QSlider::TicksBelow)
                        centerX -= tickSize;
                    rect.moveLeft(centerX);
                }
            }
                break;
            case SC_SliderGroove: {
                QPoint grooveCenter = slider->rect.center();
                if (slider->orientation == Qt::Horizontal) {
                    rect.setHeight(7);
                    if (slider->tickPosition & QSlider::TicksAbove)
                        grooveCenter.ry() += tickSize;
                    if (slider->tickPosition & QSlider::TicksBelow)
                        grooveCenter.ry() -= tickSize;
                } else {
                    rect.setWidth(7);
                    if (slider->tickPosition & QSlider::TicksAbove)
                        grooveCenter.rx() += tickSize;
                    if (slider->tickPosition & QSlider::TicksBelow)
                        grooveCenter.rx() -= tickSize;
                }
                rect.moveCenter(grooveCenter);
                break;
            }
            default:
                break;
            }
        }
        break;
#endif // QT_NO_SLIDER
    case CC_ScrollBar:
        break;
#ifndef QT_NO_SPINBOX
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            QSize bs;
            int center = spinbox->rect.height() / 2;
            int fw = spinbox->frame ? pixelMetric(PM_SpinBoxFrameWidth, spinbox, widget) : 0;
            int y = fw;
            bs.setHeight(qMax(8, spinbox->rect.height()/2 - y));
            bs.setWidth(15);
            int x, lx, rx;
            x = spinbox->rect.width() - y - bs.width() + 2;
            lx = fw;
            rx = x - fw;
            switch (subControl) {
            case SC_SpinBoxUp:
                if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                    return QRect();
                rect = QRect(x, fw, bs.width(), center - fw);
                break;
            case SC_SpinBoxDown:
                if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons)
                    return QRect();

                rect = QRect(x, center, bs.width(), spinbox->rect.bottom() - center - fw + 1);
                break;
            case SC_SpinBoxEditField:
                if (spinbox->buttonSymbols == QAbstractSpinBox::NoButtons) {
                    rect = QRect(lx, fw, spinbox->rect.width() - 2*fw, spinbox->rect.height() - 2*fw);
                } else {
                    rect = QRect(lx, fw, rx - qMax(fw - 1, 0), spinbox->rect.height() - 2*fw);
                }
                break;
            case SC_SpinBoxFrame:
                rect = spinbox->rect;
            default:
                break;
            }
            rect = visualRect(spinbox->direction, spinbox->rect, rect);
        }
        break;
#endif // Qt_NO_SPINBOX
#ifndef QT_NO_GROUPBOX
    case CC_GroupBox:
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            int topMargin = 0;
            int topHeight = 0;
            int verticalAlignment = styleHint(SH_GroupBox_TextLabelVerticalAlignment, groupBox, widget);
            bool flat = groupBox->features & QStyleOptionFrameV2::Flat;
            if (!groupBox->text.isEmpty()) {
                topHeight = groupBox->fontMetrics.height();
                if (verticalAlignment & Qt::AlignVCenter)
                    topMargin = topHeight / 2;
                else if (verticalAlignment & Qt::AlignTop)
                    topMargin = topHeight;
            }
            QRect frameRect = groupBox->rect;
            frameRect.setTop(topMargin);
            if (subControl == SC_GroupBoxFrame) {
                return rect;
            }
            else if (subControl == SC_GroupBoxContents) {
                if( flat ) {
                    int margin = 0;
                    int leftMarginExtension = 16;
                    rect = frameRect.adjusted(leftMarginExtension + margin, margin + topHeight, -margin, -margin);
                }
                break;
            }
            if(flat) {
                if (const QGroupBox *groupBoxWidget = qobject_cast<const QGroupBox *>(widget)) {
                    //Prepare metrics for a bold font
                    QFont font = widget->font();
                    font.setBold(true);
                    QFontMetrics fontMetrics(font);

                    QSize textRect = fontMetrics.boundingRect(groupBoxWidget->title()).size() + QSize(2, 2);
                    if (subControl == SC_GroupBoxCheckBox) {
                        int indicatorWidth = pixelMetric(PM_IndicatorWidth, option, widget);
                        int indicatorHeight = pixelMetric(PM_IndicatorHeight, option, widget);
                        rect.setWidth(indicatorWidth);
                        rect.setHeight(indicatorHeight);
                        rect.moveTop((fontMetrics.height() - indicatorHeight) / 2 + 2);
                    } else if (subControl == SC_GroupBoxLabel) {
                        rect.setSize(textRect);
                    }
                }
            }
        }
        return rect;
#ifndef QT_NO_COMBOBOX
    case CC_ComboBox:
        switch (subControl) {
        case SC_ComboBoxArrow:
            rect = visualRect(option->direction, option->rect, rect);
            rect.setRect(rect.right() - 15, rect.top() - 2,
                         16, rect.height() + 4);
            rect = visualRect(option->direction, option->rect, rect);
            break;
        case SC_ComboBoxEditField: {
            int frameWidth = pixelMetric(PM_DefaultFrameWidth)+1;
            rect = visualRect(option->direction, option->rect, rect);
            rect.setRect(option->rect.left() + frameWidth, option->rect.top() + frameWidth,
                         option->rect.width() - 16 - 2 * frameWidth,
                         option->rect.height() - 2 * frameWidth);
            if (const QStyleOptionComboBox *box = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
                if (!box->editable) {
                    rect.adjust(2, 0, 0, 0);
                    if (box->state & (State_Sunken | State_On))
                        rect.translate(1, 1);
                }
            }
            rect = visualRect(option->direction, option->rect, rect);
            break;
        }
        default:
            break;
        }
        break;
#endif // QT_NO_COMBOBOX
#endif //QT_NO_GROUPBOX
        case CC_TitleBar:
        if (const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(option)) {
            SubControl sc = subControl;
            QRect &ret = rect;
            const int indent = 3;
            const int controlTopMargin = 3;
            const int controlBottomMargin = 3;
            const int controlWidthMargin = 2;
            const int controlHeight = tb->rect.height() - controlTopMargin - controlBottomMargin ;
            const int delta = controlHeight + controlWidthMargin;
            int offset = 0;

            bool isMinimized = tb->titleBarState & Qt::WindowMinimized;
            bool isMaximized = tb->titleBarState & Qt::WindowMaximized;

            switch (sc) {
            case SC_TitleBarLabel:
                if (tb->titleBarFlags & (Qt::WindowTitleHint | Qt::WindowSystemMenuHint)) {
                    ret = tb->rect;
                    if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                        ret.adjust(delta, 0, -delta, 0);
                    if (tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
                        ret.adjust(0, 0, -delta, 0);
                    if (tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
                        ret.adjust(0, 0, -delta, 0);
                    if (tb->titleBarFlags & Qt::WindowShadeButtonHint)
                        ret.adjust(0, 0, -delta, 0);
                    if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
                        ret.adjust(0, 0, -delta, 0);
                }
                break;
            case SC_TitleBarContextHelpButton:
                if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
                    offset += delta;
            case SC_TitleBarMinButton:
                if (!isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarMinButton)
                    break;
            case SC_TitleBarNormalButton:
                if (isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
                    offset += delta;
                else if (isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarNormalButton)
                    break;
            case SC_TitleBarMaxButton:
                if (!isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarMaxButton)
                    break;
            case SC_TitleBarShadeButton:
                if (!isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarShadeButton)
                    break;
            case SC_TitleBarUnshadeButton:
                if (isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarUnshadeButton)
                    break;
            case SC_TitleBarCloseButton:
                if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                    offset += delta;
                else if (sc == SC_TitleBarCloseButton)
                    break;
                ret.setRect(tb->rect.right() - indent - offset, tb->rect.top() + controlTopMargin,
                            controlHeight, controlHeight);
                break;
            case SC_TitleBarSysMenu:
                if (tb->titleBarFlags & Qt::WindowSystemMenuHint) {
                    ret.setRect(tb->rect.left() + controlWidthMargin + indent, tb->rect.top() + controlTopMargin,
                                controlHeight, controlHeight);
                }
                break;
            default:
                break;
            }
            ret = visualRect(tb->direction, tb->rect, ret);
        }
        break;
    default:
        break;
    }

    return rect;
}


/*!
  \reimp
*/
QRect QHaikuStyle::itemPixmapRect(const QRect &r, int flags, const QPixmap &pixmap) const
{
    return QWindowsStyle::itemPixmapRect(r, flags, pixmap);
}

/*!
  \reimp
*/
void QHaikuStyle::drawItemPixmap(QPainter *painter, const QRect &rect,
                            int alignment, const QPixmap &pixmap) const
{
    QWindowsStyle::drawItemPixmap(painter, rect, alignment, pixmap);
}

/*!
  \reimp
*/
QStyle::SubControl QHaikuStyle::hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                              const QPoint &pt, const QWidget *w) const
{
    return QWindowsStyle::hitTestComplexControl(cc, opt, pt, w);
}

/*!
  \reimp
*/
QPixmap QHaikuStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                        const QStyleOption *opt) const
{
    return QWindowsStyle::generatedIconPixmap(iconMode, pixmap, opt);
}

/*!
  \reimp
*/
int QHaikuStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget,
                               QStyleHintReturn *returnData) const
{
    int ret = 0;
    switch (hint) {
    case SH_ScrollBar_MiddleClickAbsolutePosition:
        ret = true;
        break;
    case SH_EtchDisabledText:
        ret = 1;
        break;
    case SH_Menu_AllowActiveAndDisabled:
        ret = false;
        break;
    case SH_MainWindow_SpaceBelowMenuBar:
        ret = 0;
        break;
    case SH_MenuBar_MouseTracking:
        ret = 1;
        break;
    case SH_TitleBar_AutoRaise:
        ret = 1;
        break;
    case SH_TitleBar_NoBorder:
        ret = 1;
        break;
    case SH_ItemView_ShowDecorationSelected:
        ret = true;
        break;
    case SH_Table_GridLineColor:
        if (option) {
            ret = option->palette.background().color().darker(120).rgb();
            break;
        }
    case SH_ComboBox_Popup:
#ifdef QT3_SUPPORT
        if (widget && widget->inherits("Q3ComboBox"))
            return 0;
#endif
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(option))
            ret = !cmb->editable;
        else
            ret = 0;
        break;
    case SH_WindowFrame_Mask:
        ret = 1;
        if (QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask *>(returnData)) {
            //left rounded corner
            mask->region = option->rect;
            mask->region -= QRect(option->rect.left(), option->rect.top(), 5, 1);
            mask->region -= QRect(option->rect.left(), option->rect.top() + 1, 3, 1);
            mask->region -= QRect(option->rect.left(), option->rect.top() + 2, 2, 1);
            mask->region -= QRect(option->rect.left(), option->rect.top() + 3, 1, 2);

            //right rounded corner
            mask->region -= QRect(option->rect.right() - 4, option->rect.top(), 5, 1);
            mask->region -= QRect(option->rect.right() - 2, option->rect.top() + 1, 3, 1);
            mask->region -= QRect(option->rect.right() - 1, option->rect.top() + 2, 2, 1);
            mask->region -= QRect(option->rect.right() , option->rect.top() + 3, 1, 2);
        }
        break;
    case SH_MessageBox_TextInteractionFlags:
        ret = Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse;
        break;
    case SH_DialogButtonBox_ButtonsHaveIcons:
        ret = true;
        break;
    case SH_MessageBox_CenterButtons:
        ret = false;
        break;
#ifndef QT_NO_WIZARD
    case SH_WizardStyle:
        ret = QWizard::ClassicStyle;
        break;
#endif
    case SH_ItemView_ArrowKeysNavigateIntoChildren:
        ret = false;
        break;
    case SH_Menu_SubMenuPopupDelay:
        ret = 225; // default from GtkMenu
        break;
    default:
        ret = QWindowsStyle::styleHint(hint, option, widget, returnData);
        break;
    }
    return ret;
}

/*! \reimp */
QRect QHaikuStyle::subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *w) const
{
    QRect r = QWindowsStyle::subElementRect(sr, opt, w);
    switch (sr) {
    case SE_PushButtonFocusRect:
        r.adjust(0, 1, 0, -1);
        break;
    case SE_DockWidgetTitleBarText: {
        const QStyleOptionDockWidgetV2 *v2
            = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(opt);
        bool verticalTitleBar = v2 == 0 ? false : v2->verticalTitleBar;
        if (verticalTitleBar) {
            r.adjust(0, 0, 0, -4);
        } else {
            if (QApplication::layoutDirection() == Qt::LeftToRight)
                r.adjust(4, 0, 0, 0);
            else
                r.adjust(0, 0, -4, 0);
        }

        break;
    }
    case SE_ProgressBarContents:
        r = subElementRect(SE_ProgressBarGroove, opt, w);
        break;
    default:
        break;
    }
    return r;
}

void QHaikuStylePrivate::lookupIconTheme() const
{
}

/*!
    \internal
*/

static QPixmap pixmapfromHaikuBitmap(BBitmap *bmp)
{
    if (!bmp)
        return QPixmap();

    const int width = bmp->Bounds().IntegerWidth() + 1;
    const int height = bmp->Bounds().IntegerHeight() + 1;

    QImage image(width, height, QImage::Format_ARGB32);

    const int bytesPerLine = width * 4;
    uchar *pixels = (uchar*)bmp->Bits();

    for (int y = 0; y < height; ++y)
       memcpy(image.scanLine(y), pixels + y * bytesPerLine, bytesPerLine);

    return QPixmap::fromImage(image);
}

static QPixmap haikuIconFromMime(const char *mime, icon_size which)
{
    QPixmap pixmap;
    BMimeType mtype(mime);
    BBitmap *bmp = new BBitmap(BRect(0, 0, which - 1, which - 1), B_RGBA32);

    if (mtype.GetIcon(bmp, which) != B_OK) {
        BMimeType super;
        mtype.GetSupertype(&super);
        if (super.GetIcon(bmp, which) != B_OK) {
            delete bmp;
            bmp = NULL;
        }
    }

    if (bmp) {
        pixmap = pixmapfromHaikuBitmap(bmp);
        delete bmp;
    }

    return pixmap;
}

QIcon QHaikuStyle::standardIcon(StandardPixmap standardIcon,
                                const QStyleOption *option,
                                const QWidget *widget) const
{
    QIcon icon;
    QPixmap pixmap;

    switch (standardIcon) {
    case SP_ComputerIcon:
        icon.addPixmap(haikuIconFromMime("application/x-vnd.Be-root", B_MINI_ICON));
        icon.addPixmap(haikuIconFromMime("application/x-vnd.Be-root", B_LARGE_ICON));
        break;
    default:
        break;
    }

    if (icon.isNull())
        icon = QCommonStyle::standardIcon(standardIcon, option, widget);

    return icon;
}

/*!
 \reimp
 */
QPixmap QHaikuStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                                      const QWidget *widget) const
{
    return QWindowsStyle::standardPixmap(standardPixmap, opt, widget);
}

QT_END_NAMESPACE

#endif // QT_NO_STYLE_HAIKU || QT_PLUGIN
