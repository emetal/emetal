/******************************************************************************
* $Id: orsRefPtr.h 2008/10/05 10:41 JWS $
*
* Project:  OpenRS
* Purpose:  �����ü���������ָ�룬���Է���stl����
*
* Author:   GUO Wei, JIANG Wanshou, jws@lmars.whu.edu.cn
******************************************************************************
* Copyright (c) 2008, Liesmars, Wuhan University
* History:
*      
****************************************************************************/
#ifndef OPENRS_SHARE_PTR_H
#define OPENRS_SHARE_PTR_H

//
// ʹ��Ҫ�󣬱���װ����_T�������addRef(), release()�ӿڻ򷽷�
//	��OpenRS���棬��_T ���Լ̳� orsIObject
//
// ʹ�÷��������ڳ��ڳ���һ�����󣬿��ƶ������������
//
//

template<typename _T> 
class ref_ptr
{
public:
	typedef _T element_type;

	ref_ptr() :m_ptr(NULL) {}
	ref_ptr(_T* _T1):m_ptr(_T1)              
	{ 
		if ( NULL != m_ptr ) 
			m_ptr->addRef(); 
	}

	ref_ptr(const ref_ptr& rp):m_ptr(rp.m_ptr)  
	{ 
		if (NULL != m_ptr) 
			m_ptr->addRef();
	}

	~ref_ptr()                           
	{ 
		if (NULL != m_ptr) 
			m_ptr->release(); 
		m_ptr=0; 
	}

	inline ref_ptr& operator = (const ref_ptr& rp)
	{
		if (m_ptr==rp.m_ptr) 
			return *this;

		_T* tmpPtr = m_ptr;

		m_ptr = rp.m_ptr;
		if (NULL != m_ptr) 
			m_ptr->addRef();

		if (NULL != tmpPtr) 
			tmpPtr->release();

		return *this;
	}

	inline ref_ptr& operator = (_T* ptr)
	{
		if (m_ptr==ptr) 
			return *this;

		_T* tmpPtr = m_ptr;
		m_ptr = ptr;

		if (NULL != m_ptr) 
			m_ptr->addRef();
		if (NULL != tmpPtr) 
			tmpPtr->release();

		return *this;
	}

	// comparison operators for orsRefPtr.
	inline bool operator == (const ref_ptr& rp) const { return (m_ptr==rp.m_ptr); }
	inline bool operator != (const ref_ptr& rp) const { return (m_ptr!=rp.m_ptr); }
	inline bool operator < (const ref_ptr& rp) const { return (m_ptr<rp.m_ptr); }
	inline bool operator > (const ref_ptr& rp) const { return (m_ptr>rp.m_ptr); }

	// comparion operator for const _T*.
	inline bool operator == (const _T* ptr) const { return (m_ptr==ptr); }
	inline bool operator != (const _T* ptr) const { return (m_ptr!=ptr); }
	inline bool operator < (const _T* ptr) const { return (m_ptr<ptr); }
	inline bool operator > (const _T* ptr) const { return (m_ptr>ptr); }


	inline _T& operator*()  { return *m_ptr; }

	inline const _T& operator*() const { return *m_ptr; }

	inline _T* operator->() { return m_ptr; }

	inline const _T* operator->() const   { return m_ptr; }

	inline bool operator!() const	{ return NULL == m_ptr; }

	inline bool valid() const	{ return NULL != m_ptr; }

	inline _T* get() { return m_ptr; }

	inline const _T* get() const { return m_ptr; }

private:
	_T* m_ptr;
};

#endif



