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
#include "RcTextReader.h"
#include "TextEntity.h"

using namespace std;

namespace
{
  // NAME  DIALOGEX  0, 0, 340, 180
  const wregex kDlgExp(L"^([^ ]+) DIALOGEX [0-9]+, [0-9]+, [0-9]+, [0-9]+$");

  // LTEXT  "Text",ID_TEXT,53,25,156,24[,FLAGS]
  const wregex kTxtExp(L"^ +([LCR]TEXT) +\"(.+)\",([^,]+),"
                       L"(\\d+),(\\d+),(\\d+),(\\d+)(?:,(.*))?$");

  // CONTROL  "Run as..",ID_RUN,"Button",FLAGS,30,190,81,10
  const wregex kCtlExp(L"^ +(CONTROL) +\"(.+)\",([^,]+),\"([A-Za-z]+)\",([^,]*),"
                       L"(\\d+),(\\d+),(\\d+),(\\d+)$");

  // RADIOBUTTON  "Use..",IDC_USE,30,48,142,10[,FLAGS]
  const wregex kRbtExp(L"^ +(RADIOBUTTON) +\"(.+)\",([^,]+),"
                       L"(\\d+),(\\d+),(\\d+),(\\d+).*$");

  // [DEF]PUSHBUTTON  "OK",ID_OK,103,47,50,16[,FLAGS]
  const wregex kBtnExp(L"^ +((?:DEF)?PUSHBUTTON) +\"(.+)\",([^,]+),"
                       L"(\\d+),(\\d+),(\\d+),(\\d+)(?:,(.*))?$");
}

//----------------------------------------------------------------------------
// Class definition - RcTextReader public methods.

RcTextReader::RcTextReader(const wstring & aFilename)
: mIStream(aFilename), mCodepage(CODEPAGE_ENGLISH), mFound(false), mLineNr(0)
{
  // detect ANSI code page
  string cp = DotCodepage(aFilename);
  if (CODEPAGE_ENGLISH != mCodepage)    // custom ANSI encoding
    mIStream.imbue( std::locale(cp) );
}

bool RcTextReader::IsOpen() const
{
  return mIStream.is_open();
}

UINT RcTextReader::GetCodepage() const
{
  return mCodepage;
}

bool RcTextReader::HasFoundText() const
{
  return mFound;
}

