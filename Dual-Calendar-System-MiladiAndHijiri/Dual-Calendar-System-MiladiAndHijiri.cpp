#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "MyGlobalTypes.h"
#include "MyInputLib.h"
#include "MyDateLib.h"
#include "MyHijriDateLib.h"

using namespace std;

// =============================================================================
//  Global application state
// =============================================================================

// Hijri manual offset: loaded from disk, may be modified by the user.
// Passed into every Hijri conversion call that accepts it.
short GlobalHijriOffset = 0;

const string OffsetFileName = "HijriOffset.txt";

// =============================================================================
//  Persistence Layer
// =============================================================================

// Loads GlobalHijriOffset from HijriOffset.txt.
// If the file doesn't exist or is unreadable, defaults to 0 and creates it.
void LoadHijriOffset()
{
    ifstream InFile(OffsetFileName);
    if (InFile.is_open())
    {
        int Value = 0;
        if (InFile >> Value)
            GlobalHijriOffset = (short)Value;
        InFile.close();
    }
    else
    {
        // File absent - initialise to 0 and persist it
        GlobalHijriOffset = 0;
        ofstream OutFile(OffsetFileName);
        if (OutFile.is_open())
        {
            OutFile << 0;
            OutFile.close();
        }
    }
}

// Overwrites HijriOffset.txt with the new value and updates the runtime state.
void SaveHijriOffset(short NewOffset)
{
    GlobalHijriOffset = NewOffset;
    ofstream OutFile(OffsetFileName, ios::trunc);
    if (OutFile.is_open())
    {
        OutFile << NewOffset;
        OutFile.close();
        cout << "\n  [OK] Offset saved to " << OffsetFileName << "\n";
    }
    else
    {
        cout << "\n  [ERROR] Could not write to " << OffsetFileName
            << ". Change is active for this session only.\n";
    }
}

// =============================================================================
//  Shared UI helpers
// =============================================================================

void PrintSeparator(char Ch = '=', int Width = 60)
{
    cout << "\n" << string(Width, Ch) << "\n";
}

void PrintHeader(const string& Title)
{
    PrintSeparator();
    cout << "  " << Title;
    PrintSeparator();
}

void PressEnterToContinue()
{
    cout << "\n  Press ENTER to continue...";
    system("pause>0");
}

// Reads a Gregorian date from the user with full validation.
stDate ReadGregorianDate(const string& Prompt = "")
{
    if (!Prompt.empty()) cout << Prompt << "\n";
    stDate Date;
    Date.Day = (short)MyInputLib::ReadNumberInRange(1, 31, "  Day    ", false);
    Date.Month = (short)MyInputLib::ReadNumberInRange(1, 12, "  Month  ", false);
    Date.Year = (short)MyInputLib::ReadPositiveNumber("  Year   : ");

    while (!MyDateLib::IsValidDate(Date))
    {
        cout << "  [!] " << MyDateLib::GetMonthFullName(Date.Month)
            << " only has " << MyDateLib::NumberOfDaysInMonth(Date)
            << " days. Re-enter the day:\n";
        Date.Day = (short)MyInputLib::ReadNumberInRange(1, 31, "  Day    ", false);
    }
    return Date;
}

// Reads a Hijri date from the user with full validation.
stHijriDate ReadHijriDate(const string& Prompt = "")
{
    if (!Prompt.empty()) cout << Prompt << "\n";
    stHijriDate Date;
    Date.Day = (short)MyInputLib::ReadNumberInRange(1, 30, "  Day    ", false);
    Date.Month = (short)MyInputLib::ReadNumberInRange(1, 12, "  Month  ", false);
    Date.Year = (short)MyInputLib::ReadPositiveNumber("  Year   : ");

    while (!MyHijriDateLib::IsValidDate(Date))
    {
        cout << "  [!] " << MyHijriDateLib::GetHijriMonthName(Date.Month)
            << " only has "
            << MyHijriDateLib::NumberOfDaysInHijriMonth(Date.Month, Date.Year)
            << " days. Re-enter the day:\n";
        Date.Day = (short)MyInputLib::ReadNumberInRange(1, 30, "  Day    ", false);
    }
    return Date;
}

