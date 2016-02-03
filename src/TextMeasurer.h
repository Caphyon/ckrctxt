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

#include "TextDlg.h"

// Layout Specifications: Win32
// http://msdn.microsoft.com/en-us/library/windows/desktop/bb226818.aspx
#define CONTROLBUTTON_X   12
#define CONTROLBUTTON_Y   (10 - 8)
#define PUSHBUTTON_X      (12 - 8)
#define PUSHBUTTON_Y      (14 - 8)

struct TextEntity;
struct TextSize;

/**
 * Measures the (minimum) required size of a TextEntity.
 *
 * @author  Teodor MICU
 */
class TextMeasurer
{
public:
  /**
   * Class constructor.
   */
  TextMeasurer();

  /**
   * Class destructor.
   */
  ~TextMeasurer();

  /**
   * Detect the required rectangle width (X) and height (Y) so
   * that the given text string would fit after word wrap.
   *
   * If rectangle height (px) is equal to text height (px), the text is
   * measured for one line fit.
   * If the rectangle height is greater than the text height, the text
   * will be wrapped in lines less or equal to the rectangle width (px).
   */
  void ComputeSize(TextEntity & /*aTextEnt*/);

  /**
   * Check if rectangle area is big enough to fit the text.
   *
   * @return  true is text doesn't fit in rectangle area, false otherwise
   */
  bool IsResizeNeeded() const;

  /**
   * @return  current and computed size of a text in du&px
   */
  TextSize GetRealSize() const;

private:
  TextDlg mDlg;                         // dialog were the size is computed
  TextEntity * mTextEnt;                // text block to measure
  LONG mRcWidth;                        // computed rectangle width (px)
  LONG mRcHeight;                       // computed rectangle height (px)
  LONG mTxtWidth;                       // computed text width (px)
  LONG mTxtHeight;                      // computed text height (px)
};
