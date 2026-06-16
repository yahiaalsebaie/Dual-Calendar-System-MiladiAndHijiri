#pragma once
#pragma warning(disable : 4996)

#include "MyGlobalTypes.h"   // stDate, stPeriod, enBeforAfter, enDateFormat
#include <string>
#include <iomanip>
#include <ctime>

using namespace std;

//  MyDateLib.h  -  Gregorian (Miladi) Calendar Computation Library
namespace MyDateLib
{
    // =========================================================================
    //  Internal constants
    // =========================================================================
    const string DayNameShort[7] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
    const string MonthNamesLong[13] = {
        "","January","February","March","April","May","June",
        "July","August","September","October","November","December"
    };
    const string MonthNamesShort[13] = {
        "","Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };

 
    string  GetMonthFullName(short MonthNumber);
    short   NumberOfDaysInMonth(stDate Date);
    bool    IsValidDate(stDate Date);

    // =========================================================================
    //  Year utilities
    // =========================================================================
    bool IsLeapYear(short Year)
    {
        return ((Year % 400 == 0) || (Year % 100 != 0 && Year % 4 == 0));
    }

    short NumberOfDaysInYear(short Year)
    {
        return IsLeapYear(Year) ? 366 : 365;
    }

    // =========================================================================
    //  Month utilities
    // =========================================================================
    short NumberOfDaysInMonth(short Month, short Year)
    {
        if (Month < 1 || Month > 12) return 0;
        if (Month == 2) return IsLeapYear(Year) ? 29 : 28;
        return (Month == 4 || Month == 6 || Month == 9 || Month == 11) ? 30 : 31;
    }

    short NumberOfDaysInMonth(stDate Date)
    {
        return NumberOfDaysInMonth(Date.Month, Date.Year);
    }

    string GetMonthFullName(short MonthNumber)
    {
        return MonthNamesLong[MonthNumber];
    }

    string GetMonthShortName(short MonthNumber)
    {
        return MonthNamesShort[MonthNumber];
    }

    // =========================================================================
    //  Validation
    // =========================================================================
    bool IsValidDate(stDate Date)
    {
        if (Date.Month < 1 || Date.Month > 12) return false;
        if (Date.Day   < 1 || Date.Day > NumberOfDaysInMonth(Date.Month, Date.Year)) return false;
        if (Date.Year < 1) return false;
        return true;
    }

    // =========================================================================
    //  Day-of-week  (Tomohiko Sakamoto / Zeller's congruence variant)
    //  Returns 0 = Sunday … 6 = Saturday
    // =========================================================================
    short DayOfWeekOrder(short Day, short Month, short Year)
    {
        short a = (14 - Month) / 12;
        short y = Year - a;
        short m = Month + (12 * a) - 2;
        return (Day + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12)) % 7;
    }

    short DayOfWeekOrder(stDate Date)
    {
        return DayOfWeekOrder(Date.Day, Date.Month, Date.Year);
    }

    // Alias used by MyHijriDateLib to obtain accurate Gregorian weekday
    short GetDayOfWeekOrder(short Day, short Month, short Year)
    {
        return DayOfWeekOrder(Day, Month, Year);
    }

    string GetDayShortName(short Day, short Month, short Year)
    {
        return DayNameShort[DayOfWeekOrder(Day, Month, Year)];
    }

    string GetDayShortName(stDate Date)
    {
        return DayNameShort[DayOfWeekOrder(Date)];
    }

    string GetDayLongName(short Day, short Month, short Year)
    {
        const string Long[7] = { "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday" };
        return Long[DayOfWeekOrder(Day, Month, Year)];
    }

    // =========================================================================
    //  Absolute-day count (days since Day 1 of Year 1)
    //  Used internally and for cross-calendar conversion
    // =========================================================================
    long GetAbsoluteDays(short Day, short Month, short Year)
    {
        long Total = 0;
        for (short y = 1; y < Year; y++) Total += NumberOfDaysInYear(y);
        for (short m = 1; m < Month; m++) Total += NumberOfDaysInMonth(m, Year);
        Total += Day;
        return Total;
    }

    long GetAbsoluteDays(stDate Date)
    {
        return GetAbsoluteDays(Date.Day, Date.Month, Date.Year);
    }

    // =========================================================================
    //  Date arithmetic
    // =========================================================================
    short TotalDaysFromYearBeginning(stDate Date)
    {
        short Count = 0;
        for (short m = 1; m < Date.Month; m++) Count += NumberOfDaysInMonth(m, Date.Year);
        return Count + Date.Day;
    }

    bool IsLastDayInMonth(stDate Date)
    {
        return Date.Day == NumberOfDaysInMonth(Date);
    }

    bool IsLastMonthInYear(short Month) { return Month == 12; }
    bool IsFirstDayInMonth(short Day) { return Day == 1; }
    bool IsFirstMonthInYear(short Month) { return Month == 1; }

