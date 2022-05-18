#ifndef OPENRS_ARRAY_H
#define OPENRS_ARRAY_H

#define my_max(a,b)    (((a) > (b)) ? (a) : (b))
#define my_min(a,b)    (((a) < (b)) ? (a) : (b))

template< typename T >
inline void ors_data_swap(T& lhs, T& rhs)
{
	T temp = lhs;
	lhs = rhs;
	rhs = temp;
}

#if defined(kyLINUX64) // || defined(LINUX64)
#include <vector>
using namespace std;
#define orsArray std::vector

#else

#include "orsAllocator.h"


template< typename T,typename Allocator = orsAllocator<T> >
class orsArray
{
public:
    typedef  orsArray<T, Allocator>  self;
    typedef  Allocator             allocator_type;

    typedef  T                  value_type;
    typedef  value_type&        reference;
    typedef  const value_type&  const_reference;
    typedef  value_type*        pointer;
    typedef  const value_type*  const_pointer;
    typedef  unsigned long		size_type;
    typedef  int				difference_type;

    typedef  value_type*                       iterator;
    typedef  const value_type*                 const_iterator;

protected:
    pointer         m_start, m_finish, m_storage;
    allocator_type  m_alloc;

private:
	template< typename InputIterator, typename Integer, typename OutputIterator >
		inline  OutputIterator copy_n( InputIterator first, Integer count,
		OutputIterator result )
	{
		for( unsigned i = 0; i < count; ++i,++first,++result )
			*result = *first;
		return result;
	}

	template< typename InputIterator, typename Integer >
		inline Integer range_length( InputIterator first, InputIterator last,Integer length)
	{
		Integer len = 0;
		while( first != last )
		{
			++first;
			++len;
		}
		return len;
	}

	// 按赋值copy, 原对象会被析构
	template<class _II, class _OI>
		inline _OI copy(_II _F, _II _L, _OI _X)
	{
		for (; _F != _L; ++_X, ++_F)
			*_X = *_F;
		return (_X);
	}

	template< typename InputIterator, typename Integer >
		inline void advance( InputIterator& iter, Integer n )
	{
		while( n-- )
			++iter;
	}

	// 调用构造函数，类型可能不同, 原对象不会被析构
	template< typename T1, typename T2 >
		inline void construct(T1* ptr, const T2& value)
	{
		m_alloc.construct(ptr, value);
	}

	// 赋值填充，类型可能不同，原对象不会被析构
	template< typename ForwardIterator, typename T2 >
		void init_fill( ForwardIterator first, ForwardIterator last,
		const T2& value)
	{
		ForwardIterator current = first;
		try
		{
			for( ; current != last; ++current )
				construct( &(*current), value );
		}
		catch(...)
		{
			destroy( first, current );
			throw;
		}
	}

	// 填充拷贝
	template< typename forward_iterator, typename T1, typename input_iterator >
		inline forward_iterator init_fill_copy( forward_iterator result_first,
		forward_iterator result_middle,
		const T1& value,
		input_iterator first,
		input_iterator last )
	{
		init_fill( result_first, result_middle, value );
		try
		{
			return init_copy( first, last, result_middle );
		}
		catch(...)
		{
			destroy( result_first, result_middle );
			throw;
		}
	}

	// 奇怪的拷贝
	template< typename input_iterator1, typename input_iterator2,typename forward_iterator >
		inline forward_iterator init_copy_copy( input_iterator1 first1,
		input_iterator1 last1,
		input_iterator2 first2,
		input_iterator2 last2,
		forward_iterator result )
	{
		forward_iterator middle = init_copy( first1, last1, result );
		try
		{
			return init_copy( first2, last2, middle );
		}
		catch(...)
		{
			destroy( result, middle );
			throw;
		}
	}

	// 后向拷贝
	template<class _BI1, class _BI2> inline
		_BI2 copy_backward(_BI1 _F, _BI1 _L, _BI2 _X)
	{
		while (_F != _L)
			*--_X = *--_L;
		return (_X);
	}

	// 按赋值填充n个对象
	template<class _OI, class _Sz, class _Ty> inline
		void fill_n(_OI _F, _Sz _N, const _Ty& _X)
	{
		for (; 0 < _N; --_N, ++_F)
			*_F = _X;
	}

