// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilochecker.h
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

#ifndef __CONCERT_ilocheckerH
#define __CONCERT_ilocheckerH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloenv.h>

enum IloPrecisionType {IloAbsolutePrecision    = 0, 
IloRelativePrecision    = 1, 
IloDefaultPrecisionType = 2};

void ilo_check_error_stop_here();

class IloCheckerI {
private:
	IloEnv           _env;
	IloNum           _precision;
	IloPrecisionType _precisionType;
	IloInt           _nbChecks;
	IloInt           _nbWarnings;
	IloInt           _nbErrors;
	unsigned int     _seed;
	unsigned int     _maxrand;
	IloInt           _tries;
	IloBool          _desactivated;

	void errorPrecision(IloNum precision,
		IloPrecisionType precisionType);
	void checkPrecisionIsValid() const;
	void checkPrecisionTypeIsValid() const;

	unsigned int rawRandom();
	void initRandom();

public:
	IloInt    getInt(IloInt max) { IloInt res; generateRandom(res, 0, static_cast<IloNum>(max)); return res; }
	IloNum    getFloat() { IloNum num=0; generateRandom(num); return num; }

	IloCheckerI(IloEnv env, 
		IloNum precision, 
		IloPrecisionType precisionType);
	IloCheckerI(IloEnv env); 

	void operator delete(void* ptr, size_t size);
#ifdef ILODELETEOPERATOR
	void operator delete(void* obj, const IloEnvI*) {}
	void operator delete(void* obj, const IloEnv&) {}
#endif
	IloBool isActive() const { return !_desactivated;}
	void setNotActive() { _desactivated = IloTrue;}
	IloBool checkTrue(const char* str,
		IloBool truth);
	IloBool checkTrue(const char* str,
		const char* loc, 
		IloBool truth);

	IloBool checkFalse(const char* str,
		IloBool truth);

	IloBool checkEqual(const char* str, 
		const char* val1, 
		const char* val2);

	IloBool checkEqual(const char* str, 
		IloAny val1, 
		IloAny val2);

	IloBool checkEqual(const char* str, 
		IloNum val1, 
		IloNum val2,
		IloNum precision,
		IloPrecisionType precisionType);

	IloBool checkDiff(const char* str, 
		const char* val1, 
		const char* val2);

	IloBool checkDiff(const char* str, 
		IloAny val1, 
		IloAny val2);

	IloBool checkDiff(const char* str, 
		IloNum val1, 
		IloNum val2);

	IloBool checkGreater(const char* str,
		IloNum val1, 
		IloNum val2);

	IloBool checkGreaterOrEqual(const char* str,
		IloNum val1, 
		IloNum val2);

	IloBool checkInside(const char* str,
		IloNum val, 
		IloNum lb,
		IloNum ub);

	IloBool checkLess(const char* str,
		IloNum val1, 
		IloNum val2);

	IloBool checkLessOrEqual(const char* str,
		IloNum val1, 
		IloNum val2);
	void display();

	IloEnv getEnv() const { return _env; }
	IloNum getPrecision() const { return _precision; }
	IloPrecisionType getPrecisionType() const { return _precisionType; }
	IloInt getNumberOfChecks() const {  return _nbChecks; }
	IloInt getNumberOfWarnings() const {  return _nbWarnings; }
	IloInt getNumberOfErrors() const { return _nbErrors; }

	ILOSTD(ostream)& out() const { return getEnv().out(); }
	ILOSTD(ostream)& warning() const { return getEnv().warning(); }
	ILOSTD(ostream)& error() const { return getEnv().error(); }

	void setPrecision(IloNum precision) {
		_precision = precision;
		checkPrecisionIsValid();
	}
	void setPrecisionType(IloPrecisionType precisionType) {
		_precisionType = precisionType;
		checkPrecisionTypeIsValid();
	}

	void setRandomSeed(IloInt s);
	void generateRandom(IloInt &v, IloNum min=0, IloNum max=(IloNum)IloIntMax);
	void generateRandom(IloNum &v, IloNum min=0.0, IloNum max=1.0);
	void startLoop(IloInt n) { _tries=n; }
	IloBool next() { return (--_tries<0); }
};

#define IloDefaultPrecision ((IloNum)-1)