    stDate IncreaseByOneDay(stDate Date)
    {
        if (IsLastMonthInYear(Date.Month) && IsLastDayInMonth(Date))
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

    stDate IncreaseByXDays(stDate Date, int Days)
    {
        for (int i = 0; i < Days; i++) Date = IncreaseByOneDay(Date);
        return Date;
    }

    stDate CorrectInvalidDay(stDate Date)
    {
        short Max = NumberOfDaysInMonth(Date.Month, Date.Year);
        if (Date.Day > Max) Date.Day = Max;
        return Date;
    }

    stDate IncreaseByOneMonth(stDate Date)
    {
        if (Date.Month == 12) { Date.Month = 1; Date.Year++; }
        else Date.Month++;
        return CorrectInvalidDay(Date);
    }

    stDate IncreaseByXMonths(stDate Date, int Months)
    {
        for (int i = 0; i < Months; i++) Date = IncreaseByOneMonth(Date);
        return Date;
    }

    stDate IncreaseByOneYear(stDate Date) { Date.Year++; return CorrectInvalidDay(Date); }
    stDate IncreaseByXYears(stDate Date, int Years)
    {
        for (int i = 0; i < Years; i++) Date = IncreaseByOneYear(Date);
        return Date;
    }

    stDate DecreaseByOneDay(stDate Date)
    {
        if (IsFirstMonthInYear(Date.Month) && IsFirstDayInMonth(Date.Day))
        {
            Date.Day = 31; Date.Month = 12; Date.Year--; return Date;
        }
        if (IsFirstDayInMonth(Date.Day))
        {
            Date.Month--; Date.Day = NumberOfDaysInMonth(Date.Month, Date.Year); return Date;
        }
        Date.Day--;
        return Date;
    }

    stDate DecreaseByXDays(stDate Date, int Days)
    {
        for (int i = 0; i < Days; i++) Date = DecreaseByOneDay(Date);
        return Date;
    }

    stDate DecreaseByOneMonth(stDate Date)
    {
        if (Date.Month == 1) { Date.Month = 12; Date.Year--; }
        else Date.Month--;
        return CorrectInvalidDay(Date);
    }

    stDate DecreaseByXMonths(stDate Date, int Months)
    {
        for (int i = 0; i < Months; i++) Date = DecreaseByOneMonth(Date);
        return Date;
    }

    stDate DecreaseByOneYear(stDate Date) { Date.Year--; return CorrectInvalidDay(Date); }
    stDate DecreaseByXYears(stDate Date, int Years)
    {
        for (int i = 0; i < Years; i++) Date = DecreaseByOneYear(Date);
        return Date;
    }

    // =========================================================================
    //  Difference
    // =========================================================================
    void SwapDates(stDate& D1, stDate& D2) { stDate t = D1; D1 = D2; D2 = t; }

    bool IsDate1BeforeDate2(stDate D1, stDate D2)
    {
        return (D1.Year < D2.Year) ? true :
            (D1.Year > D2.Year) ? false :
            (D1.Month < D2.Month) ? true :
            (D1.Month > D2.Month) ? false :
            (D1.Day < D2.Day);
    }

    bool IsDate1EqualDate2(stDate D1, stDate D2)
    {
        return D1.Year == D2.Year && D1.Month == D2.Month && D1.Day == D2.Day;
    }

    bool IsDate1AfterDate2(stDate D1, stDate D2)
    {
        return !IsDate1BeforeDate2(D1, D2) && !IsDate1EqualDate2(D1, D2);
    }

    enBeforAfter CompareDates(stDate D1, stDate D2)
    {
        if (IsDate1BeforeDate2(D1, D2)) return enBeforAfter::Before;
        return IsDate1EqualDate2(D1, D2) ? enBeforAfter::Equal : enBeforAfter::After;
    }

    long GetDifferenceInDays(stDate D1, stDate D2, bool IncludeEndDay = false)
    {
        if (IsDate1AfterDate2(D1, D2)) SwapDates(D1, D2);
        long Diff = GetAbsoluteDays(D2) - GetAbsoluteDays(D1);
        return IncludeEndDay ? Diff + 1 : Diff;
    }

    // =========================================================================
    //  Weekday helpers
    // =========================================================================
    bool IsWeekend(stDate Date)
    {
        short d = DayOfWeekOrder(Date);
        return (d == 5 || d == 6); // Fri & Sat
    }

    bool IsBusinessDay(stDate Date) { return !IsWeekend(Date); }

    short DaysUntilEndOfWeek(stDate Date) { return 6 - DayOfWeekOrder(Date); }

    short DaysUntilEndOfMonth(stDate Date, bool IncludeEndDay = false)
    {
        short Remaining = NumberOfDaysInMonth(Date) - Date.Day;
        return IncludeEndDay ? Remaining + 1 : Remaining;
    }

    short DaysUntilEndOfYear(stDate Date, bool IncludeEndDay = false)
    {
        short Remaining = NumberOfDaysInYear(Date.Year) - TotalDaysFromYearBeginning(Date);
        return IncludeEndDay ? Remaining + 1 : Remaining;
    }

    // =========================================================================
    //  Calendar rendering helpers  (return strings; UI layer decides how to print)
    // =========================================================================

    // Returns a formatted 7-column month grid as a multi-line string
    string BuildMonthCalendarString(short Month, short Year)
    {
        string Out = "";
        const string Sep = "   _______________________________________\n";

        Out += Sep;
        Out += "            [ " + GetMonthFullName(Month) + " " + to_string(Year) + " ]\n";
        for (short i = 0; i < 7; i++)
            Out += "   " + string(DayNameShort[i]).substr(0, 3) + " ";
        Out += "\n";

        short StartDay = DayOfWeekOrder(1, Month, Year);
        for (short i = 0; i < StartDay; i++) Out += "       ";

        short TotalDays = NumberOfDaysInMonth(Month, Year);
        short Col = StartDay;
        for (short d = 1; d <= TotalDays; d++)
        {
            // Each column is 7 chars wide to match the weekday header above.
            // For single-digit days use two padding spaces, for two-digit use one.
            Out += "   " + string(d < 10 ? "  " : " ") + to_string(d) + " ";
            Col++;
            if (Col == 7) { Out += "\n"; Col = 0; }
        }
        Out += "\n" + Sep;
        return Out;
    }

    string BuildYearCalendarString(short Year)
    {
        string Out = "\n   ====[ Calendar " + to_string(Year) + " ]====\n";
        for (short m = 1; m <= 12; m++)
            Out += BuildMonthCalendarString(m, Year);
        return Out;
    }

    // =========================================================================
    //  Formatting
    // =========================================================================
    string DateToString(stDate Date, enDateFormat Format = enDateFormat::DayMonthYearSlash)
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
            return GetDayLongName(Date.Day, Date.Month, Date.Year)
                + ", " + D + "/" + GetMonthFullName(Date.Month) + "/" + Y;
        case YearDayMonthSlash:  return Y + "/" + D + "/" + M;
        case MonthDayYearDash:   return M + "-" + D + "-" + Y;
        case DayMonthYearDash:   return D + "-" + M + "-" + Y;
        case NamedCustom:
            return "Day: " + D + ", Month: " + M + ", Year: " + Y;
        default:
            return D + "/" + M + "/" + Y;
        }
    }

    // =========================================================================
    //  System date (today)
    // =========================================================================
    stDate GetSystemDate()
    {
        time_t Now = time(nullptr);
        tm* T = localtime(&Now);
        return { (short)(T->tm_mday), (short)(T->tm_mon + 1), (short)(T->tm_year + 1900) };
    }

    // =========================================================================
    //  Period utilities
    // =========================================================================
    bool IsOverlapPeriod(stPeriod P1, stPeriod P2, bool IncludeLastDay = true)
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

    int CalculatePeriodLength(stPeriod Period, bool IncludeEndDay = false)
    {
        return (int)GetDifferenceInDays(Period.StartDate, Period.EndDate, IncludeEndDay);
    }

    bool IsDateInPeriod(stPeriod Period, stDate Date)
    {
        return (CompareDates(Period.StartDate, Date) != enBeforAfter::After)
            && (CompareDates(Period.EndDate, Date) != enBeforAfter::Before);
    }

    int CountOverlapDays(stPeriod P1, stPeriod P2)
    {
        if (!IsOverlapPeriod(P1, P2)) return 0;
        stPeriod Shared;
        Shared.StartDate = (CompareDates(P1.StartDate, P2.StartDate) == enBeforAfter::After)
            ? P1.StartDate : P2.StartDate;
        Shared.EndDate = (CompareDates(P1.EndDate, P2.EndDate) == enBeforAfter::Before)
            ? P1.EndDate : P2.EndDate;
        return CalculatePeriodLength(Shared, true);
    }

    // Vacation helpers
    short CalculateActualVacationDays(stDate From, stDate To, bool IncludeLastDay = false)
    {
        long All = GetDifferenceInDays(From, To, IncludeLastDay);
        short Count = 0;
        for (long i = 0; i < All; i++)
        {
            if (IsBusinessDay(From)) Count++;
            From = IncreaseByOneDay(From);
        }
        return Count;
    }

    stDate CalculateVacationReturnDate(stDate From, short VacationDays)
    {
        stDate ReturnDate = From;
        while (VacationDays > 0)
        {
            if (IsBusinessDay(ReturnDate)) VacationDays--;
            ReturnDate = IncreaseByOneDay(ReturnDate);
        }
        while (IsWeekend(ReturnDate)) ReturnDate = IncreaseByOneDay(ReturnDate);
        return ReturnDate;
    }

} 