/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MacStringHelpers.h"
#include "nsObjCExceptions.h"

#include "mozilla/IntegerTypeTraits.h"
#include <limits>

namespace mozilla {

nsresult CopyCocoaStringToXPCOMString(NSString* aFrom, nsAString& aTo) {
  NS_OBJC_BEGIN_TRY_ABORT_BLOCK_NSRESULT;

  NSUInteger len = [aFrom length];
  if (len > std::numeric_limits<nsAString::size_type>::max()) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  if (!aTo.SetLength(len, mozilla::fallible)) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  [aFrom getCharacters:reinterpret_cast<unichar*>(aTo.BeginWriting()) range:NSMakeRange(0, len)];

  return NS_OK;

  NS_OBJC_END_TRY_ABORT_BLOCK_NSRESULT;
}

}  // namespace Mozilla
