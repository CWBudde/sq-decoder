#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace SQDecoder {

class SQDecoderProcessor : public Steinberg::Vst::AudioEffect {
public:
  SQDecoderProcessor();

  static Steinberg::FUnknown* createInstance(void* /*context*/)
  {
    return static_cast<Steinberg::Vst::IAudioProcessor*>(new SQDecoderProcessor());
  }

  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
  Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override;
  Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override;
  Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override;

private:
  void applyParameterChanges(Steinberg::Vst::ProcessData& data);

  float separation_ = 1.0f;
};

} // namespace SQDecoder
