#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace SQDecoder {

class SQDecoderController : public Steinberg::Vst::EditController {
public:
  SQDecoderController() = default;

  static Steinberg::FUnknown* createInstance(void* /*context*/)
  {
    return static_cast<Steinberg::Vst::IEditController*>(new SQDecoderController());
  }

  Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override;
  Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override;
};

} // namespace SQDecoder
