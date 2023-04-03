// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilortti.h
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

#ifndef __CONCERT_ilorttiH
#define __CONCERT_ilorttiH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

class ILO_EXPORTED IloRtti {
public:
	typedef IloInt TypeIndex;
	typedef void (*TypeInfo)();
private:
	static TypeIndex _counter;
protected:
	static TypeIndex getNewRtti() {
		return ++_counter;
	}
public:
	
	virtual ~IloRtti();

	
	static  TypeIndex    GetTypeIndex();
	
	virtual TypeIndex    getTypeIndex() const;

	
	static  TypeInfo    GetTypeInfo();
	
	static  TypeInfo    GetBaseTypeInfo();
	
	virtual TypeInfo    getTypeInfo() const;
	
	virtual IloBool     isType (TypeInfo typeinfo) const;
	
	virtual const char* getRttiTypeName() const = 0;

        virtual TypeInfo getBaseTypeInfo() const = 0;
        virtual TypeIndex getBaseTypeIndex() const = 0;
};

typedef IloRtti::TypeIndex IloTypeIndex;
typedef IloRtti::TypeInfo IloTypeInfo;

IloTypeInfo IloGetBaseTypeInfo(IloTypeInfo);

# define ILORTTIDECL                                                    \
	static IloTypeIndex _rttiIndex;                                       \
public:                                                                 \
	                                                                   \
	static  IloTypeInfo GetTypeInfo();                                    \
	                                                                   \
	static  IloTypeInfo GetBaseTypeInfo();                                \
	                                                                   \
	virtual IloTypeInfo getTypeInfo() const ILO_OVERRIDE;                              \
	                                                                   \
	virtual IloTypeInfo getBaseTypeInfo() const ILO_OVERRIDE;   \
	                                                                   \
	virtual IloBool     isType(IloTypeInfo typeinfo) const ILO_OVERRIDE;               \
	virtual IloTypeIndex getTypeIndex() const ILO_OVERRIDE;                            \
	virtual IloTypeIndex getBaseTypeIndex() const ILO_OVERRIDE; \
	static  IloTypeIndex GetTypeIndex();                                  \
	static  void        InitTypeIndex();                                  \
	                                                                   \
	virtual const char* getRttiTypeName() const ILO_OVERRIDE;                          \
	                                                                   \
	static  const char* GetRttiTypeName();                                \

#ifdef ILO_MSVC
# define ILODUMMYRTTI(_this)\
	if (_rttiIndex == _rttiIndex - 1)\
	ILOSTD(cout) << (_this);
#else
# define ILODUMMYRTTI(_this)
#endif

#define ILONOTEMPLATE()
#define ILOGENTEMPLATE_1(a)	template < a >
#define ILOGENTEMPLATE_2(a,b)	template < a , b >
#define ILOGENTEMPLATE_3(a,b,c) template < a , b , c >

#define ILORTTIN(_thisMacro, _this, _baseMacro, _base, _templateMacro, _template) \
	_templateMacro _template \
	IloTypeInfo _thisMacro _this::GetBaseTypeInfo() { \
	ILODUMMYRTTI(ILO_STRINGIZE(_thisMacro _this))\
	const IloRtti::TypeInfo type = _baseMacro _base::GetTypeInfo(); \
	return type; \
} \
	_templateMacro _template \
	IloTypeInfo _thisMacro _this::GetTypeInfo() { \
	ILODUMMYRTTI(ILO_STRINGIZE(_thisMacro _this))\
	return (IloRtti::TypeInfo(&_thisMacro _this::GetBaseTypeInfo));\
} \
	_templateMacro _template \
	IloTypeInfo _thisMacro _this::getTypeInfo() const { \
	return (IloRtti::TypeInfo(&_thisMacro _this::GetBaseTypeInfo)); \
} \
	_templateMacro _template                            \
	IloTypeInfo _thisMacro _this::getBaseTypeInfo() const {         \
           const IloRtti::TypeInfo type = _baseMacro _base::GetTypeInfo(); \
           return type;                                                 \
        }                                                               \
	_templateMacro _template \
	IloBool _thisMacro _this::isType (IloTypeInfo typeinfo) const { \
	return (typeinfo == GetTypeInfo()  || \
	_baseMacro _base::isType (typeinfo)  ); \
} \
	_templateMacro _template \
	IloTypeIndex _thisMacro _this::_rttiIndex = 0; \
	_templateMacro _template \
	IloTypeIndex _thisMacro _this::getTypeIndex() const { \
	return _rttiIndex; \
} \
	_templateMacro _template                              \
	IloTypeIndex _thisMacro _this::getBaseTypeIndex() const {   \
	return _baseMacro _base::GetTypeIndex();                    \
        }                                                           \
	_templateMacro _template \
	void _thisMacro _this::InitTypeIndex() { \
	if (_rttiIndex == 0) { \
	IloEnvI::Lock(); \
	if (_rttiIndex == 0) { \
	_rttiIndex = IloRtti::getNewRtti(); \
	} \
	IloEnvI::Unlock(); \
	} \
} \
	_templateMacro _template \
	IloTypeIndex _thisMacro _this::GetTypeIndex() { \
	InitTypeIndex(); \
	return _rttiIndex; \
} \
	_templateMacro _template \
	const char* _thisMacro _this::getRttiTypeName() const { \
	return name2(_thisMacro,_STRINGIZE) _this; \
} \
	_templateMacro _template \
	const char* _thisMacro _this::GetRttiTypeName() { \
	return name2(_thisMacro,_STRINGIZE) _this; \
}

#define ILOEMPTYMACROARGUMENT
#define ILOSIMPLECLASS(X)	X
#define ILOTEMPLATECLASS_1(X,Y) X < Y >
#define ILOTEMPLATECLASS_2(X,Y,Z) X < Y , Z >
#define ILOTEMPLATECLASS_3(X,Y,Z,K) X < Y, Z, K >

#define ILOSIMPLECLASS_STRINGIZE(X)	#X
#define ILOTEMPLATECLASS_1_STRINGIZE(X,Y) #X "<" #Y ">"
#define ILOTEMPLATECLASS_2_STRINGIZE(X,Y,Z) #X "<" #Y "," #Z ">"
#define ILOTEMPLATECLASS_3_STRINGIZE(X,Y,Z,K) #X "<" #Y "," #Z "," #K ">"

#define ILORTTI1(_this, _base, _t1)  \
	ILORTTIN(ILOSIMPLECLASS,(_this),ILOSIMPLECLASS,(_base),ILOGENTEMPLATE_1,(_t1))

#define ILORTTI2(_this,_base,_t1,_t2) \
	ILORTTIN(ILOSIMPLECLASS,(_this),ILOSIMPLECLASS,(_base),ILOGENTEMPLATE_2,(_t1,_t2))

#define ILORTTI3(_this,_base,_t1,_t2,t3) \
	ILORTTIN(ILOSIMPLECLASS,(_this),ILOSIMPLECLASS,(_base),ILOGENTEMPLATE_3,(_t1,_t2,t3))

#define ILORTTI( _this, _base )													\
	ILORTTIN(ILOSIMPLECLASS,( _this ),ILOSIMPLECLASS,( _base ),ILONOTEMPLATE,())

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif
