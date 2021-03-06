ckrctxt :: RC Text Bounds Checker
=================================

`ckrctxt` is a simple tool that can check if any static *text block* (single or multi line) from a Visual Studio [resource file (RC)][RC] doesn't fit on given rectangle **size**.
On application UI the text appears truncated or *clipped*.

I assume most software developers write Win32 applications to be [High DPI][hidpi] aware.
Otherwise, it would be immediately visible if an application is [not ready for High DPI][issues] because everything appears blury and text is pixelated.

From all [High DPI issues][issues], this tool is a solution for the most subtle one: [Clipped UI elements or text][clipped].

A dialog created with Visual Studio resource editor usually looks fine on standard DPI.
However, after changing the Windows display DPI to other standard configuration (125%, 150%, 200% and up to 500% on Windows 10) you might notice that the full text doesn't fit anymore.

`ckrctxt` tool was created to automatically detect such problems at build for [Advanced Installer][advinst].

[advinst]: http://www.advancedinstaller.com/
[hidpi]: https://msdn.microsoft.com/en-us/library/dd464646.aspx
[issues]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn469266.aspx#appendix_c_common_high_dpi_issues
[clipped]: https://msdn.microsoft.com/en-us/library/windows/desktop/dn469266.aspx#clipped_ui_elements_or_text
[RC]: https://msdn.microsoft.com/en-us/library/windows/desktop/aa380599.aspx


### Build automation

An integration into any build system will basically execute:

	ckrctxt --file [path-to-project].rc

Performance: it can scan a RC file with 40k lines, from which 4.5k text blocks to analyse in just *~300ms*.


### Command line options

    Usage: ckrctxt [options]

    Options:
    -q, --quiet           no output, just return the counter
    --no-header           do not show header for current DPI
    -f, --file NAME       Resource Script file name or path


Examples
--------

Single line text found at 150% display DPI:

    display dpi: 144|144 (150%)

    Dialog ID: IDD_EDIT_BUILD
    -------------------------------
    15225: LTEXT "Version:"
    - detected text size: (59,19)px
    - detected rect size: (25,8)du -> (56,19)px
    + minimum required rectangle size: (26,8)du

    15227: LTEXT "Build:"
    - detected text size: (41,19)px
    - detected rect size: (16,8)du -> (36,19)px
    + minimum required rectangle size: (18,8)du

    Dialog ID: IDD_FORM_CREATE_FILE
    -------------------------------
    17156: CONTROL "Create file with encoding:"
    - detected text size: (181,19)px
    - detected rect size: (90,10)du -> (178,24)px
    + minimum required rectangle size: (93,10)du

Quick indication: the *CONTROL* element on line *17156* from *IDD_FORM_CREATE_FILE* dialog should have *93du* width.

Multi line text found at 150% display DPI:

	display dpi: 144|144 (150%)

    Dialog ID: IDD_WIZARD_INSTALL
    -------------------------------
    17246: CONTROL "The XYZ will not be installed by default.
                    The user can choose to install it from ...
                    or by selecting ..."
    - detected text size: (377,52)px
    - detected rect size: (267,29)du -> (383,47)px
    + minimum required rectangle size: (263,32)du  [MULTILINE]

Quick indication: the *CONTROL* element on line *17246* from *IDD_WIZARD_INSTALL* dialog should be extended to four lines (*32du*).

Full description:

- the multi line text from this checkbox fits in a rectangle of (377,52) pixels
- the rectangle of this *CONTROL* element is (267,29) dialog units, converted to pixels  this is (383,47)
- the required text height is 52px, but the rectangle height is 47px
- the solution is to modify the rectangle height to 52px, aproximated to 32du


Implementation limits
---------------------

At this time, the internal parser searches only for these text [elements][ID]:

- `[LCR]TEXT`
- `[DEF]PUSHBUTTON`
- `CONTROL` (check, 3state, radio)
- `RADIOBUTTON`

[ID]: https://msdn.microsoft.com/en-us/library/windows/desktop/aa381043.aspx