	// 按赋值填充一段对象
	template<class _FI, class _Ty>
		inline void fill(_FI _F, _FI _L, const _Ty& _X)
	{
		for (; _F != _L; ++_F)
			*_F = _X;
	}

	void destroy(iterator _F, iterator _L)
	{
		for (; _F != _L; ++_F)
			m_alloc.destroy(_F);
	}

	// 按构造函数初始化一段连续对象
	iterator init_copy(const_iterator _F, const_iterator _L, iterator _P)
	{
		for (; _F != _L; ++_P, ++_F)
			m_alloc.construct(_P, *_F);

		return (_P);
	}

	// 按构造函数初始化n个连续对象
	void init_fill_n(iterator _F, size_type _N, const T &_X)
	{
		for (; 0 < _N; --_N, ++_F)
			m_alloc.construct(_F, _X);
	}

public:
    orsArray() : m_start(NULL),
		m_finish(NULL),
		m_storage(NULL)  {}

    explicit orsArray( size_type size )
	{  fill_init( size, value_type() );  }

    orsArray( size_type size, const_reference value )
	{  fill_init( size, value );  }
    orsArray( int size, const_reference value )
	{  fill_init( static_cast<size_type>( size ), value );  }
    orsArray( long size, const_reference value )
	{  fill_init( static_cast<size_type>( size ), value );  }

    template< typename InputIterator >
		orsArray( InputIterator first, InputIterator last, size_type size = 0 )
    {
        range_init( first, last, size);
    }

    orsArray( const self& rhs )
    {
        alloc_data( rhs.size() );
        try
        {
            init_copy( rhs.begin(), rhs.end(), m_start );
        }
        catch(...)
        {
            dealloc_data();
            throw;
        }
        m_finish = m_start + rhs.size();
    }

    self& operator=( const self& rhs )
    {
        if( this != &rhs )
            assign( rhs.begin(), rhs.end(), rhs.size() );
        return *this;
    }

    ~orsArray()
    {
        destroy( begin(), end() );
        dealloc_data();
    }

    iterator begin()              {  return m_start;  }
    iterator end()                {  return m_finish;  }
    const_iterator begin() const  {  return m_start;  }
    const_iterator end() const    {  return m_finish;  }

    reference front()              {  return *begin();  }
    reference back()               {  return *(end() - 1);  }
    const_reference front() const  {  return *begin();  }
    const_reference back() const   {  return *(end() - 1);  }

    bool empty() const          {  return ( m_start == m_finish );  }
    size_type size() const      {  return ( m_finish - m_start );  }
    size_type space() const     {  return ( m_storage - m_finish );  }
    size_type capacity() const  {  return ( m_storage - m_start );  }
    size_type max_size() const  {  return size_t_max / sizeof(value_type);  }

    reference operator[]( size_type index )
	{  return m_start[index];  }
    const_reference operator[]( size_type index ) const
	{  return m_start[index];  }

    reference at( size_type index )
    {
        return m_start[index];
    }
    const_reference at( size_type index ) const
    {
        return m_start[index];
    }

    void push_back( const_reference value )
    {
        if( m_finish != m_storage )
        {
            construct( m_finish, value );
            ++m_finish;
        }
		else
		{
			insert_aux(end(), value);			
		}
    }

    void pop_back()
    {
        --m_finish;
        m_alloc.destroy( m_finish );
    }

    void clear()
    {
        destroy( begin(), end() );
        dealloc_data();
        m_start = NULL;
        m_finish = NULL;
        m_storage = NULL;
    }

    void reserve( size_type new_size )
    {
		if( capacity() < new_size )
        {
			if (0 == capacity()) {
				alloc_data(new_size);
			}
			else {
				self temp(begin(), end(), new_size);

				temp = *this;

				swap(temp);
			}
        }
    }

    void swap( self& rhs )
    {
        if( this != &rhs )
        {
            ors_data_swap( m_start, rhs.m_start );
            ors_data_swap( m_finish, rhs.m_finish );
            ors_data_swap( m_storage, rhs.m_storage );
            ors_data_swap( m_alloc, rhs.m_alloc );
        }
    }

    void resize( size_type new_size, const_reference value = value_type() )
    {
        const size_type len = size();
        if( new_size < len )
            erase( begin() + new_size, end() );
        else if( new_size > len )
            insert( end(), new_size - len, value );
    }


    void reverse();

    iterator erase( iterator position );
    iterator erase( iterator first, iterator last );


