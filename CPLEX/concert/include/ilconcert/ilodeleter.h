// -------------------------------------------------------------- -*- C++ -*-
// File: ./include/ilconcert/ilodeleter.h
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
#ifndef __CONCERT_ilodeleterH
#define __CONCERT_ilodeleterH

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#include <ilconcert/iloexpression.h>

class IloDeleterI;

class IloDeleterI : public IloBaseDeleterI {
public:
	class InitVisitor;
	class DeleteVisitor;
	class ChangeVisitor;
private:
	friend class IloEnvI;
	friend class InitVisitor;
	friend class DeleteVisitor;
	friend class ChangeVisitor;
	IloInt _verbosityLevel;
	IloDeleterMode _mode;
	IloBool _inhibit;
public:
	enum Status {
		NotDeletable = -1,
		BeingDeleted = 0,
		Deletable = 1,
		Marked = 2
	};

	class Item {
		IloExtractableI* _extractable;
	public:
		Item* _next;
		Item(IloExtractableI* extractable, Item* next=0) :
		_extractable(extractable), _next(next) {}
		IloExtractableI* getExtractable() const {return _extractable;}
		void setExtractable(IloExtractableI* ext) { _extractable = ext; }
	};

	
	class Node {
		Status _status; 
		
		
		Item* _first;
	public:
		Node(IloEnvI*, Status status = NotDeletable);
		~Node();
		IloBool contains(IloExtractableI* user);
		void setStatus(Status b) { _status = b; }
		Status getStatus() const { return _status; }
		Item* getFirst() const { return _first; }
		Item** getFirstPtr() { return &_first; }
		void setFirst(Item* item) { _first = item; }
	};

	class InitVisitor : public IloExtractableVisitor {
		friend class IloDeleterI;
		IloDeleterI* _deleter;
	public:
		InitVisitor(IloDeleterI* deleter) : IloExtractableVisitor(),
			_deleter(deleter) {}
		virtual void visitChildren(IloExtractableI* f, IloExtractableI* c) ILO_OVERRIDE;
	};
	class DeleteVisitor : public IloExtractableVisitor {
		IloDeleterI* _deleter;
	public:
		DeleteVisitor(IloDeleterI* deleter) : IloExtractableVisitor(),
			_deleter(deleter) {}
		virtual void visitChildren(IloExtractableI* f, IloExtractableI* c) ILO_OVERRIDE;
	};
	class ChangeVisitor : public IloChangeVisitor {
		IloDeleterI* _deleter;
	public:
		ChangeVisitor(IloDeleterI* deleter) : IloChangeVisitor(),
			_deleter(deleter) {}
		virtual void addSubExtractable(IloExtractableI* f, IloExtractableI* c) ILO_OVERRIDE;
		virtual void addSubExtractable(IloExtractableI* f, IloExtractableArray c);
		virtual void removeSubExtractable(IloExtractableI* f, IloExtractableI* c) ILO_OVERRIDE;
		virtual void removeSubExtractable(IloExtractableI* f, IloExtractableArray c);
	};

private:
	InitVisitor* _initVisitor;
	DeleteVisitor* _deleteVisitor;
	ChangeVisitor* _changeVisitor;
	IloArray<IloAny> _infos;
	void checkInfosBigEnough(IloTypeIndex index) {
		if (_infos.getSize()<=index)
			_infos.add(1+index-_infos.getSize(), 0);
	}

	IloArray<IloAny> _nodes;
	Item* _freeItems;

	IloBool isNotDeletable(IloInt id) {
		Node* node = (Node*)_nodes[id];
		return !node || (node->getStatus() == NotDeletable);
	}
	IloBool isBeingDeleted(IloInt id) {
		Node* node = (Node*)_nodes[id];
		return node && (node->getStatus() == BeingDeleted);
	}
public:

