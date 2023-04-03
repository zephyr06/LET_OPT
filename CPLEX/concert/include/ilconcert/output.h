// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/output.h
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

#ifndef __CONCERT_outputH
#define __CONCERT_outputH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloenv.h>

#if defined(ILOUSESTL)

#include <string>
#if defined(ILO_HP) && !defined(IL_STD)
#define ILO_STDSTRING string
#else
#define ILO_STDSTRING std::string
#endif

typedef int IloDebugLevel;

class IloOutput;

class IloOutputCollector {
	friend class IloOutput;
public:
	typedef enum {
		ILO_OUT_DEBUG,
		ILO_OUT_INFO,
		ILO_OUT_WARNING,
		ILO_OUT_ERROR,
		ILO_OUT_FATAL
	} IloMessageType;

private:
	IloOutput*     _out;
	IloMessageType _type;
	IloInt         _lineno;
	ILO_STDSTRING  _filename;
	IloDebugLevel  _debugLevel;

	
	ILO_STDSTRING  _currentMessage;

	const char* getFilename() const { return _filename.c_str();}
	IloInt getLineNumber()    const { return _lineno;}
	IloBool isActive() const;
protected:
	IloOutputCollector();

public:
	virtual ~IloOutputCollector();

	void initialize(IloOutput* out,
		IloInt lineno, const char* filename,
		IloMessageType type,
		IloDebugLevel debugLevel
		);

	void append(const char* txt);
	void append(ILO_STDSTRING s);
	void append(unsigned long ul);
	void append(IloInt i);
	void append(IloNum x);
	void append(char c);

	
	template <class T>
	void append(const T& obj) {
		if ( !isActive() ) return;  
		ILOSTD_OSTREAM oss;
#ifdef ILOSTRINGSTL
		oss << obj;
		ILO_STDSTRING ossText = oss.str();
		_currentMessage += ossText;
#else
		oss << obj << ILOSTD(ends);
		char * ossText = oss.str();
		_currentMessage += ossText;
		oss.rdbuf()->freeze(false);
#endif
	}
	void send();

}; 

typedef int IloDebugLevel;

class IloOutput : public IloEnvObjectI  {
private:
	IloOutputCollector _collector;

public:
	~IloOutput();

	IloOutputCollector& initCollector(IloInt lineno,
		const char* filename,
		IloOutputCollector::IloMessageType messageType,
		IloDebugLevel debugLevel=1);

	
	virtual void fatal  (const char* s) = 0;
	virtual void error  (const char* s) = 0;
	virtual void warning(const char* s) = 0;
	virtual void info   (const char* s) = 0;
	virtual void debug  (const char* s, IloDebugLevel level=1) = 0;

	virtual void p_fatal  (IloInt lineno, const char* filename, const char* s);
	virtual void p_error  (IloInt lineno, const char* filename, const char* s);
	virtual void p_warning(IloInt lineno, const char* filename, const char* s);
	virtual void p_info   (IloInt lineno, const char* filename, const char* s);
	virtual void p_debug  (IloInt lineno, const char* filename, const char* s, IloDebugLevel level=1);

	virtual void setDebugLevel(IloDebugLevel level) = 0;
	virtual IloBool isDebug(IloDebugLevel level) const = 0;

	
	void setDebug(IloBool isDebug);
	
	IloBool isDebug() const { return isDebug(1);}

protected:
	IloOutput(IloEnv env);
}; 

class IloMuteOutput : public IloOutput {

public:
	IloMuteOutput(IloEnv env) :IloOutput(env) {}
	virtual void setDebugLevel(int) ILO_OVERRIDE {}
	virtual IloBool isDebug(int) const ILO_OVERRIDE {return IloTrue;}
	virtual void fatal  (const char*) ILO_OVERRIDE {return;}
	virtual void error  (const char*) ILO_OVERRIDE {return;}
	virtual void warning(const char*) ILO_OVERRIDE {return;}
	virtual void info   (const char*) ILO_OVERRIDE {return;}
	virtual void debug  (const char*, IloDebugLevel) ILO_OVERRIDE {return;}
	using IloOutput::isDebug;
	using IloOutput::info;
	using IloOutput::debug;
};

