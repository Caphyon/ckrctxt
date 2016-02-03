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
#include "TextReporter.h"
#include "TextSize.h"

using std::endl;

TextReporter::TextReporter(std::wostream & aOStream, UINT aCodepage)
: mOStream(aOStream)
{
  if (CODEPAGE_ENGLISH != aCodepage)    // custom ANSI encoding
  {
    char cpstr[20];
    cpstr[0] = '.';                     // ".1252"
    sprintf_s(cpstr+1, 20-2, "%d", aCodepage);

    aOStream.imbue( std::locale(cpstr) );
    ::SetConsoleOutputCP(aCodepage);
  }
}

TextReporter & TextReporter::operator<<(const TextSize & aSzText)
{
  // we assume TextSize structure was already initialized
  ATLASSERT (aSzText.mText.size() > 0);

  // write new header on dialog change
  if (mOldDlgName != aSzText.mDlgName)
  {
    mOStream << L"Dialog ID: " << aSzText.mDlgName << endl
             << L"-------------------------------" << endl;
    mOldDlgName = aSzText.mDlgName;
  }

  // write text and required size
  mOStream
    << aSzText.mLineNr << L": " << aSzText.mType
                                << L" \"" << aSzText.mText << L'"' << endl
    << L"- detected text size: (" << aSzText.mTxtWidthPX << L","
                                  << aSzText.mTxtHeightPX << L")px" << endl
    << L"- detected rect size: (" << aSzText.mRcWidthDU << L","
                                  << aSzText.mRcHeightDU << L")du -> ("
                                  << aSzText.mRcWidthPX << L","
                                  << aSzText.mRcHeightPX << L")px" << endl
    << L"+ minimum required rectangle size: ("
                                  << aSzText.mTxtWidthDU << L","
                                  << aSzText.mTxtHeightDU << L")du";

  if ( aSzText.mMultiLine && (aSzText.mTxtHeightDU > 8) )
    mOStream << L"  [MULTILINE]";

  mOStream << endl << endl;
  return *this;
}
