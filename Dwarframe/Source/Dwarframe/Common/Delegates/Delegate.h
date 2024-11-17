/////////////////////////////////////////////////////////////////
// File: Delegate.h
//
// Copyright (C) 04.08.2022 PixelAnt Games. All rights reserved.
/////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////
// Engine
#include <Delegates/DelegateHelper.h>

/////////////////////////////////////////////////////////////////
// STD
#include <functional>
#include <vector>
#include <memory>
#include <iostream>

/////////////////////////////////////////////////////////////////
// ThirdParty

namespace SLUG
{

    template<typename Signature>
    class TDelegate;

    template<typename ReturnType, typename... Args>
    class TDelegate<ReturnType(Args...)>
    {
    public:
        TDelegate();
        ~TDelegate();

        TDelegate(const TDelegate& Delegate) = delete;
        TDelegate(TDelegate&& Delegate) noexcept;

        TDelegate& operator=(const TDelegate& Delegate) = delete;
        TDelegate& operator=(TDelegate&& Delegate) noexcept;

        template<typename FunctorType>
        static TDelegate<ReturnType(Args...)> CreateWithFunctor(FunctorType Functor);

        template<typename FunctorType>
        static TDelegate<ReturnType(Args...)> CreateWithFunctor(FunctorType *Functor);

        template<typename Type>
        static TDelegate<ReturnType(Args...)> CreateWithMemberFunction(Type *ObjectPtr, ReturnType(Type::*FunctionPtr)(Args...));

        static TDelegate<ReturnType(Args...)> CreateWithRawFunction(ReturnType(*FunctionPtr)(Args...));

        template<typename FunctorType>
        DelegateIndicator BindFunctor(FunctorType Functor);

        template<typename FunctorType>
        DelegateIndicator BindFunctor(FunctorType *Functor);

        template<typename Type>
        DelegateIndicator BindMemberFunction(Type *ObjectPtr, ReturnType(Type::*FunctionPtr)(Args...args));

        DelegateIndicator BindRawFunction(ReturnType(*FunctionPtr)(Args...));

        [[nodiscard]] bool IsBound() const;
        [[nodiscard]] bool IsBound(const DelegateIndicator& Indicator) const;

        void Clear();
        ReturnType Invoke(Args... args) const;

        [[nodiscard]] DelegateIndicator GetDelegateIndicator() const { return m_Indicator; }

    private:
        DelegateIndicator m_Indicator;
        IDelegateStorage<ReturnType, Args...> *m_Wrapper;
    };

    template<typename ReturnType, typename... Args>
    TDelegate<ReturnType(Args...)>::TDelegate() : m_Indicator(), m_Wrapper(nullptr) {}

    template<typename ReturnType, typename... Args>
    TDelegate<ReturnType(Args...)>::~TDelegate()
    {
        m_Indicator.Invalidate();
        if (m_Wrapper)
        {
            delete m_Wrapper;
            m_Wrapper = nullptr;
        }
    }

    template<typename ReturnType, typename... Args>
    TDelegate<ReturnType(Args...)>::TDelegate(TDelegate&& Delegate) noexcept
        : m_Wrapper(std::move(Delegate.m_Wrapper)), m_Indicator(std::move(Delegate.m_Indicator))
    {
        Delegate.m_Wrapper = nullptr;
    }

    template<typename ReturnType, typename... Args>
    TDelegate<ReturnType(Args...)>& TDelegate<ReturnType(Args...)>::operator=(TDelegate&& Delegate) noexcept
    {
        this->m_Wrapper = Delegate.m_Wrapper;
        Delegate.m_Wrapper = nullptr;
        this->m_Indicator = Delegate.m_Indicator;
        return *this;
    }

    template<typename ReturnType, typename... Args>
    template<typename FunctorType>
    TDelegate<ReturnType(Args...)> TDelegate<ReturnType(Args...)>::CreateWithFunctor(FunctorType Functor)
    {
        TDelegate<ReturnType(Args...)> Temporary;
        Temporary.m_Wrapper = new TDelegateFunctorStorage<FunctorType, ReturnType, Args...>(&Functor);
        Temporary.m_Indicator = DelegateIndicator::GenerateDelegateIndicator();

        return Temporary;
    }

    template<typename ReturnType, typename... Args>
    template<typename FunctorType>
    TDelegate<ReturnType(Args...)> TDelegate<ReturnType(Args...)>::CreateWithFunctor(FunctorType *Functor)
    {
        TDelegate<ReturnType(Args...)> Temporary;
        Temporary.m_Wrapper = new TDelegateFunctorStorage<FunctorType, ReturnType, Args...>(Functor);
        Temporary.m_Indicator = DelegateIndicator::GenerateDelegateIndicator();

        return Temporary;
    }

    template<typename ReturnType, typename... Args>
    template<typename Type>
    TDelegate<ReturnType(Args...)> TDelegate<ReturnType(Args...)>::CreateWithMemberFunction(Type *ObjectPtr, ReturnType(Type::*FunctionPtr)(Args...))
    {
        TDelegate<ReturnType(Args...)> Temporary;
        Temporary.m_Wrapper = new TDelegateMemberFunctionStorage<Type, ReturnType, Args...>(ObjectPtr, FunctionPtr);
        Temporary.m_Indicator = DelegateIndicator::GenerateDelegateIndicator();

        return Temporary;
    }

