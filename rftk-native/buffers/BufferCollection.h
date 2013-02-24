#pragma once

#include <string>
#include <map>
#include <typeinfo>
#include <boost/any.hpp>

#include "VectorBuffer.h"
#include "MatrixBuffer.h"
#include "Tensor3Buffer.h"

//Using #define for compatibility with swig
#define X_FLOAT_DATA    "X_Float"
#define SAMPLE_WEIGHTS  "SampleWeights"
#define CLASS_LABELS    "ClassLabels"
#define FEATURE_VALUES  "Feature_Values"
#define HISTOGRAM_LEFT            "Histogram_Left"
#define HISTOGRAM_RIGHT           "Histogram_Right"
#define THRESHOLDS                "Thresholds"
#define THRESHOLD_COUNTS          "ThresholdCounts"
#define IMPURITY_HISTOGRAM_LEFT   "Impurity_Histogram_Left"
#define IMPURITY_HISTOGRAM_RIGHT  "Impurity_Histogram_Right"
#define YS_HISTOGRAM_LEFT         "Ys_Histogram_Left"
#define YS_HISTOGRAM_RIGHT        "Ys_Histogram_Right"

#define PIXEL_INDICES   "PixelIndices"
#define DEPTH_IMAGES    "DepthImages"
#define OFFSET_SCALES   "OffsetScales"

class BufferCollection
{
public:
#define DECLARE_BUFFER_LEGACY_INTERFACE_FOR_TYPE(BUFFER_TYPE) \
bool Has ## BUFFER_TYPE(std::string name) const; \
void Add ## BUFFER_TYPE(std::string name, BUFFER_TYPE const& data ); \
void Append ## BUFFER_TYPE(std::string name, BUFFER_TYPE const& data ); \
const BUFFER_TYPE& Get ## BUFFER_TYPE(const std::string& name) const; \
BUFFER_TYPE& Get ## BUFFER_TYPE(const std::string& name);

    DECLARE_BUFFER_LEGACY_INTERFACE_FOR_TYPE(Float32VectorBuffer)
    DECLARE_BUFFER_LEGACY_INTERFACE_FOR_TYPE(Int32VectorBuffer)
    DECLARE_BUFFER_LEGACY_INTERFACE_FOR_TYPE(Float32MatrixBuffer)
    DECLARE_BUFFER_LEGACY_INTERFACE_FOR_TYPE(Int32MatrixBuffer)
    DECLARE_BUFFER_LEGACY_INTERFACE_FOR_TYPE(Float32Tensor3Buffer)
    DECLARE_BUFFER_LEGACY_INTERFACE_FOR_TYPE(Int32Tensor3Buffer)

#undef DECLARE_BUFFER_LEGACY_INTERFACE_FOR_TYPE

    bool HasBuffer(std::string name) const;

    template<typename BufferType>
    void AddBuffer(std::string name, BufferType const& data);
    template<typename BufferType>
    void AppendBuffer(std::string name, BufferType const& buffer);
    template<typename BufferType>
    BufferType const& GetBuffer(std::string name) const;
    template<typename BufferType>
    BufferType& GetBuffer(std::string name);

private:
    // Checks for a buffer of a specific type.
    //
    // Remove this when the transition to the templated interface is complete.
    // Right now it's being used to check for violated assumptions during the
    // transition.
    template<typename BufferType>
    bool HasBuffer(std::string name) const {
        if (HasBuffer(name)) {
            BufferMapType::const_iterator bufferIter = mBuffers.find(name);
            return bufferIter->second.type() == typeid(BufferType);
        }
        return false;
    }

private:
    typedef std::map<std::string, boost::any> BufferMapType;

    BufferMapType mBuffers;
};


template<typename BufferType>
void BufferCollection::AddBuffer(std::string name, BufferType const& buffer)
{
    mBuffers[name] = boost::any(buffer);
}

template<typename BufferType>
void BufferCollection::AppendBuffer(std::string name, BufferType const& buffer)
{
    if (!HasBuffer(name)) {
        AddBuffer(name, buffer);
    }
    else {
        GetBuffer<BufferType>(name).Append(buffer);
    }
}

template<typename BufferType>
BufferType const& BufferCollection::GetBuffer(std::string name) const
{
    ASSERT(HasBuffer(name));
    BufferMapType::const_iterator bufferIter = mBuffers.find(name);
    // use pointers so any_cast doesn't copy the buffer
    return *boost::any_cast<BufferType>(&bufferIter->second);
}

template<typename BufferType>
BufferType& BufferCollection::GetBuffer(std::string name)
{
    ASSERT(HasBuffer(name));
    BufferMapType::iterator bufferIter = mBuffers.find(name);
    // use pointers so any_cast doesn't copy the buffer
    return *boost::any_cast<BufferType>(&bufferIter->second);
}

