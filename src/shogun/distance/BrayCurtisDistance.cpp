/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soeren Sonnenburg, Evan Shelhamer, Sergey Lisitsyn
 */

#include <shogun/lib/config.h>
#include <shogun/lib/common.h>
#include <shogun/io/SGIO.h>
#include <shogun/distance/BrayCurtisDistance.h>
#include <shogun/features/Features.h>

using namespace shogun;

BrayCurtisDistance::BrayCurtisDistance()
: DenseDistance<float64_t>()
{
}

BrayCurtisDistance::BrayCurtisDistance(const std::shared_ptr<DenseFeatures<float64_t>>& l, const std::shared_ptr<DenseFeatures<float64_t>>& r)
: DenseDistance<float64_t>()
{
	init(l, r);
}

BrayCurtisDistance::~BrayCurtisDistance()
{
	cleanup();
}

bool BrayCurtisDistance::init(std::shared_ptr<Features> l, std::shared_ptr<Features> r)
{
	return DenseDistance<float64_t>::init(l,r);
}

void BrayCurtisDistance::cleanup()
{
}

float64_t BrayCurtisDistance::compute(int32_t idx_a, int32_t idx_b)
{
	int32_t alen, blen;
	bool afree, bfree;

	float64_t* avec=
		(std::static_pointer_cast<DenseFeatures<float64_t>>(lhs))->get_feature_vector(idx_a, alen, afree);
	float64_t* bvec=
		(std::static_pointer_cast<DenseFeatures<float64_t>>(rhs))->get_feature_vector(idx_b, blen, bfree);

	ASSERT(alen==blen)

	float64_t s1=0;
	float64_t s2=0;
	{
		for (int32_t i=0; i<alen; i++)
		{
			s1+=fabs(avec[i]-bvec[i]);
			s2+=fabs(avec[i]+bvec[i]);
		}
	}

	(std::static_pointer_cast<DenseFeatures<float64_t>>(lhs))->free_feature_vector(avec, idx_a, afree);
	(std::static_pointer_cast<DenseFeatures<float64_t>>(rhs))->free_feature_vector(bvec, idx_b, bfree);

	// trap division by zero
	if(s2==0)
		return 0;

	return (s1/s2);
}
