#pragma once

#include "ImageTypes.hpp"

using BSplineTransform = itk::BSplineTransform<double, 2, 3>;

namespace rt
{
class DeformableRegistration {
public:
	DeformableRegistration(const Image8UC3& fixedImage, const ResampleFilter::Pointer resampleFilter)
		: fixedImage_(fixedImage), resampleFilter_(resampleFilter) 
		{
			deformTransform_ = NULL;
		}
	void setResampleFilter(const ResampleFilter::Pointer resampleFilter) { resampleFilter_ = resampleFilter; }
	void setFixedImage(const Image8UC3& fixedImage) { fixedImage_ = fixedImage; }

private:
	void compute_();
	void DeformableRegistration::generate_bspline_transform_();
	void clear_();

	BSplineTransform deformTransform_;
	ResampleFilter::Pointer resampleFilter_;
	Image8UC3 fixedImage_;
};
}