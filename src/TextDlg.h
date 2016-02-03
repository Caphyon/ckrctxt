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

#pragma once

#include "resource.h"

/**
 * Simple dialog used for computing the required size for some text.
 *
 * @author  Teodor MICU
 */
class TextDlg : public CDialogImpl<TextDlg>
{
public:
  enum { IDD = IDD_MEASURE_DIALOG };

  BEGIN_MSG_MAP(TextDlg)
    MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
  END_MSG_MAP()

  /**
   * Calculate dialog base units based on the current font.
   */
  CSize GetDlgBaseUnits() const;

  /**
   * Calculate the size of text based on the current font.
   */
  CSize GetTextSize(const std::wstring & /*aText*/) const;

  /**
   * Calculate the height of (one line) text based on the current font.
   */
  LONG GetTextHeight() const;

  /**
   * Convert du->px based on the current font.
   */
  CSize DUtoPX(LONG /*aWidthDU*/, LONG /*aHeightDU*/) const;

  /**
   * Convert px->du based on the current font.
   */
  CSize PXtoDU(LONG /*aWidthDU*/, LONG /*aHeightDU*/) const;

private:
  LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  CDCHandle mDC;                        // handle for device context
  CSize mDlgBU;                         // dialog base units
  LONG mTxtHeight;
};
