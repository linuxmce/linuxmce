// -*- Mode: c++ -*-
// Copyright (c) 2003-2005, Daniel Kristjansson

#include <cassert>
#include <algorithm>
using namespace std;

#include "cc708window.h"
#include "mythlogging.h"

/************************************************************************

    FCC Addons to EIA-708.

    * Decoders must support the standard, large, and small caption sizes
      and must allow the caption provider to choose a size and allow the
      viewer to choose an alternative size.

    * Decoders must support the eight fonts listed in EIA-708. Caption
      providers may specify 1 of these 8 font styles to be used to write
      caption text. Decoders must include the ability for consumers to
      choose among the eight fonts. The decoder must display the font
      chosen by the caption provider unless the viewer chooses a different
      font.

    * Decoders must implement the same 8 character background colors
      as those that Section 9 requires be implemented for character
      foreground (white, black, red, green, blue, yellow, magenta and cyan).

    * Decoders must implement options for altering the appearance of
      caption character edges.

    * Decoders must display the color chosen by the caption provider,
      and must allow viewers to override the foreground and/or background
      color chosen by the caption provider and select alternate colors.

    * Decoders must be capable of decoding and processing data for the
      six standard services, but information from only one service need
      be displayed at a given time.

    * Decoders must include an option that permits a viewer to choose a
      setting that will display captions as intended by the caption
      provider (a default). Decoders must also include an option that
      allows a viewer's chosen settings to remain until the viewer
      chooses to alter these settings, including during periods when
      the television is turned off.

    * Cable providers and other multichannel video programming
      distributors must transmit captions in a format that will be
      understandable to this decoder circuitry in digital cable
      television sets when transmitting programming to digital
      television devices.

******************************************************************************/

const uint k708JustifyLeft            = 0;
const uint k708JustifyRight           = 1;
const uint k708JustifyCenter          = 2;
const uint k708JustifyFull            = 3;

const uint k708EffectSnap             = 0;
const uint k708EffectFade             = 1;
const uint k708EffectWipe             = 2;

const uint k708BorderNone             = 0;
const uint k708BorderRaised           = 1;
const uint k708BorderDepressed        = 2;
const uint k708BorderUniform          = 3;
const uint k708BorderShadowLeft       = 4;
const uint k708BorderShadowRight      = 5;

const uint k708DirLeftToRight         = 0;
const uint k708DirRightToLeft         = 1;
const uint k708DirTopToBottom         = 2;
const uint k708DirBottomToTop         = 3;

const uint k708AttrSizeSmall          = 0;
const uint k708AttrSizeStandard       = 1;
const uint k708AttrSizeLarge          = 2;

const uint k708AttrOffsetSubscript    = 0;
const uint k708AttrOffsetNormal       = 1;
const uint k708AttrOffsetSuperscript  = 2;

const uint k708AttrFontDefault               = 1;
const uint k708AttrFontMonospacedSerif       = 1;
const uint k708AttrFontProportionalSerif     = 2;
const uint k708AttrFontMonospacedSansSerif   = 3;
const uint k708AttrFontProportionalSansSerif = 4;
const uint k708AttrFontCasual                = 5;
const uint k708AttrFontCursive               = 6;
const uint k708AttrFontSmallCaps             = 7;

extern const uint k708AttrEdgeNone            = 0;
extern const uint k708AttrEdgeRaised          = 1;
extern const uint k708AttrEdgeDepressed       = 2;
extern const uint k708AttrEdgeUniform         = 3;
extern const uint k708AttrEdgeLeftDropShadow  = 4;
extern const uint k708AttrEdgeRightDropShadow = 5;

const uint k708AttrColorBlack         = 0;
const uint k708AttrColorWhite         = 63;

const uint k708AttrOpacitySolid       = 0;
const uint k708AttrOpacityFlash       = 1;
const uint k708AttrOpacityTranslucent = 2;
const uint k708AttrOpacityTransparent = 3;

