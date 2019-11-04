#ifndef HANDLE_HPP
#define HANDLE_HPP

template<class T>
class jptr {
public:
	T* p;
	int *pcount;

	jptr(T* w) :pcount(new int(1)), p(w) {}
	jptr() :pcount(new int(1)), p(nullptr) {}
	jptr(const jptr& hw)
	{
		p = hw.p;
		pcount = hw.pcount;
		++(*hw.pcount);
	}
	template<class t>
	jptr& operator=(const t& hw)
	{
		++*hw.pcount;
		decrs();
		p = hw.p;
		pcount = hw.pcount;
		return *this;
	}
	T *operator->()const
	{
		return p;
	}
	int count()
	{
		return *pcount;
	}
	template<class t>
	bool operator==(jptr<t> jp)
	{
		return jp->p == p;
	}
	T operator*()const
	{
		return *p;
	}
	template<class t>
	operator jptr<t>()
	{
		jptr<t> tmp(dynamic_cast<t*>(p));
		tmp.pcount = pcount;
		++*pcount;
		return tmp;
	}
	~jptr()
	{
		decrs();
	}
private:
	void decrs()
	{
		
		--*pcount;
		if (*pcount == 0) {
			delete pcount;
			if(p)
				delete p;
		}
	}
};

#endif