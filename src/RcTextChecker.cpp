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
#include "RcTextChecker.h"
#include "RcTextReader.h"
#include "TextDlg.h"
#include "TextEntity.h"
#include "TextSize.h"
#include "TextMeasurer.h"
#include "TextReporter.h"

using namespace std;

int RcTextChecker::Run(const wstring & aFilename, bool quiet) const
{
  // open resource script file
  RcTextReader rsfile(aFilename);
  if ( !rsfile.IsOpen() )
  {
    wcout << L"Err: resource script file '" << aFilename
          << L"' was not found or could not be opened!" << endl << endl;
    return -1;
  }

  TextEntity textEnt;         // one text block from resource script file
  TextMeasurer textMsr;
  TextReporter textRep( wcout, rsfile.GetCodepage() );

  int count = 0;
  while (true)
  {
    rsfile >> textEnt;
    if ( !rsfile.HasFoundText() )
      break;

    // compute text block size (px)
    textMsr.ComputeSize(textEnt);

    // text doesn't fit in rectangle area
    if ( textMsr.IsResizeNeeded() )
    {
      if (!quiet)
        textRep << textMsr.GetRealSize();
      ++count;
    }
  }

  return count;
}

//----------------------------------------------------------------------------
// Global helper functions.

void usage(char * aMsg = nullptr);
void printDPI();

int _tmain(int argc, _TCHAR ** argv)
{
  if (argc < 3)
  {
    usage( "Err: expected at least two arguments!" );
    return -1;
  }

  bool quiet = false;
  bool showdpi = true;
  wstring rsname;                       // resource script file name
  for (int i = 1; i < argc; ++i)        // read command arguments
  {
    wstring argvi( argv[i] );
    if ( (argvi == L"-f") || (argvi == L"--file") )
    {
      if (++i < argc)
        rsname = wstring( argv[i] );
    }
    else if ( (argvi == L"-q") || (argvi == L"--quiet") )
      quiet = true;
    else if (argvi == L"--no-header")
      showdpi = false;
    else
      wcout << L"Err: unknown argument: " << argv[i] << endl;
  }

  // check if we've got a name
  if ( rsname.empty() )
  {
    cout << endl;
    usage();
    return -1;
  }

  // DPI header
  if (!quiet && showdpi)
    printDPI();

  DWORD start = ::GetTickCount();
  RcTextChecker batch;
  int count = batch.Run(rsname, quiet);
  DWORD finish = ::GetTickCount();

  // display summary
  if (!quiet && (count > 0))
    cout << "No. of detected problems: " << count
         << " (in " << (finish - start) << "ms)" << endl << endl;

  return count;
}

// show all supported arguments
void usage(char * aMsg)
{
  if (nullptr != aMsg)
    cout << aMsg << endl
         << endl;

  cout << "Usage: ckrctxt [options]" << endl
       << endl
       << "Options:" << endl
       << "-q, --quiet           no output, just return the counter" << endl
       << "--no-header           do not show header for current DPI" << endl
       << "-f, --file NAME       Resource Script file name or path" << endl
       << endl;
}

// show current DPI settings
void printDPI()
{
  int dpiY = ::GetDeviceCaps(::GetDC(NULL), LOGPIXELSY);
  int dpiX = ::GetDeviceCaps(::GetDC(NULL), LOGPIXELSX);
  cout << "display dpi: " << dpiX << "|" << dpiY
                          << " (" << ::MulDiv(dpiY, 100, 96) << "%)"
                          << endl << endl;
}
