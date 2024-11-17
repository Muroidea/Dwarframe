/////////////////////////////////////////////////////////////////
// File: MulticastDelegate.h
//
// Copyright (C) 11.08.2022 PixelAnt Games. All rights reserved.
/////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////
// Engine
#include <BasicStructures/SpinLock.h>
#include <Delegates/Delegate.h>

/////////////////////////////////////////////////////////////////
// STD

/////////////////////////////////////////////////////////////////
// ThirdParty

namespace SLUG
{

    template<typename Signature>
    class TMulticastDelegate;

    template<typename... Args>
    class TMulticastDelegate<void(Args...)>
    {
    public:
        template<typename FunctorType>
        DelegateIndicator BindFunctor(FunctorType Functor);

        template<typename FunctorType>
        DelegateIndicator BindFunctor(FunctorType *Functor);

        template<typename Type>
        DelegateIndicator BindMemberFunction(Type *ObjectPtr, void(Type::*FunctionPtr)(Args...args));

        DelegateIndicator BindRawFunction(void(*FunctionPtr)(Args...));

        DelegateIndicator BindDelegate(TDelegate<void(Args...)>&& Delegate);

        bool IsBound() const;
        bool IsBoundObject(const DelegateIndicator& Indicator) const;

        void Clear();

        void Invoke(Args... args);

        bool Remove(DelegateIndicator& Indicator);

    private:
        std::vector<TDelegate<void(Args...)>> m_Delegates;
        SpinLock m_Lock;
    };

    template<typename... Args>
    template<typename FunctorType>
    DelegateIndicator TMulticastDelegate<void(Args...)>::BindFunctor(FunctorType Functor)
    {
        //std::lock_guard<std::mutex> LockGuard(m_Mutex);
        m_Lock.Lock();
        TDelegate<void(Args...)> Delegate = TDelegate<void(Args...)>::CreateWithFunctor(Functor);
        m_Delegates.emplace_back(std::move(Delegate));
        m_Lock.Unlock();

        return Delegate.GetDelegateIndicator();
    }

    template<typename... Args>
    template<typename FunctorType>
    DelegateIndicator TMulticastDelegate<void(Args...)>::BindFunctor(FunctorType *Functor)
    {
        //std::lock_guard<std::mutex> LockGuard(m_Mutex);
        m_Lock.Lock();
        TDelegate<void(Args...)> Delegate = TDelegate<void(Args...)>::CreateWithFunctor(Functor);
        m_Delegates.emplace_back(std::move(Delegate));
        m_Lock.Unlock();

        return Delegate.GetDelegateIndicator();
    }

    template<typename... Args>
    template<typename Type>
    DelegateIndicator TMulticastDelegate<void(Args...)>::BindMemberFunction(Type *ObjectPtr, void(Type::*FunctionPtr)(Args...args))
    {
        //std::lock_guard<std::mutex> LockGuard(m_Mutex);
        m_Lock.Lock();
        TDelegate<void(Args...)> Delegate = TDelegate<void(Args...)>::CreateWithMemberFunction(ObjectPtr, FunctionPtr);
        m_Delegates.emplace_back(std::move(Delegate));
        m_Lock.Unlock();

        return Delegate.GetDelegateIndicator();
    }

    template<typename... Args>
    DelegateIndicator TMulticastDelegate<void(Args...)>::BindRawFunction(void(*FunctionPtr)(Args...))
    {
        //std::lock_guard<std::mutex> LockGuard(m_Mutex);
        m_Lock.Lock();
        TDelegate<void(Args...)> Delegate = TDelegate<void(Args...)>::CreateWithRawFunction(FunctionPtr);
        m_Delegates.emplace_back(std::move(Delegate));
        m_Lock.Unlock();

        return Delegate.GetDelegateIndicator();
    }

    template<typename... Args>
    DelegateIndicator TMulticastDelegate<void(Args...)>::BindDelegate(TDelegate<void(Args...)>&& Delegate)
    {
        DelegateIndicator Indicator = Delegate.GetDelegateIndicator();

        if (IsBoundObject(Indicator))
        {
            return Indicator;
        }

        //std::lock_guard<std::mutex> LockGuard(m_Mutex);
        m_Lock.Lock();
        m_Delegates.emplace_back(std::move(Delegate));
        m_Lock.Unlock();

        return Indicator;
    }

    template<typename... Args>
    bool TMulticastDelegate<void(Args...)>::IsBound() const
    {
        return !m_Delegates.empty();
    }

    template<typename... Args>
    bool TMulticastDelegate<void(Args...)>::IsBoundObject(const DelegateIndicator& Indicator) const
    {
        for (const TDelegate<void(Args...)>& Delegate : m_Delegates)
        {
            if (Delegate.GetDelegateIndicator() == Indicator)
            {
                return true;
            }
        }

        return false;
    }

    template<typename... Args>
    void TMulticastDelegate<void(Args...)>::Clear()
    {
        //std::lock_guard<std::mutex> LockGuard(m_Mutex);
        m_Lock.Lock();
        m_Delegates.clear();
        m_Lock.Unlock();
    }

    template<typename... Args>
    void TMulticastDelegate<void(Args...)>::Invoke(Args... args)
    {
        //std::lock_guard<std::mutex> LockGuard(m_Mutex);
        m_Lock.Lock();
        for (const TDelegate<void(Args...)>& Delegate : m_Delegates)
        {
            Delegate.Invoke(std::forward<Args>(args)...);
        }
        m_Lock.Unlock();
    }

    template<typename... Args>
    bool TMulticastDelegate<void(Args...)>::Remove(DelegateIndicator& Indicator)
    {
        //std::lock_guard<std::mutex> LockGuard(m_Mutex);
        m_Lock.Lock();
        for (auto It = m_Delegates.begin(); It != m_Delegates.end(); It++)
        {
            if (It->GetDelegateIndicator() == Indicator)
            {
                m_Delegates.erase(It);
                Indicator.Invalidate();
                m_Lock.Unlock();
                return true;
            }
        }
        m_Lock.Unlock();

        return false;
    }
}

#define DEFINE_MULTICAST_DELEGATE(DelegateName, ...) using DelegateName = SLUG::TMulticastDelegate<void(__VA_ARGS__)>;

#define DEFINE_MULTICAST_DELEGATE_ONE_PARAM(DelegateName, ParamOne) DEFINE_MULTICAST_DELEGATE(DelegateName, ParamOne)

#define DEFINE_MULTICAST_DELEGATE_TWO_PARAM(DelegateName, ParamOne, ParamTwo) DEFINE_MULTICAST_DELEGATE(DelegateName, ParamOne, ParamTwo)

#define DEFINE_MULTICAST_DELEGATE_THREE_PARAM(DelegateName, ParamOne, ParamTwo, ParamThree) DEFINE_MULTICAST_DELEGATE(DelegateName, ParamOne, ParamTwo, ParamThree)