// Displays the date format picker and returns the chosen format.
enDateFormat PickDateFormat()
{
    cout << "\n  Choose a display format:\n";
    cout << "  [1] DD/MM/YYYY\n";
    cout << "  [2] MM/DD/YYYY\n";
    cout << "  [3] YYYY/MM/DD\n";
    cout << "  [4] Full Text (DayName, DD/MonthName/YYYY)\n";
    cout << "  [5] YYYY/DD/MM\n";
    cout << "  [6] MM-DD-YYYY\n";
    cout << "  [7] DD-MM-YYYY\n";
    cout << "  [8] Named (Day: D, Month: M, Year: Y)\n";
    int Choice = MyInputLib::ReadNumberInRange(1, 8, "  Format choice");
    return (enDateFormat)Choice;
}

// =============================================================================
//  Screen 0: Accuracy Warning
//  Displayed once at startup - never suppressed.
// =============================================================================
void PrintAccuracyWarning()
{
    cout << "\n";
    cout << "  +----------------------------------------------------------+\n";
    cout << "  |           ASTRONOMICAL ACCURACY DISCLAIMER               |\n";
    cout << "  !----------------------------------------------------------!\n";
    cout << "  |  Hijri calendar calculations in this application are     |\n";
    cout << "  |  based on a TABULAR (arithmetic) model, not on actual    |\n";
    cout << "  |  lunar-sighting observations.                            |\n";
    cout << "  |                                                          |\n";
    cout << "  |  Results may vary by 1-3 days from the officially        |\n";
    cout << "  |  announced crescent moon dates, especially for ancient   |\n";
    cout << "  |  historical dates prior to 1000 AH.                      |\n";
    cout << "  |                                                          |\n";
    cout << "  |  Use Settings --> Adjust Hijri Offset to reconcile the   |\n";
    cout << "  |  displayed date with your regional authority's result.   |\n";
    cout << "  +----------------------------------------------------------+\n";
    PressEnterToContinue();
}

// =============================================================================
//  Screen: Settings
//  Shows current offset and lets the user update it (-3 to +3 days).
// =============================================================================
void ShowSettingsScreen()
{
    bool Stay = true;
    while (Stay)
    {
        system("cls");
        PrintHeader("SETTINGS  -  Hijri Calendar Offset");

        cout << "  The offset shifts the Hijri date returned by all\n"
            << "  Gregorian-->Hijri conversions.\n\n"
            << "  Current offset : " << (int)GlobalHijriOffset << " day(s)\n\n"
            << "  Example: If today's official Hijri date is 1 day ahead of\n"
            << "  what this app shows, set the offset to +1.\n";

        PrintSeparator('-');
        cout << "  [1] Change offset\n";
        cout << "  [2] Reset offset to 0\n";
        cout << "  [0] Back to Main Menu\n";
        PrintSeparator('-');

        int Choice = MyInputLib::ReadNumberInRange(0, 2, "  Your choice");
        switch (Choice)
        {
        case 1:
        {
            int NewVal = MyInputLib::ReadNumberInRange(-3, 3,
                "  New offset (-3 to +3)");
            SaveHijriOffset((short)NewVal);
            cout << "  Hijri offset updated to " << NewVal << " day(s).\n";
            break;
        }
        case 2:
            SaveHijriOffset(0);
            cout << "  Hijri offset reset to 0.\n";
            break;
        case 0:
            Stay = false;
            break;
        }
        if (Stay) PressEnterToContinue();
    }
}
// =============================================================================
//  Show All Gregorian 12 Screens 
// =============================================================================
void ShowTodayFullGregorianInfoScreen()
{
    system("cls");
    PrintHeader("TODAY'S GREGORIAN INFO");
    stDate D = MyDateLib::GetSystemDate();
    cout << "  Date        : " << MyDateLib::DateToString(D, FullDateText) << "\n"
        << "  Day of week : " << MyDateLib::GetDayLongName(D.Day, D.Month, D.Year) << "\n"
        << "  Day in year : " << MyDateLib::TotalDaysFromYearBeginning(D) << "\n"
        << "  Days left   : " << MyDateLib::DaysUntilEndOfYear(D) << "\n"
        << "  Leap year?  : " << (MyDateLib::IsLeapYear(D.Year) ? "Yes" : "No") << "\n"
        << "  Days in month: " << MyDateLib::NumberOfDaysInMonth(D) << "\n";

    // Also show today in Hijri
    stHijriDate HToday = MyHijriDateLib::ConvertGregorianToHijriWeekdayCorrect(D, GlobalHijriOffset);
    cout << "\n  Hijri equiv : " << MyHijriDateLib::PrintHijriDate(HToday)
        << "  (offset=" << (int)GlobalHijriOffset << ")\n";
}

