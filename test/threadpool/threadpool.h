#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include<vector>
#include<queue>
#include<thread>
#include<atomic>
#include<condition_variable>
#include<future>
#include<functional>
#include<stdexcept>

//线程池,可以提交变参函数或拉姆达表达式的匿名函数执行,可以获取执行返回值
//不支持类成员函数, 支持类静态成员函数或全局函数,Opteron()函数等

#define MAX_THREAD_NUM 256
	
class threadpool
{
	using Task = std::function<void()>;
	//线程池
	std::vector<thread> pool;
	//任务队列
	std::queue<Task> tasks;
	//同步
	std::mutex m_lock;
	//条件阻塞
	std::condition_variable cv_task;
	//是否关闭提交
	std::atomic<bool> stoped;
	//空闲线程数量
	std::atomic<int> idlThrNum;
public:
	inline threadpool(unsigned short size = 4) : stoped{false}
	{
		idlThrNum = size < 1 ? 1 : size;
		for(size = 0; size < idlThrNum; ++size)
		{
			//初始化线程队列
			pool.emplace_back(
				[this]
				{
					//工作线程函数,当关闭的时候
					while(!this->stoped)
					{
						std::function<void()> task;
						{
							//获取一个执行的task
							std::unique_lock<std::mutex> lock{this->m_lock};
							//unique_lock相比lock_guard的好处时可以随时unlock和lock
							//只有当wait(lck, pred)中的pred为false时调用wait()才会阻塞当前线程，
							//并且在收到其他线程的通知后只有当pred为true时才会被解除阻塞。
							this->cv_task.wait(lock, 
								[this]
								{
									return this->stoped.load() || !this->tasks.empty();
								}
							);
							if(this->stoped && this->tasks.empty())
								return;
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}
						idlThrNum--;
						task();
						idlThrNum++;
					}
				}
			);
		}
	}

	inline ~threadpool()
	{
		stoped.store(true);
		cv_task.notify_all();
		for(std::thread &thread : pool){
			if(thread.joinable())
				thread.join();
		}
	}

public:
	//提交任务
	//调用.get()获取返回值会等待任务执行完成、
	//有两种方法可以实现调用类成员
	//1、bind: commit(std::bind(&Dog::sayHello, &dog));
	//2、mem_fn:commit(std::mem_fn(&Dog::sayHello), &dog);
	template<class F, class ... Args>
	auto commit(F&& f, Args&&...args)->std::future<decltype(f(args...))>
	{
		if(stoped.load())
			throw std::runtime_error("commit on ThreadPool is stopped.");

		//RetType为f的返回类型
		using RetType = decltype(f(args...));
		auto task = std::make_shared<std::packaged_task<RetType()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);
		std::future<RetType> future = task->get_future();
		{
			std::lock_guard<mutex> lock{m_lock};
			tasks.emplace([task](){(*task)();});
			cv_task.notify_one();
			return future;
		}
	}
	
	int idlCount(){return idlThrNum;}
};

#endif

