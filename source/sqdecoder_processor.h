#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

#include <memory>

namespace SQDecoder {

class HilbertTransformer;

class SQDecoderProcessor : public Steinberg::Vst::AudioEffect {
public:
  SQDecoderProcessor();

  static Steinberg::FUnknown* createInstance(void* /*context*/)
  {
    return static_cast<Steinberg::Vst::IAudioProcessor*>(new SQDecoderProcessor());
  }

  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
  Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override;
  Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override;
  Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override;
  Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override;

private:
  std::unique_ptr<HilbertTransformer> hilbert_left_;
  std::unique_ptr<HilbertTransformer> hilbert_right_;
};

} // namespace SQDecoder
