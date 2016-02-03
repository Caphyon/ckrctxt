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
#include "TextDlg.h"

LRESULT TextDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  mDC = GetDC();
  mDC.SelectFont( GetFont() );

  // base units for non-system font (custom)
  // http://support.microsoft.com/kb/145994
  RECT dlgbu = { 0, 0, 4, 8 };
  MapDialogRect(&dlgbu);
  mDlgBU.cx = dlgbu.right;
  mDlgBU.cy = dlgbu.bottom;

  // detect text height
  TEXTMETRIC tm;
  mDC.GetTextMetrics( &tm );

  CSize szText;
  szText.cx = tm.tmAveCharWidth;
  szText.cy = tm.tmHeight;
  mDC.LPtoDP(&szText);
  mTxtHeight = szText.cy;

  return TRUE;
}

CSize TextDlg::GetDlgBaseUnits() const
{
  return mDlgBU;
}

CSize TextDlg::GetTextSize(const std::wstring & aText) const
{
  CSize szText;
  mDC.GetTextExtent(aText.c_str(), -1, &szText);
  mDC.LPtoDP(&szText);

  return szText;
}

LONG TextDlg::GetTextHeight() const
{
  return mTxtHeight;
}

CSize TextDlg::DUtoPX(LONG aWidthDU, LONG aHeightDU) const
{
  RECT rc = { 0, 0, aWidthDU, aHeightDU };
  ::MapDialogRect(*this, &rc);

  return CSize(rc.right, rc.bottom);
}

CSize TextDlg::PXtoDU(LONG aWidthDU, LONG aHeightDU) const
{
  LONG widthDU  = ::MulDiv(aWidthDU, 4, mDlgBU.cx);
  LONG heightDU = ::MulDiv(aHeightDU, 8, mDlgBU.cy);

  return CSize(widthDU, heightDU);
}