void ShowConvertGregorianToHijriScreen()
{
    system("cls");
    PrintHeader("GREGORIAN  -->  HIJRI  CONVERSION");
    cout << "  [NOTE] Results are arithmetic approximations. Verify with\n"
        << "  lunar-sighting authorities for religious purposes.\n\n";
    stDate D = ReadGregorianDate("  Enter Gregorian date:");
    stHijriDate H = MyHijriDateLib::ConvertGregorianToHijriWeekdayCorrect(D, GlobalHijriOffset);
    cout << "\n  Gregorian : " << MyDateLib::DateToString(D, FullDateText) << "\n"
        << "  Hijri     : " << MyHijriDateLib::PrintHijriDate(H) << "\n"
        << "  Offset    : " << (int)GlobalHijriOffset << " day(s) applied\n";
}
void ShowCompareTwoGregorianDatesScreen()
{
    system("cls");
    PrintHeader("COMPARE TWO GREGORIAN DATES");
    stDate D1 = ReadGregorianDate("  Enter first date:");
    stDate D2 = ReadGregorianDate("  Enter second date:");
    enBeforAfter Cmp = MyDateLib::CompareDates(D1, D2);
    string s1 = MyDateLib::DateToString(D1);
    string s2 = MyDateLib::DateToString(D2);
    if (Cmp == enBeforAfter::Before) cout << "\n  " << s1 << "  is BEFORE  " << s2 << "\n";
    else if (Cmp == enBeforAfter::After)  cout << "\n  " << s1 << "  is AFTER   " << s2 << "\n";
    else                                  cout << "\n  Both dates are EQUAL.\n";
}
void ShowDateDifferenceBetweenTwoDatesScreen()
{
    system("cls");
    PrintHeader("DAYS BETWEEN TWO GREGORIAN DATES");
    stDate D1 = ReadGregorianDate("  Enter first date:");
    stDate D2 = ReadGregorianDate("  Enter second date:");
    int IncEnd = MyInputLib::ReadNumberInRange(0, 1, "  Include end day? (1=Yes, 0=No)");
    long Diff = MyDateLib::GetDifferenceInDays(D1, D2, IncEnd == 1);
    cout << "\n  Difference: " << Diff << " day(s)\n";
}
void ShowAddDaysMonthsYearsToDateScreen()
{
    system("cls");
    PrintHeader("ADD TO A GREGORIAN DATE");
    stDate D = ReadGregorianDate("  Starting date:");
    int  Days = MyInputLib::ReadNumber("  Days to add   : ");
    int  Months = MyInputLib::ReadNumber("  Months to add : ");
    int  Years = MyInputLib::ReadNumber("  Years to add  : ");

    stDate Result = D;
    if (Days > 0) Result = MyDateLib::IncreaseByXDays(Result, Days);
    if (Months > 0) Result = MyDateLib::IncreaseByXMonths(Result, Months);
    if (Years > 0) Result = MyDateLib::IncreaseByXYears(Result, Years);

    cout << "\n  Original : " << MyDateLib::DateToString(D, FullDateText) << "\n"
        << "  Result   : " << MyDateLib::DateToString(Result, FullDateText) << "\n";
}
void ShowSubtractDaysMonthsYearsToDateScreen()
{
    system("cls");
    PrintHeader("SUBTRACT FROM A GREGORIAN DATE");
    stDate D = ReadGregorianDate("  Starting date:");
    int  Days = MyInputLib::ReadNumber("  Days to subtract   : ");
    int  Months = MyInputLib::ReadNumber("  Months to subtract : ");
    int  Years = MyInputLib::ReadNumber("  Years to subtract  : ");

    stDate Result = D;
    if (Days > 0) Result = MyDateLib::DecreaseByXDays(Result, Days);
    if (Months > 0) Result = MyDateLib::DecreaseByXMonths(Result, Months);
    if (Years > 0) Result = MyDateLib::DecreaseByXYears(Result, Years);

    cout << "\n  Original : " << MyDateLib::DateToString(D, FullDateText) << "\n"
        << "  Result   : " << MyDateLib::DateToString(Result, FullDateText) << "\n";

}
void ShowMonthCalenderScreen()
{
    system("cls");
    PrintHeader("GREGORIAN MONTH CALENDAR");
    int M = MyInputLib::ReadNumberInRange(1, 12, "  Month");
    int Y = MyInputLib::ReadPositiveNumber("  Year  : ");
    cout << MyDateLib::BuildMonthCalendarString((short)M, (short)Y);
}
void ShowYearCalenderScreen()
{
    system("cls");
    PrintHeader("GREGORIAN YEAR CALENDAR");
    int Y = MyInputLib::ReadPositiveNumber("  Year  : ");
    cout << MyDateLib::BuildYearCalendarString((short)Y);
}
void ShowIsWeekendOrBusinessDayScreen()
{
    system("cls");
    PrintHeader("WEEKEND / BUSINESS DAY CHECK");
    stDate D = ReadGregorianDate("  Enter date:");
    cout << "\n  " << MyDateLib::DateToString(D, FullDateText) << "\n"
        << "  Weekend?     " << (MyDateLib::IsWeekend(D) ? "Yes" : "No") << "\n"
        << "  Business day? " << (MyDateLib::IsBusinessDay(D) ? "Yes" : "No") << "\n";
}
void ShowDaysRemainingInMonthAndYearScreen()
{
    system("cls");
    PrintHeader("DAYS REMAINING");
    stDate D = ReadGregorianDate("  Enter date:");
    cout << "\n  Days remaining in month : "
        << MyDateLib::DaysUntilEndOfMonth(D) << "\n"
        << "  Days remaining in year  : "
        << MyDateLib::DaysUntilEndOfYear(D) << "\n";
}
void ShowFormatDateScreen()
{
    system("cls");
    PrintHeader("FORMAT A GREGORIAN DATE");
    stDate D = ReadGregorianDate("  Enter date:");
    enDateFormat Fmt = PickDateFormat();
    cout << "\n  Formatted: " << MyDateLib::DateToString(D, Fmt) << "\n";
}
void ShowPeriodOverlapCheckerScreen()
{
    system("cls");
    PrintHeader("GREGORIAN PERIOD OVERLAP");
    cout << "  Period 1:\n";
    stPeriod P1;
    P1.StartDate = ReadGregorianDate("    Start date:");
    P1.EndDate = ReadGregorianDate("    End date:");
    cout << "  Period 2:\n";
    stPeriod P2;
    P2.StartDate = ReadGregorianDate("    Start date:");
    P2.EndDate = ReadGregorianDate("    End date:");

    bool Overlap = MyDateLib::IsOverlapPeriod(P1, P2);
    cout << "\n  Overlap? " << (Overlap ? "YES" : "NO") << "\n";
    if (Overlap)
        cout << "  Shared days: " << MyDateLib::CountOverlapDays(P1, P2) << "\n";

}


