/////////////////////////////////////////////////////////////////
// File: DelegateHelper
//
// Copyright (C) 29.07.2022 PixelAnt Games. All rights reserved.
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Engine

/////////////////////////////////////////////////////////////////
// STD
#include <atomic>
#include <cstdint>
#include <iostream>

/////////////////////////////////////////////////////////////////
// ThirdParty

#pragma once

namespace SLUG {

    class DelegateIndicator // Every unicast delegate will return an indicator, so we can return it and later use to remove from multicast
    {
    public:
        DelegateIndicator()
                : m_ID(0) {}

        ~DelegateIndicator() = default;

        bool operator==(const DelegateIndicator& Other) const
        {
            return m_ID == Other.m_ID;
        }

        bool operator!=(const DelegateIndicator& Other) const
        {
            return m_ID != Other.m_ID;
        }

        bool IsValid() const
        {
            return m_ID != 0;
        }

        void Invalidate()
        {
            m_ID = 0;
        }

    private:
        static DelegateIndicator&& GenerateDelegateIndicator();

        static std::atomic<int64_t> s_ID;

        int64_t m_ID;

        template<typename Signature>
        friend class TDelegate;
    };

    template<typename ReturnType, typename... Args>
    class IDelegateStorage
    {
    public:
        virtual ~IDelegateStorage() = default;

        virtual ReturnType Invoke(Args... args) const = 0;
        virtual bool IsBound() = 0;
    };

    template<typename Type, typename ReturnType, typename... Args>
    class TDelegateMemberFunctionStorage : public IDelegateStorage<ReturnType, Args...>
    {
    public:
        explicit TDelegateMemberFunctionStorage(Type *InstancePtr, ReturnType(Type::*FunctionPtr)(Args...))
                : m_InstancePtr(InstancePtr), m_FunctionPtr(FunctionPtr) {}

        ~TDelegateMemberFunctionStorage() = default;

        ReturnType Invoke(Args... args) const override
        {
            return (m_InstancePtr->*m_FunctionPtr)(std::forward<Args>(args)...);
        }

        bool IsBound() override
        {
            return m_InstancePtr && m_FunctionPtr;
        }

        bool IsBoundObject(Type *InstancePtr, ReturnType(Type::*FunctionPtr)(Args...))
        {
            return m_InstancePtr && m_InstancePtr == InstancePtr &&
                    m_FunctionPtr && m_FunctionPtr == FunctionPtr;
        }

    private:
        Type *m_InstancePtr = nullptr;
        ReturnType (Type::*m_FunctionPtr)(Args...) = nullptr;
    };

    template<typename FunctorType, typename ReturnType, typename... Args>
    class TDelegateFunctorStorage : public IDelegateStorage<ReturnType, Args...>
    {
    public:
        explicit TDelegateFunctorStorage(FunctorType *FunctorPtr)
                : m_FunctorPtr(FunctorPtr) {}

        ~TDelegateFunctorStorage() = default;

        ReturnType Invoke(Args... args) const override
        {
            return (*m_FunctorPtr)(std::forward<Args>(args)...);
        }

        bool IsBound() override
        {
            return m_FunctorPtr;
        }

        bool IsBoundObject(FunctorType *FunctorPtr)
        {
            return m_FunctorPtr && m_FunctorPtr == FunctorPtr;
        }

    private:
        FunctorType *m_FunctorPtr;
    };

    template<typename ReturnType, typename... Args>
    class TDelegateRawFunctionStorage : public IDelegateStorage<ReturnType, Args...>
    {
    public:
        explicit TDelegateRawFunctionStorage(ReturnType(*FunctionPtr)(Args...))
                : m_FunctionPtr(FunctionPtr) {}

        ~TDelegateRawFunctionStorage() = default;

        ReturnType Invoke(Args... args) const override
        {
            return m_FunctionPtr(std::forward<Args>(args)...);
        }

        bool IsBound() override
        {
            return m_FunctionPtr;
        }

        bool IsBoundObject(ReturnType(*FunctionPtr)(Args...))
        {
            return m_FunctionPtr && m_FunctionPtr == FunctionPtr;
        }

    private:
        ReturnType (*m_FunctionPtr)(Args...) = nullptr;
    };

}