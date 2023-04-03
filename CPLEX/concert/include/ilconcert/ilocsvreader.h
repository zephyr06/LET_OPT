// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilocsvreader.h
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
#ifndef __CONCERT_ilocsvreaderH
#define __CONCERT_ilocsvreaderH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloenv.h>
#include <ilconcert/ilohash.h>

#if defined(ILO_WINDOWS) || defined(ILO_SGI) && !defined(IL_STD)
#include <stdio.h>
#endif

class IloCsvTableReaderI;
class IloCsvTableReader;

class IloCsvLineI {

private :
	char *               _line;
	char **              _fields;
	IloCsvTableReaderI * _csv;
	IloInt               _lineNumber;
	IloEnvI *            _env;
	IloInt               _nbFields;

	void clear();
	void init(const char *);
	IloInt computeLength(char *);
	void setFieldsToLine(char **);
	void check(const char *, IloInt) const;

public :
	IloCsvLineI(IloEnv env) : _line(0),
		_fields(0),
		_csv(0),
		_lineNumber(0),
		_env(env.getImpl()),
		_nbFields(0) { }
	IloCsvLineI(const char *,
		IloCsvTableReaderI *,
		IloInt);
	virtual ~IloCsvLineI();
	char ** getFields() const { return _fields; }
	char * getLine() const { return _line; }
	IloEnv getEnv() const { return _env; }
	inline IloInt getNumberOfFields() const;
	inline IloNum getFloatByPosition(IloInt) const;
	inline IloInt getIntByPosition(IloInt) const;
	inline char * getStringByPosition(IloInt) const;
	inline IloNum getFloatByHeader(const char *) const;
	inline IloInt getIntByHeader(const char *) const;
	inline char * getStringByHeader(const char *) const;
	inline IloNum getFloatByPositionOrDefaultValue(IloInt, IloNum) const;
	inline IloInt getIntByPositionOrDefaultValue(IloInt, IloInt) const;
	inline char * getStringByPositionOrDefaultValue(IloInt, const char *) const;
	inline IloNum getFloatByHeaderOrDefaultValue(const char *, IloNum) const;
	inline IloInt getIntByHeaderOrDefaultValue(const char *, IloInt) const;
	inline char * getStringByHeaderOrDefaultValue(const char *, const char *) const;
	inline IloBool printValueOfKeys() const;
	IloInt getLineNumber() const { return _lineNumber; }
	void buildCsvLine(char *, char **, IloInt, IloCsvTableReaderI *);
	IloCsvTableReaderI * getCsv() const { return _csv; }
	void setFields(char ** fields) { _fields = fields; }
	void setLine(char * line) { _line = line; }
	void copy(const IloCsvLineI *);
	IloBool emptyFieldByPosition(IloInt) const;
	IloBool emptyFieldByHeader(const char *) const;
	void operator delete(void *, size_t);
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloEnvI *);
	void operator delete(void *, const IloEnv&);
#endif
	class Exception : public IloException {
	public: Exception(const char * s) : IloException(s) { }
	};

};

class IloCsvLine{
private :
	IloCsvLineI * _impl;
public :
	
	IloCsvLine(): _impl(0) {}
	
	IloCsvLine(IloCsvLineI* impl): _impl(impl) {}
	
	IloCsvLine(const IloCsvLine& csvLine): _impl(csvLine._impl) {}
	~IloCsvLine() { }
	
	void end() {
		delete _impl;
		_impl = 0;
	}
	
	IloCsvLineI* getImpl() const { return _impl; }
	
	void operator=(const IloCsvLine& csvLine) { _impl = csvLine.getImpl(); }
	
	IloCsvLine(const char *,
		IloCsvTableReader,
		IloInt);
	
	IloInt getNumberOfFields() const;
	
	IloNum getFloatByPosition(IloInt i) const;
	
	IloInt getIntByPosition(IloInt i) const;
	
	char * getStringByPosition(IloInt i) const;
	
	IloNum getFloatByHeader(const char * name) const;
	
	IloInt getIntByHeader(const char * name) const;
	
	char * getStringByHeader(const char * name) const;

	
	IloNum getFloatByPositionOrDefaultValue(IloInt i, IloNum defaultValue) const;
	
	IloInt getIntByPositionOrDefaultValue(IloInt i, IloInt defaultValue) const;
	
