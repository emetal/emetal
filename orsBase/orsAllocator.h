#ifndef OPENRS_ALLOCTOR_H
#define OPENRS_ALLOCTOR_H

#include <new>

static const int size_t_max = static_cast<int>( -1 );

template< typename T >
class orsAllocator
{
	void *(*m_mallocFun)( size_t size );
	void (*m_freeFun)( void *ptr );

public:
    typedef  T                  value_type;
    typedef  value_type&        reference;
    typedef  const value_type&  const_reference;
    typedef  value_type*        pointer;
    typedef  const value_type*  const_pointer;
    typedef  value_type*        iterator;
    typedef  const value_type*  const_iterator;
    typedef  int				size_type;
    typedef  int				difference_type;
    typedef  orsAllocator<T>       self;

    template< typename U >
    struct rebind
    {
        typedef  orsAllocator<U>  other;
    };

    template< typename U >
    orsAllocator( const orsAllocator<U>& ) throw()  
	{
		m_mallocFun = malloc;
		m_freeFun = free;
	}

    orsAllocator() throw()  
	{
		m_mallocFun = malloc;
		m_freeFun = free;
	}

    orsAllocator( const self& ) throw()  
	{
		m_mallocFun = malloc;
		m_freeFun = free;	
	}

    template< typename U >
    self& operator=( const orsAllocator<U>& ) throw()  {  return *this;  }

    self& operator=( const self& ) throw()  {  return *this;  }

    ~orsAllocator() throw()  {}

    pointer address( reference X ) throw()  {  return ( &X );  }

    const_pointer address( const_reference X ) throw()  {  return ( &X );  }

    size_type max_size()
    {
        return size_type( size_t_max / sizeof(T) );
    }

    pointer allocate( size_type n, const void* = NULL )
    {
		pointer ptr0 = (pointer)m_mallocFun(n * sizeof(value_type));
		pointer ptr = (pointer)ptr0;

		// initialize
		for( int i=0; i<n; i++, ptr++ )
			new(ptr) T();				// 构造

		return (pointer)ptr0;
    }

    void deallocate( pointer ptr, size_type n = 0 )
    {
		m_freeFun( ptr );
    }

	// 构造
    void construct( pointer ptr, const T& value )
    {
		new(ptr) T(value);
    }

    void destroy( pointer ptr )
    {
		//printf("\torsAllocator: destroy\n");
        ptr->~T();
    }

};  //end orsAllocator



#endif
