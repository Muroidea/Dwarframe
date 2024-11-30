#ifndef CORE_H
#define CORE_H

#include <stdint.h>

struct NoCopiable
{
    NoCopiable() = default;
    NoCopiable(const NoCopiable&) = delete;
    NoCopiable& operator=(const NoCopiable&) = delete;
};

struct NoMovable
{
    NoMovable() = default;
    NoMovable(const NoMovable&) = delete;
    NoMovable& operator=(const NoMovable&) = delete;
};

/*
template <typename OtherClassName>\
uint32 IsSameClass(OtherClassName* Object) const { return std::is_same_v<ClassName, OtherClassName> == true; }\
template <typename OtherClassName>\
uint32 IsBaseOf(OtherClassName* Object) const { return std::is_base_of_v<ClassName, OtherClassName> == true; }
*/

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using HashValue = uint64;
using float32 = float;
using float64 = double;

//****************************************************

class TypeID
{
	inline static uint32 sCounter{ 0 };

public:
	template<typename T>
	static uint32 Value()
	{
		static uint32 ID = sCounter++;
		return ID;
	}
};

#define IMPLEMENT_SIMPLE_RTTI(ClassName)\
	public:\
		virtual uint32 GetID() const { return TypeID::Value<ClassName>(); }

//****************************************************

#ifdef DX12_FRAMEWORK_DLL
	#define DWARFRAME_API __declspec(dllexport)
#else
	#define DWARFRAME_API __declspec(dllimport)
#endif // DX12_FRAMEWORK_DLL

#define STRINGIZE(x) #x
#define LINE_STRING STRINGIZE(__LINE__)

#define WITH_EDITOR 1 

#ifdef _DEBUG

	/*
	// Enables the Nsight Aftermath code instrumentation for GPU crash dump creation.
	#define USE_NSIGHT_AFTERMATH 1

	#if defined (USE_NSIGHT_AFTERMATH)
		#include "NVIDIA_Nsight_Aftermath_SDK_2024.1.0.24075/Aftermath/GFSDK_Aftermath.h"
		#include "NVIDIA_Nsight_Aftermath_SDK_2024.1.0.24075/Aftermath/GFSDK_Aftermath_GpuCrashDump.h"
		#include "NVIDIA_Nsight_Aftermath_SDK_2024.1.0.24075/Aftermath/GFSDK_Aftermath_GpuCrashDumpDecoding.h"
	#endif
	*/
	#define ThrowIfFailed(expr)																\
	do {																					\
		if(FAILED(expr))																	\
		{																					\
			assert(0 && #expr);																\
			throw std::runtime_error(__FILE__ "(" LINE_STRING "): FAILED( " #expr " )");	\
		}																					\
	} while(false)
	/*
	#define ThrowIfFailed(expr)																\
	if(expr)																				\
	{																						\
		assert(0 && #expr);																	\
		throw std::runtime_error(__FILE__ "(" LINE_STRING "): FAILED( " #expr " )");		\
	}
	*/
#else
	#define ThrowIfFailed(Expr, Description) Expr;
#endif


#define MEM_KiB(KiB)			(KiB * 1024)
#define MEM_MiB(MiB)	 MEM_KiB(MiB * 1024)

#endif // !CORE_H