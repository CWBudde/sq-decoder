#include "sqdecoder_controller.h"
#include "sqdecoder_processor.h"
#include "sqdecoder_shared.h"

#include "public.sdk/source/main/pluginfactory.h"

#define stringPluginName SQDecoder::kPluginName
#define stringVendorName SQDecoder::kVendorName
#define stringVendorUrl SQDecoder::kVendorUrl
#define stringVendorEmail SQDecoder::kVendorEmail
#define stringVersion SQDecoder::kVersionString

BEGIN_FACTORY_DEF(stringVendorName, stringVendorUrl, stringVendorEmail)

DEF_CLASS2(INLINE_UID_FROM_FUID(SQDecoder::kSQDecoderProcessorUID),
          Steinberg::PClassInfo::kManyInstances,
          kVstAudioEffectClass,
          stringPluginName,
          Steinberg::Vst::kDistributable,
          "Fx",
          stringVersion,
          kVstVersionString,
          SQDecoder::SQDecoderProcessor::createInstance)

DEF_CLASS2(INLINE_UID_FROM_FUID(SQDecoder::kSQDecoderControllerUID),
          Steinberg::PClassInfo::kManyInstances,
          kVstComponentControllerClass,
          stringPluginName " Controller",
          0,
          "",
          stringVersion,
          kVstVersionString,
          SQDecoder::SQDecoderController::createInstance)

END_FACTORY
