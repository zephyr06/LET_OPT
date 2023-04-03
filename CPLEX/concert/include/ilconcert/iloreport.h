// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloreport.h
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
#ifndef __CONCERT_reportH
#define __CONCERT_reportH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloreport_impl.h>

#if !(defined(SUN21))
#define ILOHANDLEDTOR_REPORT(Hname) ~Hname(){}
#else
#define ILOHANDLEDTOR_REPORT(Hname)
#endif

#if defined(ILO_HP_aCC)
#define ILOCOPYCTOR_REPORT(Hname) Hname(const Hname& h):_impl(h._impl){}
#else
#define ILOCOPYCTOR_REPORT(Hname)
#endif

#define ILOHANDLEMINI_REPORT(Hname, Iname)\
	typedef Iname ImplClass; \
public:\
	Hname():_impl(0){}\
	Hname(Iname* impl):_impl(impl){}\
	ILOCOPYCTOR_REPORT(Hname)\
	ILOHANDLEDTOR_REPORT(Hname)\
	Iname* getImpl() const {return _impl;}\
	void operator=(const Hname& h){_impl = h._impl;}\
protected:\
	Iname* _impl

class IloReportItem {
	ILOHANDLEMINI_REPORT(IloReportItem, IloReportItemI);

	friend ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloReportItem o);

public:

	
	IloReportItem(IloEnv env, const char* name)
		:_impl(new (env) IloReportItemI(env,name))
	{}

	
	const char* getName() const {return _impl->getName();}

	
	IloBool hasValue(const char* valueName) const {return _impl->hasValue(valueName);}
	
	const char* getValue(const char* valueName, const char* defValue) const {return _impl->getValue(valueName, defValue);}
	
	void setValue (const char* valueName, const char* value) {_impl->setValue(valueName, value);}
	
	bool getValue(const char* valueName, bool defValue) const {return _impl->getValue(valueName, defValue);}
	IloBool getBoolValue(const char* valueName, IloBool defValue) const {return _impl->getValue(valueName, defValue);}
	
	IloInt getValue(const char* valueName, IloInt defValue) const {return _impl->getValue(valueName, defValue);}
	IloInt getIntValue(const char* valueName, IloInt defValue) const {return _impl->getValue(valueName, defValue);}
	
	IloNum getValue(const char* valueName, IloNum defValue) const {return _impl->getValue(valueName, defValue);}
	IloNum getFloatValue(const char* valueName, IloNum defValue) const {return _impl->getValue(valueName, defValue);}

	
	void setValue(const char* valueName, bool value) {_impl->setValue(valueName, value);}
	void setBoolValue(const char* valueName, IloBool value) {_impl->setValue(valueName, value);}
	
	void setValue(const char* valueName, IloInt value) {_impl->setValue(valueName, value);}
	void setIntValue(const char* valueName, IloInt value) {_impl->setValue(valueName, value);}
	
	void setValue(const char* valueName, IloNum value) {_impl->setValue(valueName, value);}
	void setFloatValue(const char* valueName, IloNum value) {_impl->setValue(valueName, value);}

	
	void displayXML(ILOSTD(ostream)& outs) const {_impl->displayXML(outs);}
	
	void display(ILOSTD(ostream)& outs) const {_impl->display(outs);}

};

class IloSettings {

	ILOHANDLEMINI_REPORT(IloSettings, IloSettingsI);

	friend ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloSettings o);

public:

	
	IloSettings(IloEnv env)
		:_impl(new (env) IloSettingsI(env))
	{}

	
	IloBool hasValue(const char* valueName) const {return _impl->hasValue(valueName);}
	
	const char* getValue(const char* valueName, const char* defValue) const {return _impl->getValue(valueName, defValue);}
	
	void setValue(const char* valueName, const char* value) {_impl->setValue(valueName, value);}
	
	bool getValue(const char* valueName, bool defValue) const {return _impl->getValue(valueName, defValue);}
	IloBool getBoolValue(const char* valueName, IloBool defValue) const {return _impl->getValue(valueName, defValue);}
	
	IloInt getValue(const char* valueName, IloInt defValue) const {return _impl->getValue(valueName, defValue);}
	IloInt getIntValue(const char* valueName, IloInt defValue) const {return _impl->getValue(valueName, defValue);}
	
	IloNum getValue(const char* valueName, IloNum defValue) const {return _impl->getValue(valueName, defValue);}
	IloNum getFloatValue(const char* valueName, IloNum defValue) const {return _impl->getValue(valueName, defValue);}

	
	void setValue(const char* valueName, bool value) {_impl->setValue(valueName, value);}
	void setBoolValue(const char* valueName, IloBool value) {_impl->setValue(valueName, value);}
	
	void setValue(const char* valueName, IloInt value) {_impl->setValue(valueName, value);}
	void setIntValue(const char* valueName, IloInt value) {_impl->setValue(valueName, value);}
	
	void setValue(const char* valueName, IloNum value) {_impl->setValue(valueName, value);}
	void setFloatValue(const char* valueName, IloNum value) {_impl->setValue(valueName, value);}

	
	void read(const char* fileName) {_impl->read(fileName);}
	
	void write(const char* fileName) {_impl->write(fileName);}

	
	void displayXML(ILOSTD(ostream)& outs) const {_impl->displayXML(outs);}
	
	void display(ILOSTD(ostream)& outs) const {_impl->display(outs);}

	
	void setCommented(const char* str) const {_impl->setCommented(str);}

	
	void removeValue(const char* str) const { _impl->removeValue(str); }
}; 

