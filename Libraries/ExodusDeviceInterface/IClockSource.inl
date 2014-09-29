//----------------------------------------------------------------------------------------
//Enumerations
//----------------------------------------------------------------------------------------
enum class IClockSource::ClockType
{
	Direct,
	Divider,
	Multiplier
};

//----------------------------------------------------------------------------------------
//Interface version functions
//----------------------------------------------------------------------------------------
unsigned int IClockSource::ThisIClockSourceVersion()
{
	return 1;
}
