// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/iloreport_impl.h
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
#ifndef __CONCERT_report_implH
#define __CONCERT_report_implH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/ilosys.h>
#include <ilconcert/iloenv.h>
#include <ilconcert/ilodict.h>

#define ILOREPORT_MAX_LINE_SIZE 256
class IloReportResultI;

typedef enum {
	IloResultSuccess = 0,
	IloResultFail = 1,
	IloResultError = 2,
	IloResultNotRun = 3,
	IloResultInconsistentData = 4,
	IloResultUnknown = 5
} IloResultStatus;

class ILO_EXPORTED IloReportAbsItemI : public IloDestroyableI {

	friend class IloReportResultI;

public:

	class Exception : public IloException {
		const char* _stringmsg;
	public:
		Exception(const char* msg): IloException(msg), _stringmsg(0){
			if (msg){
				IloInt length = strlen(msg);
				ILOPROTECTNEW(_stringmsg, new char[length+1]);
				IloStrncpy((char *)_stringmsg, length+1, msg, length+1);
			}
		}
	};

	typedef IlodDictionary<char*>::Iterator IloReportElementIterator;

	const char* getName() const {return _name;}

	
	IloBool hasValue(const char* valueName) const;
	IloBool removeValue(const char* valueName);
	const char* getValue(const char* valueName, const char* defValue) const;
	void setValue (const char* valueName, const char* value);

	bool getValue(const char* valueName, bool defValue) const;
	IloInt getValue(const char* valueName, IloInt defValue) const;
	IloNum getValue(const char* valueName, IloNum defValue) const;

	void setValue(const char* valueName, bool value);
	void setValue(const char* valueName, IloInt value);
	void setValue(const char* valueName, IloNum value);

	virtual void displayXML(ILOSTD(ostream)& out) const;
	virtual void display(ILOSTD(ostream)& out) const;

protected:

	IloReportAbsItemI(IloEnv e, const char* name);
	virtual ~IloReportAbsItemI();

	virtual void displayHeadXML(ILOSTD(ostream)& out) const;
	virtual void displayBodyXML(ILOSTD(ostream)& out) const;
	virtual void displayTailXML(ILOSTD(ostream)& out) const;

	bool Char2Bool(const char* value) const;
	IloInt Char2Int(const char* value) const;
	IloNum Char2Num(const char* value) const;

	void Bool2Char(bool value, char*, const size_t buffSize);
	char* Int2Char(IloInt value) const;
	char* Num2Char(IloNum value) const;
	char* precision(double r) const;

protected:
	char* _name;
	IlodDictionary<char*> _dictTable;

}; 

class IloReportItemI : public IloReportAbsItemI {
	
public:
	IloReportItemI(IloEnv e, const char* name);

protected:

	IloReportItemI* makeClone();

};

class IloReportTreeI : public IloReportAbsItemI {

	typedef IloArray<IloReportItemI*> IloReportItemList;
	typedef IlodDictionary<IloReportItemList>::Iterator IloReportItemListIterator;
	typedef IlodDictionary<IloReportAbsItemI*>::Iterator IloReportAbsItemIterator;

public:

	IloReportTreeI(IloEnv e, const char* name);
	virtual ~IloReportTreeI();

	IloBool hasSubSection(const char* subSectionName) const;
	IloReportAbsItemI* getSubSection(const char* subSectionName) const;
	void addSubSection(IloReportAbsItemI* subSection);

	void createList(const char* listName);
	void addToList(const char* listName, IloReportItemI* element);
	IloInt getNumberOfElementsInList(const char* listName) const;
	IloReportItemI* getElementFromList(const char* listName, IloInt idx) const;
	IloReportItemList getList(const char* listName) const;

	void displayXML(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;

protected:

	IlodDictionary<IloReportAbsItemI*> _childrenTable;
	IlodDictionary<IloReportItemList> _listTable;

};

class IloSettingsI : public IloReportAbsItemI {

public:

	typedef IloReportElementIterator IloSettingIterator;

	IloSettingsI(IloEnv e);
	virtual ~IloSettingsI();

	virtual void read(const char* fileName) {readFromFileWithSections(fileName);}
	virtual void write(const char* fileName) {writeToFileWithSections(fileName);}

	void readFromFileWithSections(const char* fileName, const char* startSection = 0, const char* endSection = 0, char lineDelim = '\n');
	void writeToFileWithSections(const char* fileName, const char* startSection = 0, const char* endSection = 0, char lineDelim = '\n');
	void writeToStreamWithSections(ILOSTD(ostream)& out, const char* startSection = 0, const char* endSection = 0, char lineDelim = '\n');

	void setCommented(const char* string);

	class Iterator {
		IloSettingsI* _settings;
		IloSettingIterator _it;
	public:
		Iterator(IloSettingsI* settings); 
		IloBool ok() const { return _it.ok(); }
		const char* operator * () { return *_it; }
		const char* getKey() { return _it.getKey(); }
		void operator++() { ++_it; }
	};
	friend class Iterator;

protected:
	static const char* MSG_INPUT_FILE_NOT_FOUND;

	IloBool parseLine(const char* line, char* valueName, char* value);
	const char* getCommented() const {return _comment;}

private:

	char _comment[ILOREPORT_MAX_LINE_SIZE];

}; 

class IloReportResultI : public IloReportItemI 
{
public:

	IloReportResultI(IloEnv e);

	void setStrategyName(const char* name) {setValue("strategyName",name);}
	void setPhaseName(const char* name) {setValue("phaseName",name);}
	void setCost(IloNum cost) {setValue("cost",cost);}
	void setTime(IloNum time) {setValue("time",time);}
	void setFeasible(IloBool isFeasible) {setValue("isFeasible",isFeasible);}
	void addKpi(const char* name, IloNum value) {_kpiTable->setValue(name,value);}

