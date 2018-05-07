#pragma once

#include <stdexcept>
#include <list>

#include "NativeImport.hpp"
#include "Internal/NativeCast.hpp"

#define PAWN_NATIVES_HAS_FUNC

namespace pawn_natives
{
	int AmxLoad(AMX * amx);

	class NativeFuncBase
	{
	public:
		void Init()
		{
			// Install this as a native.
			//Install(NULL);
		}

	protected:
		NativeFuncBase(unsigned int count, char const * const name, AMX_NATIVE native)
		:
			count_(count * sizeof (cell)),
			name_(name),
			native_(native),
			amx_(0),
			params_(0)
		{
			if (!all_)
				all_ = new std::list<NativeFuncBase *>();
			if (all_)
				all_->push_back(this);
		}
		
		~NativeFuncBase() = default;

		AMX * GetAMX() const { return amx_; }
		cell * GetParams() const { return params_; }
		
		cell CallDoOuter(AMX * amx, cell * params)
		{
			cell
				ret = 0;
			if (amx && params)
			{
				// Check that there are enough parameters.
				amx_ = amx;
				params_ = params;
				try
				{
					if (count_ > (unsigned int)params[0])
						throw std::invalid_argument("Insufficient arguments.");
					ret = this->CallDoInner(amx, params);
				}
				catch (std::exception & e)
				{
					char
						msg[1024];
					sprintf(msg, "Exception in %s: \"%s\"", name_, e.what());
					LOG_NATIVE_ERROR(msg);
				}
				catch (...)
				{
					char
						msg[1024];
					sprintf(msg, "Unknown exception in in %s", name_);
					LOG_NATIVE_ERROR(msg);
					params_ = 0;
					amx_ = 0;
					throw;
				}
				params_ = 0;
				amx_ = 0;
			}
			return (cell)ret;
		}

	private:
		virtual cell CallDoInner(AMX *, cell *) = 0;

		friend int AmxLoad(AMX * amx);

		NativeFuncBase() = delete;
		NativeFuncBase(NativeFuncBase const &) = delete;
		NativeFuncBase(NativeFuncBase const &&) = delete;
		NativeFuncBase const & operator=(NativeFuncBase const &) const = delete;
		NativeFuncBase const & operator=(NativeFuncBase const &&) const = delete;

		unsigned int
			count_;

		char const * const
			name_;

		AMX_NATIVE const
			native_;

		AMX *
			amx_;

		cell *
			params_;

		static std::list<NativeFuncBase *> *
			all_;
	};

	template <typename FUNC_TYPE>
	class NativeFunc {};

	// A pretty horrible combination of templates that make hooks quite seamless.
	template <typename RET>
	class NativeFunc0 : protected NativeFuncBase
	{
	public:
		inline RET operator()()
		{
			return Do();
		}

	protected:
		NativeFunc0(char const * const name, AMX_NATIVE native) : NativeFuncBase(0, name, native) {}
		~NativeFunc0() = default;

	private:
		cell CallDoInner(AMX *, cell *)
		{
			RET
				ret = this->Do();
			return *(cell *)&ret;
		}

		virtual RET Do() const = 0;
	};

	// Template specialisation for void returns, since they can't use "return X()".
	template <>
	class NativeFunc0<void> : protected NativeFuncBase
	{
	public:
		inline void operator()()
		{
			Do();
		}

	protected:
		NativeFunc0(char const * const name, AMX_NATIVE native) : NativeFuncBase(0, name, native) {}
		~NativeFunc0() = default;

	private:
		cell CallDoInner(AMX *, cell *)
		{
			this->Do();
			return 1;
		}

		virtual void Do() const = 0;
	};

	template <typename RET>
	class NativeFunc<RET()> : public NativeFunc0<RET> { protected: NativeFunc(char const * const name, AMX_NATIVE native) : NativeFunc0<RET>(name, native) {} };
};

