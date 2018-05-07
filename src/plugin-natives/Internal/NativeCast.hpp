#pragma once

#include <stdexcept>
#include <amx/amx.h>

namespace pawn_natives
{
	template <typename T>
	struct ParamLookup
	{
		static T Val(cell ref)
		{
			return (T)ref;
		}

		static T * Ptr(cell * ref)
		{
			return (T *)ref;
		}
	};

	template <>
	struct ParamLookup<float>
	{
		static float Val(cell ref)
		{
			return amx_ctof(ref);
		}

		static float * Ptr(cell * ref)
		{
			return (float *)ref;
		}
	};

	template <typename T>
	class ParamCast
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(ParamLookup<T>::Val(params[idx]))
		{
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T()
		{
			return value_;
		}

	private:
		T
			value_;
	};

	template <typename T>
	class ParamCast<T const>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(ParamLookup<T>::Val(params[idx]))
		{
			// In theory, because `T` could contain `const`, we don't actually
			// need specialisations for constant parameters.  The pointer would
			// point in to AMX memory but be `const`.  However, people can cast
			// away `const` if they REALLY want to, so make a copy of the
			// parameter so that they still can't do that to modify the
			// original.  If they REALLY REALLY want to modify the original
			// parameter in AMX memory they will have to re-extract the pointer
			// from `params`.
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T const () const
		{
			return value_;
		}

	private:
		T
			value_;
	};

	template <>
	class ParamCast<varargs_t>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(amx, params, idx)
		{
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator varargs_t()
		{
			return &value_;
		}

	private:
		struct varargs_s
			value_;
	};

	template <>
	class ParamCast<varargs_t const>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(amx, params, idx)
		{
			// This is used as "..." - instead of passing actual varargs, which
			// could be complex.
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator varargs_t const()
		{
			return &value_;
		}

	private:
		struct varargs_s
			value_;
	};

	template <typename T>
	class ParamCast<T *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		{
			cell *
				src;
			amx_GetAddr(amx, params[idx], &src);
			value_ = ParamLookup<T>::Ptr(src);
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
			// This one doesn't because we are passing the direct pointer, which means any writes
			// are done directly in to AMX memory.
		}

		operator T *()
		{
			return value_;
		}

	private:
		T *
			value_;
	};

	template <typename T>
	class ParamCast<T &>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(ParamLookup<T>::Ref(params[idx]))
		{
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
			// This one doesn't because we are passing the direct pointer, which means any writes
			// are done directly in to AMX memory.
		}

		operator T &()
		{
			return value_;
		}

	private:
		T &
			value_;
	};

	template <typename T>
	class ParamCast<T const *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		{
			// In theory, because `T` could contain `const`, we don't actually
			// need specialisations for constant parameters.  The pointer would
			// point in to AMX memory but be `const`.  However, people can cast
			// away `const` if they REALLY want to, so make a copy of the
			// parameter so that they still can't do that to modify the
			// original.  If they REALLY REALLY want to modify the original
			// parameter in AMX memory they will have to re-extract the pointer
			// from `params`.
			cell *
				src;
			amx_GetAddr(amx, params[idx], &src);
			value_ = ParamLookup<T>::Ptr(src);
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T const *() const
		{
			return value_;
		}

	private:
		T *
			value_;
	};

	template <>
	class ParamCast<char *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
			:
			fake_('\0'),
			len_((int)params[idx + 1])
		{
			// Can't use `amx_StrParam` here, it allocates on the stack.  This
			// code wraps a lot of `sampgdk`, which fortunately is entirely
			// const-correct so we don't need to worry about strings not being
			// copied incorrectly.  We can also make the assumption that any
			// string is immediately followed by its length when it is an
			// output.
			if (len_ < 0)
				throw std::length_error("Invalid string length.");
			if (len_)
			{
				amx_GetAddr(amx, params[idx], &addr_);
				value_ = (char *)malloc(len_);
				if (!value_)
					throw std::bad_alloc();
				//amx_GetString(value_, addr_, 0, len_);
			}
			else
				value_ = &fake_;
		}

		~ParamCast()
		{
			// This is the only version that actually needs to write data back.
			if (len_)
				amx_SetString(addr_, value_, 0, 0, len_);
			free(value_);
		}

		operator char *()
		{
			return value_;
		}

	private:
		int
			len_;

		cell *
			addr_;

		char *
			value_;

		char
			fake_;
	};

	template <>
	class ParamCast<char const *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
			:
			fake_('\0')
		{
			// Can't use `amx_StrParam` here, it allocates on the stack.  This
			// `const` version is not optional at all - it ensures that the
			// string data is NOT written back.
			cell *
				addr;
			int
				len;
			amx_GetAddr(amx, params[idx], &addr);
			amx_StrLen(addr, &len);
			if (len)
			{
				value_ = (char *)malloc(len + 1);
				if (!value_)
					throw std::bad_alloc();
				amx_GetString(value_, addr, 0, len + 1);
			}
			else
				value_ = &fake_;
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
			free(value_);
		}

		operator char const *()
		{
			return value_;
		}

	private:
		char *
			value_;

		char
			fake_;
	};

	template <>
	class ParamCast<std::string &>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			len_((int)params[idx + 1])
		{
			// Can't use `amx_StrParam` here, it allocates on the stack.  This
			// code wraps a lot of `sampgdk`, which fortunately is entirely
			// const-correct so we don't need to worry about strings not being
			// copied incorrectly.  We can also make the assumption that any
			// string is immediately followed by its length when it is an
			// output.
			if (len_ < 0)
				throw std::length_error("Invalid string length.");
			if (len_)
			{
				amx_GetAddr(amx, params[idx], &addr_);
				char *
					src = (char *)alloca(len_);
				amx_GetString(src, addr_, 0, len_);
				value_ = src;
			}
			else
				value_.clear();
		}

		~ParamCast()
		{
			// This is the only version that actually needs to write data back.
			if (len_)
				amx_SetString(addr_, value_.c_str(), 0, 0, len_);
		}

		operator std::string &()
		{
			return value_;
		}

	private:
		int
			len_;

		cell *
			addr_;

		std::string
			value_;
	};

	template <>
	class ParamCast<std::string const &>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		{
			// Can't use `amx_StrParam` here, it allocates on the stack.  This
			// `const` version is not optional at all - it ensures that the
			// string data is NOT written back.
			cell *
				addr;
			int
				len;
			amx_GetAddr(amx, params[idx], &addr);
			amx_StrLen(addr, &len);
			if (len)
			{
				char *
					src = (char *)alloca(len + 1);
				amx_GetString(src, addr, 0, len + 1);
				value_ = src;
			}
			else
				value_.clear();
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator std::string const &()
		{
			return value_;
		}

	private:
		std::string
			value_;
	};
};