CC708Window::CC708Window()
    : priority(0),              m_visible(0),
      anchor_point(0),          relative_pos(0),
      anchor_vertical(0),       anchor_horizontal(0),
      row_count(0),             column_count(0),
      row_lock(0),              column_lock(0),
      pen_style(0),             window_style(0),

      fill_color(0),            fill_opacity(0),
      border_color(0),          border_type(0),
      scroll_dir(0),            print_dir(0),
      effect_dir(0),            display_effect(0),
      effect_speed(0),
      justify(0),               word_wrap(0),

      true_row_count(0),        true_column_count(0),
      text(NULL),               m_exists(false),
      m_changed(true),          lock(QMutex::Recursive)
{
}

void CC708Window::DefineWindow(int _priority,         int _visible,
                               int _anchor_point,     int _relative_pos,
                               int _anchor_vertical,  int _anchor_horizontal,
                               int _row_count,        int _column_count,
                               int _row_lock,         int _column_lock,
                               int _pen_style,        int _window_style)
{
    // The DefineWindow command may be sent frequently to allow a
    // caption decoder just tuning in to get in synch quickly.
    // Usually the row_count and column_count are unchanged, but it is
    // possible to add or remove rows or columns.  Due to the
    // one-dimensional row-major representation of characters, if the
    // number of columns is changed, a new array must be created and
    // the old characters copied in.  If only the number of rows
    // decreases, the array can be left unchanged.  If only the number
    // of rows increases, the old characters can be copied into the
    // new character array directly without any index translation.
    QMutexLocker locker(&lock);

    _row_count++;
    _column_count++;

    priority          = _priority;
    SetVisible(_visible);
    anchor_point      = _anchor_point;
    relative_pos      = _relative_pos;
    anchor_vertical   = _anchor_vertical;
    anchor_horizontal = _anchor_horizontal;
    row_lock          = _row_lock;
    column_lock       = _column_lock;

    if ((!_pen_style && !GetExists()) || _pen_style)
        pen.SetPenStyle(_pen_style ? _pen_style : 1);

    if ((!_window_style && !GetExists()) || _window_style)
        SetWindowStyle(_window_style ? _window_style : 1);

    Resize(_row_count, _column_count);
    row_count = _row_count;
    column_count = _column_count;
    LimitPenLocation();

    SetExists(true);
}

// Expand the internal array of characters if necessary to accommodate
// the current values of row_count and column_count.  Any new (space)
// characters exposed are given the current pen attributes.  At the
// end, row_count and column_count are NOT updated.
void CC708Window::Resize(uint new_rows, uint new_columns)
{
    if (!GetExists() || text == NULL)
    {
        true_row_count = 0;
        true_column_count = 0;
    }
    if (new_rows > true_row_count || new_columns > true_column_count)
    {
        // Expand the array if the new size exceeds the current capacity
        // in either dimension.
        CC708Character *new_text =
            new CC708Character[new_rows * new_columns];
        pen.column = 0;
        pen.row = 0;
        uint i, j;
        for (i = 0; text && i < row_count; ++i)
        {
            for (j = 0; j < column_count; ++j)
                new_text[i * new_columns + j] = text[i * true_column_count + j];
            for (; j < new_columns; ++j)
                new_text[i * new_columns + j].attr = pen.attr;
        }
        for (; i < new_rows; ++i)
            for (j = 0; j < new_columns; ++j)
                new_text[i * new_columns + j].attr = pen.attr;

        delete [] text;
        text = new_text;
        true_row_count = new_rows;
        true_column_count = new_columns;
        SetChanged();
    }
    else if (new_rows > row_count || new_columns > column_count)
    {
        // At least one dimension expanded into existing space, so
        // those newly exposed characters must be cleared.
        for (uint i = 0; i < row_count; ++i)
            for (uint j = column_count; j < new_columns; ++j)
            {
                text[i * true_column_count + j].character = ' ';
                text[i * true_column_count + j].attr = pen.attr;
            }
        for (uint i = row_count; i < new_rows; ++i)
            for (uint j = 0; j < new_columns; ++j)
            {
                text[i * true_column_count + j].character = ' ';
                text[i * true_column_count + j].attr = pen.attr;
            }
        SetChanged();
    }
    SetExists(true);
}


CC708Window::~CC708Window()
{
    QMutexLocker locker(&lock);

    SetExists(false);
    true_row_count    = 0;
    true_column_count = 0;

    if (text)
    {
        delete [] text;
        text = NULL;
    }
}