	char * getStringByPositionOrDefaultValue(IloInt i, const char * defaultValue) const;
	
	IloNum getFloatByHeaderOrDefaultValue(const char * name, IloNum defaultValue) const;
	
	IloInt getIntByHeaderOrDefaultValue(const char * name, IloInt defaultValue) const;
	
	char * getStringByHeaderOrDefaultValue(const char * name, const char * defaultValue) const;
	
	IloBool printValueOfKeys() const;
	char ** getFields() const;
	
	IloInt getLineNumber() const;
	
	void copy(const IloCsvLine);
	IloEnv getEnv() const;
	
	IloBool emptyFieldByPosition(IloInt i) const;
	
	IloBool emptyFieldByHeader(const char * name) const;
};

inline ILOSTD(ostream)& operator<<(ILOSTD(ostream)& out, const IloCsvLine& line) {
	if (line.getImpl()) {
		IloInt lineNumber = line.getLineNumber();
		out << "Line number : ";
		out << lineNumber;
		out << ILOSTD(endl);
		IloInt numberOfFields = line.getNumberOfFields();
		for (IloInt i = 0; i < numberOfFields; i++) {
			out << line.getStringByPosition(i);
			if(i < numberOfFields - 1)
				out << ',';
		}
		out << ILOSTD(endl);
	}
	return out;
}

class OffsetAndLineNumber{
	IloInt _offset;
	IloInt _lineNumber;

public :
	OffsetAndLineNumber() : _offset(0), _lineNumber(0) { }
	OffsetAndLineNumber(IloInt offset, IloInt lineNumber) : _offset(offset), _lineNumber(lineNumber) { }
	OffsetAndLineNumber(const OffsetAndLineNumber& value) {
		_offset = value.getOffset();
		_lineNumber = value.getLineNumber();
	}
	~OffsetAndLineNumber() { }
	IloInt getOffset() const { return _offset; }
	IloInt getLineNumber() const { return _lineNumber; }
};

class IloCsvReaderI {
	IloEnv                                               _env;
	const char *                                         _fileName;
	FILE *                                               _infile;
	IloEnvHashTable<const char *, OffsetAndLineNumber *> _csvBeginOffsetAndLineNumberHashTable;
	IloEnvHashTable<const char *, IloInt>                _csvEndOffsetHashTable;
	IloArray<const char *>                               _arrayOfTableNames;
	IloInt                                               _numberOfTables;
	char                                                 _comment;
	char *                                               _separator;
	char                                                 _quote;
	char                                                 _decimal;
	char                                                 _maxchar;
	IloBool                                              _isCached;
	IloBool                                              _allowTableSplitting;
	IloBool                                              _multiTable;
	IloCsvTableReaderI *                                 _readerForUniqueTableFile;
	IloNum                                               _csvFormat;
	IloNum                                               _fileVersion;
	const char *                                         _requiredBy;
	char *                                               _buffer;
	IloBool 			       		       _duplicatedTable;
	IloBool					        _emptyTableName;
	IloInt						_emptyLineNumber;

