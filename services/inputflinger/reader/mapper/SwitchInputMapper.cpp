/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../Macros.h"

#include "SwitchInputMapper.h"

namespace android {

SwitchInputMapper::SwitchInputMapper(InputDeviceContext& deviceContext,
                                     const InputReaderConfiguration& readerConfig)
      : InputMapper(deviceContext, readerConfig), mSwitchValues(0), mUpdatedSwitchMask(0) {}

SwitchInputMapper::~SwitchInputMapper() {}

uint32_t SwitchInputMapper::getSources() const {
    return AINPUT_SOURCE_SWITCH;
}

std::list<NotifyArgs> SwitchInputMapper::process(const RawEvent& rawEvent) {
    std::list<NotifyArgs> out;
    switch (rawEvent.type) {
        case EV_SW:
            processSwitch(rawEvent.code, rawEvent.value);
            break;

        case EV_SYN:
            if (rawEvent.code == SYN_REPORT) {
                out += sync(rawEvent.when);
            }
    }
    return out;
}

void SwitchInputMapper::processSwitch(int32_t switchCode, int32_t switchValue) {
    if (switchCode >= 0 && switchCode < 32) {
        if (switchValue) {
            mSwitchValues |= 1 << switchCode;
        } else {
            mSwitchValues &= ~(1 << switchCode);
        }
        mUpdatedSwitchMask |= 1 << switchCode;
    }
}

std::list<NotifyArgs> SwitchInputMapper::sync(nsecs_t when) {
    std::list<NotifyArgs> out;
    if (mUpdatedSwitchMask) {
        uint32_t updatedSwitchValues = mSwitchValues & mUpdatedSwitchMask;
        out.push_back(NotifySwitchArgs(getContext()->getNextId(), when, /*policyFlags=*/0,
                                       updatedSwitchValues, mUpdatedSwitchMask));

        mUpdatedSwitchMask = 0;
    }
    return out;
}

int32_t SwitchInputMapper::getSwitchState(uint32_t sourceMask, int32_t switchCode) {
    return getDeviceContext().getSwitchState(switchCode);
}

void SwitchInputMapper::dump(std::string& dump) {
    dump += INDENT2 "Switch Input Mapper:\n";
    dump += StringPrintf(INDENT3 "SwitchValues: %x\n", mSwitchValues);
}

} // namespace android