// =============================================================================
//  Screen: Gregorian (Miladi) Sub-Menu
// =============================================================================
void ShowMiladiMenu()
{
	system("cls");
    bool Stay = true;
    while (Stay)
    {
        system("cls");
        PrintHeader("GREGORIAN (MILADI) CALENDAR MENU");

        stDate Today = MyDateLib::GetSystemDate();
        cout << "  Today (system): "
            << MyDateLib::DateToString(Today, enDateFormat::FullDateText) << "\n";
        PrintSeparator('-');

        cout<< "  [1]  Show today's full Gregorian info\n"
            << "  [2]  Convert Gregorian --> Hijri\n"
            << "  [3]  Compare two Gregorian dates\n"
            << "  [4]  Date difference (days between two dates)\n"
            << "  [5]  Add days / months / years to a date\n"
            << "  [6]  Subtract days / months / years from a date\n"
            << "  [7]  Print month calendar\n"
            << "  [8]  Print year calendar\n"
            << "  [9]  Is date a weekend / business day?\n"
            << "  [10] Days remaining in month / year\n"
            << "  [11] Format a date in a chosen style\n"
            << "  [12] Period overlap checker\n"
            << "  [0]  Back to Main Menu\n";
        PrintSeparator('-');

        int Choice = MyInputLib::ReadNumberInRange(0, 12, "  Your choice");

        switch (Choice)
        {
            // ---- [1] Today's full info ----------------------------------------
        case 1:
        {
            ShowTodayFullGregorianInfoScreen();
            break;
        }

        // ---- [2] Gregorian --> Hijri conversion --------------------------------
        case 2:
        {
            ShowConvertGregorianToHijriScreen();
            break;
        }

        // ---- [3] Compare two dates -------------------------------------------
        case 3:
        {
			ShowCompareTwoGregorianDatesScreen();
            break;
        }

        // ---- [4] Difference in days ------------------------------------------
        case 4:
        {
            ShowDateDifferenceBetweenTwoDatesScreen();
            break;
        }

        // ---- [5] Add to a date -----------------------------------------------
        case 5:
        {
            ShowAddDaysMonthsYearsToDateScreen();
            break;
        }

        // ---- [6] Subtract from a date ----------------------------------------
        case 6:
        {
			ShowSubtractDaysMonthsYearsToDateScreen();
            break;
        }

        // ---- [7] Month calendar ----------------------------------------------
        case 7:
        {
			ShowMonthCalenderScreen();
            break;
        }

        // ---- [8] Year calendar -----------------------------------------------
        case 8:
        {
			ShowYearCalenderScreen();
            break;
        }

        // ---- [9] Weekend / business day check --------------------------------
        case 9:
        {
			ShowIsWeekendOrBusinessDayScreen();
            break;
        }

        // ---- [10] Days remaining in month / year ----------------------------
        case 10:
        {
			ShowDaysRemainingInMonthAndYearScreen();
            break;
        }

        // ---- [11] Format a date ----------------------------------------------
        case 11:
        {
			ShowFormatDateScreen();
            break;
        }

        // ---- [12] Period overlap ---------------------------------------------
        case 12:
        {
			ShowPeriodOverlapCheckerScreen();
            break;
        }

        case 0: Stay = false; break;
        }

        if (Stay && Choice != 0) PressEnterToContinue();
    }
}

