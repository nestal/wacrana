/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the future
    distribution for more details.
*/

//
// Created by nestal on 10/2/17.
//

#pragma once

#include <future>
#include <utility>
#include <type_traits>
#include <unordered_map>

namespace exe {

class TaskBase
{
public:
	virtual void Execute() = 0;
};

template <typename T, typename Function>
class Task : public TaskBase
{
public:
	using R = decltype(std::declval<Function>()(std::declval<T>()));

	Task(const std::shared_future<T>& val, Function&& func) : m_val{val}, m_callback{std::move(func)}
	{
	}

	std::future<R> Result()
	{
		return m_result.get_future();
	}

	void Execute() override
	{
		Continue(m_val.get());
	}

private:
	template <typename Arg, typename Q=R>
	typename std::enable_if<!std::is_void<Q>::value>::type Continue(Arg&& val)
	{
		m_result.set_value(m_callback(std::forward<Arg>(val)));
	}

	template <typename Arg, typename Q=R>
	typename std::enable_if<std::is_void<Q>::value>::type Continue(Arg&& val)
	{
		m_callback(std::forward<Arg>(val));
		m_result.set_value();
	}

private:
	std::shared_future<T>   m_val;
	std::promise<R>         m_result;
	Function                m_callback;
};

class TaskScheduler;

struct Token
{
	TaskScheduler   *host;
	std::intptr_t   event;
};

class Executor
{
public:
	virtual void Execute(const std::function<void()>& func) = 0;
};

class LocalExecutor : public Executor
{
public:
	void Execute(const std::function<void()>& func) override
	{
		func();
	}
};

class TaskScheduler
{
public:
	TaskScheduler() = default;

	template <typename T, typename Function>
	auto Add(const std::shared_future<T>& val, Function&& func, Token& out)
	{
		out.event = m_seq++;
		out.host  = this;

		auto task = std::make_shared<Task<T, Function>>(std::move(val), std::forward<Function>(func));
		auto result = task->Result();

		m_tasks.emplace(out.event, std::move(task));
		return result;
	}

	void Schedule(Token token, Executor *executor)
	{
		auto it = m_tasks.find(token.event);
		if (it != m_tasks.end())
		{
			executor->Execute([task=std::move(it->second)]
			{
				task->Execute();
			});
			it = m_tasks.erase(it);
		}
	}

	std::size_t Count() const
	{
		return m_tasks.size();
	}

private:
	std::unordered_map<std::intptr_t, std::shared_ptr<TaskBase>>    m_tasks;
	std::intptr_t m_seq{0};
};

template <typename T>
class Future
{
public:
	Future() = default;
	
	Future(std::future<T>&& shared_state, std::promise<Token>&& token) :
		m_shared_state{std::move(shared_state)},
		m_token{std::move(token)}
	{
	}
	
	template <typename Func>
	void Then(Func&& continuation, TaskScheduler *host, Executor *exe)
	{
		Token token;
		auto result = m_shared_state.share();
		host->Add(result, std::forward<Func>(continuation), token);

		if (result.wait_for(std::chrono::system_clock::duration::zero()) == std::future_status::ready)
		{
			std::cout << "result ready" << std::endl;
			host->Schedule(token, exe);
		}
		else
		{
			std::cout << "result not ready" << std::endl;
			m_token.set_value(token);
		}
		
//		return Future<typename Next::ContResult>{next->ResultFuture(), std::move(next_token)};
	}
	
	Future(Future&& f) :
		m_shared_state{std::move(f.m_shared_state)},
		m_token{std::move(f.m_token)}
	{
	}
	
	~Future()
	{
		if (m_shared_state.valid())
			m_token.set_value({});
	}
	
private:
	std::future<T>      m_shared_state;
	std::promise<Token> m_token;
};

template <typename Func>
auto Async(Func&& func, Executor *exe)
{
	using T = decltype(func());
	
	std::promise<T>     shared_state;
	std::promise<Token> token;

	auto future = shared_state.get_future();

	std::thread{[
		func    = std::forward<Func>(func),
		result  = std::move(shared_state),
		token   = token.get_future(),
		exe
	] () mutable
	{
		// Run the function "func" in the spawned thread. The return value will be
		// moved to the lambda closure to be used later.
		result.set_value(func());
		
		// notify
		if (token.wait_for(std::chrono::system_clock::duration::zero()) == std::future_status::ready)
		{
			auto t = token.get();
			if (t.host)
				t.host->Schedule(t, exe);
		}
	}}.detach();
	
	return Future<T>{std::move(future), std::move(token)};
}

} // end of namespace