void CC708Window::Clear(void)
{
    QMutexLocker locker(&lock);

    if (!GetExists() || !text)
        return;

    for (uint i = 0; i < true_row_count * true_column_count; i++)
    {
        text[i].character = QChar(' ');
        text[i].attr = pen.attr;
    }
    SetChanged();
}

CC708Character &CC708Window::GetCCChar(void) const
{
    assert(GetExists());
    assert(text);
    assert(pen.row    < true_row_count);
    assert(pen.column < true_column_count);
    return text[pen.row * true_column_count + pen.column];
}

vector<CC708String*> CC708Window::GetStrings(void) const
{
    // Note on implementation.  In many cases, one line will be
    // computed as the concatenation of 3 strings: a prefix of spaces
    // with a default foreground color, followed by the actual text in
    // a specific foreground color, followed by a suffix of spaces
    // with a default foreground color.  This leads to 3
    // FormattedTextChunk objects when 1 would suffice.  The prefix
    // and suffix ultimately get optimized away, but not before a
    // certain amount of unnecessary work.
    //
    // This can be solved with two steps.  First, suppress a format
    // change when only non-underlined spaces have been seen so far.
    // (Background changes can be ignored because the subtitle code
    // suppresses leading spaces.)  Second, for trailing
    // non-underlined spaces, either suppress a format change, or
    // avoid creating such a string when it appears at the end of the
    // row.  (We can't do the latter for an initial string of spaces,
    // because the spaces are needed for coordinate calculations.)
    QMutexLocker locker(&lock);

    vector<CC708String*> list;

    CC708String *cur = NULL;

    if (!text)
        return list;

    bool createdNonblankStrings = false;
    QChar chars[k708MaxColumns];
    for (uint j = 0; j < row_count; j++)
    {
        bool inLeadingSpaces = true;
        bool inTrailingSpaces = true;
        bool createdString = false;
        uint strStart = 0;
        for (uint i = 0; i < column_count; i++)
        {
            CC708Character &chr = text[j * true_column_count + i];
            chars[i] = chr.character;
            if (!cur)
            {
                cur = new CC708String;
                cur->x    = i;
                cur->y    = j;
                cur->attr = chr.attr;
                strStart = i;
            }
            bool isDisplayable = (chr.character != ' ' || chr.attr.underline);
            if (inLeadingSpaces && isDisplayable)
            {
                cur->attr = chr.attr;
                inLeadingSpaces = false;
            }
            if (isDisplayable)
            {
                inTrailingSpaces = false;
            }
            if (cur->attr != chr.attr)
            {
                cur->str = QString(&chars[strStart], i - strStart);
                list.push_back(cur);
                createdString = true;
                createdNonblankStrings = true;
                inTrailingSpaces = true;
                cur = NULL;
                i--;
            }
        }
        if (cur)
        {
            // If the entire string is spaces, we still may need to
            // create a chunk to preserve spacing between lines.
            if (!inTrailingSpaces || !createdString)
            {
                int allSpaces = (inLeadingSpaces || inTrailingSpaces);
                int length = allSpaces ? 0 : column_count - strStart;
                if (length)
                    createdNonblankStrings = true;
                cur->str = QString(&chars[strStart], length);
                list.push_back(cur);
            }
            else
                delete cur;
            cur = NULL;
        }
    }
    if (!createdNonblankStrings)
        list.clear();
    return list;
}

void CC708Window::SetWindowStyle(uint style)
{
    const uint style2justify[] =
    {
        k708JustifyLeft, k708JustifyLeft, k708JustifyLeft,   k708JustifyCenter,
        k708JustifyLeft, k708JustifyLeft, k708JustifyCenter, k708JustifyLeft,
    };

    if ((style < 1) || (style > 7))
        return;

    fill_color     = k708AttrColorBlack;
    fill_opacity   = ((2 == style) || (5 == style)) ?
        k708AttrOpacityTransparent : k708AttrOpacitySolid;
    border_color   = k708AttrColorBlack;
    border_type    = k708BorderNone;
    scroll_dir     = (style < 7) ? k708DirBottomToTop : k708DirRightToLeft;
    print_dir      = (style < 7) ? k708DirLeftToRight : k708DirTopToBottom;
    effect_dir     = scroll_dir;
    display_effect = k708EffectSnap;
    effect_speed   = 0;
    justify        = style2justify[style];
    word_wrap      = (style > 3) && (style < 7) ? 1 : 0;

    /// HACK -- begin
    // It appears that ths is missused by broadcasters (FOX -- Dollhouse)
    fill_opacity   = k708AttrOpacityTransparent;
    /// HACK -- end
}

