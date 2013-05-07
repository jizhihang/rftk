#pragma once

#include <set>
#include <utility>
#include <vector>
#include <limits>

#include <Forest.h>

#include "ActiveSplitNode.h"
#include "TrainConfigParams.h"
#include "OnlineSamplingParams.h"
#include "ProbabilityOfErrorFrontierQueue.h"


class OnlineForestLearner
{
public:

    OnlineForestLearner( const TrainConfigParams& trainConfigParams,
                          const OnlineSamplingParams& samplingParams,
                          const unsigned int maxFrontierSize,
                          const int maxDepth = std::numeric_limits<float>::max());
    ~OnlineForestLearner();

    Forest GetForest() const;
    void Train(BufferCollection data, Int32VectorBuffer indices );

private:
    void UpdateActiveFrontier();

    TrainConfigParams mTrainConfigParams;
    OnlineSamplingParams mOnlineSamplingParams;
    unsigned int mMaxFrontierSize;
    int mMaxDepth;

    Forest mForest;
    ProbabilityOfErrorFrontierQueue mFrontierQueue;
    std::map< std::pair<int, int>, ActiveSplitNode* > mActiveFrontierLeaves;
};