// Defer declaring the other classes to a super macro file.
#define PAWN_HOOK_TEMPLATE   typename A
#define PAWN_HOOK_NAME       NativeFunc1
#define PAWN_HOOK_TYPES      A
#define PAWN_HOOK_PARAMETERS A a
#define PAWN_HOOK_CALLING    a
#define PAWN_HOOK_NUMBER     1
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B
#define PAWN_HOOK_NAME       NativeFunc2
#define PAWN_HOOK_TYPES      A, B
#define PAWN_HOOK_PARAMETERS A a, B b
#define PAWN_HOOK_CALLING    a, b
#define PAWN_HOOK_NUMBER     2
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C
#define PAWN_HOOK_NAME       NativeFunc3
#define PAWN_HOOK_TYPES      A, B, C
#define PAWN_HOOK_PARAMETERS A a, B b, C c
#define PAWN_HOOK_CALLING    a, b, c
#define PAWN_HOOK_NUMBER     3
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D
#define PAWN_HOOK_NAME       NativeFunc4
#define PAWN_HOOK_TYPES      A, B, C, D
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d
#define PAWN_HOOK_CALLING    a, b, c, d
#define PAWN_HOOK_NUMBER     4
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E
#define PAWN_HOOK_NAME       NativeFunc5
#define PAWN_HOOK_TYPES      A, B, C, D, E
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e
#define PAWN_HOOK_CALLING    a, b, c, d, e
#define PAWN_HOOK_NUMBER     5
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F
#define PAWN_HOOK_NAME       NativeFunc6
#define PAWN_HOOK_TYPES      A, B, C, D, E, F
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f
#define PAWN_HOOK_CALLING    a, b, c, d, e, f
#define PAWN_HOOK_NUMBER     6
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G
#define PAWN_HOOK_NAME       NativeFunc7
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g
#define PAWN_HOOK_NUMBER     7
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H
#define PAWN_HOOK_NAME       NativeFunc8
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h
#define PAWN_HOOK_NUMBER     8
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I
#define PAWN_HOOK_NAME       NativeFunc9
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H, I
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h, i
#define PAWN_HOOK_NUMBER     9
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J
#define PAWN_HOOK_NAME       NativeFunc10
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j
#define PAWN_HOOK_NUMBER     10
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K
#define PAWN_HOOK_NAME       NativeFunc11
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k
#define PAWN_HOOK_NUMBER     11
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L
#define PAWN_HOOK_NAME       NativeFunc12
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l
#define PAWN_HOOK_NUMBER     12
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M
#define PAWN_HOOK_NAME       NativeFunc13
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L, M
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l, m
#define PAWN_HOOK_NUMBER     13
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M, typename N
#define PAWN_HOOK_NAME       NativeFunc14
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L, M, N
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l, m, n
#define PAWN_HOOK_NUMBER     14
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M, typename N, typename O
#define PAWN_HOOK_NAME       NativeFunc15
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L, M, N, O
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o
#define PAWN_HOOK_NUMBER     15
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

#define PAWN_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M, typename N, typename O, typename P
#define PAWN_HOOK_NAME       NativeFunc16
#define PAWN_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P
#define PAWN_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o, P p
#define PAWN_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p
#define PAWN_HOOK_NUMBER     16
#include "Internal/NativeFuncImpl.hpp"
#undef PAWN_HOOK_NUMBER
#undef PAWN_HOOK_CALLING
#undef PAWN_HOOK_PARAMETERS
#undef PAWN_HOOK_TYPES
#undef PAWN_HOOK_NAME
#undef PAWN_HOOK_TEMPLATE

