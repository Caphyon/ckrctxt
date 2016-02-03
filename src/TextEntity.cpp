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
#include "TextEntity.h"
#include "TextSize.h"

using std::wstring;

TextEntity::operator TextSize() const
{
  TextSize szText;
  szText.mDlgName    = this->mDlgName;
  szText.mLineNr     = this->mLineNr;
  szText.mText       = this->mText;
  szText.mType       = this->mType;
  szText.mMultiLine  = this->mMultiLine;
  szText.mRcWidthDU  = this->mRc.right;
  szText.mRcHeightDU = this->mRc.bottom;

  return szText;
}

void TextEntity::ReplaceAll(const wstring & aToken, const wstring & aReplace)
{
  if (mText.empty() || aToken.empty())
    return;

  size_t pos = mText.find(aToken);
  while (pos != wstring::npos)
  {
    mText.replace(pos, aToken.length(), aReplace);
    pos = mText.find(aToken, pos + aReplace.length());
  }
}