// =============================================================================
//  Screen: Islamic (Hijri) Sub-Menu
// =============================================================================
// Individual Hijri screens (extracted from ShowHijriMenu)
void ShowTodayFullHijriInfoScreen()
{
    system("cls");
    PrintHeader("TODAY'S HIJRI INFO");
    stDate Gr = MyDateLib::GetSystemDate();
    stHijriDate H = MyHijriDateLib::ConvertGregorianToHijriWeekdayCorrect(
        Gr, GlobalHijriOffset);
    cout << "  Hijri date   : " << MyHijriDateLib::PrintHijriDate(H, false) << "\n"
        << "  Day of week  : " << MyHijriDateLib::GetDayOfWeekName(H, false) << "\n"
        << "  Days in month: " << MyHijriDateLib::NumberOfDaysInHijriMonth(H) << "\n"
        << "  Days in year : " << MyHijriDateLib::NumberOfDaysInHijriYear(H.Year) << "\n"
        << "  Leap year?   : "
        << (MyHijriDateLib::IsLeapHijriYear(H.Year) ? "Yes (Karimah)" : "No") << "\n"
        << "  Gregorian    : " << MyDateLib::DateToString(Gr, FullDateText) << "\n"
        << "  Offset used  : " << (int)GlobalHijriOffset << " day(s)\n";
}