void CC708Window::AddChar(QChar ch)
{
    if (!GetExists())
        return;

    QString dbg_char = ch;
    if (ch.toLatin1() < 32)
        dbg_char = QString("0x%1").arg( (int)ch.toLatin1(), 0,16);

    if (!IsPenValid())
    {
        LOG(VB_VBI, LOG_INFO,
            QString("AddChar(%1) at (c %2, r %3) INVALID win(%4,%5)")
                .arg(dbg_char).arg(pen.column).arg(pen.row)
                .arg(true_column_count).arg(true_row_count));
        return;
    }

    if (ch.toLatin1() == 0x0D)
    {
        Scroll(pen.row + 1, 0);
        SetChanged();
        return;
    }
    QMutexLocker locker(&lock);

    if (ch.toLatin1() == 0x08)
    {
        DecrPenLocation();
        CC708Character& chr = GetCCChar();
        chr.attr      = pen.attr;
        chr.character = QChar(' ');
        SetChanged();
        return;
    }

    CC708Character& chr = GetCCChar();
    chr.attr      = pen.attr;
    chr.character = ch;
    int c = pen.column;
    int r = pen.row;
    IncrPenLocation();
    SetChanged();

    LOG(VB_VBI, LOG_INFO, QString("AddChar(%1) at (c %2, r %3) -> (%4,%5)")
            .arg(dbg_char).arg(c).arg(r).arg(pen.column).arg(pen.row));
}

void CC708Window::Scroll(int row, int col)
{
    QMutexLocker locker(&lock);

    if (!true_row_count || !true_column_count)
        return;

    if (text && (k708DirBottomToTop == scroll_dir) &&
        (row >= (int)true_row_count))
    {
        for (uint j = 0; j < true_row_count - 1; j++)
            for (uint i = 0; i < true_column_count; i++)
                text[(true_column_count * j) + i] =
                    text[(true_column_count * (j+1)) + i];
        //uint colsz = true_column_count * sizeof(CC708Character);
        //memmove(text, text + colsz, colsz * (true_row_count - 1));

        CC708Character tmp(*this);
        for (uint i = 0; i < true_column_count; i++)
            text[(true_column_count * (true_row_count - 1)) + i] = tmp;

        pen.row = true_row_count - 1;
        SetChanged();
    }
    else
    {
        pen.row = row;
    }
    // TODO implement other 3 scroll directions...

    pen.column = col;
}

void CC708Window::IncrPenLocation(void)
{
    // TODO: Scroll direction and up/down printing,
    // and word wrap not handled yet...
    int new_column = pen.column, new_row = pen.row;

    new_column += (print_dir == k708DirLeftToRight) ? +1 : 0;
    new_column += (print_dir == k708DirRightToLeft) ? -1 : 0;
    new_row    += (print_dir == k708DirTopToBottom) ? +1 : 0;
    new_row    += (print_dir == k708DirBottomToTop) ? -1 : 0;

#if 0
    LOG(VB_VBI, LOG_INFO, QString("IncrPen dir%1: (c %2, r %3) -> (%4,%5)")
            .arg(print_dir).arg(pen.column).arg(pen.row)
            .arg(new_column).arg(new_row));
#endif

    if (k708DirLeftToRight == print_dir || k708DirRightToLeft == print_dir)
    {
        // basic wrapping for l->r, r->l languages
        if (!row_lock && column_lock && (new_column >= (int)true_column_count))
        {
            new_column  = 0;
            new_row    += 1;
        }
        else if (!row_lock && column_lock && (new_column < 0))
        {
            new_column  = (int)true_column_count - 1;
            new_row    -= 1;
        }
        Scroll(new_row, new_column);
    }
    else
    {
        pen.column = max(new_column, 0);
        pen.row    = max(new_row,    0);
    }
    // TODO implement other 2 scroll directions...

    LimitPenLocation();
}

