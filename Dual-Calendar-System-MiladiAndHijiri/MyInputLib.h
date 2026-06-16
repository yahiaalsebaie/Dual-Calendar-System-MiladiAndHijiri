
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <limits>

using namespace std;

namespace MyInputLib
{
    // -----------------------------------------------------------------------
    //  Integer - with full cin-fail recovery
    // -----------------------------------------------------------------------
    int ReadNumber(string Message = "Please enter a number: ")
    {
        int Number = 0;
        cout << Message;
        cin >> Number;
        while (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid Number, Enter a valid one: ";
            cin >> Number;
        }
        return Number;
    }

    // -----------------------------------------------------------------------
    //  Float
    // -----------------------------------------------------------------------
    float ReadFloatNumber(string Message)
    {
        float Number = 0;
        cout << Message;
        cin >> Number;
        return Number;
    }

    // -----------------------------------------------------------------------
    //  Positive integer (> 0)
    // -----------------------------------------------------------------------
    int ReadPositiveNumber(string Message)
    {
        int Number = 0;
        do
        {
            cout << Message;
            cin >> Number;
            while (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Number, Enter a valid one: ";
                cin >> Number;
            }
        } while (Number <= 0);
        return Number;
    }

    // -----------------------------------------------------------------------
    //  Unsigned long long - rejects negative sign
    // -----------------------------------------------------------------------
    unsigned long long ReadUnsignedLongPositiveNumber(
        bool   isIncludeZero = false,
        string Message = "Please enter a number: ")
    {
        unsigned long long Number = 0;
        do
        {
            cout << Message;
            cin >> ws;
            if (cin.peek() == '-')
                cin.setstate(ios::failbit);
            else
                cin >> Number;

            while (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Number, Enter a valid one: ";
                cin >> ws;
                if (cin.peek() == '-')
                    cin.setstate(ios::failbit);
                else
                    cin >> Number;
            }
        } while (isIncludeZero ? (Number < 0) : (Number <= 0));
        return Number;
    }

    // -----------------------------------------------------------------------
    //  Long long positive
    // -----------------------------------------------------------------------
    long long ReadLongPositiveNumber(
        bool   isIncludeZero = false,
        string Message = "Please enter a number: ")
    {
        long long Number = 0;
        do
        {
            cout << Message;
            cin >> Number;
            while (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Number, Enter a valid one: ";
                cin >> Number;
            }
        } while (isIncludeZero ? (Number < 0) : (Number <= 0));
        return Number;
    }

    // -----------------------------------------------------------------------
    //  Integer within [From, To] inclusive
    // -----------------------------------------------------------------------
    int ReadNumberInRange(
        int    From,
        int    To,
        string Message = "Enter number: ",
        bool   isIncludedRange = true)
    {
        int Number = 0;
        do
        {
            if (isIncludedRange)
                cout << Message << " [" << From << " to " << To << "]: ";
            else
                cout << Message;

            cin >> Number;
            while (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Number, Enter a valid one: ";
                cin >> Number;
            }
        } while (Number < From || Number > To);
        return Number;
    }

    // -----------------------------------------------------------------------
    //  Full-line string
    // -----------------------------------------------------------------------
    string ReadText(string Message)
    {
        string Text = "";
        cout << Message;
        getline(cin >> ws, Text);
        return Text;
    }

    // -----------------------------------------------------------------------
    //  Fill a vector of integers interactively
    // -----------------------------------------------------------------------
    void ReadVectorNumbers(vector<int>& vNumbers)
    {
        char AddMore = 'y';
        do
        {
            int Number;
            cout << "Enter a Number: ";
            cin >> Number;
            while (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Number, Enter a valid one: ";
                cin >> Number;
            }
            vNumbers.push_back(Number);
            cout << "Add more Numbers? (y/n): ";
            cin >> AddMore;
        } while (AddMore == 'y' || AddMore == 'Y');
    }
}