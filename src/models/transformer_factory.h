#pragma once

#include "marian.h"

#include "models/decoder.h"
#include "models/encoder.h"
//#include "models/states.h"
//#include "layers/constructors.h"
//#include "layers/factory.h"

namespace marian {
Ptr<EncoderBase> NewEncoderTransformer(Ptr<Options> options);
Ptr<DecoderBase> NewDecoderTransformer(Ptr<Options> options);
}  // namespace marian

#ifndef _WIN32  // TODO: remove this once I updated the Linux-side makefile
#include "models/transformer.h"
#endif