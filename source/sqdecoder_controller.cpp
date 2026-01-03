#include "sqdecoder_controller.h"

#include "pluginterfaces/base/ibstream.h"

namespace SQDecoder {

using namespace Steinberg;
using namespace Steinberg::Vst;

Steinberg::tresult PLUGIN_API SQDecoderController::setState(Steinberg::IBStream* state)
{
  return kResultOk;
}

Steinberg::tresult PLUGIN_API SQDecoderController::getState(Steinberg::IBStream* state)
{
  return kResultOk;
}

} // namespace SQDecoder
