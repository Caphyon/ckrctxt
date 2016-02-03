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

struct TextEntity;

/**
 * Reader for a resource script file. Ignores all non-text blocks.
 *
 * @author  Teodor MICU
 */
class RcTextReader
{
public:
  /**
   * Class constructor.
   */
  RcTextReader(const std::wstring & /*aFilename*/);

  /**
   * @return  true if the file was found and is open for reading
   */
  bool IsOpen() const;

  /**
   * @return  the ANSI codepage found in the resource script file
   */
  UINT GetCodepage() const;

  /**
   * Method that checks if a text block was (previously) read from file.
   *
   * @return  true if last extraction was successful, false otherwise
   */
  bool HasFoundText() const;

  /**
   * Read the next text block with all associated parameters.
   */
  RcTextReader & operator>>(TextEntity & /*aTextEnt*/);

private:
  /**
   * Get the string from pragma code_page() directive, dot prefixed.
   * (".1252" for ENGLISH_US, the default ANSI encoding)
   */
  std::string DotCodepage(const std::wstring & /*aFilename*/);

  std::wifstream mIStream;    // (file) input stream
  UINT mCodepage;

  // attributes read from the resource script file
  std::wstring mDlgName;      // current dialog name
  int mLineNr;                // current line# read from file
  bool mFound;                // new text block read from file
};
