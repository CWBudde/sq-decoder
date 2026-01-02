#include "sqdecoder_controller.h"

#include "sqdecoder_shared.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"

namespace SQDecoder {

using namespace Steinberg;
using namespace Steinberg::Vst;

Steinberg::tresult PLUGIN_API SQDecoderController::initialize(Steinberg::FUnknown* context)
{
  auto result = EditController::initialize(context);
  if (result != kResultOk) {
    return result;
  }

  auto* param = parameters.addParameter(STR16("Separation"), STR16("%"), 0, 1.0,
                                        ParameterInfo::kCanAutomate, kParamSeparation);
  if (param) {
    param->setNormalized(1.0);
  }

  return kResultOk;
}

Steinberg::tresult PLUGIN_API SQDecoderController::setState(Steinberg::IBStream* state)
{
  if (!state) {
    return kResultFalse;
  }

  IBStreamer streamer(state, kLittleEndian);
  float sep = 1.0f;
  if (streamer.readFloat(sep)) {
    setParamNormalized(kParamSeparation, sep);
  }

  return kResultOk;
}

Steinberg::tresult PLUGIN_API SQDecoderController::getState(Steinberg::IBStream* state)
{
  if (!state) {
    return kResultFalse;
  }

  IBStreamer streamer(state, kLittleEndian);
  streamer.writeFloat(static_cast<float>(getParamNormalized(kParamSeparation)));

  return kResultOk;
}

} // namespace SQDecoder
