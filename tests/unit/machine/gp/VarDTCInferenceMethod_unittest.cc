/*
 * Copyright (c) The Shogun Machine Learning Toolbox
 * Written (W) 2015 Wu Lin
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


#include <gtest/gtest.h>
#include <shogun/lib/config.h>
#include <shogun/labels/RegressionLabels.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/kernel/GaussianKernel.h>
#include <shogun/machine/gp/VarDTCInferenceMethod.h>
#include <shogun/machine/gp/ZeroMean.h>
#include <shogun/machine/gp/ConstMean.h>
#include <shogun/machine/gp/GaussianLikelihood.h>
#include <shogun/machine/gp/GaussianARDSparseKernel.h>
#include <shogun/mathematics/Math.h>

using namespace shogun;


TEST(VarDTCInferenceMethod,get_negative_log_marginal_likelihood)
{
	index_t n=6;
	index_t dim=2;
	index_t m=3;

	SGMatrix<float64_t> feat_train(dim, n);
	SGMatrix<float64_t> lat_feat_train(dim, m);
	SGVector<float64_t> lab_train(n);

	feat_train(0,0)=-0.81263;
	feat_train(0,1)=-0.99976;
	feat_train(0,2)=1.17037;
	feat_train(0,3)=1.51752;
	feat_train(0,4)=1.57765;
	feat_train(0,5)=3.89440;

	feat_train(1,0)=0.5;
	feat_train(1,1)=0.4576;
	feat_train(1,2)=5.17637;
	feat_train(1,3)=2.56752;
	feat_train(1,4)=4.57765;
	feat_train(1,5)=2.89440;

	lat_feat_train(0,0)=1.00000;
	lat_feat_train(0,1)=3.00000;
	lat_feat_train(0,2)=4.00000;

	lat_feat_train(1,0)=3.00000;
	lat_feat_train(1,1)=2.00000;
	lat_feat_train(1,2)=-5.00000;

	lab_train[0]=0.46;
	lab_train[1]=0.7;
	lab_train[2]=-1.16;
	lab_train[3]=1.5;
	lab_train[4]=3.5;
	lab_train[5]=-5.0;

	// shogun representation of features and labels
	auto features_train=std::make_shared<DenseFeatures<float64_t>>(feat_train);
	auto inducing_features_train=std::make_shared<DenseFeatures<float64_t>>(lat_feat_train);
	auto labels_train=std::make_shared<RegressionLabels>(lab_train);

	float64_t ell=log(2.0);
	auto kernel=std::make_shared<GaussianKernel>(10,2.0*exp(ell*2.0));

	float64_t mean_weight=0.0;
	auto mean=std::make_shared<ConstMean>(mean_weight);

	float64_t sigma=0.5;
	auto lik=std::make_shared<GaussianLikelihood>(sigma);

	// specify GP regression with FITC inference
	auto inf=std::make_shared<VarDTCInferenceMethod>(kernel, features_train,
		mean, labels_train, lik, inducing_features_train);

	float64_t ind_noise=1e-6;
	inf->set_inducing_noise(ind_noise);

	float64_t scale=1.5;
	inf->set_scale(scale);

	inf->enable_optimizing_inducing_features(false);

	float64_t nlz=inf->get_negative_log_marginal_likelihood();

	// comparison of posterior negative marginal likelihood with
	// result from varsgp package:
	// http://www.aueb.gr/users/mtitsias/code/varsgp.tar.gz
	// nlZ =
	//58.616164107936129
	EXPECT_NEAR(nlz, 58.616164107936129, 1E-6);
	// clean up


}

TEST(VarDTCInferenceMethod,get_marginal_likelihood_derivatives)
{
	index_t n=6;
	index_t dim=2;
	index_t m=3;

	SGMatrix<float64_t> feat_train(dim, n);
	SGMatrix<float64_t> lat_feat_train(dim, m);
	SGVector<float64_t> lab_train(n);

	feat_train(0,0)=-0.81263;
	feat_train(0,1)=-0.99976;
	feat_train(0,2)=1.17037;
	feat_train(0,3)=1.51752;
	feat_train(0,4)=1.57765;
	feat_train(0,5)=3.89440;

	feat_train(1,0)=0.5;
	feat_train(1,1)=0.4576;
	feat_train(1,2)=5.17637;
	feat_train(1,3)=2.56752;
	feat_train(1,4)=4.57765;
	feat_train(1,5)=2.89440;

	lat_feat_train(0,0)=1.00000;
	lat_feat_train(0,1)=3.00000;
	lat_feat_train(0,2)=4.00000;

	lat_feat_train(1,0)=3.00000;
	lat_feat_train(1,1)=2.00000;
	lat_feat_train(1,2)=-5.00000;

	lab_train[0]=0.46;
	lab_train[1]=0.7;
	lab_train[2]=-1.16;
	lab_train[3]=1.5;
	lab_train[4]=3.5;
	lab_train[5]=-5.0;

	// shogun representation of features and labels
	auto features_train=std::make_shared<DenseFeatures<float64_t>>(feat_train);
	auto inducing_features_train=std::make_shared<DenseFeatures<float64_t>>(lat_feat_train);
	auto labels_train=std::make_shared<RegressionLabels>(lab_train);

	float64_t ell=log(2.0);
	auto kernel=std::make_shared<GaussianKernel>(10,2.0*exp(ell*2.0));

	float64_t mean_weight=0.0;
	auto mean=std::make_shared<ConstMean>(mean_weight);

	float64_t sigma=0.5;
	auto lik=std::make_shared<GaussianLikelihood>(sigma);

	// specify GP regression with FITC inference
	auto inf=std::make_shared<VarDTCInferenceMethod>(kernel, features_train,
		mean, labels_train, lik, inducing_features_train);

	float64_t ind_noise=1e-6;
	inf->set_inducing_noise(ind_noise);

	float64_t scale=1.5;
	inf->set_scale(scale);

	inf->enable_optimizing_inducing_features(false);

	// build parameter dictionary
	std::map<SGObject::Parameters::value_type, std::shared_ptr<SGObject>> parameter_dictionary;
	inf->build_gradient_parameter_dictionary(parameter_dictionary);

	// compute derivatives wrt parameters
	auto gradient=
		inf->get_negative_log_marginal_likelihood_derivatives(parameter_dictionary);

	// get parameters to compute derivatives
	float64_t dnlZ_sf2=gradient["log_scale"][0];
	float64_t dnlZ_lik=gradient["log_sigma"][0];
	float64_t dnlZ_width=gradient["width"][0];

	// comparison of partial derivatives of negative log marginal likelihood
	// result from varsgp package:
	// http://www.aueb.gr/users/mtitsias/code/varsgp.tar.gz
	//cov=
	//11.103836410254763
	//17.692318958964869
	//lik=
	//-91.123579890090099
	//
	EXPECT_NEAR(dnlZ_lik, -91.123579890090099, 1E-5);
	EXPECT_NEAR(dnlZ_width, 11.103836410254763, 1E-5);
	EXPECT_NEAR(dnlZ_sf2, 17.692318958964869, 1E-5);
}

TEST(VarDTCInferenceMethod,get_marginal_likelihood_derivative_wrt_inducing_features)
{
	float64_t rel_tolerance=1e-5;
	float64_t abs_tolerance;
	index_t n=6;
	index_t dim=2;
	index_t m=3;

	SGMatrix<float64_t> feat_train(dim, n);
	SGMatrix<float64_t> lat_feat_train(dim, m);
	SGVector<float64_t> lab_train(n);

	feat_train(0,0)=-0.81263;
	feat_train(0,1)=-0.99976;
	feat_train(0,2)=1.17037;
	feat_train(0,3)=1.51752;
	feat_train(0,4)=1.57765;
	feat_train(0,5)=3.89440;

	feat_train(1,0)=0.5;
	feat_train(1,1)=0.4576;
	feat_train(1,2)=5.17637;
	feat_train(1,3)=2.56752;
	feat_train(1,4)=4.57765;
	feat_train(1,5)=2.89440;

	lat_feat_train(0,0)=1.00000;
	lat_feat_train(0,1)=3.00000;
	lat_feat_train(0,2)=4.00000;

	lat_feat_train(1,0)=3.00000;
	lat_feat_train(1,1)=2.00000;
	lat_feat_train(1,2)=-5.00000;

	lab_train[0]=0.46;
	lab_train[1]=0.7;
	lab_train[2]=-1.16;
	lab_train[3]=1.5;
	lab_train[4]=3.5;
	lab_train[5]=-5.0;

	// shogun representation of features and labels
	auto features_train=std::make_shared<DenseFeatures<float64_t>>(feat_train);
	auto inducing_features_train=std::make_shared<DenseFeatures<float64_t>>(lat_feat_train);
	auto labels_train=std::make_shared<RegressionLabels>(lab_train);

	auto kernel=std::make_shared<GaussianARDSparseKernel>(10);
	kernel->set_scalar_weights(1.0/2.0);

	float64_t mean_weight=0.0;
	auto mean=std::make_shared<ConstMean>(mean_weight);

	float64_t sigma=0.5;
	auto lik=std::make_shared<GaussianLikelihood>(sigma);

	// specify GP regression with FITC inference
	auto inf=std::make_shared<VarDTCInferenceMethod>(kernel, features_train,
		mean, labels_train, lik, inducing_features_train);

	float64_t ind_noise=1e-6;
	inf->set_inducing_noise(ind_noise);

	float64_t scale=1.5;
	inf->set_scale(scale);

	inf->enable_optimizing_inducing_features(false);

	// build parameter dictionary
	std::map<SGObject::Parameters::value_type, std::shared_ptr<SGObject>> parameter_dictionary;
	inf->build_gradient_parameter_dictionary(parameter_dictionary);

	// compute derivatives wrt parameters
	auto gradient=
		inf->get_negative_log_marginal_likelihood_derivatives(parameter_dictionary);

	// get parameters to compute derivatives
	SGVector<float64_t> dnlZ_lat=gradient["inducing_features"];
	SGMatrix<float64_t> deriv_lat(dnlZ_lat.vector, dim, m, false);
	// get parameters to compute derivatives
	// comparison of partial derivatives of negative log marginal likelihood
	// with result from varsgp package:
	// http://www.aueb.gr/users/mtitsias/code/varsgp.tar.gz
	// dXu=
	//-3.026588124830805 -10.984866584498826   0.000007222318628
	//7.574618915520174  -7.260614222976087  -0.000050353461401

	abs_tolerance = Math::get_abs_tolerance(-3.026588124830805, rel_tolerance);
	EXPECT_NEAR(deriv_lat(0,0),  -3.026588124830805,  abs_tolerance);
	abs_tolerance = Math::get_abs_tolerance(-10.984866584498826, rel_tolerance);
	EXPECT_NEAR(deriv_lat(0,1),  -10.984866584498826,  abs_tolerance);
	abs_tolerance = Math::get_abs_tolerance(0.000007222318628, rel_tolerance);
	EXPECT_NEAR(deriv_lat(0,2),  0.000007222318628,  abs_tolerance);

	abs_tolerance = Math::get_abs_tolerance(7.574618915520174, rel_tolerance);
	EXPECT_NEAR(deriv_lat(1,0),  7.574618915520174,  abs_tolerance);
	abs_tolerance = Math::get_abs_tolerance(-7.260614222976087, rel_tolerance);
	EXPECT_NEAR(deriv_lat(1,1),  -7.260614222976087,  abs_tolerance);
	abs_tolerance = Math::get_abs_tolerance(-0.000050353461401, rel_tolerance);
	EXPECT_NEAR(deriv_lat(1,2),  -0.000050353461401,  abs_tolerance);
}
