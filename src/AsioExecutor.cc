/*
	Copyright © 2017 Wan Wai Ho <me@nestal.net>
    
    This file is subject to the terms and conditions of the GNU General Public
    License.  See the file COPYING in the main directory of the wacrana
    distribution for more details.
*/

//
// Created by nestal on 10/18/17.
//

#include "AsioExecutor.hh"

namespace wacrana {

AsioExecutor::AsioExecutor(boost::asio::io_service& ios) : m_ios{ios}
{
}

void AsioExecutor::ExecuteTask(std::function<void()>&& task)
{
	m_ios.post(std::move(task));
}

} // end of namespace