void CC708Window::DecrPenLocation(void)
{
    // TODO: Scroll direction and up/down printing,
    // and word wrap not handled yet...
    int new_column = pen.column, new_row = pen.row;

    new_column -= (print_dir == k708DirLeftToRight) ? +1 : 0;
    new_column -= (print_dir == k708DirRightToLeft) ? -1 : 0;
    new_row    -= (print_dir == k708DirTopToBottom) ? +1 : 0;
    new_row    -= (print_dir == k708DirBottomToTop) ? -1 : 0;

#if 0
    LOG(VB_VBI, LOG_INFO, QString("DecrPen dir%1: (c %2, r %3) -> (%4,%5)")
            .arg(print_dir).arg(pen.column).arg(pen.row)
            .arg(new_column).arg(new_row));
#endif

    if (k708DirLeftToRight == print_dir || k708DirRightToLeft == print_dir)
    {
        // basic wrapping for l->r, r->l languages
        if (!row_lock && column_lock && (new_column >= (int)true_column_count))
        {
            new_column  = 0;
            new_row    += 1;
        }
        else if (!row_lock && column_lock && (new_column < 0))
        {
            new_column  = (int)true_column_count - 1;
            new_row    -= 1;
        }
        Scroll(new_row, new_column);
    }
    else
    {
        pen.column = max(new_column, 0);
        pen.row    = max(new_row,    0);
    }
    // TODO implement other 2 scroll directions...

    LimitPenLocation();
}

void CC708Window::SetPenLocation(uint row, uint column)
{
    Scroll(row, column);
    LimitPenLocation();
}

void CC708Window::LimitPenLocation(void)
{
    // basic limiting
    uint max_col = max((int)true_column_count - 1, 0);
    uint max_row = max((int)true_row_count    - 1, 0);
    pen.column   = min(pen.column, max_col);
    pen.row      = min(pen.row,    max_row);
}

/***************************************************************************/

void CC708Pen::SetPenStyle(uint style)
{
    static const uint style2font[] = { 0, 0, 1, 2, 3, 4, 3, 4 };

    if ((style < 1) || (style > 7))
        return;

    attr.pen_size   = k708AttrSizeStandard;
    attr.offset     = k708AttrOffsetNormal;
    attr.font_tag   = style2font[style];
    attr.italics    = 0;
    attr.underline  = 0;
    attr.boldface   = 0;
    attr.edge_type  = 0;
    attr.fg_color   = k708AttrColorWhite;
    attr.fg_opacity = k708AttrOpacitySolid;
    attr.bg_color   = k708AttrColorBlack;
    attr.bg_opacity = (style<6) ?
        k708AttrOpacitySolid : k708AttrOpacityTransparent;
    attr.edge_color = k708AttrColorBlack;
    attr.actual_fg_color = QColor();
}

CC708Character::CC708Character(const CC708Window &win)
    : attr(win.pen.attr), character(' ')
{
}

bool CC708CharacterAttribute::operator==(
    const CC708CharacterAttribute &other) const
{
    return ((pen_size   == other.pen_size)   &&
            (offset     == other.offset)     &&
            (text_tag   == other.text_tag)   &&
            (font_tag   == other.font_tag)   &&
            (edge_type  == other.edge_type)  &&
            (underline  == other.underline)  &&
            (italics    == other.italics)    &&
            (fg_color   == other.fg_color)   &&
            (fg_opacity == other.fg_opacity) &&
            (bg_color   == other.bg_color)   &&
            (bg_opacity == other.bg_opacity) &&
            (edge_color == other.edge_color));
}

QColor CC708CharacterAttribute::ConvertToQColor(uint c)
{
    // Color is expressed in 6 bits, 2 each for red, green, and blue.
    // U.S. ATSC programs seem to use just the higher-order bit,
    // i.e. values 0 and 2, so the last two elements of X[] are both
    // set to the maximum 255, otherwise font colors are dim.
    static int X[] = {0, 96, 255, 255};
    return QColor(X[(c>>4)&3], X[(c>>2)&3], X[c&3]);
}
