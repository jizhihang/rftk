#pragma once

#include <cmath>

#include "asserts.h"
#include "VectorBuffer.h"
#include "MatrixBuffer.h"
#include "Tensor3Buffer.h"
#include "BufferCollection.h"
#include "BufferCollectionStack.h"
#include "BufferCollectionUtils.h"
#include "UniqueBufferId.h"
#include "ImageUtils.h"
#include "ScaledDepthDeltaFeatureBinding.h"

// ----------------------------------------------------------------------------
//
// ScaledDepthDeltaFeature is the depth delta of a pair of pixels
//
// ----------------------------------------------------------------------------
template <class BufferTypes>
class ScaledDepthDeltaFeature
{
public:
    ScaledDepthDeltaFeature( const BufferId& floatParamsBufferId,
                            const BufferId& intParamsBufferId,
                            const BufferId& indicesBufferId,
                            const BufferId& pixelIndicesBufferId,
                            const BufferId& scalesBufferId,
                            const BufferId& depthsDataBufferId );

    ScaledDepthDeltaFeature( const BufferId& floatParamsBufferId,
                            const BufferId& intParamsBufferId,
                            const BufferId& indicesBufferId,
                            const BufferId& pixelIndicesBufferId,
                            const BufferId& depthsDataBufferId );

    ScaledDepthDeltaFeature( const BufferId& indicesBufferId,
                            const BufferId& pixelIndicesBufferId,
                            const BufferId& depthsDataBufferId  );

    ~ScaledDepthDeltaFeature();

    ScaledDepthDeltaFeatureBinding<BufferTypes> Bind(const BufferCollectionStack& readCollection) const;

    void LogFeatureInfo(  const BufferCollectionStack& readCollection, int depth,
                          const int featureOffset, const double featureImpurity, const bool isSelectedFeature, 
                          BufferCollection& extraInfo) const;

    int FeatureIndex(const typename BufferTypes::ParamsContinuous x, 
                      const typename BufferTypes::ParamsContinuous y) const;

    typedef typename BufferTypes::FeatureValue Float;
    typedef typename BufferTypes::Index Int;
    typedef ScaledDepthDeltaFeatureBinding<BufferTypes> FeatureBinding;

    const BufferId mFloatParamsBufferId;
    const BufferId mIntParamsBufferId;
    const BufferId mIndicesBufferId;
    const BufferId mPixelIndicesBufferId;
    const BufferId mScalesBufferId;
    const BufferId mDepthsImgsBufferId;
};

template <class BufferTypes>
ScaledDepthDeltaFeature<BufferTypes>::ScaledDepthDeltaFeature( const BufferId& floatParamsBufferId,
                                                                      const BufferId& intParamsBufferId,
                                                                      const BufferId& indicesBufferId,
                                                                      const BufferId& pixelIndicesBufferId,
                                                                      const BufferId& depthsDataBufferId,
                                                                      const BufferId& scalesBufferId )
: mFloatParamsBufferId(floatParamsBufferId)
, mIntParamsBufferId(intParamsBufferId)
, mIndicesBufferId(indicesBufferId)
, mPixelIndicesBufferId(pixelIndicesBufferId)
, mScalesBufferId(scalesBufferId)
, mDepthsImgsBufferId(depthsDataBufferId)
{}

template <class BufferTypes>
ScaledDepthDeltaFeature<BufferTypes>::ScaledDepthDeltaFeature( const BufferId& floatParamsBufferId,
                                                                      const BufferId& intParamsBufferId,
                                                                      const BufferId& indicesBufferId,
                                                                      const BufferId& pixelIndicesBufferId,
                                                                      const BufferId& depthsDataBufferId )
: mFloatParamsBufferId(floatParamsBufferId)
, mIntParamsBufferId(intParamsBufferId)
, mIndicesBufferId(indicesBufferId)
, mPixelIndicesBufferId(pixelIndicesBufferId)
, mScalesBufferId(NullKey)
, mDepthsImgsBufferId(depthsDataBufferId)
{}


template <class BufferTypes>
ScaledDepthDeltaFeature<BufferTypes>::ScaledDepthDeltaFeature( const BufferId& indicesBufferId,
                                                                      const BufferId& pixelIndicesBufferId,
                                                                      const BufferId& depthsDataBufferId )
: mFloatParamsBufferId(GetBufferId("floatParams"))
, mIntParamsBufferId(GetBufferId("intParams"))
, mIndicesBufferId(indicesBufferId)
, mPixelIndicesBufferId(pixelIndicesBufferId)
, mScalesBufferId(NullKey)
, mDepthsImgsBufferId(depthsDataBufferId)
{}

template <class BufferTypes>
ScaledDepthDeltaFeature<BufferTypes>::~ScaledDepthDeltaFeature()
{}