	void init(const char * fileName,
		const char * separator,
		const char quote,
		const char decimal,
		const char comment);
	IloBool stockTableNamesAndOffsets();
	char * addOffset(const char *, OffsetAndLineNumber *);
	IloBool isNewTable(char *, char *);
	void testReaderForUniquetable(const char *) const;
	void testReaderForMultitable(const char *) const;
	IloBool isDuplicatedTable(char *, OffsetAndLineNumber *);

public :
	IloCsvReaderI(IloEnv env,
		const char * fileName,
		IloBool isCached = IloFalse,
		IloBool multiTable = IloTrue,
		IloBool allowTableSplitting = IloFalse,
		const char * separator = ",;\t",
		const char decimal = '.',
		const char quote = '\"',
		const char comment = '#');
	void clear();
	virtual ~IloCsvReaderI();
	IloInt getEndOffset(const char *) const;
	OffsetAndLineNumber * getOffsetAndLineNumber(const char *) const;
	const char * getNameOfTable(IloInt i) const { return _arrayOfTableNames[i]; }
	const char * getFileName() const { return _fileName; }
	IloCsvTableReaderI * getTableByNumber(IloInt i);
	IloCsvTableReaderI * getTableByName(const char * name);
	IloCsvTableReaderI * getTable();
	IloInt getNumberOfTables();
	IloEnv getEnv() const { return _env; }
	char getComment() const { return _comment; }
	char *  getSeparator() const { return _separator; }
	char getQuote() const { return _quote; }
	char getDecimal() const { return _decimal; }
	char getMaxChar() const { return _maxchar; }
	char computeMaxChar(const char * separator);
	IloBool isCached() const { return _isCached; }
	char * getFirstField(char *);
	IloBool isEmpty(char *);
	IloBool allowTableSplitting() const { return _allowTableSplitting; }
	IloBool skipCommentAndEmptyLines(FILE *, char *, const char, IloInt *, IloInt *);
	void setCsvFormat();
	void setRequiredByAndFileVersion();
	IloNum getCsvFormat();
	const char * getRequiredBy();
	IloNum getFileVersion();
	IloBool isMultiTable() const { return _multiTable; }
	IloCsvLineI* getLineByNumber(IloInt i);
	IloCsvLineI* getLineByKey(const char *);
	IloInt getNumberOfItems();
	IloInt getNumberOfColumns();
	IloCsvLineI * getFirstLine() const;
	IloBool isHeadingExists(const char *)const;
	IloBool isTableExists(const char *) const;
	IloInt getNumberOfKeys() const;
	IloBool printKeys() const;
	IloCsvTableReaderI * getReaderForUniqueTableFile() const;
	IloBool duplicatedTable() const { return _duplicatedTable; }
	IloBool emptyTableName() const { return _emptyTableName; }
	IloInt getEmptyLineNumber() const { return _emptyLineNumber; }
	char * getNameOfLatestTableRead() const { return _buffer; }
	void trim(char *) const;
	IloInt getPosition(const char *) const;
	void testNumberOfFields(IloCsvLineI *);
	void operator=(const IloCsvReaderI& csv);
	void operator delete(void*, size_t);
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloEnvI *);
	void operator delete(void *, const IloEnv&);
#endif

	
	virtual IloNum parseFloat(const char* floatAsString) const;
	
	virtual IloInt parseInt  (const char* intAsString, IloBool& overflow) const;

	class Exception : public IloException {
	public: Exception(const char * s) : IloException(s) { }
	};

	class CsvTableIteratorI {
		IloCsvReaderI *  	      _csv;
		IloCsvTableReaderI *      _currentTable;
		IloInt                    _index;
		IloBool                   _ok;
		IloInt                    _size;

		IloCsvReaderI * getCsv() const { return _csv; }
		IloInt getIndex() const { return _index; }
		IloCsvTableReaderI * getCurrentTable() const { return _currentTable; }
		IloInt getSize() const { return _size; }
		void copy(const CsvTableIteratorI& csv);
	public:
		CsvTableIteratorI()
			: _csv(0), _currentTable(0), _index(0), _ok(0), _size(0) {}
		CsvTableIteratorI(IloCsvReaderI * csv);
		CsvTableIteratorI(const CsvTableIteratorI& csv);
		virtual ~CsvTableIteratorI();
		void clear();
		void operator=(const CsvTableIteratorI& csvIterator);
		virtual void initIterator();
		virtual void operator++();
		IloCsvTableReaderI * operator*() const { return _currentTable; }
		IloBool ok() const { return _ok; }
	};
	friend class CsvTableIteratorI;
	friend class OffsetAndLineNumber;
};

class IloCsvTableReaderI {

protected :
	IloCsvLineI *             _firstLine;
	IloInt                    _length;
	IloInt                    _nbFields;
	IloInt                    _currentNbFields;
	IloCsvReaderI *  	        _csv;
	const char *              _nameOfTable;
	IloArray<IloInt>          _keysPosition;
	FILE *                    _infile;
	IloInt                    _effectifBeginOffset;
	IloInt                    _endOffset;
	IloInt                    _effectifBeginLineNumber;
	IloEnvI *                 _env;

