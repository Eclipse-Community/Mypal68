/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/WebGPUBinding.h"
#include "ShaderModule.h"

#include "Device.h"

namespace mozilla {
namespace webgpu {

ShaderModule::~ShaderModule() = default;

GPU_IMPL_CYCLE_COLLECTION(ShaderModule, mParent)
GPU_IMPL_JS_WRAP(ShaderModule)

}  // namespace webgpu
}  // namespace mozilla