void ShowConvertHijriToGregorianScreen()
{
    system("cls");
    PrintHeader("HIJRI  -->  GREGORIAN  CONVERSION");
    cout << "  [NOTE] Approximate result - verify for religious rulings.\n\n";
    stHijriDate H = ReadHijriDate("  Enter Hijri date:");
    stDate Gr = MyHijriDateLib::ConvertHijriToGregorian(H, GlobalHijriOffset);
    cout<<"\n  Hijri     : " << MyHijriDateLib::PrintHijriDate(H, false) << "\n"
        << "  Gregorian : " << MyDateLib::DateToString(Gr, FullDateText) << "\n"
        << "  Offset    : " << (int)GlobalHijriOffset << " day(s) applied\n";
}

void ShowCompareTwoHijriDatesScreen()
{
    system("cls");
    PrintHeader("COMPARE TWO HIJRI DATES");
    stHijriDate D1 = ReadHijriDate("  Enter first Hijri date:");
    stHijriDate D2 = ReadHijriDate("  Enter second Hijri date:");
    enBeforAfter Cmp = MyHijriDateLib::CompareDates(D1, D2);
    string s1 = MyHijriDateLib::DateToString(D1);
    string s2 = MyHijriDateLib::DateToString(D2);
    if (Cmp == enBeforAfter::Before) cout << "\n  " << s1 << "  is BEFORE  " << s2 << "\n";
    else if (Cmp == enBeforAfter::After)  cout << "\n  " << s1 << "  is AFTER   " << s2 << "\n";
    else                                  cout << "\n  Both dates are EQUAL.\n";
}

void ShowHijriDateDifferenceScreen()
{
    system("cls");
    PrintHeader("DAYS BETWEEN TWO HIJRI DATES");
    stHijriDate D1 = ReadHijriDate("  Enter first Hijri date:");
    stHijriDate D2 = ReadHijriDate("  Enter second Hijri date:");
    int IncEnd = MyInputLib::ReadNumberInRange(0, 1, "  Include end day? (1=Yes, 0=No)");
    long Diff = MyHijriDateLib::GetDifferenceInDays(D1, D2, IncEnd == 1);
    cout << "\n  Difference: " << Diff << " day(s)\n";
}

void ShowAddToHijriDateScreen()
{
    system("cls");
    PrintHeader("ADD TO A HIJRI DATE");
    stHijriDate D = ReadHijriDate("  Starting Hijri date:");
    int Days = MyInputLib::ReadNumber("  Days to add   : ");
    int Months = MyInputLib::ReadNumber("  Months to add : ");
    int Years = MyInputLib::ReadNumber("  Years to add  : ");

    stHijriDate Result = D;
    if (Days > 0) Result = MyHijriDateLib::IncreaseByXDays(Result, Days);
    if (Months > 0) Result = MyHijriDateLib::IncreaseByXMonths(Result, Months);
    if (Years > 0) Result = MyHijriDateLib::IncreaseByXYears(Result, Years);

    cout << "\n  Original : " << MyHijriDateLib::PrintHijriDate(D, false) << "\n"
        << "  Result   : " << MyHijriDateLib::PrintHijriDate(Result, false) << "\n";
}