RcTextReader & RcTextReader::operator>>(TextEntity & aTextEnt)
{
  wstring line;                         // first line read
  wstring line23;                       // (optional) extra lines read
  TextEntity::TypeCode code = TextEntity::OTHER;

  wsmatch mr;                           // match results

  mFound = false;
  while ( mIStream.good() )
  {
    // get first line, might be a sequence of two or three
    getline(mIStream, line);
    ++mLineNr;

    // empty line, or one CR (#13)
    if (line.size() <= 1)
      continue;

    // update current dialog name
    if (L' ' != line[0])                // not a SPACE
    {
      if ( regex_match(line, mr, kDlgExp) )
        mDlgName = mr[1];

      // all TEXT lines start with four spaces
      continue;
    }

    // get next lines, one CONTROL line might be split in multiple lines
    while ( (L',' == line[line.size()-1]) ||
            (L'|' == line[line.size()-1]) )
    {
      getline(mIStream, line23);
      ++mLineNr;

      wstring::size_type first = line23.find_first_not_of(L" \t");
      wstring::size_type last = line23.find_last_not_of(L" \t\r");
      line.append( line23.substr(first, last-first+1) );
    }

    // detect type
    if ( regex_match(line, mr, kTxtExp) )
      code = TextEntity::TEXT;
    else if ( regex_match(line, mr, kBtnExp) )
      code = TextEntity::PUSHBUTTON;
    else if ( regex_match(line, mr, kCtlExp) )
      code = TextEntity::CONTROLBUTTON;
    else if ( regex_match(line, mr, kRbtExp) )
      code = TextEntity::RADIOBUTTON;
    else
      continue;

    // ignore these small buttons (resized and repositioned in code)
    if ( (TextEntity::PUSHBUTTON == code) && (L"..." == mr[2]) )
      continue;

    // ignore (on purpose) truncated static text
    if ( (TextEntity::TEXT == code) && (mr[8].matched) )
    {
      if ( wstring::npos != mr[8].str().find(L"ELLIPSIS") )
        continue;
    }

    mFound = true;
    break;
  } // end while

  // stop processing if no text block was found
  if (!mFound)
    return *this;

  aTextEnt.mText = mr[2];
  aTextEnt.mType = mr[1];

  // TODO: SS_NOPREFIX flag -> no escape
  // "" is an escape sequence
  aTextEnt.ReplaceAll(L"\"\"", L"\"");

  // && is an escape sequence
  aTextEnt.ReplaceAll(L"&&", L"&");

  // remove & from text string if followed by an alpha
  wstring::size_type amppos = aTextEnt.mText.find(L"&");
  if ( (amppos < aTextEnt.mText.size()) &&
       isalpha(aTextEnt.mText[amppos+1]) )
    aTextEnt.mText.erase(amppos, 1);

  aTextEnt.mMultiLine = true;           // TEXT does word wrap by default
  switch (code)
  {
  case TextEntity::CONTROLBUTTON:
    // must have at least 9 values (+1)
    assert(mr.size() >= 10);

    // multiline control button
    if ( (L"Button" == mr[4]) &&
         (wstring::npos == mr[5].str().find(L"BS_MULTILINE")) )
    {
      aTextEnt.mMultiLine = false;
    }
    // multiline static text
    else if ( (L"Static"== mr[4]) &&
              (wstring::npos != mr[5].str().find(L"SS_LEFTNOWORDWRAP")) )
    {
      code = TextEntity::TEXT;
      aTextEnt.mMultiLine = false;
    }

    aTextEnt.mRc.left   = stoi( mr[6] );          // X
    aTextEnt.mRc.top    = stoi( mr[7] );          // Y
    aTextEnt.mRc.right  = stoi( mr[8] );          // Width
    aTextEnt.mRc.bottom = stoi( mr[9] );          // Height
    break;

  case TextEntity::RADIOBUTTON:
  case TextEntity::PUSHBUTTON:
    // multiline push button
    if ( (mr.size() < 9) ||
         (wstring::npos == mr[8].str().find(L"BS_MULTILINE")) )
      aTextEnt.mMultiLine = false;
    // NO break;

  case TextEntity::TEXT:
    // must have at least 7 values (+1)
    assert(mr.size() >= 8);

    aTextEnt.mRc.left   = stoi( mr[4] );          // X
    aTextEnt.mRc.top    = stoi( mr[5] );          // Y
    aTextEnt.mRc.right  = stoi( mr[6] );          // Width
    aTextEnt.mRc.bottom = stoi( mr[7] );          // Height
    break;

  default:
    assert(!"unsupported type code!");
  } // end switch

  aTextEnt.mDlgName = mDlgName;
  aTextEnt.mLineNr = mLineNr;
  aTextEnt.mCode = code;
  return *this;
}

//----------------------------------------------------------------------------
// Class definition - RcTextReader private methods.

string RcTextReader::DotCodepage(const wstring & aFilename)
{
  ifstream file(aFilename);
  if ( !file.is_open() )
    return string(".1252");

  string line;
  smatch mr;                            // match results
  regex exp("^#pragma code_page\\((\\d+)\\)$");

  while ( !file.eof() )
  {
    getline(file, line);

    // empty line
    if (line.size() <= 1)
      continue;

    // skip anything but macros
    if ('#' != line[0])
      continue;

    if ( regex_match(line, mr, exp) )
     break;
  }
  file.close();

  // no #pragma code_page()
  if ( mr.empty() )
    return string(".1252");

  mCodepage = stoi( mr[1] );
  return "." + mr[1].str();
}