class IloReportResult
{

	ILOHANDLEMINI_REPORT(IloReportResult, IloReportResultI);

	friend ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloReportResult o);

public:

	
	IloReportResult(IloEnv e)
		:_impl(new (e) IloReportResultI(e))
	{}

	
	void setStrategyName(const char* name) {_impl->setStrategyName(name);}
	
	void setPhaseName(const char* name) {_impl->setPhaseName(name);}
	
	void setCost(IloNum cost) {_impl->setCost(cost);}
	
	void setTime(IloNum time) {_impl->setTime(time);}
	
	void setFeasible(IloBool isFeasible) {_impl->setFeasible(isFeasible);}
	
	void addKpi(const char* name, IloNum value) {_impl->addKpi(name,value);}

	
	void addResultInfo(const char* valueName, const char* value) {_impl->addResultInfo(valueName, value);}
	
	void addResultInfo(const char* valueName, bool value) {_impl->addResultInfo(valueName, value);}
	
	void addResultInfo(const char* valueName, IloInt value) {_impl->addResultInfo(valueName, value);}
	
	void addResultInfo(const char* valueName, IloNum value) {_impl->addResultInfo(valueName, value);}

	
	void displayXML(ILOSTD(ostream)& outs) const {_impl->displayXML(outs);}
	
	void display(ILOSTD(ostream)& outs) const {_impl->display(outs);}

};

class IloReportStrategy
{

	ILOHANDLEMINI_REPORT(IloReportStrategy, IloReportStrategyI);

	friend ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloReportStrategy o);

public:

	
	IloReportStrategy(IloEnv e)
		:_impl(new (e) IloReportStrategyI(e))
	{}

	
	void setName(const char* name) {_impl->setName(name);}
	
	void setFatherName(const char* name) {_impl->setFatherName(name);}
	
	void setStartTime(IloNum time) {_impl->setStartTime(time);}
	
	void setEndTime(IloNum time) {_impl->setEndTime(time);}
	
	void setTimeLimit(IloNum time) {_impl->setTimeLimit(time);}
	
	void setSolutionLimit(IloInt nbOfSolutions) {_impl->setSolutionLimit(nbOfSolutions);}
	
	void setFailLimit(IloInt fails) {_impl->setFailLimit(fails);}
	
	void setTimeUsed(IloNum time) {_impl->setTimeUsed(time);}
	
	void setNumberOfSolutions(IloInt nbOfSolutions) {_impl->setNumberOfSolutions(nbOfSolutions);}
	
	void setNumberOfFails(IloInt fails) {_impl->setNumberOfFails(fails);}
	
	void setOptimizationRunNumber(IloInt runNb) {_impl->setOptimizationRunNumber(runNb);}

	
	const char* getName() const {return _impl->getName();}
	
	const char* getFatherName() const {return _impl->getFatherName();}
	
	IloNum getStartTime() const {return _impl->getStartTime();}
	
	IloNum getEndTime() const {return _impl->getEndTime();}
	
	IloNum getTimeLimit() const {return _impl->getTimeLimit();}
	
	IloInt getSolutionLimit() const {return _impl->getSolutionLimit();}
	
	IloInt getFailLimit() const {return _impl->getFailLimit();}
	
	IloNum getTimeUsed() const {return _impl->getTimeUsed();}
	
	IloInt getNumberOfSolutions() const {return _impl->getNumberOfSolutions();}
	
	IloInt getNumberOfFails() const {return _impl->getNumberOfFails();}
	
	IloInt getOptimizationRunNumber() const {return _impl->getOptimizationRunNumber();}

	
	void addStrategyInfo(const char* valueName, const char* value) {_impl->addStrategyInfo(valueName, value);}
	
	void addStrategyInfo(const char* valueName, bool value) {_impl->addStrategyInfo(valueName, value);}
	
	void addStrategyInfo(const char* valueName, IloInt value) {_impl->addStrategyInfo(valueName, value);}
	
	void addStrategyInfo(const char* valueName, IloNum value) {_impl->addStrategyInfo(valueName, value);}

	
	void displayXML(ILOSTD(ostream)& outs) const {_impl->displayXML(outs);}
	
	void display(ILOSTD(ostream)& outs) const {_impl->display(outs);}

};

class IloReport {

	ILOHANDLEMINI_REPORT(IloReport, IloReportI);