	void setFirstLine(IloCsvLineI * line) { _firstLine = line; }
	void formatLine(char *);
	IloBool skipCommentAndEmptyLines(FILE *, char *, const char, IloInt *, IloInt * offset = 0);
public :
	IloCsvTableReaderI(IloEnv env);
	IloCsvTableReaderI(IloCsvReaderI * csv, const char * name = 0, OffsetAndLineNumber * value = 0);
	virtual ~IloCsvTableReaderI();
	virtual IloCsvLineI* getLineByNumber(IloInt i) = 0;
	virtual IloCsvLineI* getLineByKey(const char *) = 0;
	virtual IloInt getNumberOfItems() = 0;
	IloInt getNumberOfColumns();
	IloCsvReaderI * getCsv() const { return _csv; }
	IloEnv getEnv() const { return _env; }
	char getComment() const { return _csv->getComment(); }
	char *  getSeparator() const { return _csv->getSeparator(); }
	char getQuote() const { return _csv->getQuote(); }
	char getDecimal() const { return _csv->getDecimal(); }
	char getMaxChar() const { return _csv->getMaxChar(); }
	IloCsvLineI * getFirstLine() const { return _firstLine; }
	IloBool isHeadingExists(const char *) const;
	IloInt getLength() const { return _length; }
	IloInt getNumberOfFields() const { return _nbFields; }
	IloInt getCurrentNumberOfFields() const { return _currentNbFields; }
	const char * getNameOfTable() const { return _nameOfTable; }
	IloCsvReaderI * getFileCsvReader() const { return _csv; }
	IloBool allowTableSplitting() const { return _csv->allowTableSplitting(); }
	char * getFirstField(char * line) { return _csv->getFirstField(line); }
	void trim(char * string) const { _csv->trim(string); }
	const char * getFileName() const { return _csv->getFileName(); }
	IloInt getEffectifBeginOffset() const { return _effectifBeginOffset; }
	IloInt getEffectifBeginLineNumber() const { return _effectifBeginLineNumber; }
	IloInt getEndOffset() const { return _endOffset; }
	virtual OffsetAndLineNumber * getOffsetAndLineNumber(const char *) const = 0;
	IloBool isCached() const;
	IloBool isMultiTable() const { return _csv->isMultiTable(); }
	IloInt getNumberOfKeys() const { return _keysPosition.getSize(); }
	IloInt getKeyPosition(IloInt i) const { return _keysPosition[i]; }
	IloBool printKeys() const;
	void partialClear();
	virtual void clearTable() = 0;
	void partialCopy(IloCsvTableReaderI *);
	virtual void copy(IloCsvTableReaderI *) = 0;
	IloInt getPosition(const char *) const;
	void testNumberOfFields(IloCsvLineI *);
	void operator=(const IloCsvTableReaderI& csv);
	void operator delete(void*, size_t);
#ifdef ILODELETEOPERATOR
	void operator delete(void *, const IloEnvI *);
	void operator delete(void *, const IloEnv&);
#endif
	class Exception : public IloException {
	public: Exception(const char * s) : IloException(s) { }
	};
};

class IloCachedCsvTableReaderI :public IloCsvTableReaderI {
	IloArray<const char *>                                _arrayOfKeysValues;
	IloEnvHashTable<const char *, OffsetAndLineNumber *>  _csvLineOffsetAndNumberHashTable;
	IloBool						_duplicatedLine;

	IloBool addLineOffset(const char *, OffsetAndLineNumber *);
	IloBool stockLineOffsets(FILE *, char *, const char *);
public :
	IloCachedCsvTableReaderI(IloEnv env);
	IloCachedCsvTableReaderI(IloCsvReaderI *, const char * name = 0, OffsetAndLineNumber * value = 0);
	virtual ~IloCachedCsvTableReaderI();
	virtual IloCsvLineI * getLineByNumber(IloInt) ILO_OVERRIDE;
	virtual IloCsvLineI * getLineByKey(const char *) ILO_OVERRIDE;
	virtual IloInt getNumberOfItems() ILO_OVERRIDE;
	const char * getLineKey(IloInt i) const { return _arrayOfKeysValues[i]; }
	virtual OffsetAndLineNumber * getOffsetAndLineNumber(const char *) const ILO_OVERRIDE;
	virtual void copy(IloCsvTableReaderI *) ILO_OVERRIDE;
	IloBool getDuplicatedLine() const { return _duplicatedLine; }
	void clearCachedTable();
	virtual void clearTable() ILO_OVERRIDE;
	class CachedCsvLineIteratorI {
		FILE *                        _itfile;
		IloCachedCsvTableReaderI *    _csv;
		char *                        _itbuffer;
		IloCsvLineI *                 _currentLine;
		IloBool                       _ok;
		IloInt                        _index;
		char **                       _fieldsBlock;
		IloInt                        _size;
		IloInt                        _offset;
		IloEnvI *                     _env;

