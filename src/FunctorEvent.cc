/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 10/14/17.
//

#include "FunctorEvent.hh"

namespace wacrana {

class MainGuiExecutor : public BrightFuture::ExecutorBase<MainGuiExecutor> // CRTP
{
public:
	// Called by ExecutorBase using CRTP
	void ExecuteTask(std::function<void()>&& task)
	{
		PostMain(std::move(task));
	}

	friend BrightFuture::Executor* MainExec();

private:
	MainGuiExecutor() = default;
};

BrightFuture::Executor* MainExec()
{
	static MainGuiExecutor inst;
	return &inst;
}

} // end of namespace
