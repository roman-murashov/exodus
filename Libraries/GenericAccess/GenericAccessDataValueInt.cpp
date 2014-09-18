#include "GenericAccessDataValueInt.h"
#include "DataConversion/DataConversion.pkg"
#include <limits>
#include <stdio.h>

//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
GenericAccessDataValueInt::GenericAccessDataValueInt(int value)
:dataValue(value)
{
	displayMode = INTDISPLAYMODE_DECIMAL;
	minChars = 0;
	minValue = std::numeric_limits<int>::min();
	maxValue = std::numeric_limits<int>::max();
}

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueInt::GetIGenericAccessDataValueIntVersion() const
{
	return ThisIGenericAccessDataValueIntVersion();
}

//----------------------------------------------------------------------------------------
//Type functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueInt::DataType GenericAccessDataValueInt::GetType() const
{
	return DATATYPE_INT;
}

//----------------------------------------------------------------------------------------
//Value read functions
//----------------------------------------------------------------------------------------
int GenericAccessDataValueInt::GetValue() const
{
	return dataValue;
}

//----------------------------------------------------------------------------------------
std::wstring GenericAccessDataValueInt::GetValueString() const
{
	std::wstring result;
	switch(displayMode)
	{
	case INTDISPLAYMODE_BINARY:
		IntToStringBase2(dataValue, result, minChars);
		break;
	case INTDISPLAYMODE_OCTAL:
		IntToStringBase8(dataValue, result, minChars);
		break;
	case INTDISPLAYMODE_DECIMAL:
		IntToStringBase10(dataValue, result, minChars);
		break;
	case INTDISPLAYMODE_HEXADECIMAL:
		IntToStringBase16(dataValue, result, minChars);
		break;
	}
	return result;
}

//----------------------------------------------------------------------------------------
//Value write functions
//----------------------------------------------------------------------------------------
bool GenericAccessDataValueInt::SetValueInt(int value)
{
	dataValue = value;
	ApplyLimitSettingsToCurrentValue();
	return true;
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueInt::SetValueUInt(unsigned int value)
{
	return SetValueInt((int)value);
}

//----------------------------------------------------------------------------------------
bool GenericAccessDataValueInt::SetValueString(const std::wstring& value)
{
	//Calculate the default base to use for the specified number based on the specified
	//display mode
	unsigned int defaultBase = 10;
	switch(displayMode)
	{
	case INTDISPLAYMODE_BINARY:
		defaultBase = 2;
		break;
	case INTDISPLAYMODE_OCTAL:
		defaultBase = 8;
		break;
	case INTDISPLAYMODE_DECIMAL:
		defaultBase = 10;
		break;
	case INTDISPLAYMODE_HEXADECIMAL:
		defaultBase = 16;
		break;
	}

	//Attempt to convert the string to an integer
	int valueConverted;
	if(!StringToInt(value, valueConverted, defaultBase))
	{
		return false;
	}

	//Attempt to set this value to the specified integer
	return SetValueInt(valueConverted);
}

//----------------------------------------------------------------------------------------
//Value display functions
//----------------------------------------------------------------------------------------
GenericAccessDataValueInt::IntDisplayMode GenericAccessDataValueInt::GetDisplayMode() const
{
	return displayMode;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::SetDisplayMode(IntDisplayMode state)
{
	displayMode = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueInt::GetMinChars() const
{
	return minChars;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::SetMinChars(unsigned int state)
{
	minChars = state;
}

//----------------------------------------------------------------------------------------
unsigned int GenericAccessDataValueInt::CalculateDisplayChars(IntDisplayMode adisplayMode, int aminValue, int amaxValue) const
{
	//Build strings for the max and min values
	std::wstring minValueString;
	std::wstring maxValueString;
	switch(adisplayMode)
	{
	case INTDISPLAYMODE_BINARY:
		IntToStringBase2(abs(aminValue), minValueString, 0, false);
		IntToStringBase2(abs(amaxValue), maxValueString, 0, false);
		break;
	case INTDISPLAYMODE_OCTAL:
		IntToStringBase8(abs(aminValue), minValueString, 0, false);
		IntToStringBase8(abs(amaxValue), maxValueString, 0, false);
		break;
	case INTDISPLAYMODE_DECIMAL:
		IntToStringBase10(abs(aminValue), minValueString, 0, false);
		IntToStringBase10(abs(amaxValue), maxValueString, 0, false);
		break;
	case INTDISPLAYMODE_HEXADECIMAL:
		IntToStringBase16(abs(aminValue), minValueString, 0, false);
		IntToStringBase16(abs(amaxValue), maxValueString, 0, false);
		break;
	}

	//Calculate the minimum number of chars to be able to hold any value within the
	//maximum and minimum range
	unsigned int displayChars = (minValueString.length() > maxValueString.length())? (unsigned int)minValueString.length(): (unsigned int)maxValueString.length();
	return displayChars;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
int GenericAccessDataValueInt::GetMinValue() const
{
	return minValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::SetMinValue(int state)
{
	minValue = state;
}

//----------------------------------------------------------------------------------------
int GenericAccessDataValueInt::GetMaxValue() const
{
	return maxValue;
}

//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::SetMaxValue(int state)
{
	maxValue = state;
}

//----------------------------------------------------------------------------------------
//Value limit functions
//----------------------------------------------------------------------------------------
void GenericAccessDataValueInt::ApplyLimitSettingsToCurrentValue()
{
	dataValue = (dataValue < minValue)? minValue: dataValue;
	dataValue = (dataValue > maxValue)? maxValue: dataValue;
}
