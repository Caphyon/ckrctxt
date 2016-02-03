// Copyright 2016 Caphyon Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"
#include "TextMeasurer.h"
#include "TextEntity.h"
#include "TextSize.h"

using std::wstring;

TextMeasurer::TextMeasurer()
: mTextEnt(nullptr), mRcWidth(0), mRcHeight(0), mTxtWidth(0), mTxtHeight(0)
{
  mDlg.Create(NULL);
}

TextMeasurer::~TextMeasurer()
{
  mDlg.DestroyWindow();
}

void TextMeasurer::ComputeSize(TextEntity & aTextEnt)
{
  mTextEnt = &aTextEnt;

  // we assume text block was already read from file prior to this call
  ATLASSERT (mTextEnt->mText.size() > 0);

  LONG & rcWidthDU = mTextEnt->mRc.right;
  LONG & rcHeightDU = mTextEnt->mRc.bottom;
  CSize sz;

  // adjust visible text area in rectangle (du)
  switch (mTextEnt->mCode)
  {
  case TextEntity::RADIOBUTTON:
  case TextEntity::CONTROLBUTTON:
    // compute rectangle width (px)
    sz = mDlg.DUtoPX(rcWidthDU, rcHeightDU);
    mRcWidth  = sz.cx - ::GetSystemMetrics(SM_CXMENUCHECK);
    mRcHeight = sz.cy;
    break;

  case TextEntity::PUSHBUTTON:
    // compute rectangle width (px)
    sz = mDlg.DUtoPX(rcWidthDU - PUSHBUTTON_X, rcHeightDU);
    mRcWidth  = sz.cx;
    mRcHeight = sz.cy;
    break;

  default:                              // TEXT
    // compute rectangle width (px)
    sz = mDlg.DUtoPX(rcWidthDU, rcHeightDU);
    mRcWidth  = sz.cx;
    mRcHeight = sz.cy;
  } // end switch

  sz = mDlg.GetTextSize(mTextEnt->mText);
  mTxtWidth  = sz.cx;
  mTxtHeight = sz.cy;

  if (!mTextEnt->mMultiLine)
    return;

  // simple case: no escape sequence for new line
  if ( wstring::npos == mTextEnt->mText.find(L"\\n") )
    // rectangle height has space for just one line of text
    if ( (mTxtWidth <= mRcWidth) || (mRcHeight <= mTxtHeight) )
      return;

  // detect text height for empty lines found in some escape seq "\n\n"
  LONG baseUnitY = mDlg.GetTextHeight();

  mTxtWidth = mTxtHeight = 0;
  wstring wline;                        // substring wrapped on one line
  wstring::size_type offset = 0;
  wstring::size_type lastpos;           // last known good position
  wstring::size_type nextpos = 0;       // next position to try out
  wstring::size_type nextnl;            // next new line sequence \n
  bool               foundnl = false;

  // wrap text in multiple lines less or equal to mRcWidth
  do
  {
    lastpos = nextpos;              // last word is included in current line
    nextpos = mTextEnt->mText.find(L" ", lastpos);
    if (wstring::npos == nextpos)   // remaining text has no spaces
      nextpos = mTextEnt->mText.size();
    else
      ++nextpos;                    // +1 to include the current white space

    // \n escape sequence forces END of line
    nextnl = mTextEnt->mText.find(L"\\n", lastpos);
    if (nextnl < nextpos)
    {
      foundnl = true;
      nextpos = nextnl;
    }

    // compute width of current line
    wline = mTextEnt->mText.substr(offset, nextpos-offset);
    sz = mDlg.GetTextSize(wline);

    if (sz.cx <= mRcWidth)              // current substring is not full
    {
      if (!foundnl)
        continue;
    }
    // one word is NOT too big to fit on one line?
    else  if (offset < lastpos)
    {
      if ( !foundnl &&
           (mTextEnt->mText.size() != nextpos) )  // not end of text
      {
        ATLASSERT ( iswspace(wline[wline.size()-1]) );

        // remove trailing white space from current line
        wline.pop_back();
        sz = mDlg.GetTextSize(wline);
      }

      if (sz.cx > mRcWidth)             // doesn't fit w/ last space
      {
        // current substring is too big, get back to last position
        nextpos = lastpos;
        foundnl = false;

        // recompute width of current line
        wline = mTextEnt->mText.substr(offset, nextpos-offset);
        sz = mDlg.GetTextSize(wline);
      }
    }

    if (mTxtWidth < sz.cx)              // adjust maximum (line of) text width
      mTxtWidth = sz.cx;

    if (offset == nextpos)              // adjust text height (px)
      mTxtHeight += baseUnitY;          // empty line -> no text to measure
    else
      mTxtHeight += sz.cy;              // non-empty text

    if (foundnl)
    {
      foundnl = false;
      nextpos += 2;                     // advance over "\n"
    }

    // start of a new line
    offset = nextpos;
  } while (nextpos < mTextEnt->mText.size());

  if (mTextEnt->mText.size() == offset) // last word was processed
    return;

  // last word was NOT processed
  wline = mTextEnt->mText.substr(offset);
  sz = mDlg.GetTextSize(wline);

  if (mTxtWidth < sz.cx)                // adjust maximum (line of) text width
    mTxtWidth = sz.cx;
  mTxtHeight += sz.cy;                  // adjust text height (px)
  ATLASSERT (mTxtWidth <= mRcWidth);
}

bool TextMeasurer::IsResizeNeeded() const
{
  return (mTxtWidth > mRcWidth) || (mTxtHeight > mRcHeight);
}

TextSize TextMeasurer::GetRealSize() const
{
  ATLASSERT (nullptr != mTextEnt);

  TextSize szText = *mTextEnt;
  szText.mRcWidthPX   = mRcWidth;
  szText.mRcHeightPX  = mRcHeight;
  szText.mTxtWidthPX  = mTxtWidth;
  szText.mTxtHeightPX = mTxtHeight;

  // convert pixels to dialog units
  CSize sz = mDlg.PXtoDU(mTxtWidth, mTxtHeight);
  szText.mTxtWidthDU  = sz.cx;
  szText.mTxtHeightDU = sz.cy;

  // adjust text width on round-off error
  sz = mDlg.DUtoPX(szText.mTxtWidthDU, szText.mTxtHeightDU);
  if (szText.mTxtWidthPX > sz.cx)       // usually only 1px loss
    ++szText.mTxtWidthDU;
#if _DEBUG
  sz = mDlg.DUtoPX(szText.mTxtWidthDU, szText.mTxtHeightDU);
  ATLASSERT (szText.mTxtWidthPX <= sz.cx);
#endif

  // adjust computed text area to rectangle area
  switch (mTextEnt->mCode)
  {
  case TextEntity::RADIOBUTTON:
  case TextEntity::CONTROLBUTTON:
    szText.mTxtWidthDU += CONTROLBUTTON_X;

    // recommended size, but not required
    if (szText.mTxtHeightDU + CONTROLBUTTON_Y <= szText.mRcHeightDU)
      szText.mTxtHeightDU += CONTROLBUTTON_Y;
    break;

  case TextEntity::PUSHBUTTON:
    szText.mTxtWidthDU += PUSHBUTTON_X;

    // recommended size, but not required
    if (szText.mTxtHeightDU + PUSHBUTTON_Y <= szText.mRcHeightDU)
      szText.mTxtHeightDU += PUSHBUTTON_Y;
    break;
  } // end switch

  return szText;
}