class IloChecker {
protected:
	IloCheckerI* _impl;
public:
	IloChecker(const IloEnv env,
		IloNum precision = 0.0, 
		IloPrecisionType precisionType = IloRelativePrecision,
		IloBool active = IloTrue);
	IloBool isActive() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->isActive();
	}
	void setNotActive() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->setNotActive();
	}
	IloChecker() : _impl(0) { }
	IloChecker(const IloChecker& c) : _impl(c._impl){}
	void end();
	IloCheckerI* getImpl() const { return _impl; }

	void operator=(const IloChecker& c) { _impl = c._impl; }
	void display() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		getImpl()->display();
	}
	IloEnv getEnv() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->getEnv();
	}
	IloBool checkTrue(const char* str,
		IloBool val) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkTrue(str, val);
	}
	IloBool checkTrue(const char* str,
		const char* loc, 
		IloBool val) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkTrue(str, loc, val);
	}
	IloBool checkFalse(const char* str,
		IloBool val) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkFalse(str, val);
	}

	IloBool checkEqual(const char* strInfo, 
		const char* str1, 
		const char* str2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkEqual(strInfo, str1, str2);
	}

	IloBool checkEqual(const char* str, 
		IloAny val1, 
		IloAny val2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkEqual(str, val1, val2);
	}

	IloBool checkEqual(const char* str, 
		IloNum val1, 
		IloNum val2, IloNum precision = IloDefaultPrecision,
		IloPrecisionType precisionType = IloDefaultPrecisionType) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkEqual(str, val1, val2, precision, precisionType);
	}

	IloBool checkGreater(const char* str,
		IloNum val1, 
		IloNum val2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkGreater(str, val1, val2);
	}

	IloBool checkGreaterOrEqual(const char* str,
		IloNum val1, 
		IloNum val2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkGreaterOrEqual(str, val1, val2);
	}

	IloBool checkLess(const char* str,
		IloNum val1, 
		IloNum val2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkLess(str, val1, val2);
	}

	IloBool checkInside(const char* str,
		IloNum val, 
		IloNum lb,
		IloNum ub) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkInside(str, val, lb, ub);
	}

	IloBool checkLessOrEqual(const char* str,
		IloNum val1, 
		IloNum val2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkLessOrEqual(str, val1, val2);
	}

	IloBool checkDiff(const char* str, 
		const char* val1, 
		const char* val2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkDiff(str, val1, val2);
	}

	IloBool checkDiff(const char* str, 
		IloAny val1, 
		IloAny val2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkDiff(str, val1, val2);
	}

	IloBool checkDiff(const char* str, 
		IloNum val1, 
		IloNum val2) const {
			IloAssert(getImpl() != 0, "accessing NULL handle");
			return getImpl()->checkDiff(str, val1, val2);
	}

	ILOSTD(ostream)& out() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getEnv().out();
	}

	ILOSTD(ostream)& warning() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getEnv().warning();
	}

	ILOSTD(ostream)& error() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getEnv().error();
	}

	IloNum getPrecision() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->getPrecision();
	}
	IloPrecisionType getPrecisionType() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->getPrecisionType();
	}

	void getPrecision(IloNum val) {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		getImpl()->setPrecision(val);
	}
	void setPrecisionType(IloPrecisionType val) {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		getImpl()->setPrecisionType(val);
	}

	IloInt getNumberOfChecks() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->getNumberOfChecks();
	}
	IloInt getNumberOfWarnings() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->getNumberOfWarnings();
	}
	IloInt getNumberOfErrors() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->getNumberOfErrors();
	}
	void startLoop(IloInt n) const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		getImpl()->startLoop(n);
	}
	IloBool next() const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		return getImpl()->next();
	}
	void setRandomSeed(IloInt s) const { 
		IloAssert(getImpl() != 0, "accessing NULL handle");
		getImpl()->setRandomSeed(s);
	}
	void generateRandom(IloInt& v, IloNum min, IloNum max) const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		IloAssert(min < max, "Invalid bounds in IloChecker::generateRandom.");
		getImpl()->generateRandom(v,min,max);
	}
	void generateRandom(IloNum& v, IloNum min, IloNum max) const {
		IloAssert(getImpl() != 0, "accessing NULL handle");
		IloAssert(min < max, "Invalid bounds in IloChecker::generateRandom.");
		getImpl()->generateRandom(v,min,max);
	}
	IloInt getRandomInt(IloInt max) const {
		assert(_impl!=0 );
		return _impl->getInt(max);
	}
	IloNum getRandomFloat() const {
		assert(_impl!=0 );
		return _impl->getFloat();
	}
};

char* locationInfo(int lineNumber, const char * fileName);
#define ILO_LOCATION locationInfo(__LINE__, __FILE__)

#define IloTryRand(chk,var,lo,hi,num) \
	for(chk.startLoop(num);(chk).generateRandom((var),(lo),(hi)),!chk.next();)

#define IloTryCheck(chk, str, code) \
	try {\
	code;\
}\
	catch (IloException& e) {\
	chk.getEnv().out() << "Error " << str << "With exception " << e << endl;\
}

#define IloCheckIloExceptionIsThrown(chk,label,call)   \
	do { IloBool ok=IloFalse; try { call; } catch(IloException&) { ok=IloTrue; } chk.checkTrue(label, ok); } while(0)

#define IloCheckIloExceptionIsNotThrown(chk,label,call)   \
	do { IloBool ok=IloFalse; try { call; } catch(IloException&) { ok=IloTrue; } chk.checkFalse(label, ok); } while(0)

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