class IloStreamOutput : public IloOutput {
private:
	ILOSTD(ostream) & _os;
public:
	using IloOutput::info;
	using IloOutput::debug;
	IloStreamOutput(IloEnv env);
	IloStreamOutput(IloEnv env, ILOSTD(ostream) &);

	virtual void fatal  (const char* s) ILO_OVERRIDE;
	virtual void error  (const char* s) ILO_OVERRIDE;
	virtual void warning(const char* s) ILO_OVERRIDE;
	virtual void info   (const char* s) ILO_OVERRIDE;
	virtual void debug  (const char* s, IloDebugLevel level=1) ILO_OVERRIDE;

	static const char* FATAL_PREFIX;
	static const char* ERROR_PREFIX;
	static const char* WARNING_PREFIX;
	static const char* INFO_PREFIX;
	static const char* DEBUG_PREFIX;

protected:
	virtual void printMessage(const char* prefix, const char* msg);
	virtual void printDebugMessage(int level, const char* msg);

public:
	ILOSTD(ostream)& getStream() const { return _os;}
}; 

class IloDefaultOutput : public IloStreamOutput {
	IloBool _errorsAsExceptions;
	IloInt _debugLevel;

	void throwErrorException(const char* s);

public:
	IloDefaultOutput(IloEnv env,
		IloBool errorsAsExceptions=IloTrue,
		IloDebugLevel debugLevel=1);
	IloDefaultOutput(IloEnv env,
		ILOSTD(ostream)&,
		IloBool errorsAsExceptions=IloTrue,
		IloDebugLevel debugLevel=1
		);

	virtual void setDebugLevel(IloDebugLevel level) ILO_OVERRIDE {
		_debugLevel = level;}
	virtual IloBool isDebug(IloDebugLevel level) const ILO_OVERRIDE {
		return level <= _debugLevel;}
	IloDebugLevel getDebugLevel() const { return IloDebugLevel(_debugLevel);}

	IloBool useExceptions() const;
	void useExceptions(IloBool useExceptions);

	virtual void fatal(const char* s) ILO_OVERRIDE;
	virtual void error(const char* s) ILO_OVERRIDE;
	using IloStreamOutput::isDebug;

}; 

class IloOutputException : public IloException {
public:
	IloOutputException(const char* m) : IloException(m, IloTrue) {}
}; 

class IloOutputCollectorTerminator {
	static IloOutputCollectorTerminator _Singleton;
public:
	IloOutputCollectorTerminator() {}

	static IloOutputCollectorTerminator& GetSingleton();

}; 

IloOutputCollector& operator<<(IloOutputCollector&, ILO_STDSTRING s);
IloOutputCollector& operator<<(IloOutputCollector&, const char* text);
IloOutputCollector& operator<<(IloOutputCollector&, char c);
IloOutputCollector& operator<<(IloOutputCollector&, int  i);
IloOutputCollector& operator<<(IloOutputCollector&, IloInt i);
IloOutputCollector& operator<<(IloOutputCollector&, unsigned long ul);
IloOutputCollector& operator<<(IloOutputCollector&, IloNum x);
IloOutputCollector& operator<<(IloOutputCollector&, IloOutputCollectorTerminator& );

#ifndef ILO_SUN55
template <class T>
IloOutputCollector& operator<<(IloOutputCollector& wr, const T& obj) {
	wr.append(obj);
	return wr;
}
#endif

#define ILOMSG_ENDL IloOutputCollectorTerminator::GetSingleton()

#define IloFatal(_out_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_FATAL)
#define IloError(_out_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_ERROR)
#define IloWarning(_out_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_WARNING)
#define IloInfo(_out_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_INFO)

#define IloDebug(_out_, _level_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_DEBUG, _level_)
#define IloDebug1(_out_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_DEBUG, 1)
#define IloDebug2(_out_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_DEBUG, 2)
#define IloDebug3(_out_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_DEBUG, 3)
#define IloDebugn(_out_,_level_) (_out_)->initCollector(__LINE__, __FILE__, IloOutputCollector::ILO_OUT_DEBUG, _level_)

#endif 

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
