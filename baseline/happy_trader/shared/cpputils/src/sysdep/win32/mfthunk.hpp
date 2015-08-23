#ifndef _CPPUTILS_MFTHUNK_HPP
#define _CPPUTILS_MFTHUNK_HPP

#pragma once


#include <windows.h>
#include <stddef.h>
#include <assert.h>

namespace CppUtils {

#ifdef _MSC_VER

#if defined(_M_IX86)

#pragma pack(push,1)
struct _stdcallthunk
{
    BYTE  m_pop_eax;
    BYTE  m_push;
    DWORD m_this;
    BYTE  m_push_eax;
    BYTE  m_jmp;
    DWORD m_adr;

    void Init(void* _this, void* _fun)
    {
        m_pop_eax  = 0x58;
        m_push     = 0x68;
        m_this     = (DWORD)_this;
        m_push_eax = 0x50;
        m_jmp      = 0xE9;
        m_adr      = DWORD((INT_PTR)_fun - ((INT_PTR)this + sizeof(_stdcallthunk)));
        FlushInstructionCache(GetCurrentProcess(), this, sizeof(_stdcallthunk));
    }  

    void* GetCodeAddress()
    {
        return this;
    }
};
#pragma pack(pop)

#else // _M_IX86

    #error "Only IX86!"

#endif // _M_IX86

class stdcall_proxy
{
public:
    template<class T, class R>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)()))()
    {
        return (R(__stdcall*)())bind(_this, &mpf, sizeof(mpf));
    }

    template<class T, class R, class P1>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)(P1)))(P1)
    {
        return (R(__stdcall*)(P1))bind(_this, &mpf, sizeof(mpf));
    }

    template<class T, class R, class P1, class P2>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)(P1, P2)))(P1, P2)
    {
        return (R(__stdcall*)(P1,P2))bind(_this, &mpf, sizeof(mpf));
    }

    template<class T, class R, class P1, class P2, class P3>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)(P1, P2, P3)))(P1, P2, P3)
    {
        return (R(__stdcall*)(P1, P2, P3))bind(_this, &mpf, sizeof(mpf));
    }

    template<class T, class R, class P1, class P2, class P3, class P4>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)(P1, P2, P3, P4)))(P1, P2, P3, P4)
    {
        return (R(__stdcall*)(P1, P2, P3, P4))bind(_this, &mpf, sizeof(mpf));
    }

    template<class T, class R, class P1, class P2, class P3, class P4, class P5>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)(P1, P2, P3, P4, P5)))(P1, P2, P3, P4, P5)
    {
        return (R(__stdcall*)(P1, P2, P3, P4, P5))bind(_this, &mpf, sizeof(mpf));
    }

    template<class T, class R, class P1, class P2, class P3, class P4, class P5, class P6>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)(P1, P2, P3, P4, P5, P6)))(P1, P2, P3, P4, P5, P6)
    {
        return (R(__stdcall*)(P1, P2, P3, P4, P5, P6))bind(_this, &mpf, sizeof(mpf));
    }

    template<class T, class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)(P1, P2, P3, P4, P5, P6, P7)))(P1, P2, P3, P4, P5, P6, P7)
    {
        return (R(__stdcall*)(P1, P2, P3, P4, P5, P6, P7))bind(_this, &mpf, sizeof(mpf));
    }

		template<class T, class R, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
    R (__stdcall* bind(T* _this, R (__stdcall T::*mpf)(P1, P2, P3, P4, P5, P6, P7, P8)))(P1, P2, P3, P4, P5, P6, P7, P8)
    {
        return (R(__stdcall*)(P1, P2, P3, P4, P5, P6, P7, P8))bind(_this, &mpf, sizeof(mpf));
    }

    void* bind(void* _this, void* mpf, size_t cbmpf)
    {
        if(cbmpf == sizeof(void*))
            thunk.Init(_this, *((void**)mpf));
        else if(cbmpf == sizeof(void*) + sizeof(DWORD))
            thunk.Init((char*)_this + *(((DWORD*)mpf) + 1), *((void**)mpf));
        else assert(!"Invalid sizeof pointer to member function!");
        return thunk.GetCodeAddress();
    }

private:
    _stdcallthunk thunk;
};

#endif // _MSC_VER

}; // end of namespace

#endif // _MFTHUNK_HPP