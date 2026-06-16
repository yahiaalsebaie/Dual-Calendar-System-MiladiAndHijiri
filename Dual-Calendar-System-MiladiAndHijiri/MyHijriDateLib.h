

//  بسم الله الرحمن الرحيم
#pragma once
#pragma warning(disable : 4996)
//  MyHijriDateLib.h  -  Islamic (Hijri) Calendar Computation Library
#include "MyGlobalTypes.h"   // stHijriDate, stHijriPeriod, enBeforAfter, enDateFormat
#include "MyDateLib.h"       // GetDayOfWeekOrder() for accurate Gregorian weekday
#include <string>
#include <vector>

using namespace std;

namespace MyHijriDateLib
{
    // =========================================================================
    //  Internal constants
    // =========================================================================

    // Aligned with Gregorian: 0=Sunday … 6=Saturday
    const string DayLongName[7] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };
    const string DayShortName[7] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };

    // Leap years inside a 30-year Hijri cycle (11 out of 30)
    const short LeapHijriYears[] = { 2, 5, 7, 10, 13, 16, 18, 21, 24, 26, 29 };

    const string HijriMonthNames[] = {
        "", "Muharram", "Safar", "Rabi' al-Awwal", "Rabi' al-Thani",
        "Jumada al-Awwal", "Jumada al-Thani", "Rajab", "Sha'ban",
        "Ramadan", "Shawwal", "Zhu al-Qi'dah", "Zhu al-Hijjah"
    };

    // =========================================================================
    //  Year / Month utilities
    // =========================================================================

    bool IsLeapHijriYear(short Year)
    {
        short IndexInCycle = Year % 30;
        for (short i = 0; i < 11; i++)
            if (IndexInCycle == LeapHijriYears[i]) return true;
        return false;
    }

    short NumberOfDaysInHijriYear(short Year)
    {
        return IsLeapHijriYear(Year) ? 355 : 354;
    }

    short NumberOfDaysInHijriMonth(short Month, short Year)
    {
        if (Month < 1 || Month > 12) return 0;
        if (Month == 12) return IsLeapHijriYear(Year) ? 30 : 29;
        return (Month % 2 != 0) ? 30 : 29;   // odd=30, even=29
    }

    short NumberOfDaysInHijriMonth(stHijriDate Date)
    {
        return NumberOfDaysInHijriMonth(Date.Month, Date.Year);
    }

    string GetHijriMonthName(short MonthNumber)
    {
        return HijriMonthNames[MonthNumber];
    }

    // =========================================================================
    /*
      * HijriOffset - applied AFTER the raw sum, shifting the result by N days.
      * A positive offset moves the date forward; negative moves it back.
      * This lets users reconcile a 1-2 day lunar-sighting discrepancy
      * without touching the astronomical formula.
      * Absolute day count from Hijri epoch(1 Muharram 1 AH)
    */
    // =========================================================================
    long GetAbsoluteHijriDays(short Day, short Month, short Year, short HijriOffset = 0)
    {
        long Total = 0;
        for (short y = 1; y < Year; y++) Total += NumberOfDaysInHijriYear(y);
        for (short m = 1; m < Month; m++) Total += NumberOfDaysInHijriMonth(m, Year);
        Total += Day;
        return Total + HijriOffset;
    }

    long GetAbsoluteHijriDays(stHijriDate Date, short HijriOffset = 0)
    {
        return GetAbsoluteHijriDays(Date.Day, Date.Month, Date.Year, HijriOffset);
    }

    // =========================================================================
    //  Reverse: absolute Hijri days -> stHijriDate
    //  Offset must already be embedded in AbsoluteDays before calling.
    // =========================================================================
    stHijriDate GetHijriDateFromAbsoluteDays(long AbsoluteDays)
    {
        stHijriDate Date;

        short Year = 1;
        while (true)
        {
            short DaysInYear = NumberOfDaysInHijriYear(Year);
            if (AbsoluteDays > DaysInYear) { AbsoluteDays -= DaysInYear; Year++; }
            else break;
        }
        Date.Year = Year;

        short Month = 1;
        while (true)
        {
            short DaysInMonth = NumberOfDaysInHijriMonth(Month, Date.Year);
            if (AbsoluteDays > DaysInMonth) { AbsoluteDays -= DaysInMonth; Month++; }
            else break;
        }
        Date.Month = Month;
        Date.Day = (short)AbsoluteDays;
        return Date;
    }

    // =========================================================================
    //  Day-of-week
    //  1 Muharram 1 AH = Friday --> index 5 in 0-based Sun…Sat array
    //  Offset +5 aligns the epoch correctly.
    // =========================================================================
    short GetDayOfWeekIndex(short Day, short Month, short Year)
    {
        long AbsDays = GetAbsoluteHijriDays(Day, Month, Year, 0); // No offset for weekday..
        return (short)((AbsDays + 5 - 1) % 7);
    }

    short GetDayOfWeekIndex(stHijriDate Date)
    {
        return GetDayOfWeekIndex(Date.Day, Date.Month, Date.Year);
    }

    string GetDayOfWeekName(short Day, short Month, short Year, bool ShortName = false)
    {
        short Idx = GetDayOfWeekIndex(Day, Month, Year);
        return ShortName ? DayShortName[Idx] : DayLongName[Idx];
    }

    string GetDayOfWeekName(stHijriDate Date, bool ShortName = false)
    {
        return GetDayOfWeekName(Date.Day, Date.Month, Date.Year, ShortName);
    }

    // =========================================================================
    //  Validation
    // =========================================================================
    bool IsValidDate(stHijriDate Date)
    {
        if (Date.Month < 1 || Date.Month > 12) return false;
        if (Date.Year < 1) return false;
        if (Date.Day   < 1 || Date.Day > NumberOfDaysInHijriMonth(Date.Month, Date.Year)) return false;
        return true;
    }

    // =========================================================================
    //  Gregorian helpers duplicated here to keep the library self-contained
    //  (avoids circular includes while staying independent of MyDateLib internals)
    // =========================================================================
    bool IsLeapGregorianYear(short Year)
    {
        return ((Year % 400 == 0) || (Year % 100 != 0 && Year % 4 == 0));
    }

    short DaysInGregorianMonth(short Month, short Year)
    {
        if (Month < 1 || Month > 12) return 0;
        if (Month == 2) return IsLeapGregorianYear(Year) ? 29 : 28;
        return (Month == 4 || Month == 6 || Month == 9 || Month == 11) ? 30 : 31;
    }

    long GetAbsoluteGregorianDays(short Day, short Month, short Year)
    {
        long Total = 0;
        for (short y = 1; y < Year; y++) Total += IsLeapGregorianYear(y) ? 366 : 365;
        for (short m = 1; m < Month; m++) Total += DaysInGregorianMonth(m, Year);
        Total += Day;
        return Total;
    }

    // =========================================================================
    //  Core conversion:  Gregorian --> Hijri
    //
    //  HijriOffset is subtracted from the Gregorian absolute-day count before
    //  the Hijri epoch offset (227015) is applied, then added back as an
    //  absolute-day shift so the final stHijriDate reflects the user correction.
    //
    //  Why here and not in GetAbsoluteHijriDays?
    //    Because the direction of the offset flips for this conversion path:
    //    +N in "Hijri --> Gregorian" means the Hijri date is N days later,
    //    so we ADD to the Gregorian absolute days.
    //    We keep both paths consistent by applying the offset to the
    //    intermediary absolute Hijri day count in the same direction.
    // =========================================================================
    stHijriDate ConvertGregorianToHijri(short Day, short Month, short Year,
        short HijriOffset = 0)
    {
        long AbsGreg = GetAbsoluteGregorianDays(Day, Month, Year);

        // 227015 = Gregorian absolute days up to 15 July 622 CE
        // (the conventionally accepted Hijri epoch anchor)
        long AbsHijri = AbsGreg - 227015 + HijriOffset;

        if (AbsHijri <= 0) return { 1, 1, 1 }; // guard for pre-Hijra dates

        return GetHijriDateFromAbsoluteDays(AbsHijri);
    }

    stHijriDate ConvertGregorianToHijri(stDate GregorianDate, short HijriOffset = 0)
    {
        return ConvertGregorianToHijri(
            GregorianDate.Day, GregorianDate.Month, GregorianDate.Year, HijriOffset);
    }

    // =========================================================================
    //  Core conversion:  Hijri --> Gregorian  (returns stDate)
    //  Note: The "Notice" that was previously emitted via cout inside this
    //  function has been removed. The UI layer displays accuracy warnings
    //  via PrintAccuracyWarning() at startup and beside conversion results.
    // =========================================================================
    stDate ConvertHijriToGregorian(short Day, short Month, short Year,
        short HijriOffset = 0)
    {
        long AbsHijri = GetAbsoluteHijriDays(Day, Month, Year, HijriOffset);
        long AbsGreg = AbsHijri + 227015;

        short gYear = 1;
        while (true)
        {
            long DaysInYear = IsLeapGregorianYear(gYear) ? 366 : 365;
            if (AbsGreg > DaysInYear) { AbsGreg -= DaysInYear; gYear++; }
            else break;
        }

        short gMonth = 1;
        while (true)
        {
            short D = DaysInGregorianMonth(gMonth, gYear);
            if (AbsGreg > D) { AbsGreg -= D; gMonth++; }
            else break;
        }

        return { (short)AbsGreg, gMonth, gYear };
    }

    stDate ConvertHijriToGregorian(stHijriDate Date, short HijriOffset = 0)
    {
        return ConvertHijriToGregorian(Date.Day, Date.Month, Date.Year, HijriOffset);
    }

    // =========================================================================
    //  Weekday-corrected Gregorian-->Hijri conversion
    //  Computes the approximate Hijri date, then snaps to the nearest date
    //  whose weekday matches the authoritative Gregorian weekday.
    //  This corrects the occasional 1-day slip from the arithmetic model.
    // =========================================================================
    stHijriDate ConvertGregorianToHijriWeekdayCorrect(short Day, short Month, short Year,
        short HijriOffset = 0)
    {
        // Authoritative weekday from Gregorian algorithm
        short TargetWeekday = MyDateLib::GetDayOfWeekOrder(Day, Month, Year);

        // Arithmetic approximation (without user offset) - perform weekday
        // correction first, then apply user HijriOffset as a final shift.
        stHijriDate Approx = ConvertGregorianToHijri(Day, Month, Year, 0);

        // Weekday of the approximation
        short CalcWeekday = GetDayOfWeekIndex(Approx);

        short AutoOffset = TargetWeekday - CalcWeekday;
        if (AutoOffset > 3) AutoOffset -= 7;   // shortest path wrap
        if (AutoOffset < -3) AutoOffset += 7;

        if (AutoOffset == 0)
        {
            // Apply user offset and return
            long Abs = GetAbsoluteHijriDays(Approx, 0) + HijriOffset;
            return GetHijriDateFromAbsoluteDays(Abs);
        }

        long CorrectedAbsDays = GetAbsoluteHijriDays(Approx, 0) + AutoOffset + HijriOffset;
        return GetHijriDateFromAbsoluteDays(CorrectedAbsDays);
    }

    stHijriDate ConvertGregorianToHijriWeekdayCorrect(stDate GregorianDate,
        short HijriOffset = 0)
    {
        return ConvertGregorianToHijriWeekdayCorrect(
            GregorianDate.Day, GregorianDate.Month, GregorianDate.Year, HijriOffset);
    }

    // =========================================================================
    //  Formatting helpers (return strings - no cout)
    // =========================================================================

    string DateToString(stHijriDate Date, enDateFormat Format = enDateFormat::DayMonthYearSlash)
    {
        string D = to_string(Date.Day);
        string M = to_string(Date.Month);
        string Y = to_string(Date.Year);

        switch (Format)
        {
        case DayMonthYearSlash:  return D + "/" + M + "/" + Y;
        case MonthDayYearSlash:  return M + "/" + D + "/" + Y;
        case YearMonthDaySlash:  return Y + "/" + M + "/" + D;
        case FullDateText:
            return GetDayOfWeekName(Date, false)
                + ", " + D + "/" + HijriMonthNames[Date.Month] + "/" + Y + " AH";
        case YearDayMonthSlash:  return Y + "/" + D + "/" + M;
        case MonthDayYearDash:   return M + "-" + D + "-" + Y;
        case DayMonthYearDash:   return D + "-" + M + "-" + Y;
        case NamedCustom:
            return "Day: " + D + ", Month: " + GetHijriMonthName(Date.Month) + ", Year: " + Y + " AH";
        default:
            return D + "/" + M + "/" + Y;
        }
    }

    // Short one-liner for display: "Fri, 15/Ramadan/1446 AH"
    string PrintHijriDate(stHijriDate Date, bool ShortDayName = true)
    {
        return GetDayOfWeekName(Date, ShortDayName)
            + ", " + to_string(Date.Day)
            + "/" + HijriMonthNames[Date.Month]
            + "/" + to_string(Date.Year) + " AH";
    }

    // Parse "dd/mm/yyyy" (any single delimiter)
    stHijriDate StringToHijriDate(const string& sDate)
    {
        stHijriDate Out = { 0, 0, 0 };
        size_t P1 = sDate.find_first_of("/-.,\\|* ");
        if (P1 == string::npos) return Out;
        size_t P2 = sDate.find_first_of("/-.,\\|* ", P1 + 1);
        if (P2 == string::npos) return Out;
        try
        {
            Out.Day = (short)stoi(sDate.substr(0, P1));
            Out.Month = (short)stoi(sDate.substr(P1 + 1, P2 - P1 - 1));
            Out.Year = (short)stoi(sDate.substr(P2 + 1));
        }
        catch (...) { return { 0, 0, 0 }; }
        return Out;
    }

    // =========================================================================
    //  Date navigation (increment / decrement)
    // =========================================================================

    bool IsLastDayInMonth(stHijriDate Date)
    {
        return Date.Day == NumberOfDaysInHijriMonth(Date);
    }

    stHijriDate CorrectInvalidDay(stHijriDate Date)
    {
        short Max = NumberOfDaysInHijriMonth(Date.Month, Date.Year);
        if (Date.Day > Max) Date.Day = Max;
        return Date;
    }

    stHijriDate IncreaseByOneDay(stHijriDate Date)
    {
        if (Date.Month == 12 && IsLastDayInMonth(Date))
        {
            Date.Day = 1; Date.Month = 1; Date.Year++; return Date;
        }
        if (IsLastDayInMonth(Date))
        {
            Date.Day = 1; Date.Month++; return Date;
        }
        Date.Day++;
        return Date;
    }

    stHijriDate IncreaseByXDays(stHijriDate Date, int Days)
    {
        for (int i = 0; i < Days; i++) Date = IncreaseByOneDay(Date);
        return Date;
    }

    stHijriDate IncreaseByOneWeek(stHijriDate Date) { return IncreaseByXDays(Date, 7); }
    stHijriDate IncreaseByXWeeks(stHijriDate Date, int W)
    {
        for (int i = 0; i < W; i++) Date = IncreaseByOneWeek(Date);
        return Date;
    }

    stHijriDate IncreaseByOneMonth(stHijriDate Date)
    {
        if (Date.Month == 12) { Date.Month = 1; Date.Year++; }
        else Date.Month++;
        return CorrectInvalidDay(Date);
    }

    stHijriDate IncreaseByXMonths(stHijriDate Date, int Months)
    {
        for (int i = 0; i < Months; i++) Date = IncreaseByOneMonth(Date);
        return Date;
    }

    stHijriDate IncreaseByOneYear(stHijriDate Date) { Date.Year++; return CorrectInvalidDay(Date); }
    stHijriDate IncreaseByXYears(stHijriDate Date, int Y)
    {
        for (int i = 0; i < Y; i++) Date = IncreaseByOneYear(Date);
        return Date;
    }

    stHijriDate DecreaseByOneDay(stHijriDate Date)
    {
        if (Date.Month == 1 && Date.Day == 1)
        {
            Date.Month = 12; Date.Year--; Date.Day = NumberOfDaysInHijriMonth(12, Date.Year); return Date;
        }
        if (Date.Day == 1)
        {
            Date.Month--; Date.Day = NumberOfDaysInHijriMonth(Date.Month, Date.Year); return Date;
        }
        Date.Day--;
        return Date;
    }

    stHijriDate DecreaseByXDays(stHijriDate Date, int Days)
    {
        for (int i = 0; i < Days; i++) Date = DecreaseByOneDay(Date);
        return Date;
    }

    stHijriDate DecreaseByOneMonth(stHijriDate Date)
    {
        if (Date.Month == 1) { Date.Month = 12; Date.Year--; }
        else Date.Month--;
        return CorrectInvalidDay(Date);
    }

    stHijriDate DecreaseByXMonths(stHijriDate Date, int M)
    {
        for (int i = 0; i < M; i++) Date = DecreaseByOneMonth(Date);
        return Date;
    }

    stHijriDate DecreaseByOneYear(stHijriDate Date) { Date.Year--; return CorrectInvalidDay(Date); }
    stHijriDate DecreaseByXYears(stHijriDate Date, int Y)
    {
        for (int i = 0; i < Y; i++) Date = DecreaseByOneYear(Date);
        return Date;
    }

    // =========================================================================
    //  Comparison
    // =========================================================================
    bool IsDate1EqualDate2(stHijriDate D1, stHijriDate D2)
    {
        return D1.Year == D2.Year && D1.Month == D2.Month && D1.Day == D2.Day;
    }

    bool IsDate1BeforeDate2(stHijriDate D1, stHijriDate D2)
    {
        return (D1.Year < D2.Year) ? true :
            (D1.Year > D2.Year) ? false :
            (D1.Month < D2.Month) ? true :
            (D1.Month > D2.Month) ? false :
            (D1.Day < D2.Day);
    }

    bool IsDate1AfterDate2(stHijriDate D1, stHijriDate D2)
    {
        return !IsDate1BeforeDate2(D1, D2) && !IsDate1EqualDate2(D1, D2);
    }

    enBeforAfter CompareDates(stHijriDate D1, stHijriDate D2)
    {
        if (IsDate1BeforeDate2(D1, D2)) return enBeforAfter::Before;
        return IsDate1EqualDate2(D1, D2) ? enBeforAfter::Equal : enBeforAfter::After;
    }

    void SwapDates(stHijriDate& D1, stHijriDate& D2) { stHijriDate t = D1; D1 = D2; D2 = t; }

    long GetDifferenceInDays(stHijriDate D1, stHijriDate D2, bool IncludeEndDay = false)
    {
        long A = GetAbsoluteHijriDays(D1, 0);
        long B = GetAbsoluteHijriDays(D2, 0);
        if (A > B) { long tmp = A; A = B; B = tmp; }
        long Diff = B - A;
        return IncludeEndDay ? Diff + 1 : Diff;
    }

    // =========================================================================
    //  Period utilities (uses stHijriPeriod from MyGlobalTypes.h)
    // =========================================================================
    bool IsOverlapPeriod(stHijriPeriod P1, stHijriPeriod P2, bool IncludeLastDay = true)
    {
        if (IncludeLastDay)
        {
            if (CompareDates(P1.EndDate, P2.StartDate) == enBeforAfter::Before) return false;
            if (CompareDates(P2.EndDate, P1.StartDate) == enBeforAfter::Before) return false;
        }
        else
        {
            if (CompareDates(P1.EndDate, P2.StartDate) != enBeforAfter::After) return false;
            if (CompareDates(P2.EndDate, P1.StartDate) != enBeforAfter::After) return false;
        }
        return true;
    }

    int CalculatePeriodLength(stHijriPeriod P, bool IncludeEndDay = false)
    {
        return (int)GetDifferenceInDays(P.StartDate, P.EndDate, IncludeEndDay);
    }

    bool IsDateInPeriod(stHijriPeriod P, stHijriDate Date)
    {
        return (CompareDates(P.StartDate, Date) != enBeforAfter::After)
            && (CompareDates(P.EndDate, Date) != enBeforAfter::Before);
    }

    int CountOverlapDays(stHijriPeriod P1, stHijriPeriod P2)
    {
        if (!IsOverlapPeriod(P1, P2)) return 0;
        stHijriPeriod Shared;
        Shared.StartDate = (CompareDates(P1.StartDate, P2.StartDate) == enBeforAfter::After)
            ? P1.StartDate : P2.StartDate;
        Shared.EndDate = (CompareDates(P1.EndDate, P2.EndDate) == enBeforAfter::Before)
            ? P1.EndDate : P2.EndDate;
        return CalculatePeriodLength(Shared, true);
    }

} // namespace MyHijriDateLib