template <class BufferTypes>
ScaledDepthDeltaFeatureBinding<BufferTypes> ScaledDepthDeltaFeature<BufferTypes>::Bind(const BufferCollectionStack& readCollection) const
{
    MatrixBufferTemplate<typename BufferTypes::ParamsContinuous> const* floatParams = 
        readCollection.GetBufferPtr< MatrixBufferTemplate<typename BufferTypes::ParamsContinuous> >(mFloatParamsBufferId);
    MatrixBufferTemplate<typename BufferTypes::ParamsInteger> const* intParams = 
        readCollection.GetBufferPtr< MatrixBufferTemplate<typename BufferTypes::ParamsInteger> >(mIntParamsBufferId);
    VectorBufferTemplate<typename BufferTypes::Index> const* indices = 
        readCollection.GetBufferPtr< VectorBufferTemplate<typename BufferTypes::Index> >(mIndicesBufferId);
    MatrixBufferTemplate<typename BufferTypes::Index> const* pixelIndices = 
        readCollection.GetBufferPtr< MatrixBufferTemplate<typename BufferTypes::Index> >(mPixelIndicesBufferId);
    Tensor3BufferTemplate<typename BufferTypes::SourceContinuous> const* depthImgs = 
        readCollection.GetBufferPtr< Tensor3BufferTemplate<typename BufferTypes::SourceContinuous> >(mDepthsImgsBufferId);
    
    MatrixBufferTemplate<typename BufferTypes::SourceContinuous> const* scales = NULL;
    if( mScalesBufferId != NullKey )
    {
        scales = readCollection.GetBufferPtr< MatrixBufferTemplate<typename BufferTypes::SourceContinuous> >(mScalesBufferId);
    }

    ASSERT_ARG_DIM_1D(floatParams->GetN(), intParams->GetN());

    return ScaledDepthDeltaFeatureBinding<BufferTypes>(floatParams, intParams, indices, pixelIndices, depthImgs, scales);
}


template <class BufferTypes>
void ScaledDepthDeltaFeature<BufferTypes>::LogFeatureInfo( const BufferCollectionStack& readCollection, int depth,
                                                          const int featureOffset, const double featureImpurity, const bool isSelectedFeature, 
                                                          BufferCollection& extraInfo) const
{
    UNUSED_PARAM(depth);
    
    MatrixBufferTemplate<typename BufferTypes::ParamsContinuous> const* floatParams = 
            readCollection.GetBufferPtr< MatrixBufferTemplate<typename BufferTypes::ParamsContinuous> >(mFloatParamsBufferId);

    const typename BufferTypes::ParamsContinuous um = floatParams->Get(featureOffset,FEATURE_SPECIFIC_PARAMS_START);
    const typename BufferTypes::ParamsContinuous un = floatParams->Get(featureOffset,FEATURE_SPECIFIC_PARAMS_START+1);
    const typename BufferTypes::ParamsContinuous vm = floatParams->Get(featureOffset,FEATURE_SPECIFIC_PARAMS_START+2);
    const typename BufferTypes::ParamsContinuous vn = floatParams->Get(featureOffset,FEATURE_SPECIFIC_PARAMS_START+3);

    int uIndex = FeatureIndex(um, un);
    int vIndex = FeatureIndex(vm, vn);
    int deltaIndex = FeatureIndex(um-vm, un-vn);

    IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-Sampled-U", uIndex, 1.0);
    IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-Sampled-V", vIndex, 1.0);
    IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-Sampled-Delta", deltaIndex, 1.0);

    IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-ImpuritySampled-U", uIndex, featureImpurity);
    IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-ImpuritySampled-V", vIndex, featureImpurity);
    IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-ImpuritySampled-Delta", deltaIndex, featureImpurity);

    if(isSelectedFeature)
    {
        IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-Selected-U", uIndex, 1.0);
        IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-Selected-V", vIndex, 1.0);
        IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-Selected-Delta", deltaIndex, 1.0);

        IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-ImpuritySelected-U", uIndex, featureImpurity);
        IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-ImpuritySelected-V", vIndex, featureImpurity);
        IncrementValue<double>(extraInfo, "ScaledDepthDeltaFeature-ImpuritySelected-Delta", deltaIndex, featureImpurity);
    }
}

template <class BufferTypes>
int ScaledDepthDeltaFeature<BufferTypes>::FeatureIndex(const typename BufferTypes::ParamsContinuous x, 
                                                        const typename BufferTypes::ParamsContinuous y) const
{
    const double resolution = 50.0;
    int xIndex = int(std::max(0.0, std::min(log2(x)*(resolution/20.0) + (resolution/2.0), resolution)));
    int yIndex = int(std::max(0.0, std::min(log2(y)*(resolution/20.0) + (resolution/2.0), resolution)) * resolution);
    return xIndex + yIndex;
}
