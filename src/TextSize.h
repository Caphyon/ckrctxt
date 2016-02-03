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

/**
 * Current and computed size of a TextEntity.
 *
 * @author  Teodor MICU
 */
struct TextSize
{
  std::wstring mDlgName;      // dialog on which this text block was found
  int mLineNr;                // line# were this text block was found
  std::wstring mText;         // text string literal
  std::wstring mType;         // type string literal
  bool mMultiLine;            // found or implicit multiline flag

  LONG mRcWidthDU;            // rectangle width (du) -- read from resource
  LONG mRcWidthPX;            // computed rectangle width (px)
  LONG mRcHeightDU;           // rectangle height (du) -- read from resource
  LONG mRcHeightPX;           // computed rectangle height (px)

  LONG mTxtWidthDU;           // computed text width (du)
  LONG mTxtWidthPX;           // computed text width (px)
  LONG mTxtHeightDU;          // computed text height (du)
  LONG mTxtHeightPX;          // computed text height (px)
};
