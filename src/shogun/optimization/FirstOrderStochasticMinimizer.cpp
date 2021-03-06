/*
 * Copyright (c) The Shogun Machine Learning Toolbox
 * Written (w) 2015 Wu Lin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Shogun Development Team.
 *
 */

#include <shogun/optimization/FirstOrderStochasticMinimizer.h>
#include <shogun/optimization/SparsePenalty.h>
#include <shogun/optimization/ProximalPenalty.h>

using namespace shogun;

void FirstOrderStochasticMinimizer::set_gradient_updater(std::shared_ptr<DescendUpdater> gradient_updater)
{
	require(gradient_updater, "Gradient updater must set");
	if(m_gradient_updater != gradient_updater)
	{


		m_gradient_updater=gradient_updater;
	}
}

FirstOrderStochasticMinimizer:: ~FirstOrderStochasticMinimizer()
{


}

void FirstOrderStochasticMinimizer::set_number_passes(int32_t num_passes)
{
	require(num_passes>0, "The number ({}) to go through data must be positive", num_passes);
	m_num_passes=num_passes;
}

void FirstOrderStochasticMinimizer::set_learning_rate(std::shared_ptr<LearningRate >learning_rate)
{
	if(m_learning_rate != learning_rate)
	{


		m_learning_rate=learning_rate;
	}
}

void FirstOrderStochasticMinimizer::do_proximal_operation(SGVector<float64_t>variable_reference)
{
	auto proximal_penalty=std::dynamic_pointer_cast<ProximalPenalty>(m_penalty_type);
	if(proximal_penalty)
	{
		float64_t proximal_weight=m_penalty_weight;
		auto sparse_penalty=std::dynamic_pointer_cast<SparsePenalty>(m_penalty_type);
		if(sparse_penalty)
		{
			require(m_learning_rate, "Learning rate must set when Sparse Penalty (eg, L1) is used");
			proximal_weight*=m_learning_rate->get_learning_rate(m_iter_counter);
		}
		proximal_penalty->update_variable_for_proximity(variable_reference,proximal_weight);
	}
}

void FirstOrderStochasticMinimizer::init_minimization()
{
	require(m_fun,"Cost function must set");
	require(m_gradient_updater,"Descend updater must set");
	require(m_num_passes>0, "The number to go through data must set");
	m_cur_passes=0;
}

void FirstOrderStochasticMinimizer::init()
{
	m_gradient_updater=NULL;
	m_learning_rate=NULL;
	m_num_passes=0;
	m_cur_passes=0;
	m_iter_counter=0;

	SG_ADD((std::shared_ptr<SGObject>*)&m_learning_rate, "FirstOrderMinimizer__m_learning_rate",
		"learning_rate in FirstOrderStochasticMinimizer");
	SG_ADD((std::shared_ptr<SGObject>*)&m_gradient_updater, "FirstOrderMinimizer__m_gradient_updater",
		"gradient_updater in FirstOrderStochasticMinimizer");
	SG_ADD(&m_num_passes, "FirstOrderMinimizer__m_num_passes",
		"num_passes in FirstOrderStochasticMinimizer");
	SG_ADD(&m_cur_passes, "FirstOrderMinimizer__m_cur_passes",
		"cur_passes in FirstOrderStochasticMinimizer");
	SG_ADD(&m_iter_counter, "FirstOrderMinimizer__m_iter_counter",
		"m_iter_counter in FirstOrderStochasticMinimizer");
}