void ShowSubtractFromHijriDateScreen()
{
    system("cls");
    PrintHeader("SUBTRACT FROM A HIJRI DATE");
    stHijriDate D = ReadHijriDate("  Starting Hijri date:");
    int Days = MyInputLib::ReadNumber("  Days to subtract   : ");
    int Months = MyInputLib::ReadNumber("  Months to subtract : ");
    int Years = MyInputLib::ReadNumber("  Years to subtract  : ");

    stHijriDate Result = D;
    if (Days > 0) Result = MyHijriDateLib::DecreaseByXDays(Result, Days);
    if (Months > 0) Result = MyHijriDateLib::DecreaseByXMonths(Result, Months);
    if (Years > 0) Result = MyHijriDateLib::DecreaseByXYears(Result, Years);

    cout << "\n  Original : " << MyHijriDateLib::PrintHijriDate(D, false) << "\n"
        << "  Result   : " << MyHijriDateLib::PrintHijriDate(Result, false) << "\n";
}

void ShowHijriLeapYearCheckScreen()
{
    system("cls");
    PrintHeader("HIJRI LEAP (KARIMAH) YEAR CHECK");
    int Y = MyInputLib::ReadPositiveNumber("  Hijri year: ");
    bool Leap = MyHijriDateLib::IsLeapHijriYear((short)Y);
    cout << "\n  Year " << Y << " AH is "
        << (Leap ? "a LEAP (Karimah) year - 355 days."
            : "a REGULAR year - 354 days.") << "\n";
}

void ShowDaysInHijriMonthScreen()
{
    system("cls");
    PrintHeader("DAYS IN A HIJRI MONTH");
    int M = MyInputLib::ReadNumberInRange(1, 12, "  Month");
    int Y = MyInputLib::ReadPositiveNumber("  Year  : ");
    short Days = MyHijriDateLib::NumberOfDaysInHijriMonth((short)M, (short)Y);
    cout << "\n  " << MyHijriDateLib::GetHijriMonthName((short)M)
        << " " << Y << " AH  has  " << Days << " day(s).\n";
}

void ShowFormatHijriDateScreen()
{
    system("cls");
    PrintHeader("FORMAT A HIJRI DATE");
    stHijriDate H = ReadHijriDate("  Enter Hijri date:");
    enDateFormat Fmt = PickDateFormat();
    cout << "\n  Formatted: " << MyHijriDateLib::DateToString(H, Fmt) << "\n";
}

