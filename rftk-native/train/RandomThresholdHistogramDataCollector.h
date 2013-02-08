#pragma once

#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <vector>
#include <set>

#include "MatrixBuffer.h"
#include "Tensor3Buffer.h"
#include "BufferCollection.h"
#include "NodeDataCollectorI.h"


// Subsampling could live in here along with a max cap
class RandomThresholdHistogramDataCollector : public NodeDataCollectorI
{
public:
    RandomThresholdHistogramDataCollector(  int numberOfClasses,
                                            int numberOfThresholds,
                                            float probabilityOfNullStream);
    virtual ~RandomThresholdHistogramDataCollector();

    // Also copies/compacts weights, ys, etc
    virtual void Collect( const BufferCollection& data,
                          const Int32VectorBuffer& sampleIndices,
                          const Float32MatrixBuffer& featureValues,
                          boost::mt19937& gen );

    // Includes feature values, weights, ys, etc
    virtual const BufferCollection& GetCollectedData();

    virtual int GetNumberOfCollectedSamples();

protected:
    void UpdateThresholds(const Float32MatrixBuffer& featureValues);
    int mNumberOfThresholdSamples;
    int mNumberOfCollectedSamples;
    int mNumberOfClasses;
    int mNumberOfThresholds;
    float mProbabilityOfNullStream;
    Float32MatrixBuffer mThresholds;
    BufferCollection mData; // #features x #thresholds x #class
};

class RandomThresholdHistogramDataCollectorFactory : public NodeDataCollectorFactoryI
{
public:
    RandomThresholdHistogramDataCollectorFactory(int numberOfClasses, int numberOfThresholds, float probabilityOfNullStream);
    virtual ~RandomThresholdHistogramDataCollectorFactory();
    virtual NodeDataCollectorFactoryI* Clone() const;
    virtual NodeDataCollectorI* Create() const;

private:
    int mNumberOfClasses;
    int mNumberOfThresholds;
    float mProbabilityOfNullStream;
};
