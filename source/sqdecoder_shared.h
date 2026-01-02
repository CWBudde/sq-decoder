#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace SQDecoder {

constexpr const char* kPluginName = "SQ Decoder";
constexpr const char* kVendorName = "Local";
constexpr const char* kVendorUrl = "https://example.com";
constexpr const char* kVendorEmail = "dev@example.com";
constexpr const char* kVersionString = "1.0.0";

constexpr int32 kParamSeparation = 0;

static const Steinberg::FUID kSQDecoderProcessorUID(0x5B1C6F7A, 0x0A7E4D6D, 0xA2D3A0B5, 0x9D4B7C2F);
static const Steinberg::FUID kSQDecoderControllerUID(0x8D9A24B1, 0x1F7B4E9A, 0xB3C8D6F0, 0x2A11C44E);

} // namespace SQDecoder