void ShowHijriPeriodOverlapScreen()
{
    system("cls");
    PrintHeader("HIJRI PERIOD OVERLAP");
    cout << "  Period 1:\n";
    stHijriPeriod P1;
    P1.StartDate = ReadHijriDate("    Start date:");
    P1.EndDate = ReadHijriDate("    End date:");
    cout << "  Period 2:\n";
    stHijriPeriod P2;
    P2.StartDate = ReadHijriDate("    Start date:");
    P2.EndDate = ReadHijriDate("    End date:");

    bool Overlap = MyHijriDateLib::IsOverlapPeriod(P1, P2);
    cout << "\n  Overlap? " << (Overlap ? "YES" : "NO") << "\n";
    if (Overlap)
        cout << "  Shared days: " << MyHijriDateLib::CountOverlapDays(P1, P2) << "\n";
}
void ShowHijriMenu()
{
    system("cls");
    bool Stay = true;
    while (Stay)
    {
        system("cls");
        PrintHeader("ISLAMIC (HIJRI) CALENDAR MENU");

        stDate Today = MyDateLib::GetSystemDate();
        stHijriDate HToday = MyHijriDateLib::ConvertGregorianToHijriWeekdayCorrect(
            Today, GlobalHijriOffset);
        cout << "  Today (Hijri) : " << MyHijriDateLib::PrintHijriDate(HToday)
            << "  (offset=" << (int)GlobalHijriOffset << ")\n";
        PrintSeparator('-');

        cout<< "  [1]  Show today's full Hijri info\n"
            << "  [2]  Convert Hijri --> Gregorian\n"
            << "  [3]  Compare two Hijri dates\n"
            << "  [4]  Date difference (days between two Hijri dates)\n"
            << "  [5]  Add days / months / years to a Hijri date\n"
            << "  [6]  Subtract days / months / years from a Hijri date\n"
            << "  [7]  Is a Hijri year a leap (Karimah) year?\n"
            << "  [8]  Days in a Hijri month\n"
            << "  [9]  Format a Hijri date\n"
            << "  [10] Hijri period overlap checker\n"
            << "  [0]  Back to Main Menu\n";
        PrintSeparator('-');

        int Choice = MyInputLib::ReadNumberInRange(0, 10, "  Your choice");

        switch (Choice)
        {
            // ---- [1] Today's Hijri info ------------------------------------------
        case 1:
        {
            ShowTodayFullHijriInfoScreen();
            break;
        }

        // ---- [2] Hijri --> Gregorian -------------------------------------------
        case 2: { ShowConvertHijriToGregorianScreen(); break; }

        // ---- [3] Compare two Hijri dates ------------------------------------
        case 3: { ShowCompareTwoHijriDatesScreen(); break; }

        // ---- [4] Difference in days -----------------------------------------
        case 4: { ShowHijriDateDifferenceScreen(); break; }

        // ---- [5] Add to a Hijri date -----------------------------------------
        case 5: { ShowAddToHijriDateScreen(); break; }

        // ---- [6] Subtract from a Hijri date ----------------------------------
        case 6: { ShowSubtractFromHijriDateScreen(); break; }

        // ---- [7] Leap year check ---------------------------------------------
        case 7: { ShowHijriLeapYearCheckScreen(); break; }

        // ---- [8] Days in a Hijri month ---------------------------------------
        case 8: { ShowDaysInHijriMonthScreen(); break; }

        // ---- [9] Format a Hijri date -----------------------------------------
        case 9: { ShowFormatHijriDateScreen(); break; }

        // ---- [10] Hijri period overlap ---------------------------------------
        case 10: { ShowHijriPeriodOverlapScreen(); break; }

        case 0: Stay = false; break;
        }

        if (Stay && Choice != 0) PressEnterToContinue();
    }
}

// =============================================================================
//  Screen: Main Menu  (application event loop)
// =============================================================================
void ShowMainMenu()
{
    system("cls");
    bool Running = true;
    while (Running)
    {
        system("cls");
        PrintHeader("\tDual Calendar System Main Menu");

        stDate Today = MyDateLib::GetSystemDate();
        stHijriDate HToday = MyHijriDateLib::ConvertGregorianToHijriWeekdayCorrect(
            Today, GlobalHijriOffset);

        cout << "  " << MyDateLib::DateToString(Today, enDateFormat::FullDateText) << "\n" 
            << "  " << to_string(HToday.Day) << "/"
            << MyHijriDateLib::GetHijriMonthName(HToday.Month) << "/"
            << to_string(HToday.Year) << " AH  (Hijri offset: " << (int)GlobalHijriOffset << ")\n";
        PrintSeparator('-');

        cout<< "  [1] Gregorian (Miladi) Calendar Tools\n"
            << "  [2] Islamic   (Hijri)  Calendar Tools\n"
            << "  [3] Settings  (Hijri offset adjustment)\n"
            << "  [0] Exit\n";
        PrintSeparator('-');

        int Choice = MyInputLib::ReadNumberInRange(0, 3, " Please Enter Your choice: ");

        switch (Choice)
        {
        case 1: ShowMiladiMenu();   break;
        case 2: ShowHijriMenu();    break;
        case 3: ShowSettingsScreen(); break;
        case 0:
            cout << "\n  May your time be blessed. Goodbye!\n\n";
            Running = false;
            break;
        }
    }
}

int main()
{
    LoadHijriOffset();

    PrintAccuracyWarning();

    ShowMainMenu();

    return 0;
}