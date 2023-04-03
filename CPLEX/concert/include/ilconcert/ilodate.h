// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilodate.h
// --------------------------------------------------------------------------
// Licensed Materials - Property of IBM
//
// 5725-A06 5725-A29 5724-Y47 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
// Copyright IBM Corp. 2000, 2022
//
// US Government Users Restricted Rights - Use, duplication or
// disclosure restricted by GSA ADP Schedule Contract with
// IBM Corp.
// ---------------------------------------------------------------------------

#ifndef __CONCERT_ilodateH
#define __CONCERT_ilodateH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>

# if defined(IL_DLL)
#   if defined(IL_CE_LIB)
#     define ILO_CE_EXPORT __declspec(dllexport)
#   else
#     define ILO_CE_EXPORT __declspec(dllimport)
#   endif
# else
#   define ILO_CE_EXPORT
# endif

typedef enum {
	ILFDATE_ROUND_DOWN,
	ILFDATE_ROUND_UP,
	ILFDATE_ROUND_NEAREST
} IloDateRoundingMode;

class ILO_CE_EXPORT IloTimeValue {
	IloInt _secs;

	friend class IloLinearTimeConverter;
public:
	virtual ~IloTimeValue() {}
	IloInt getSeconds() const;
	IloInt getMinutes() const;
	IloInt getHours() const;
	IloInt getDays() const;

	IloBool equals              (const IloTimeValue& other) const;
	IloBool isLessThan          (const IloTimeValue& other) const;
	IloBool isGreaterThan       (const IloTimeValue& other) const;
	IloBool isLessThanOrEqual   (const IloTimeValue& other) const;
	IloBool isGreaterThanOrEqual(const IloTimeValue& other) const;

	IloBool isPositive() const { return _secs >= 0;}

	static IloInt CompareTimes(const IloTimeValue& v1, const IloTimeValue& v2);

	virtual void display(ILOSTD(ostream)& ) const = 0;

protected:
	void assign(IloInt secs);

	IloInt add(const IloTimeValue& other) const;
	IloInt subtract(const IloTimeValue& other) const;
	IloInt multiply(IloInt scale) const;
	IloNum divide(const IloTimeValue& other) const;
	IloNum divide(IloNum factor) const;
	IloInt abs() const;

	IloTimeValue(IloInt secs = 0);
	IloTimeValue(IloInt days, IloInt hours, IloInt minutes, IloInt seconds);

	static void PrintDays(ILOSTD(ostream)&, IloInt days, const char* format);
	static void PrintFormatted(ILOSTD(ostream)&, IloInt nbValues, IloInt* values, const char** seps);

}; 

class ILO_CE_EXPORT IloDuration : public IloTimeValue {
	friend class IloDate;

	IloDuration();

public:
	IloDuration(IloInt days, IloInt hours, IloInt minutes, IloInt seconds=0);

	void display(ILOSTD(ostream)&) const ILO_OVERRIDE;

	static IloDuration Create(IloInt secs);
	static IloDuration Subtract(const IloTimeValue& v1, const IloTimeValue& v2);

	IloDuration operator-(const IloDuration& other) const;
	IloDuration operator+(const IloDuration& other) const;
	IloNum      operator/(const IloDuration& other) const;
	IloDuration multiply(IloInt scale) const;
	IloDuration divide(IloInt scale,
		IloDateRoundingMode rmode= ILFDATE_ROUND_NEAREST) const;
	IloNum divide(const IloTimeValue& other) const;
	IloNum divide(IloNum factor) const;

	IloDuration abs() const;

	static IloDuration ZERO_DURATION;
	static IloDuration ONE_DAY;
	static IloDuration HALF_DAY;
	static IloDuration ONE_HOUR;
	static IloDuration HALF_HOUR;
	static IloDuration QUARTER_HOUR;
	static IloDuration ONE_MINUTE;
	static IloDuration ONE_SECOND;
}; 

ILOSTD(ostream)& operator<< (ILOSTD(ostream)&, const IloDuration&);

class ILO_CE_EXPORT IloDate : public IloTimeValue {
private:
	static const IloInt DAYS_IN_MONTH[ 2 ][ 12 ];
	
	IloDate();
	static IloInt RefYear;
	static IloInt MaxYear;

	void getYearInfo(IloInt& year, IloInt& dayInYear, IloBool& isLeap) const;

public:
	IloDate(IloInt year, IloInt month, IloInt dayInMonth,
		IloInt hour=0, IloInt minute=0, IloInt secs=0
		);
	IloInt getYear() const;
	IloInt getDayOfYear() const;
	IloInt getMonth() const;
	IloInt getDayOfMonth() const;
	IloInt getDayOfWeek() const;
	IloInt getNbOfDaysInYear(IloInt year) const;

