#pragma once
using namespace std;
//  MyGlobalTypes.h  -  Unified Global Type Definitions
//  Console Calendar Application (Gregorian + Hijri)

// ---------------------------------------------------------------------------
//  Comparison result shared by both calendar libraries
// ---------------------------------------------------------------------------
enum enBeforAfter
{
    Before = -1,
    Equal = 0,
    After = 1
};

// ---------------------------------------------------------------------------
//  Date format options - used by both DateToString() overloads in both libs
// ---------------------------------------------------------------------------
enum enDateFormat
{
    DayMonthYearSlash = 1,   // 24/6/2026
    MonthDayYearSlash = 2,   //  6/24/2026
    YearMonthDaySlash = 3,   // 2026/6/24
    FullDateText = 4,   // Sun, 24/June/2026
    YearDayMonthSlash = 5,   // 2026/24/6
    MonthDayYearDash = 6,   //  6-24-2026
    DayMonthYearDash = 7,   // 24-6-2026
    NamedCustom = 8    // Day: 24, Month: 6, Year: 2026
};

// ---------------------------------------------------------------------------
//  Gregorian (Miladi) date
// ---------------------------------------------------------------------------
struct stDate
{
    short Day;
    short Month;
    short Year;
};

// ---------------------------------------------------------------------------
//  Hijri (Islamic) date
// ---------------------------------------------------------------------------
struct stHijriDate
{
    short Day;
    short Month;
    short Year;
};

// ---------------------------------------------------------------------------
//  Date ranges - one for each calendar
//  Keeping them separate avoids accidental Gregorian/Hijri mixing.
// ---------------------------------------------------------------------------
struct stPeriod        // Gregorian period
{
    stDate StartDate;
    stDate EndDate;
};

struct stHijriPeriod   // Hijri period
{
    stHijriDate StartDate;
    stHijriDate EndDate;
};