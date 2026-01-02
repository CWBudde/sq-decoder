#include "sqdecoder_processor.h"

#include "sqdecoder_shared.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/vstaudioprocessor.h"

namespace SQDecoder {

using namespace Steinberg;
using namespace Steinberg::Vst;

SQDecoderProcessor::SQDecoderProcessor()
{
  setControllerClass(kSQDecoderControllerUID);
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::initialize(Steinberg::FUnknown* context)
{
  auto result = AudioEffect::initialize(context);
  if (result != kResultOk) {
    return result;
  }

  addAudioInput(STR16("Input"), SpeakerArr::kStereo);
  addAudioOutput(STR16("Output"), SpeakerArr::kQuadraphonic);

  return kResultOk;
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::setState(Steinberg::IBStream* state)
{
  if (!state) {
    return kResultFalse;
  }

  IBStreamer streamer(state, kLittleEndian);
  float sep = 1.0f;
  if (streamer.readFloat(sep)) {
    separation_ = sep;
  }

  return kResultOk;
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::getState(Steinberg::IBStream* state)
{
  if (!state) {
    return kResultFalse;
  }

  IBStreamer streamer(state, kLittleEndian);
  streamer.writeFloat(separation_);

  return kResultOk;
}

void SQDecoderProcessor::applyParameterChanges(ProcessData& data)
{
  IParameterChanges* changes = data.inputParameterChanges;
  if (!changes) {
    return;
  }

  const int32 count = changes->getParameterCount();
  for (int32 i = 0; i < count; ++i) {
    IParamValueQueue* queue = changes->getParameterData(i);
    if (!queue) {
      continue;
    }

    if (queue->getParameterId() != kParamSeparation) {
      continue;
    }

    int32 sampleOffset = 0;
    ParamValue value = 0.0;
    if (queue->getPoint(queue->getPointCount() - 1, sampleOffset, value) == kResultTrue) {
      separation_ = static_cast<float>(value);
    }
  }
}

template <typename SampleType>
static void processSamples(const SampleType* inL,
                           const SampleType* inR,
                           SampleType* outLF,
                           SampleType* outRF,
                           SampleType* outLR,
                           SampleType* outRR,
                           int32 sampleFrames,
                           float separation)
{
  const SampleType k = static_cast<SampleType>(0.70710678f);
  const SampleType sep = static_cast<SampleType>(separation);

  for (int32 i = 0; i < sampleFrames; ++i) {
    const SampleType l = inL[i];
    const SampleType r = inR[i];

    const SampleType lf0 = l;
    const SampleType rf0 = r;
    const SampleType lr0 = l;
    const SampleType rr0 = r;

    const SampleType lf = l + k * r;
    const SampleType rf = r + k * l;
    const SampleType lr = l - k * r;
    const SampleType rr = r - k * l;

    outLF[i] = lf0 + sep * (lf - lf0);
    outRF[i] = rf0 + sep * (rf - rf0);
    outLR[i] = lr0 + sep * (lr - lr0);
    outRR[i] = rr0 + sep * (rr - rr0);
  }
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::process(Steinberg::Vst::ProcessData& data)
{
  applyParameterChanges(data);

  if (data.numInputs < 1 || data.numOutputs < 1) {
    return kResultOk;
  }

  if (data.inputs[0].numChannels < 2 || data.outputs[0].numChannels < 4) {
    return kResultOk;
  }

  if (data.numSamples <= 0) {
    return kResultOk;
  }

  if (data.symbolicSampleSize == kSample32) {
    auto** in = data.inputs[0].channelBuffers32;
    auto** out = data.outputs[0].channelBuffers32;

    processSamples(in[0], in[1], out[0], out[1], out[2], out[3], data.numSamples, separation_);
    return kResultOk;
  }

  if (data.symbolicSampleSize == kSample64) {
    auto** in = data.inputs[0].channelBuffers64;
    auto** out = data.outputs[0].channelBuffers64;

    processSamples(in[0], in[1], out[0], out[1], out[2], out[3], data.numSamples, separation_);
    return kResultOk;
  }

  return kResultOk;
}

} // namespace SQDecoder
