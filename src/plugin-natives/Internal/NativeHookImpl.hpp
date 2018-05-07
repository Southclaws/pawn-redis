namespace pawn_natives
{
	template <typename RET, PAWN_HOOK_TEMPLATE>
	class PAWN_HOOK_NAME : protected NativeHookBase
	{
	public:
		typedef RET (*implementation_t)(PAWN_HOOK_TYPES);

		class ScopedCall
		{
		public:
			inline RET operator()(PAWN_HOOK_PARAMETERS)
			{
				return original_(PAWN_HOOK_CALLING);
			}

			~ScopedCall()
			{
				if (removed_)
					hook_.Install();
			}

		private:
			ScopedCall(subhook::Hook & hook, implementation_t original)
			:
				hook_(hook),
				original_(original),
				removed_(hook.Remove())
			{
			}

			ScopedCall(ScopedCall const &) = default;
			ScopedCall & operator=(ScopedCall const &) = default;

			ScopedCall(ScopedCall && that)
			:
				hook_(std::move(that.hook_)),
				original_(std::move(that.original_)),
				removed_(std::move(that.removed_))
			{
				that.removed_ = false;
			}

			ScopedCall & operator=(ScopedCall const &&) = delete;

			ScopedCall() = delete;

			friend class PAWN_HOOK_NAME<RET, PAWN_HOOK_TYPES>;

			subhook::Hook &
				hook_;

			implementation_t const
				original_;

			bool
				removed_;
		};

		inline RET operator()(PAWN_HOOK_PARAMETERS)
		{
			RET
				ret;
			if (Recursing())
			{
				subhook::ScopedHookRemove
					undo(&GetHook());
				ret = original_(PAWN_HOOK_CALLING);
			}
			else
			{
				ret = Do(PAWN_HOOK_CALLING);
			}
			Recursing();
			return ret;
		}

		ScopedCall operator*() const
		{
			ScopedCall
				ret(GetHook(), original_);
			return ret;
		}

	protected:
		PAWN_HOOK_NAME(char const * const name, implementation_t original, AMX_NATIVE replacement) : NativeHookBase(PAWN_HOOK_NUMBER, name, replacement), original_(original) {}
		~PAWN_HOOK_NAME() = default;

	private:
		cell CallDoInner(AMX * amx, cell * params)
		{
			#if PAWN_HOOK_NUMBER >= 1
				ParamCast<A> a(amx, params, 1);
			#endif
			#if PAWN_HOOK_NUMBER >= 2
				ParamCast<B> b(amx, params, 2);
			#endif
			#if PAWN_HOOK_NUMBER >= 3
				ParamCast<C> c(amx, params, 3);
			#endif
			#if PAWN_HOOK_NUMBER >= 4
				ParamCast<D> d(amx, params, 4);
			#endif
			#if PAWN_HOOK_NUMBER >= 5
				ParamCast<E> e(amx, params, 5);
			#endif
			#if PAWN_HOOK_NUMBER >= 6
				ParamCast<F> f(amx, params, 6);
			#endif
			#if PAWN_HOOK_NUMBER >= 7
				ParamCast<G> g(amx, params, 7);
			#endif
			#if PAWN_HOOK_NUMBER >= 8
				ParamCast<H> h(amx, params, 8);
			#endif
			#if PAWN_HOOK_NUMBER >= 9
				ParamCast<I> i(amx, params, 9);
			#endif
			#if PAWN_HOOK_NUMBER >= 10
				ParamCast<J> j(amx, params, 10);
			#endif
			#if PAWN_HOOK_NUMBER >= 11
				ParamCast<K> k(amx, params, 11);
			#endif
			#if PAWN_HOOK_NUMBER >= 12
				ParamCast<L> l(amx, params, 12);
			#endif
			#if PAWN_HOOK_NUMBER >= 13
				ParamCast<M> m(amx, params, 13);
			#endif
			#if PAWN_HOOK_NUMBER >= 14
				ParamCast<N> n(amx, params, 14);
			#endif
			#if PAWN_HOOK_NUMBER >= 15
				ParamCast<O> o(amx, params, 15);
			#endif
			#if PAWN_HOOK_NUMBER >= 16
				ParamCast<P> p(amx, params, 16);
			#endif
				RET
					ret = this->Do(PAWN_HOOK_CALLING);
				return *(cell *)&ret;
		}

		virtual RET Do(PAWN_HOOK_PARAMETERS) const = 0;

		implementation_t const
			original_;
	};

	template <PAWN_HOOK_TEMPLATE>
	class PAWN_HOOK_NAME<void, PAWN_HOOK_TYPES> : protected NativeHookBase
	{
	public:
		typedef void (*implementation_t)(PAWN_HOOK_TYPES);

		class ScopedCall
		{
		public:
			inline void operator()(PAWN_HOOK_PARAMETERS)
			{
				original_(PAWN_HOOK_CALLING);
			}

			~ScopedCall()
			{
				if (removed_)
					hook_.Install();
			}

		private:
			ScopedCall(subhook::Hook & hook, implementation_t original)
			:
				hook_(hook),
				original_(original),
				removed_(hook.Remove())
			{
			}

			ScopedCall(ScopedCall const &) = default;
			ScopedCall & operator=(ScopedCall const &) = default;

			ScopedCall(ScopedCall && that)
			:
				hook_(std::move(that.hook_)),
				original_(std::move(that.original_)),
				removed_(std::move(that.removed_))
			{
				that.removed_ = false;
			}

			ScopedCall & operator=(ScopedCall const &&) = delete;

			ScopedCall() = delete;

			friend class PAWN_HOOK_NAME<void, PAWN_HOOK_TYPES>;

			subhook::Hook &
				hook_;

			implementation_t const
				original_;

			bool
				removed_;
		};

		inline void operator()(PAWN_HOOK_PARAMETERS)
		{
			if (Recursing())
			{
				subhook::ScopedHookRemove
					undo(&GetHook());
				original_(PAWN_HOOK_CALLING);
			}
			else
			{
				Do(PAWN_HOOK_CALLING);
			}
			Recursing();
		}

		ScopedCall operator*() const
		{
			ScopedCall
				ret(GetHook(), original_);
			return ret;
		}

	protected:
		PAWN_HOOK_NAME(char const * const name, implementation_t original, AMX_NATIVE replacement) : NativeHookBase(PAWN_HOOK_NUMBER, name, replacement), original_(original) {}
		~PAWN_HOOK_NAME() = default;

	private:
		cell CallDoInner(AMX * amx, cell * params)
		{
			#if PAWN_HOOK_NUMBER >= 1
				ParamCast<A> a(amx, params, 1);
			#endif
			#if PAWN_HOOK_NUMBER >= 2
				ParamCast<B> b(amx, params, 2);
			#endif
			#if PAWN_HOOK_NUMBER >= 3
				ParamCast<C> c(amx, params, 3);
			#endif
			#if PAWN_HOOK_NUMBER >= 4
				ParamCast<D> d(amx, params, 4);
			#endif
			#if PAWN_HOOK_NUMBER >= 5
				ParamCast<E> e(amx, params, 5);
			#endif
			#if PAWN_HOOK_NUMBER >= 6
				ParamCast<F> f(amx, params, 6);
			#endif
			#if PAWN_HOOK_NUMBER >= 7
				ParamCast<G> g(amx, params, 7);
			#endif
			#if PAWN_HOOK_NUMBER >= 8
				ParamCast<H> h(amx, params, 8);
			#endif
			#if PAWN_HOOK_NUMBER >= 9
				ParamCast<I> i(amx, params, 9);
			#endif
			#if PAWN_HOOK_NUMBER >= 10
				ParamCast<J> j(amx, params, 10);
			#endif
			#if PAWN_HOOK_NUMBER >= 11
				ParamCast<K> k(amx, params, 11);
			#endif
			#if PAWN_HOOK_NUMBER >= 12
				ParamCast<L> l(amx, params, 12);
			#endif
			#if PAWN_HOOK_NUMBER >= 13
				ParamCast<M> m(amx, params, 13);
			#endif
			#if PAWN_HOOK_NUMBER >= 14
				ParamCast<N> n(amx, params, 14);
			#endif
			#if PAWN_HOOK_NUMBER >= 15
				ParamCast<O> o(amx, params, 15);
			#endif
			#if PAWN_HOOK_NUMBER >= 16
				ParamCast<P> p(amx, params, 16);
			#endif
				this->Do(PAWN_HOOK_CALLING);
				return 1;
		}

		virtual void Do(PAWN_HOOK_PARAMETERS) const = 0;

		implementation_t const
			original_;
	};

	template <typename RET, PAWN_HOOK_TEMPLATE>
	class NativeHook<RET(PAWN_HOOK_TYPES)> : public PAWN_HOOK_NAME<RET, PAWN_HOOK_TYPES> { protected: NativeHook(char const * const name, implementation_t original, AMX_NATIVE replacement) : PAWN_HOOK_NAME<RET, PAWN_HOOK_TYPES>(name, original, replacement) {} };
};