		IloCachedCsvTableReaderI * getCsv() const { return _csv; }
		IloInt getIndex() const { return _index; }
		IloInt getSize() const { return _size; }
		void copy(const CachedCsvLineIteratorI& csvIterator);
	public:
		CachedCsvLineIteratorI()
			: _csv(0), _itbuffer(0), _currentLine(0), _ok(0), _index(0), _fieldsBlock(0), _size(0), _offset(0), _env(0) {}
		CachedCsvLineIteratorI(IloCachedCsvTableReaderI * csv);
		CachedCsvLineIteratorI(const CachedCsvLineIteratorI& csv);
		virtual ~CachedCsvLineIteratorI();
		void clear();
		void operator=(const CachedCsvLineIteratorI& csvIterator);
		virtual void initIterator();
		virtual void operator++();
		IloCsvLineI * operator*() const { return _currentLine; }
		IloBool ok() const { return _ok; }
	};
	friend class CachedCsvLineIteratorI;
};

class IloNonCachedCsvTableReaderI :public IloCsvTableReaderI {
public :
	IloNonCachedCsvTableReaderI(IloEnv env);
	IloNonCachedCsvTableReaderI(IloCsvReaderI *, const char * name = 0, OffsetAndLineNumber * value = 0);
	virtual ~IloNonCachedCsvTableReaderI();
	void initNonCachedCsv(FILE *, char *, IloInt);
	virtual IloCsvLineI * getLineByNumber(IloInt i) ILO_OVERRIDE;
	virtual IloCsvLineI * getLineByKey(const char *) ILO_OVERRIDE;
	virtual IloInt getNumberOfItems() ILO_OVERRIDE;
	const char * getFileName() const { return _csv->getFileName(); }
	virtual OffsetAndLineNumber * getOffsetAndLineNumber(const char * name) const ILO_OVERRIDE;
	virtual void copy(IloCsvTableReaderI *) ILO_OVERRIDE;
	virtual void clearTable() ILO_OVERRIDE;
	class NonCachedCsvLineIteratorI {
		FILE *                           _itfile;
		IloNonCachedCsvTableReaderI *    _csv;
		char *                           _itbuffer;
		IloCsvLineI *                    _currentLine;
		IloBool                          _ok;
		IloInt                           _index;
		char **                          _fieldsBlock;
		IloInt                           _endOffset;
		IloInt                           _offset;
		IloEnvI *                       _env;

		IloNonCachedCsvTableReaderI * getCsv() const { return _csv; }
		IloInt getIndex() const { return _index; }
		IloInt getEndOffset() const { return _endOffset; }
		IloInt getOffset() const { return _offset; }
		void copy(const NonCachedCsvLineIteratorI& csvIterator);
	public:
		NonCachedCsvLineIteratorI()
			: _csv(0), _itbuffer(NULL), _currentLine(0), _ok(0), _index(0), _fieldsBlock(0), _endOffset(0), _offset(0), _env(0) {}
		NonCachedCsvLineIteratorI(IloNonCachedCsvTableReaderI * csv);
		NonCachedCsvLineIteratorI(const NonCachedCsvLineIteratorI& csv);
		virtual ~NonCachedCsvLineIteratorI();
		void clear();
		void operator=(const NonCachedCsvLineIteratorI& csvIterator);
		virtual void initIterator();
		virtual void operator++();
		IloCsvLineI * operator*() const { return _currentLine; }
		IloBool ok() const { return _ok; }
	};
	friend class NonCachedCsvLineIteratorI;
};

class IloCsvTableReader {

	IloCsvTableReaderI * _impl;
	IloCsvLine           _currentLine;
public :
	
	IloCsvTableReader(): _impl(0) {}
	
	IloCsvTableReader(IloCsvTableReaderI* impl): _impl(impl) {}
	
	IloCsvTableReader(const IloCsvTableReader& csv): _impl(csv._impl), _currentLine(csv.getCurrentLine()) {}
	
	IloCsvTableReader(IloCsvReaderI * csvReaderImpl, const char * name = 0);
	~IloCsvTableReader();
	
	void end() {
		if(_impl !=0 ) delete _impl;
		_impl = 0;
		if(_currentLine.getImpl() != 0)
			_currentLine.end();
	}
	