    template<typename ReturnType, typename... Args>
    TDelegate<ReturnType(Args...)> TDelegate<ReturnType(Args...)>::CreateWithRawFunction(ReturnType(*FunctionPtr)(Args...))
    {
        TDelegate<ReturnType(Args...)> Temporary;
        Temporary.m_Wrapper = new TDelegateRawFunctionStorage<ReturnType, Args...>(FunctionPtr);
        Temporary.m_Indicator = DelegateIndicator::GenerateDelegateIndicator();

        return Temporary;
    }

    template<typename ReturnType, typename... Args>
    template<typename FunctorType>
    DelegateIndicator TDelegate<ReturnType(Args...)>::BindFunctor(FunctorType Functor)
    {
        *this = CreateWithFunctor<FunctorType>(Functor);
        return m_Indicator;
    }

    template<typename ReturnType, typename... Args>
    template<typename FunctorType>
    DelegateIndicator TDelegate<ReturnType(Args...)>::BindFunctor(FunctorType *Functor)
    {
        *this = CreateWithFunctor<FunctorType>(Functor);
        return m_Indicator;
    }

    template<typename ReturnType, typename... Args>
    template<typename Type>
    DelegateIndicator TDelegate<ReturnType(Args...)>::BindMemberFunction(Type *ObjectPtr, ReturnType(Type::*FunctionPtr)(Args...args))
    {
        *this = CreateWithMemberFunction<Type>(ObjectPtr, FunctionPtr);
        return m_Indicator;
    }

    template<typename ReturnType, typename... Args>
    DelegateIndicator TDelegate<ReturnType(Args...)>::BindRawFunction(ReturnType(*FunctionPtr)(Args...))
    {
        *this = CreateWithRawFunction(FunctionPtr);
        return m_Indicator;
    }

    template<typename ReturnType, typename... Args>
    bool TDelegate<ReturnType(Args...)>::IsBound() const
    {
        return m_Wrapper && m_Wrapper->IsBound() && m_Indicator.IsValid();
    }

    template<typename ReturnType, typename... Args>
    bool TDelegate<ReturnType(Args...)>::IsBound(const DelegateIndicator& Indicator) const
    {
        return m_Indicator == Indicator && IsBound();
    }

    template<typename ReturnType, typename... Args>
    void TDelegate<ReturnType(Args...)>::Clear()
    {
        m_Indicator.Invalidate();
        if (m_Wrapper)
        {
            delete m_Wrapper;
            m_Wrapper = nullptr;
        }
    }

    template<typename ReturnType, typename... Args>
    ReturnType TDelegate<ReturnType(Args...)>::Invoke(Args... args) const
    {
        if (m_Wrapper)
        {
            return m_Wrapper->Invoke(std::forward<Args>(args)...);
        }

        return ReturnType();
    }

}

#define DEFINE_UNICAST_DELEGATE(DelegateName, ReturnType, ...) using DelegateName = SLUG::TDelegate<ReturnType(__VA_ARGS__)>;

#define DEFINE_UNICAST_DELEGATE_NONE_PARAM(DelegateName) DEFINE_UNICAST_DELEGATE(DelegateName, void, void)
#define DEFINE_UNICAST_DELEGATE_RET_NONE_PARAM(DelegateName, ReturnType) DEFINE_UNICAST_DELEGATE(DelegateName, ReturnType, void)

#define DEFINE_UNICAST_DELEGATE_ONE_PARAM(DelegateName, ParamOne) DEFINE_UNICAST_DELEGATE(DelegateName, void, ParamOne)
#define DEFINE_UNICAST_DELEGATE_RET_ONE_PARAM(DelegateName, ReturnType, ParamOne) DEFINE_UNICAST_DELEGATE(DelegateName, ReturnType, ParamOne)

#define DEFINE_UNICAST_DELEGATE_TWO_PARAM(DelegateName, ParamOne, ParamTwo) DEFINE_UNICAST_DELEGATE(DelegateName, void, ParamOne, ParamTwo)
#define DEFINE_UNICAST_DELEGATE_RET_TWO_PARAM(DelegateName, ReturnType, ParamOne, ParamTwo) DEFINE_UNICAST_DELEGATE(DelegateName, ReturnType, ParamOne, ParamTwo)

#define DEFINE_UNICAST_DELEGATE_THREE_PARAM(DelegateName, ParamOne, ParamTwo, ParamThree) DEFINE_UNICAST_DELEGATE(DelegateName, void, ParamOne, ParamTwo, ParamThree)
#define DEFINE_UNICAST_DELEGATE_RET_THREE_PARAM(DelegateName, ReturnType, ParamOne, ParamTwo, ParamThree) DEFINE_UNICAST_DELEGATE(DelegateName, ReturnType, ParamOne, ParamTwo, ParamThree)