	friend ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloReport o);

public:

	
	IloReport(IloEnv e)
		:_impl(new (e) IloReportI(e))
	{}

	
	void setVersion(const char* version) {_impl->setVerticalVersion(version);}
	
	void setClientName(const char* name) {_impl->setClientName(name);}
	
	void setProblemShortName(const char* name) {_impl->setProblemShortName(name);}
	
	void setProblemName(const char* name) {_impl->setProblemName(name);}
	
	void setIniFileName(const char* name) {_impl->setIniFileName(name);}
	
	void setSettings(IloSettings s) {_impl->setSettings(s.getImpl());}

	
	void addGroupToProblemCharacteristics(IloReportItem group) {
		_impl->addGroupToProblemCharacteristics(group.getImpl());}
	
	void setResultStatus(IloResultStatus status) {_impl->setResultStatus(status);}
	
	void setResultScore(IloNum score) {_impl->setResultScore(score);}
	
	void setNbOfSolutionFound(IloInt nbOfSol) {_impl->setNbOfSolutionFound(nbOfSol);}
	
	void setFinishTime(IloNum time) {_impl->setFinishTime(time);}
	
	void setConcertMemory(IloInt memory) {_impl->setConcertMemory(memory);}
	
	void setSolverMemory(IloInt memory) {_impl->setSolverMemory(memory);}
	
	void setCplexMemory(IloInt memory) {_impl->setCplexMemory(memory);}
	
	void addResult(IloReportResult element) {_impl->addResultItem(element.getImpl());}
	
	void addStrategy(IloReportStrategy element) {_impl->addStrategyItem(element.getImpl());}
	
	IloResultStatus getResultStatus() const {return _impl->getResultStatus();}
	
	IloNum getResultScore() const {return _impl->getResultScore();}
	
	IloReportItem getGroupOfProblemCharacteristics(const char* groupName) const {
		return _impl->getGroupOfProblemCharacteristics(groupName);
	}
	
	const char* getVerticalVersion() const {return _impl->getVerticalVersion();}
	
	const char* getClientName() const {return _impl->getClientName();}
	
	const char* getProblemShortName() const {return _impl->getProblemShortName();}
	
	const char* getProblemName() const {return _impl->getProblemName();}
	
	const char* getIniFileName() const {return _impl->getIniFileName();}
	
	IloInt getNbOfSolutionFound() const {return _impl->getNbOfSolutionFound();}
	
	IloNum getFinishTime() const {return _impl->getFinishTime();}
	
	IloInt getConcertMemory() const {return _impl->getConcertMemory();}
	
	IloInt getSolverMemory() const {return _impl->getSolverMemory();}
	
	IloInt getCplexMemory() const {return _impl->getCplexMemory();}
	
	const char* getProjectName() const {return _impl->getProjectName();}
	
	const char* getResponsible() const {return _impl->getResponsible();}
	
	const char* getVerticalBuildId() const {return _impl->getVerticalBuildId();}
	
	const char* getConcertVersionId() const {return _impl->getConcertVersionId();}
	
	const char* getSolverVersionId() const {return _impl->getSolverVersionId();}
	
	const char* getCplexVersionId() const {return _impl->getCplexVersionId();}
	
	const char* getSchedulerVersionId() const {return _impl->getSchedulerVersionId();}
	
	const char* getDispatcherVersionId() const {return _impl->getDispatcherVersionId();}
	
	const char* getHostName() const {return _impl->getHostName();}
	
	const char* getHostCpu() const {return _impl->getHostCpu();}
	
	const char* getHostMhz() const {return _impl->getHostMhz();}
	
	const char* getHostMemory() const {return _impl->getHostMemory();}
	
	const char* getTime() const {return _impl->getTime();}
	
	const char* getDate() const {return _impl->getDate();}
	
	void displayXML(ILOSTD(ostream)& outs) const {_impl->displayXML(outs);}
	
	void display(ILOSTD(ostream)& outs) const {_impl->display(outs);}

};

class IloResultIterator
{
public:
	IloResultIterator(IloReport report)
		:_i(0)
	{
		IloReportTreeI* results_statistics = (IloReportTreeI*) (report.getImpl()->getSubSection("resultsStatistics"));
		_resultList = results_statistics->getList("resultList");
	}
	IloBool ok() const {return (_i < _resultList.getSize());}
	IloReportResult operator*() const {return (IloReportResultI*)(_resultList[_i]);}
	void operator++() {_i++;}

protected:
	IloArray<IloReportItemI*> _resultList;
	IloInt _i;
};

inline ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloReportItem g)
{
	g.display(out);
	return out;
}

inline ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloSettings g)
{
	g.display(out);
	return out;
}

inline ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloReportResult g)
{
	g.display(out);
	return out;
}

inline ILOSTD(ostream)& operator <<(ILOSTD(ostream)& out, const IloReport g)
{
	g.display(out);
	return out;
}

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