	IloCsvTableReaderI * getImpl() const { return _impl; }
	
	void operator=(const IloCsvTableReader& csv) { _impl = csv._impl; }
	
	IloEnv getEnv() const;
	
	const char * getNameOfTable() const;
	
	IloCsvLine  getLineByNumber(IloInt i);
	
#ifndef ILO_ZOS
	IloCsvLine  getLineByKey(IloInt numberOfKeys, const char *, ...);
#else
	IloCsvLine  getLineByKey(IloInt numberOfKeys, const char *);
	IloCsvLine  getLineByKey(IloInt numberOfKeys, const char *, const char *);
	IloCsvLine  getLineByKey(IloInt numberOfKeys, const char *, const char *, const char *);
	IloCsvLine  getLineByKey(IloInt numberOfKeys, const char *, const char *, const char *, const char *);
	IloCsvLine  getLineByKey(IloInt numberOfKeys, const char *, const char *, const char *, const char *, const char *);
#endif 
	
	IloInt getNumberOfItems();
	
	IloInt getNumberOfColumns();
	
	IloInt getNumberOfKeys() const;
	
	IloBool isHeadingExists(const char * headingName) const;
	
	IloBool isCached() const;
	
	IloBool printKeys() const;

	void copy(IloCsvTableReader);
	

	IloCsvLine getCurrentLine() const { return _currentLine; }
	
	IloInt getPosition(const char *) const;

	IloCsvLine getFirstLine() const;
	

	class LineIterator {
		IloCachedCsvTableReaderI::CachedCsvLineIteratorI       _itCached;
		IloNonCachedCsvTableReaderI::NonCachedCsvLineIteratorI _itNonCached;
		IloCsvTableReaderI *                                   _csv;
		IloBool _isCached;

		IloBool isCached() const { return _isCached; }
		IloCsvTableReaderI * getReader() const { return _csv; }
	public:
		
		LineIterator() : _itCached(), _itNonCached(), _isCached(0) { }
		
		LineIterator(IloCsvTableReader csv);
		~LineIterator() { }
		
		void end();
		
		LineIterator& operator++();
		
		IloCsvLine operator*() const;
		
		IloBool ok() const;
		void operator=(const LineIterator& lineIterator);
	};
	friend class LineIterator;
};

class IloCsvReader {
public :
	enum Status { Cached, NonCached };
private :
	IloCsvReaderI*    _impl;
	Status            _status;
	IloCsvLine        _currentLine;

	void testFile(IloEnv, const char *) const;
	void inclusionTest(IloEnv, const char, const char *, const char *) const;
	void testCharacter(IloEnv, const char, const char *, const char, const char *) const;
	void testParameters(IloEnv, const char *, const char, const char, const char) const;
	void makeTests(IloEnv, const char *, const char *, const char,
		const char, const char) const;
	void createImpl(IloEnv, const char *, IloBool, IloBool, IloBool,
		const char *, const char , const char, const char);
public :
	
	IloCsvReader(): _impl(0) {}
	
	IloCsvReader(IloCsvReaderI* impl): _impl(impl) {}
	
	IloCsvReader(const IloCsvReader& csv): _impl(csv._impl), _status(csv.getStatus()) {}
	
	IloCsvReader(IloEnv env,
		const char * problem,
		IloBool multiTable = IloFalse,
		IloBool allowTableSplitting = IloFalse,
		const char * separator = ",;\t",
		const char decimalp = '.',
		const char quote = '\"',
		const char comment = '#',
		IloBool isCached = IloFalse);
	~IloCsvReader();
	

	void end() {
		if(_impl !=0 ) delete _impl;
		_impl = 0;
		if(_currentLine.getImpl() != 0)
			_currentLine.end();
	}
	
	IloCsvReaderI* getImpl() const { return _impl; }
	
	void operator=(const IloCsvReader& csv) { _impl = csv._impl;_status = csv.getStatus(); }
	
	IloEnv getEnv() const;
	
	IloCsvTableReader  getTableByNumber(IloInt i);
	
	IloCsvTableReader  getTableByName(const char * name);
	
	IloCsvTableReader  getTable();
	