// The hooks and calls for each class are always static, because otherwise it
// would make installing hooks MUCH harder - we would need stubs that could
// handle class pointers.  Doing that would negate needing a different class for
// every hook type, even when the parameters are the same, but this way is
// probably not much more generated code, and vastly simpler.
// 
// The inheritance from `NativeFuncBase` is protected, because we don't want
// normal users getting in to that data.  However, we do want them to be able to
// use the common `IsEnabled` method, so re-export it.
#define PAWN_NATIVE_DECL(nspace,func,type) \
	PAWN_NATIVE_EXPORT PAWN_NATIVE__RETURN(type) PAWN_NATIVE_API                \
	    PAWN_NATIVE_##nspace##_##func(PAWN_NATIVE__PARAMETERS(type));           \
	                                                                            \
	namespace nspace                                                            \
	{                                                                           \
	    class Native_##nspace##_##func :                                        \
	        public pawn_natives::NativeFunc<type>                               \
	    {                                                                       \
	    public:                                                                 \
	        Native_##nspace##_##func() :                                        \
	            NativeFunc<type>(#func, (AMX_NATIVE)&Call) {}                   \
	                                                                            \
	    private:                                                                \
	        friend PAWN_NATIVE_DLLEXPORT PAWN_NATIVE__RETURN(type) PAWN_NATIVE_API\
	            ::PAWN_NATIVE_##nspace##_##func(PAWN_NATIVE__PARAMETERS(type)); \
	                                                                            \
	        static cell AMX_NATIVE_CALL                                         \
	            Call(AMX * amx, cell * params);                                 \
	                                                                            \
	        PAWN_NATIVE__RETURN(type)                                           \
	            Do PAWN_NATIVE__WITHOUT_RETURN_##type const;                    \
	    };                                                                      \
	                                                                            \
	    extern Native_##nspace##_##func func;                                   \
	}

// We can't pass exceptions to another module easily, so just don't...
// 
// I quite like this:
//   
//   PAWN_NATIVE__MAYBE_RETURN(type) {};
//   
// If there is a return type, it will compile as:
//   
//   return {};
//   
// Which means "return default value" in new C++ versions.  If there is no
// return type (void), it will compile as:
//   
//   {};
//   
// Which means nothing.
#define PAWN_NATIVE_DEFN(nspace,func,type) \
	nspace::Native_##nspace##_##func nspace::func;                              \
	                                                                            \
	cell AMX_NATIVE_CALL                                                        \
	    nspace::Native_##nspace##_##func::Call(AMX * amx, cell * params)        \
	{                                                                           \
	    return ::nspace::func.CallDoOuter(amx, params);                         \
	}                                                                           \
	                                                                            \
	PAWN_NATIVE_EXPORT PAWN_NATIVE__RETURN(type) PAWN_NATIVE_API                \
	    PAWN_NATIVE_##nspace##_##func(PAWN_NATIVE__PARAMETERS(type))            \
	{                                                                           \
	    try                                                                     \
	    {                                                                       \
	        PAWN_NATIVE__MAYBE_RETURN(type)                                     \
	            ::nspace::func.Do(PAWN_NATIVE__CALLING(type));                  \
	    }                                                                       \
	    catch (std::exception & e)                                              \
	    {                                                                       \
	        char                                                                \
	            msg[1024];                                                      \
	        sprintf(msg, "Exception in _" #func ": \"%s\"", e.what());          \
	        LOG_NATIVE_ERROR(msg);                                              \
	    }                                                                       \
	    catch (...)                                                             \
	    {                                                                       \
	        LOG_NATIVE_ERROR("Unknown exception in _" #func);                   \
	    }                                                                       \
	    PAWN_NATIVE__MAYBE_RETURN(type) {};                                     \
	}                                                                           \
	                                                                            \
	PAWN_NATIVE__RETURN(type)                                                   \
	    nspace::Native_##nspace##_##func::                                      \
	    Do PAWN_NATIVE__WITHOUT_RETURN_##type const

#define PAWN_NATIVE_DECLARE PAWN_NATIVE_DECL
#define PAWN_NATIVE_DEFINE  PAWN_NATIVE_DEFN

#define PAWN_NATIVE(nspace,func,type) PAWN_NATIVE_DECL(nspace,func,type);PAWN_NATIVE_DEFN(nspace,func,type)

#if 0

// Example:

// In your header:
PAWN_NATIVE_DECL(SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a));

// In your code:
PAWN_NATIVE_DEFN(SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a))
{
	// Implementation here...
	SetPlayerPos(playerid, x, y, z);
	return SetPlayerFacingAngle(playerid, a);
}

#endif