	static IloDate Today(IloInt hh=0, IloInt mm =0, IloInt ss =0);
	static IloDate TodayZeroAM();
	static IloDate Now();
	static IloDate Create(IloInt secs);
	static IloDate Create(IloDuration duration);
	static IloDate ZeroDate;
	inline IloBool isLeap(const IloInt year) const {
		return (0 == (year) % 4 && ((year) % 100 != 0 || (year) % 400 == 0));
	}

	IloDate addDuration     (const IloDuration& duration) const;
	IloDate subtractDuration(const IloDuration& duration) const;

	IloDuration operator-(const IloDate& other) const;
	IloDate operator+(const IloDuration& duration) const;

	void toString(char* buffer) const;
	void display(ILOSTD(ostream)& ) const ILO_OVERRIDE;
	void displayAsDuration(ILOSTD(ostream)& ) const;
}; 

ILOSTD(ostream)& operator<< (ILOSTD(ostream)&, const IloDate&);

inline IloBool operator==(const IloDate& d1, const IloDate& d2) {
	return d1.equals(d2); }
inline IloBool operator<(const IloDate& d1, const IloDate& d2) {
	return d1.isLessThan(d2); }
inline IloBool operator<=(const IloDate& d1, const IloDate& d2) {
	return d1.isLessThanOrEqual(d2); }
inline IloBool operator>(const IloDate& d1, const IloDate& d2) {
	return d1.isGreaterThan(d2); }
inline IloBool operator>=(const IloDate& d1, const IloDate& d2) {
	return d1.isGreaterThanOrEqual(d2); }

inline IloBool operator==(const IloDuration& d1, const IloDuration& d2) {
	return d1.equals(d2); }

IloDuration operator*(IloInt scale, const IloDuration& dd);
IloDuration operator/(const IloDuration& dd, IloInt factor);

class IloLinearTimeConverter {
	IloDate _origin;
	IloDuration _unit;

	IloInt getRoundedTimeVal(IloNum, IloDateRoundingMode) const;

public:
	IloLinearTimeConverter();
	IloLinearTimeConverter(IloInt secs);
	IloLinearTimeConverter(IloDuration unit);
	IloLinearTimeConverter(IloDate origin, IloDuration unit);

	void setOrigin(IloDate origin, IloDuration unit);

	IloNum convert(IloDate) const;
	IloNum convert(IloDuration) const;

	IloDate convertToDate(IloNum, IloDateRoundingMode = ILFDATE_ROUND_NEAREST) const;
	IloDuration convertToDuration(IloNum, IloDateRoundingMode = ILFDATE_ROUND_NEAREST) const;

	
	IloNum getPrecision() const;

}; 

class IloDurationFormatter {
	IloNum _durationValue;
	const IloLinearTimeConverter& _converter;
public:
	virtual ~IloDurationFormatter(){}
	IloDurationFormatter(IloNum duration, const IloLinearTimeConverter& converter)
		: _durationValue(duration), _converter(converter) {}
	virtual void display(ILOSTD(ostream)& os) const {
		os << _converter.convertToDuration(_durationValue);
	}
}; 

class IloDateFormatter {
	IloNum _dateValue;
	const IloLinearTimeConverter& _converter;
public:
	IloDateFormatter(IloNum date, const IloLinearTimeConverter& converter)
		: _dateValue(date), _converter(converter) {}
	virtual ~IloDateFormatter(){}
	virtual void display(ILOSTD(ostream)& os) const {
		os << _converter.convertToDate(_dateValue);
	}
}; 

ILOSTD(ostream)& operator<<(ILOSTD(ostream)& , const IloDurationFormatter& );
ILOSTD(ostream)& operator<<(ILOSTD(ostream)& , const IloDateFormatter& );

#define IloZeroDuration   (IloDuration::ZERO_DURATION)
#define IloHalfDayDuration IloDuration::HALF_DAY
#define IloOneDayDuration IloDuration::ONE_DAY
#define IloOneHourDuration (IloDuration::ONE_HOUR)
#define IloOneMinuteDuration IloDuration::ONE_MINUTE
#define IloOneSecondDuration IloDuration::ONE_SECOND

#define ILO_SECS_IN_MINUTE    60
#define ILO_SECS_IN_HOUR    3600
#define ILO_SECS_IN_DAY    86400
#define ILO_HOURS_IN_DAY      24
#define ILO_DAYS_IN_YEAR     365

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