    void assign( size_type new_size, const_reference value = value_type() );
    void assign( int new_size, const_reference value = value_type() )
	{  assign( static_cast<size_type>( new_size ), value );  }
    void assign( long new_size, const_reference value = value_type() )
	{  assign( static_cast<size_type>( new_size ), value );  }

    template< typename InputIterator >
		void assign( InputIterator first, InputIterator last, size_type new_size = 0 )
    {
        if( first == last )
            clear();
        else
            assign_aux( first, last, new_size);
    }


    void insert( iterator position, size_type count, const_reference value );
    void insert( iterator position, int count, const_reference value )
	{  insert( position, static_cast<size_type>( count ), value );  }
    void insert( iterator position, long count, const_reference value )
	{  insert( position, static_cast<size_type>( count ), value );  }

    iterator insert( iterator position, const_reference value = value_type() )
    {
        const size_type n = position - m_start;
        if( position == m_finish && m_finish != m_storage )
        {
            construct( m_finish, value );
            ++m_finish;
        }
        else
            insert_aux( position, value );

        return ( m_start + n );
    }

    template< typename InputIterator >
		void insert( iterator position, InputIterator first, InputIterator last, size_type extra_size = 0 )
    {
        if( first != last )
            range_insert( position, first, last, range_length(first, last, extra_size) );
    }

protected:
    //insert辅助函数
    void insert_aux( iterator position, const_reference value );

    template< typename InputIterator >
		void range_insert( iterator position, InputIterator first, InputIterator last, size_type extra_size )
	{
		size_type old_capa = capacity();
		size_type new_size = size() + extra_size;

		if( old_capa >= new_size )  //剩余空间足够
		{
			size_type elements_after = m_finish - position;
			iterator old_finish = m_finish;

/*			//position后面的已构造空间可以放进所有的新数据
			if( elements_after >= extra_size )
			{	
				m_finish = init_copy( m_finish - extra_size, m_finish, m_finish );
				copy_backward( position, old_finish - extra_size, old_finish );
				copy( first, last, position );
			}
			else  //position后面的已构造空间不能放进所有的新数据
			{
				InputIterator mid = first;
				advance( mid, elements_after );
				//将[mid, last)和[position, m_finish)依次复制到[m_finish, enough)
				m_finish = init_copy_copy( mid, last, position, m_finish, m_finish );
				copy( first, mid, position );
			}
*/
			int count = last - first;
			
			// 腾空间
			if (elements_after > 0)
				memmove(position + count, position, elements_after * sizeof(value_type));

			// 调用构造函数拷贝，不会析构被覆盖的对象
			init_copy(first, last, position);
		}
		else  //剩余空间不够
		{
			size_type new_capa = my_max( 2 * old_capa, new_size );

			pointer new_start = alloc_aux( new_capa);
			pointer new_finish = new_start;

/*
			try
			{
				new_finish = init_copy( begin(), position, new_start );
				new_finish = init_copy( first, last, new_finish );
				new_finish = init_copy( position, end(), new_finish );
			}
			catch(...)
			{
				destroy( new_start, new_finish );
				m_alloc.deallocate( new_start, new_capa );
				throw;
			}
			
			destroy( begin(), end() );
*/

			int nCopies = position - begin();
			if (nCopies > 0) {
				memcpy(new_start, begin(), nCopies * sizeof(value_type));
				new_finish += nCopies;
			}
			
			// 起，止，目的地
			new_finish = init_copy(first, last, new_finish);

			nCopies = end() - position;
			if (nCopies > 0) {
				memcpy( new_finish, position, nCopies * sizeof(value_type));
				new_finish += nCopies;
			}

			dealloc_data();

			m_start = new_start;
			m_finish = new_finish;
			m_storage = m_start + new_capa;
		}
	}


    //assign辅助函数
    template< typename InputIterator >
		void range_assign( InputIterator first, InputIterator last,	size_type new_size )
	{
		if( capacity() < new_size )
		{
			self temp( first, last, new_size );
			swap( temp );
		}
		else
		{
			iterator itr = m_start;

			for( ; (itr != m_finish) && (first != last); ++itr,++first )
				*itr = *first;

			if( (itr == m_finish) && (first != last) )
				insert( end(), first, last );
			else if( itr != m_finish )
				erase( itr, end() );
		}
	}