	void setStatus(IloInt id, Status status) {
		((Node*)_nodes[id])->setStatus(status);
	}
	void removeFromAll(IloInt id) ILO_OVERRIDE;
	void dumpNodes(IloInt id=-1) const;

private:
	IloBool isExtracted(IloExtractableI*);
	void checkNodesSize(IloInt id);
	virtual IloBool hasLink(IloExtractableI* user, IloExtractableI* object) ILO_OVERRIDE;
	virtual void addLink(IloExtractableI* user, IloExtractableI* object) ILO_OVERRIDE;
	virtual void removeLink(IloExtractableI* user, IloExtractableI* object) ILO_OVERRIDE;

	
	void synchronize();
	void extract(IloExtractableI*);

	
	IloExtractableArray _toBeExtracted;

	
	IloExtractableArray _toBeDestroyed;
	IloInt _rec;

	
	void addToBeExtracted(const IloExtractable extractable) ILO_OVERRIDE;
	void removeToBeExtracted(const IloExtractable extractable) ILO_OVERRIDE;

	
	virtual void apply(const IloChange& chg, const IloExtractableI* obj) ILO_OVERRIDE;
	void clearBeforeException();
	void clearFreeList();

public:
	IloDeleterI(IloEnvI*, IloDeleterMode mode = IloSafeDeleterMode);
	virtual ~IloDeleterI();
	IloInt getRec() const { return _rec; }

	IloEnvI* getEnv() const {return _env;}

	

	void end(IloExtractableI*, IloBool inDelete) ILO_OVERRIDE;
	void fastEnd(IloExtractableI*) ILO_OVERRIDE;
	void end(const IloExtractableArray, IloBool inDelete) ILO_OVERRIDE;
	void fillBadUser(IloExtractableI* object,
		IloExtractableI* user,
		IloExtractableArray badUsers,
		IloExtractableArray badObjects);
	void setVerbosityLevel(IloInt level) ILO_OVERRIDE {_verbosityLevel = level;}
	IloBool getVerbosityLevel() const {return _verbosityLevel;}

	IloDeleterMode getMode() const {return _mode;}
	void setMode(IloDeleterMode mode) {_mode=mode;}

	void inhibit(IloBool b) ILO_OVERRIDE {_inhibit = b;}

};

class IloDeleter {
	IloDeleterI* _impl;

public:
	IloDeleter(IloDeleterI* impl=0) : _impl(impl) {}
	IloDeleter(const IloEnv env, IloDeleterMode mode = IloSafeDeleterMode);
	void end() const;
	void end(const IloExtractable extractable) const {
		IloAssert(_impl, "Using empty IloDeleter handle.");
		IloAssert(extractable.getImpl(), "Using empty IloExtractable handle.");
		_impl->end(extractable.getImpl(), IloFalse);
	}
	void end(const IloExtractableArray extractables) const {
		IloAssert(_impl, "Using empty IloDeleter handle.");
		IloAssert(extractables.getImpl(), "Using empty IloExtractableArray handle.");
		_impl->end(extractables, IloFalse);
	}
	enum VerbosityLevel {
		None = 0,
		All = 15
	};
	void setMode(IloDeleterMode mode) const {
		IloAssert(_impl, "Using empty IloDeleter handle.");
		_impl->setMode(mode);
	}

	void setVerbosityLevel(IloInt level) const {
		IloAssert(_impl, "Using empty IloDeleter handle.");
		_impl->setVerbosityLevel(level);
	}
	IloInt getVerbosityLevel() const {
		IloAssert(_impl, "Using empty IloDeleter handle.");
		return _impl->getVerbosityLevel();
	}

	class NotRegisteredException : public IloException {
           void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	public:
		NotRegisteredException(const IloExtractable);
		NotRegisteredException(const IloChange&);
	};

	class NotDeletableException : public IloException {
		IloExtractable _extractable;
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	public:
		NotDeletableException(const IloExtractable);
	};

	class RequiresAnotherDeletionException : public IloException {
		IloExtractableArray _users;
		IloExtractableArray _objects;
		void print(ILOSTD(ostream)& out) const ILO_OVERRIDE;
	public:
		RequiresAnotherDeletionException(const IloExtractableArray, const IloExtractableArray);
		IloExtractableArray getUsers() const {return _users;}
		const IloExtractableArray getObjects() const {return _objects;}
		void end() ILO_OVERRIDE;
	};

	class BadModeMixException : public IloException {
	public:
		BadModeMixException();
	};
};

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif 
