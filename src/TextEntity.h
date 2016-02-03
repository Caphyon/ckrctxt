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

struct TextSize;

/**
 * A text block with all associated parameters.
 * (usually read from a Visual Studio resource script file)
 * - [LCR]TEXT
 * - [DEF]PUSHBUTTON
 * - CONTROL(check, 3state, radio)
 * - RADIOBUTTON
 *
 * @author  Teodor MICU
 */
struct TextEntity
{
  operator TextSize() const;
  void ReplaceAll(const std::wstring & aToken, const std::wstring & aReplace);

  enum TypeCode
  {
    TEXT = 0,       // LTEXT, CTEXT, RTEXT
    PUSHBUTTON,     // PUSHBUTTON, DEFPUSHBUTTON
    CONTROLBUTTON,  // CONTROL: checkbox, 3state, radiobutton
    RADIOBUTTON,    // RADIOBUTTON
    OTHER
  };

  std::wstring mDlgName;      // dialog on which this text block was found
  int mLineNr;                // line# were this text block was found
  std::wstring mText;         // text string literal
  std::wstring mType;         // type string literal
  TypeCode mCode;             // associated code for mType
  bool mMultiLine;            // found or implicit multiline flag
  RECT mRc;                   // rectangle coordinates and size (du)
};