    template< typename InputIterator >
		void assign_aux( InputIterator first, InputIterator last,
		size_type new_size)
    {
        new_size = my_max( static_cast<size_type>(last - first), new_size );
        range_assign( first, last, new_size );
    }


    //初始化的辅助函数
    template< typename InputIterator >
		void range_init( InputIterator first, InputIterator last, size_type size)
    {
        size_type n = last - first;
        alloc_data( my_max(n, size) );
        try
        {
            init_copy( first, last, m_start );
        }
        catch(...)
        {
            dealloc_data();
            throw;
        }
        m_finish = m_start + n;
    }

    void fill_init( size_type n, const_reference value )
    {
		alloc_data( n );
        try
        {
            init_fill_n( m_start, n, value );
        }
        catch(...)
        {
            dealloc_data();
            throw;
        }
        m_finish = m_start + n;
    }


    void alloc_data( size_type n )
    {
        if( n > 0 )
        {
            m_start = alloc_aux(n);
            m_finish = m_start;
            m_storage = m_start + n;
        }
        else
        {
            m_start = NULL;
            m_finish = NULL;
            m_storage = NULL;
        }
    }

    void dealloc_data()
    {
        if( m_start )
			m_alloc.deallocate(m_start, capacity() );	// 只有指针起作用
    }

    pointer alloc_aux( size_type n )
    {
        return m_alloc.allocate( n );
    }

};  //end class

template< typename T, typename Allocator >
void orsArray<T, Allocator>::reverse()
{
    if( size() < 2 )
        return;

    iterator first = begin(), last = end();
    --last;
    for( ; first < last; ++first,--last )
		ors_data_swap( *first, *last );
}
//-----------------------------------------------------------------------------

template< typename T, typename Allocator >
void orsArray<T, Allocator>::assign( size_type new_size, const T& value )
{
    if( capacity() < new_size )
    {
		self temp( new_size, value );
		swap( temp );
    }
    else
    {
		const size_type len = size();
		if( len > new_size )
		{
			fill_n( begin(), new_size, value );
            destroy( begin() + new_size, end() );
            m_finish = m_start + new_size;
        }
        else if( len < new_size )
        {
            fill( begin(), end(), value );
            init_fill_n( end(), new_size - len, value );
            m_finish = m_start + new_size;
        }
        else
            fill( begin(), end(), value );
    }
}

//-----------------------------------------------------------------------------

template< typename T, typename Allocator >
typename orsArray<T, Allocator>::iterator
orsArray<T, Allocator>::erase( iterator position )
{
    if( position != m_finish )
    {
		m_alloc.destroy(position);

		int nMoves = m_finish - position;
		if (nMoves > 0) 
			memmove(position, position + 1, nMoves * sizeof(value_type));

		--m_finish;

		// 恢复初始状态
		if (nMoves > 0)
			new(m_finish) T();
    }

    return position;
}

//-----------------------------------------------------------------------------
template< typename T, typename Allocator >
typename orsArray<T, Allocator>::iterator
orsArray<T, Allocator>::erase( iterator first, iterator last )
{
    if( first < last )
    {
//         size_type n = m_finish - last;
//         iterator itr;
//         if( n > 0 )
//             itr = copy_n( last, n, first );
// 		else
// 			itr = first;
// 
//         destroy( itr, m_finish );
// 
//         m_finish -= ( last - first );

		destroy(first, last);

		int nMoves = m_finish - last;
		
		if (nMoves > 0) {
			memmove(first, last, nMoves * sizeof(value_type));

			// reset objects moved
			iterator iter;
			for (iter = last; iter < m_finish; iter++)
				new(iter) T();
		}

		int nErase = last - first;

		m_finish -= nErase;
    }

    return first;
}


//-----------------------------------------------------------------------------