	IloCsvLine getLineByNumber(IloInt i);
	
#ifndef ILO_ZOS
	IloCsvLine getLineByKey(IloInt numberOfKeys, const char *, ...);
#else
	IloCsvLine getLineByKey(IloInt numberOfKeys, const char *);
	IloCsvLine getLineByKey(IloInt numberOfKeys, const char *, const char *);
	IloCsvLine getLineByKey(IloInt numberOfKeys, const char *, const char *, const char *);
	IloCsvLine getLineByKey(IloInt numberOfKeys, const char *, const char *, const char *, const char *);
	IloCsvLine getLineByKey(IloInt numberOfKeys, const char *, const char *, const char *, const char *, const char *);
#endif 
	
	IloInt getNumberOfItems();
	
	IloInt getNumberOfColumns();
	IloCsvLine getFirstLine() const;
	
	IloBool isHeadingExists(const char * headingName) const;
	IloBool isTableExists(const char * tableName) const;
	
	IloInt getNumberOfKeys() const;
	
	IloBool printKeys() const;
	
	IloInt getPosition(const char * headingName) const;
	
	Status getStatus() const { return _status; }
	
	IloInt getNumberOfTables();
	
	IloNum getCsvFormat();
	
	const char * getRequiredBy();
	
	IloNum getFileVersion();
	
	IloCsvTableReader getReaderForUniqueTableFile() const;
	

	IloCsvLine getCurrentLine() const { return _currentLine; }
	
	class TableIterator {
		IloCsvReaderI::CsvTableIteratorI _it;
	public:
		
		TableIterator(IloCsvReader csv);
		~TableIterator() { }
		void end();
		
		TableIterator& operator++();
		
		IloCsvTableReader operator*() const;
		
		IloBool ok() const;
	};
	
	class LineIterator {
		IloCsvTableReader::LineIterator   _it;
	public:
		
		LineIterator() : _it() { }
		
		LineIterator(IloCsvReader csv);
		~LineIterator() { }
		void end();
		
		LineIterator& operator++();
		
		IloCsvLine operator*() const;
		
		IloBool ok() const;
	};
	class IloCsvReaderException : public IloQuietException {
	public:
		IloCsvReaderException() : IloQuietException("Csv Reader Exception") {}
		IloCsvReaderException(const char * s, IloEnvI * env = 0, IloInt mem = 0)
			: IloQuietException(s, IloTrue) {
				if(mem != 0)
					env->free((char *) s, mem);
		}
		~IloCsvReaderException() { }
	};

	
	class IloFileNotFoundException : public IloCsvReaderException {
	public:
		IloFileNotFoundException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloFileNotFoundException() { }
	};

	
	class IloFieldNotFoundException : public IloCsvReaderException {
	public:
		IloFieldNotFoundException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloFieldNotFoundException() { }
	};

	
	class IloColumnHeaderNotFoundException : public IloCsvReaderException {
	public:
		IloColumnHeaderNotFoundException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloColumnHeaderNotFoundException() { }
	};

	
	class IloCsvReaderParameterException : public IloCsvReaderException {
	public:
		IloCsvReaderParameterException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloCsvReaderParameterException() { }
	};

	
	class IloDuplicatedTableException : public IloCsvReaderException {
	public:
		IloDuplicatedTableException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloDuplicatedTableException() { }
	};

	
	class IloTableNotFoundException : public IloCsvReaderException {
	public:
		IloTableNotFoundException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloTableNotFoundException() { }
	};

	
	class IloIncorrectCsvReaderUseException : public IloCsvReaderException {
	public:
		IloIncorrectCsvReaderUseException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloIncorrectCsvReaderUseException() { }
	};

	
	class IloLineNotFoundException : public IloCsvReaderException {
	public:
		IloLineNotFoundException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloLineNotFoundException() { }
	};

	class IloDuplicatedLineException : public IloCsvReaderException {
	public:
		IloDuplicatedLineException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloDuplicatedLineException() { }
	};

	class IloOverflowException : public IloCsvReaderException {
	public:
		IloOverflowException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloOverflowException() { }
	};

	class IloNumberOfFieldsException : public IloCsvReaderException {
	public:
		IloNumberOfFieldsException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloNumberOfFieldsException() { }
	};

	class IloEmptyTableNameException : public IloCsvReaderException {
	public:
		IloEmptyTableNameException(const char *s, IloEnvI * env = 0, IloInt mem = 0)
			: IloCsvReaderException(s, env, mem) {}
		~IloEmptyTableNameException() { }
	};
	friend class LineIterator;
	friend class TableIterator;
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
