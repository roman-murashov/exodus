//----------------------------------------------------------------------------------------
//Constructors
//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel)
:eventLevel(alevel)
{
	GetLocalTime(&time);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel, const std::wstring& atext)
:eventLevel(alevel)
{
	GetLocalTime(&time);
	SetText(atext);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(EventLevel alevel, const std::wstring& asource, const std::wstring& atext)
:eventLevel(alevel), source(asource)
{
	GetLocalTime(&time);
	SetText(atext);
}

//----------------------------------------------------------------------------------------
LogEntry::LogEntry(const LogEntry& object)
:eventLevel(object.eventLevel), time(object.time), source(object.source)
{
	SetText(object.GetText());
}

//----------------------------------------------------------------------------------------
//Getters
//----------------------------------------------------------------------------------------
std::wstring LogEntry::GetText() const
{
	return text.str();
}

//----------------------------------------------------------------------------------------
std::wstring LogEntry::GetSource() const
{
	return source;
}

//----------------------------------------------------------------------------------------
std::wstring LogEntry::GetEventLevelString() const
{
	switch(eventLevel)
	{
	case EVENTLEVEL_INFO:
		return L"1 - Information";
	case EVENTLEVEL_DEBUG:
		return L"2 - Debug";
	case EVENTLEVEL_WARNING:
		return L"3 - Warning";
	case EVENTLEVEL_ERROR:
		return L"4 - Error";
	case EVENTLEVEL_CRITICAL:
		return L"5 - Critical";
	default:
		return L"";
	}
}

//----------------------------------------------------------------------------------------
std::wstring LogEntry::GetTimeString() const
{
	std::wstringstream stream;
	stream << std::setw(2) << std::setfill(L'0') << time.wHour << L':' << time.wMinute << L':' << time.wSecond << L'.' << time.wMilliseconds;
	return stream.str();
}

//----------------------------------------------------------------------------------------
//Setters
//----------------------------------------------------------------------------------------
void LogEntry::SetText(const std::wstring& atext)
{
	text.str() = L"";
	text << atext;
}

//----------------------------------------------------------------------------------------
void LogEntry::SetSource(const std::wstring& asource)
{
	source = asource;
}

//----------------------------------------------------------------------------------------
void LogEntry::SetEventLevel(EventLevel alevel)
{
	eventLevel = alevel;
}

//----------------------------------------------------------------------------------------
//Text-based stream functions
//----------------------------------------------------------------------------------------
template<class T> LogEntry& LogEntry::operator>>(T& data)
{
	text << data;
	return *this;
}

//----------------------------------------------------------------------------------------
template<class T> LogEntry& LogEntry::operator<<(const T& data)
{
	text << data;
	return *this;
}