template< typename T, typename Allocator >
void orsArray<T, Allocator>::insert_aux( iterator position, const T& value )
{
    if( m_finish != m_storage )  //剩余空间足够
    {
//         construct( m_finish, *( m_finish - 1 ) );
//         ++m_finish;
//         copy_backward( position, m_finish - 2, m_finish - 1 );
//        *position = value;

		// 先腾出空间
		int elements_after = m_finish - position;
		if (elements_after)
			memmove(position + 1, position, elements_after);
		++m_finish;

		// 重置对象，避免析构
		//new(position) T();

		// 就地重构对象，用赋值会调用析构函数
		m_alloc.construct( position, value);
    }
    else  //剩余空间不够
    {
        size_type old_size = size();
        size_type new_capa = my_max( 2 * old_size, (size_type)8 );

        pointer new_start = alloc_aux(new_capa);
        pointer new_finish = new_start;

        if( size() > 0 )
        {
/*
			// copy from start to position into new space
            new_finish = init_copy( begin(), position, new_start );
			
			// insert the value
			construct(new_finish, value);
			++new_finish;

			// copy from position to end into new space
			new_finish = init_copy( position, end(), new_finish );
			destroy(begin(), end());
*/
			int nCopies = position - begin();
			if (nCopies > 0) {
				memcpy(new_start, begin(), nCopies * sizeof(value_type));
				new_finish += nCopies;
			}

			// insert the value
            construct( new_finish, value );
            ++new_finish;

			nCopies = end() - position;
			if (nCopies > 0) {
				memcpy(new_finish, position, nCopies * sizeof(value_type));
				new_finish += nCopies;
			}

			// deallocate memory only
			dealloc_data();
        }
		else {
			construct(new_finish, value);
			++new_finish;
		}

        m_start = new_start;
        m_finish = new_finish;
        m_storage = m_start + new_capa;
    }
}


//-----------------------------------------------------------------------------

template< typename T, typename Allocator >
void orsArray<T, Allocator>::insert( iterator position, size_type count, const_reference value )
{
    if( count < 1 || position > m_finish )
        return;

    if( space() >= count )  //剩余空间足够
    {
        size_type elements_after = m_finish - position;
        iterator old_finish = m_finish;

/*       //position后面的已构造空间可以放进所有的新数据
        if( elements_after >= count )
        {
            m_finish = init_copy( m_finish - count, m_finish, m_finish );
            copy_backward( position, old_finish - count, old_finish );
            fill_n( position, count, value );
        }
        else  //position后面的已构造空间不能放进所有的新数据
        {
            //先将value添满[m_finish, position + count)，
            //再将[position, m_finish)复制进[position + count, enough)
            m_finish = init_fill_copy( m_finish, position + count, value,
				position, m_finish );
            fill( position, old_finish, value );
        }
*/
		// 先腾出空间
		if( elements_after )
			memmove(position + count, position, elements_after);

		// 调用构造函数填充，不会调用被填充对象的析构函数
		init_fill_n(position, count, value);
    }
    else  //剩余空间不够， 复制模式
    {
        size_type old_capa = capacity();

        size_type new_capa;

        if( 2 * old_capa > old_capa + count )
            new_capa =  2 * old_capa;
        else
            new_capa = old_capa + count;

        pointer new_start = alloc_aux( new_capa);
        pointer new_finish = new_start;

/*       try
        {
            new_finish = init_copy( begin(), position, new_start );
            init_fill_n( new_finish, count, value );
            new_finish += count;
            new_finish = init_copy( position, end(), new_finish );
        }
        catch(...)
        {
            destroy( new_start, new_finish );
            m_alloc.deallocate( new_start, new_capa );
            throw;
        }
        destroy( begin(), end() );
*/

		int nCopies = position - begin();
		if (nCopies > 0) {
			memcpy(new_start, begin(), nCopies * sizeof(value_type));
			new_finish += nCopies;
		}

		init_fill_n(new_finish, count, value);
		new_finish += count;

		nCopies = end() - position;
		if (nCopies > 0) {
			memcpy(new_finish, position, nCopies * sizeof(value_type));
			new_finish += nCopies;
		}

		// 释放空间，不会调用析构函数
        dealloc_data();

        m_start = new_start;
        m_finish = new_finish;
        m_storage = m_start + new_capa;
    }
}


template< typename T, typename Allocator >
inline bool operator==( const orsArray<T, Allocator>& lhs,
                        const orsArray<T, Allocator>& rhs )
{
	if(lhs.size() != rhs.size())
		return false;
	else
	{
		for (int i=0;i<lhs.size();i++)
		{
			if(lhs[i] != rhs[i])
				return false;
		}
	}
	return true;
}

template< typename T, typename Allocator >
inline bool operator!=( const orsArray<T, Allocator>& lhs,
                        const orsArray<T, Allocator>& rhs )
{
    return !( lhs == rhs );
}

#endif

#endif
