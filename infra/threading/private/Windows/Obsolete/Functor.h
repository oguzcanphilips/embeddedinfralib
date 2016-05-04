#ifndef FUNCTOR_H
#define FUNCTOR_H

class FunctorImpl
{
public:
	virtual void Execute() = 0;
	void AddRef()
	{
		m_RefCount++;
	}
	void ReleaseRef()
	{
		m_RefCount--;
		if(m_RefCount==0)
		{
			delete this;
		}
	}
protected:
	FunctorImpl()
	: m_RefCount(1)
	{}
	virtual ~FunctorImpl(){}
private:
	unsigned long m_RefCount;	

	FunctorImpl(const FunctorImpl&); // declare only
	const FunctorImpl& operator=(const FunctorImpl&); // declare only
};

class FunctorC0 : public FunctorImpl
{
	friend class Functor;
private:
	typedef void(*FuncPtr)();
	FunctorC0(FuncPtr func)
		: m_FuncPtr(func)
	{
	}
	void Execute()
	{
		(*m_FuncPtr)();
	}
	FuncPtr m_FuncPtr;
};

template<typename A1>
class FunctorC1 : public FunctorImpl
{
	friend class Functor;
private:
	typedef void(*FuncPtr)(A1);
	FunctorC1(FuncPtr func, A1 arg1)
		: m_FuncPtr(func), m_Arg1(arg1)
	{
	}
	void Execute()
	{
		(*m_FuncPtr)(m_Arg1);
	}
	FuncPtr m_FuncPtr;
	A1      m_Arg1;
};

template<typename T>
class Functor0 : public FunctorImpl
{
	friend class Functor;
private:
	typedef void(T::*FuncPtr)(void);
	Functor0(T* obj, FuncPtr func)
	: m_Obj(obj), m_FuncPtr(func)
	{
	}
	void Execute()
	{
		(m_Obj->*m_FuncPtr)();
	}
	FuncPtr m_FuncPtr;
	T*      m_Obj;
};

template<typename T, typename A1>
class Functor1 : public FunctorImpl
{
	friend class Functor;
private:
	typedef void(T::*FuncPtr)(A1);
	Functor1(T* obj, FuncPtr func, A1 arg1)
		: m_Obj(obj), m_FuncPtr(func), m_Arg1(arg1)
	{
	}
	void Execute()
	{
		(m_Obj->*m_FuncPtr)(m_Arg1);
	}
	FuncPtr m_FuncPtr;
	T*      m_Obj;
	A1      m_Arg1;
};

class Functor 
{
public:
	Functor(void(*func)())
		: m_Impl(new FunctorC0(func))
	{}

	template<typename A1>
	Functor(void(*func)(A1), A1 arg1)
		: m_Impl(new FunctorC1<A1>(func, arg1))
	{}

	template<typename T>
	Functor(T* obj, void(T::*func)())
		: m_Impl(new Functor0<T>(obj, func))
	{}

	template<typename T, typename A1>
	Functor(T* obj, void(T::*func)(A1), A1 arg1)
		: m_Impl(new Functor1<T,A1>(obj, func, arg1))
	{}

	~Functor()
	{
		m_Impl->ReleaseRef();
	}
	void Execute()
	{
		m_Impl->Execute();
	}

	Functor(const Functor& functor)
	: m_Impl(functor.m_Impl)
	{
		m_Impl->AddRef();
	}
	const Functor& operator=(const Functor& functor)
	{
		if(m_Impl != functor.m_Impl)
		{
			m_Impl->ReleaseRef();
			m_Impl = functor.m_Impl;
			m_Impl->AddRef();
		}
	}
private:	
	FunctorImpl* m_Impl;
};

#endif
