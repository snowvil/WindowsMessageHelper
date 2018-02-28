//////////////////////////////////////////////////////////////////////////////////////
// CSingleton.h: interface for the CSingleton class.
//////////////////////////////////////////////////////////////////////////////////////
//    ex) sample
//
//    class Test : public CSingleton < Test >
//    {
//    public:
//	    Test* Sample( int num );
//	    //...
//    };
//
//
//
//    #define g_Test	Test::GetSingleton()
//
//    void SomeFuncton( void )
//    {
//	    Test * pTemp = Test::GetSingleton().Sample( 7 );
//	    // or   Test * pTemp = g_Test.Sample( 7 );
//    }
//
//////////////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------

# 파일명 : Singleton.h
# 설  명 : interface for the Singleton template class.
# 내  력 : 
# 비  고 : 

---------------------------------------------------------------------------*/

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

///////////////////////////////////////////////////////////////////////////////
// include define statement
///////////////////////////////////////////////////////////////////////////////

#include "assert.h"

///////////////////////////////////////////////////////////////////////////////
// class define statement
///////////////////////////////////////////////////////////////////////////////

template <typename T> class AFX_CLASS_EXPORT CSingleton
{
    static T* ms_Singleton;

public:
    CSingleton(void)
    {
        assert(!ms_Singleton);
		size_t offset = (size_t)(T*)1 - (size_t)(CSingleton <T>*)(T*)1;
        ms_Singleton = (T*)((size_t)this + offset);
    }

    ~CSingleton(void)
    {
        assert(ms_Singleton);
        ms_Singleton = nullptr;
    }

	constexpr static void SetSingleton(T* pT) noexcept
	{
		assert(pT);
		ms_Singleton = pT;
	}

	constexpr static T* GetSingleton(void) noexcept
    {
        assert(ms_Singleton);
        return ms_Singleton;
    }

	constexpr static T* GetSingletonPtr(void) noexcept
	{
        assert(ms_Singleton);
        return ms_Singleton;
	}

	constexpr static T& GetSingletonInstance(void) noexcept
	{
        assert(ms_Singleton);
		return(*ms_pSingleton);
	}
};

template <typename T> T* CSingleton <T>::ms_Singleton = nullptr;

#endif
