#pragma once
#include <iostream>

class FunctionWrapper
{
	struct ImplBase {
		virtual void Call() = 0;
		virtual ~ImplBase() {}
	};

	std::unique_ptr<ImplBase> _impl;

	template<typename F>
	struct ImplType : ImplBase
	{
		F f;
		ImplType(F&& f_) : f(std::move(f_)) {}
		void Call() { f(); }
	};

public:
	template<typename F>
	FunctionWrapper(F&& f) : _impl(new ImplType<F>(std::move(f))) {}

	void operator()() { _impl->Call(); }
	FunctionWrapper() = default;
	FunctionWrapper(FunctionWrapper&& other) : _impl(std::move(other._impl)) {}

	FunctionWrapper& operator=(FunctionWrapper&& other)
	{
		_impl = std::move(other._impl);
		return *this;
	}

	FunctionWrapper(const FunctionWrapper&) = delete;
	FunctionWrapper(FunctionWrapper&) = delete;
	FunctionWrapper& operator=(const FunctionWrapper&) = delete;
};