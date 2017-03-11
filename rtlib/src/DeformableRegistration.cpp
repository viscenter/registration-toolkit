#include "DeformableRegistration.hpp"

BSplineTransform DeformableRegistration::getAffineTransform() {
    return deformTransform_;
}

void DeformableRegistration::compute_() {
    clear_();
    generate_bspline_transform_();
}

void DeformableRegistration::generate_bspline_transform_() {
    // Generate the affine deformTransform_
    GrayscaleFilter::Pointer fixedFilter = GrayscaleFilter::New();
    fixedFilter->SetInput(fixedImage__);

    GrayscaleFilter::Pointer movingFilter = GrayscaleFilter::New();
    movingFilter->SetInput(resampleFilter_->GetOutput());

    // Setup
    Metric::Pointer metric = Metric::New();
    Optimizer::Pointer optimizer = Optimizer::New();
    Registration::Pointer registration = Registration::New();
    GrayInterpolator::Pointer grayInterpolator = GrayInterpolator::New();

    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetInterpolator(grayInterpolator);
    registration->SetTransform(deformTransform_);
    registration->SetfixedImage_(fixedFilter->GetOutput());
    registration->SetMovingImage(movingFilter->GetOutput());

    Image::RegionType fixedRegion = fixedImage_->GetBufferedRegion();
    registration->SetfixedImage_Region(fixedRegion);

    ///// Deformable parameters /////
    uint16_t transformMeshFillSize = 12;

    // The maximum step length when the optimizer starts moving around
    double maxStepLength =
        fixedImage_->GetLargestPossibleRegion().GetSize()[0] / 500.0;
    // Registration will stop if the step length drops below this value
    double minStepLength =
        fixedImage_->GetLargestPossibleRegion().GetSize()[0] / 500000.0;

    // Optimizer step length is reduced by this factor each iteration
    double relaxationFactor = 0.85;

    // Hard iteration limit
    int numberOfIterations = atoi(iterationsIn);

    // The registration process will stop if the metric starts changing less
    // than this
    double gradientMagnitudeTolerance = 0.0001;

    /* The metric requires two parameters to be selected: the number
       of bins used to compute the entropy and the number of spatial samples
       used to compute the density estimates. In typical application, 50
       histogram bins are sufficient and the metric is relatively insensitive
       to changes in the number of bins. The number of spatial samples
       to be used depends on the content of the image. If the images are
       smooth and do not contain much detail, then using approximately
       1 percent of the pixels will do. On the other hand, if the images
       are detailed, it may be necessary to use a much higher proportion,
       such as 20 percent. */
    int numberOfHistogramBins = 50;
    auto numberOfSamples =
        static_cast<unsigned int>(fixedRegion.GetNumberOfPixels() / 80.0);

    /////////////////////////////////

    BSplineTransform::PhysicalDimensionsType FixedPhysicalDims;
    BSplineTransform::MeshSizeType MeshSize;
    BSplineTransform::OriginType FixedOrigin;

    for (unsigned int i = 0; i < 2; i++) {
        FixedOrigin[i] = fixedImage_->GetOrigin()[i];
        FixedPhysicalDims[i] =
            fixedImage_->GetSpacing()[i] *
            static_cast<double>(
                fixedImage_->GetLargestPossibleRegion().GetSize()[i] - 1);
    }

    MeshSize.Fill(transformMeshFillSize);

    deformTransform_->SetTransformDomainOrigin(FixedOrigin);
    deformTransform_->SetTransformDomainPhysicalDimensions(FixedPhysicalDims);
    deformTransform_->SetTransformDomainMeshSize(MeshSize);
    deformTransform_->SetTransformDomainDirection(fixedImage_->GetDirection());
}

void DeformableRegistration::clear_() {
    deformTransform_ = BSplineTransform::New();
}