	void addResultInfo(const char* valueName, const char* value) {setValue(valueName,value);}
	void addResultInfo(const char* valueName, bool value) {setValue(valueName,value);}
	void addResultInfo(const char* valueName, IloInt value) {setValue(valueName,value);}
	void addResultInfo(const char* valueName, IloNum value) {setValue(valueName,value);}

	const char* getStrategyName() const {return getValue("strategyName","");}
	const char* getPhaseName() const {return getValue("phaseName","");}
	IloNum getCost() const {return getValue("cost",(IloNum)IloIntMax);}
	IloNum getTime() const {return getValue("time",-1.0);}
	IloBool isFeasible() const {return getValue("isFeasible",IloFalse);}
	IloNum getKpi(const char* name) const { return _kpiTable->getValue(name,-1.0);}

	void displayXML(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;

protected:

	IloReportItemI* _kpiTable;

};

class IloReportStrategyI : public IloReportItemI 
{

public:

	IloReportStrategyI(IloEnv e);

	void setName(const char* name) {setValue("name",name);}
	void setFatherName(const char* name) {setValue("fatherName",name);}
	void setStartTime(IloNum time) {setValue("startTime",time);}
	void setEndTime(IloNum time) {setValue("endTime",time);}
	void setTimeLimit(IloNum time) {setValue("timeLimit",time);}
	void setSolutionLimit(IloInt nbOfSolutions) {setValue("solutionLimit",nbOfSolutions);}
	void setFailLimit(IloInt fails) {setValue("failLimit",fails);}
	void setTimeUsed(IloNum time) {setValue("timeUsed",time);}
	void setNumberOfSolutions(IloInt nbOfSolutions) {setValue("numberOfSolutions",nbOfSolutions);}
	void setNumberOfFails(IloInt fails) {setValue("numberOfFails",fails);}
	void setOptimizationRunNumber(IloInt runNb) {setValue("optimizationRunNumber",runNb);}

	const char* getName() const {return getValue("name","");}
	const char* getFatherName() const {return getValue("fatherName","");}
	IloNum getStartTime() const {return getValue("startTime",-1.0);}
	IloNum getEndTime() const {return getValue("endTime",-1.0);}
	IloNum getTimeLimit() const {return getValue("timeLimit",-1.0);}
	IloInt getSolutionLimit() const {return getValue("solutionLimit",(IloInt)-1);}
	IloInt getFailLimit() const {return getValue("failLimit",(IloInt)-1);}
	IloNum getTimeUsed() const {return getValue("timeUsed",-1.0);}
	IloInt getNumberOfSolutions() const {return getValue("numberOfSolutions",(IloInt)-1);}
	IloInt getNumberOfFails() const {return getValue("numberOfFails",(IloInt)-1);}
	IloInt getOptimizationRunNumber() const {return getValue("optimizationRunNumber",(IloInt)0L);}

	void addStrategyInfo(const char* valueName, const char* value) {setValue(valueName, value);}
	void addStrategyInfo(const char* valueName, bool value) {setValue(valueName, value);}
	void addStrategyInfo(const char* valueName, IloInt value) {setValue(valueName, value);}
	void addStrategyInfo(const char* valueName, IloNum value) {setValue(valueName, value);}

	void displayXML(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	void display(ILOSTD(ostream)& out) const ILO_OVERRIDE;
};

class IloReportI : public IloReportTreeI {

public:

	IloReportI(IloEnv e);

	void setVerticalVersion(const char* version);
	
	void setClientName(const char* name);
	void setProblemShortName(const char* name);
	void setProblemName(const char* name);
	void setIniFileName(const char* name);

	void setSettings(IloSettingsI* s);
	void addGroupToProblemCharacteristics(IloReportItemI* group);

	void setResultScore(IloNum score);
	void setResultStatus(IloResultStatus status);
	void setNbOfSolutionFound(IloInt nbOfSol);
	void setFinishTime(IloNum time);
	void setConcertMemory(IloInt memory);
	void setSolverMemory(IloInt memory);
	void setCplexMemory(IloInt memory);
	void addResultItem(IloReportResultI* element);
	void addStrategyItem(IloReportStrategyI* element);

	IloNum getResultScore() const;
	IloResultStatus getResultStatus() const;
	const char* getVerticalVersion() const;
	const char* getClientName() const;
	const char* getProblemShortName() const;
	const char* getProblemName() const;
	const char* getIniFileName() const;
	IloInt getNbOfSolutionFound() const;
	IloNum getFinishTime() const;
	IloInt getConcertMemory() const;
	IloInt getSolverMemory() const;
	IloInt getCplexMemory() const;

	const char* getProjectName() const;
	const char* getResponsible() const;
	const char* getVerticalBuildId() const;
	const char* getConcertVersionId() const;
	const char* getSolverVersionId() const;
	const char* getCplexVersionId() const;
	const char* getSchedulerVersionId() const;
	const char* getDispatcherVersionId() const;
	const char* getHostName() const;
	const char* getHostCpu() const;
	const char* getHostMhz() const;
	const char* getHostMemory() const;
	const char* getTime() const;
	const char* getDate() const;

	IloReportTreeI* getProblemCharacteristics() const;
	IloBool hasProblemCharacteristics() const;
	IloReportItemI* getGroupOfProblemCharacteristics(const char* groupName) const;
	IloBool hasGroupOfProblemCharacteristics(const char* groupName) const;

protected :

	void makeStructure();
	void init();
	void readInfoFromFile();

};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 

