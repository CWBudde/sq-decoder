#include "sqdecoder_processor.h"

#include "sqdecoder_shared.h"

#include "pluginterfaces/vst/vstaudioprocessor.h"

#include "otfftpp/otfft.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace SQDecoder {

using namespace Steinberg;
using namespace Steinberg::Vst;

constexpr int kBlockSize = 1024;
constexpr int kHopSize = 512;
constexpr double kSqrt2Over2 = 0.7071067811865475;
constexpr double kPi = 3.14159265358979323846;

class HilbertTransformer {
public:
  HilbertTransformer()
      : fft_(kBlockSize),
        window_(kBlockSize),
        input_block_(kBlockSize, 0.0),
        output_accum_(kBlockSize, 0.0),
        fft_buffer_(kBlockSize),
        out_queue_(kBlockSize, 0.0),
        out_head_(0),
        out_tail_(0),
        out_count_(0),
        in_fill_(0)
  {
    buildWindow();
  }

  void reset()
  {
    std::fill(input_block_.begin(), input_block_.end(), 0.0);
    std::fill(output_accum_.begin(), output_accum_.end(), 0.0);
    out_head_ = 0;
    out_tail_ = 0;
    out_count_ = 0;
    in_fill_ = 0;
  }

  double processSample(double input)
  {
    if (in_fill_ < kBlockSize) {
      input_block_[in_fill_++] = input;
    }

    if (in_fill_ == kBlockSize) {
      processBlock();
      std::move(input_block_.begin() + kHopSize, input_block_.end(), input_block_.begin());
      std::fill(input_block_.end() - kHopSize, input_block_.end(), 0.0);
      in_fill_ = kBlockSize - kHopSize;
    }

    if (out_count_ > 0) {
      double output = out_queue_[out_head_];
      out_head_ = (out_head_ + 1) % out_queue_.size();
      --out_count_;
      return output;
    }

    return 0.0;
  }

private:
  void buildWindow()
  {
    for (int i = 0; i < kBlockSize; ++i) {
      const double phase = 2.0 * kPi * static_cast<double>(i) / static_cast<double>(kBlockSize - 1);
      const double hann = 0.5 - 0.5 * std::cos(phase);
      window_[i] = std::sqrt(hann);
    }
  }

  void processBlock()
  {
    for (int i = 0; i < kBlockSize; ++i) {
      fft_buffer_[i].Re = input_block_[i] * window_[i];
      fft_buffer_[i].Im = 0.0;
    }

    fft_.fwd(fft_buffer_.data());

    fft_buffer_[0].Re = 0.0;
    fft_buffer_[0].Im = 0.0;

    const int nyquist = kBlockSize / 2;
    fft_buffer_[nyquist].Re = 0.0;
    fft_buffer_[nyquist].Im = 0.0;

    for (int i = 1; i < nyquist; ++i) {
      const double re = fft_buffer_[i].Re;
      const double im = fft_buffer_[i].Im;
      fft_buffer_[i].Re = im;
      fft_buffer_[i].Im = -re;
    }

    for (int i = nyquist + 1; i < kBlockSize; ++i) {
      const double re = fft_buffer_[i].Re;
      const double im = fft_buffer_[i].Im;
      fft_buffer_[i].Re = -im;
      fft_buffer_[i].Im = re;
    }

    fft_.inv(fft_buffer_.data());

    for (int i = 0; i < kBlockSize; ++i) {
      output_accum_[i] += fft_buffer_[i].Re * window_[i];
    }

    for (int i = 0; i < kHopSize; ++i) {
      pushOutput(output_accum_[i]);
    }

    std::move(output_accum_.begin() + kHopSize, output_accum_.end(), output_accum_.begin());
    std::fill(output_accum_.end() - kHopSize, output_accum_.end(), 0.0);
  }

  void pushOutput(double value)
  {
    if (out_count_ >= static_cast<int>(out_queue_.size())) {
      return;
    }

    out_queue_[out_tail_] = value;
    out_tail_ = (out_tail_ + 1) % out_queue_.size();
    ++out_count_;
  }

  OTFFT::FFT fft_;
  std::vector<double> window_;
  std::vector<double> input_block_;
  std::vector<double> output_accum_;
  std::vector<OTFFT::complex_t> fft_buffer_;
  std::vector<double> out_queue_;
  size_t out_head_;
  size_t out_tail_;
  int out_count_;
  int in_fill_;
};

SQDecoderProcessor::SQDecoderProcessor()
{
  setControllerClass(kSQDecoderControllerUID);
  hilbert_left_ = std::make_unique<HilbertTransformer>();
  hilbert_right_ = std::make_unique<HilbertTransformer>();
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::initialize(Steinberg::FUnknown* context)
{
  auto result = AudioEffect::initialize(context);
  if (result != kResultOk) {
    return result;
  }

  addAudioInput(STR16("Input"), SpeakerArr::kStereo);
  addAudioOutput(STR16("Output"), SpeakerArr::kQuadraphonic);
  setLatencySamples(kBlockSize);

  return kResultOk;
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::setActive(Steinberg::TBool state)
{
  if (state && hilbert_left_ && hilbert_right_) {
    hilbert_left_->reset();
    hilbert_right_->reset();
  }

  return AudioEffect::setActive(state);
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::setState(Steinberg::IBStream* state)
{
  (void)state;
  return kResultOk;
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::getState(Steinberg::IBStream* state)
{
  (void)state;
  return kResultOk;
}

template <typename SampleType>
static void processSamples(const SampleType* inL,
                           const SampleType* inR,
                           SampleType* outLF,
                           SampleType* outRF,
                           SampleType* outLB,
                           SampleType* outRB,
                           int32 sampleFrames,
                           HilbertTransformer& hilbertL,
                           HilbertTransformer& hilbertR)
{
  for (int32 i = 0; i < sampleFrames; ++i) {
    const double lt = static_cast<double>(inL[i]);
    const double rt = static_cast<double>(inR[i]);
    const double hlt = hilbertL.processSample(lt);
    const double hrt = hilbertR.processSample(rt);

    const double lf = lt;
    const double rf = rt;
    const double lb = kSqrt2Over2 * (hlt - rt);
    const double rb = kSqrt2Over2 * (lt - hrt);

    outLF[i] = static_cast<SampleType>(lf);
    outRF[i] = static_cast<SampleType>(rf);
    outLB[i] = static_cast<SampleType>(lb);
    outRB[i] = static_cast<SampleType>(rb);
  }
}

Steinberg::tresult PLUGIN_API SQDecoderProcessor::process(Steinberg::Vst::ProcessData& data)
{
  if (!hilbert_left_ || !hilbert_right_) {
    return kResultOk;
  }

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

    processSamples(in[0], in[1], out[0], out[1], out[2], out[3], data.numSamples,
                   *hilbert_left_, *hilbert_right_);
    return kResultOk;
  }

  if (data.symbolicSampleSize == kSample64) {
    auto** in = data.inputs[0].channelBuffers64;
    auto** out = data.outputs[0].channelBuffers64;

    processSamples(in[0], in[1], out[0], out[1], out[2], out[3], data.numSamples,
                   *hilbert_left_, *hilbert_right_);
    return kResultOk;
  }

  return kResultOk;
}

} // namespace SQDecoder
