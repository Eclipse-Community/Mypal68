/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Accessible-inl.h"
#include "nsNetUtil.h"

using namespace mozilla::a11y;

NS_IMETHODIMP
xpcAccessibleHyperLink::GetStartIndex(int32_t* aStartIndex) {
  NS_ENSURE_ARG_POINTER(aStartIndex);
  *aStartIndex = 0;

  if (Intl().IsNull()) return NS_ERROR_FAILURE;

  if (Intl().IsAccessible()) {
    *aStartIndex = Intl().AsAccessible()->StartOffset();
  } else {
#if defined(XP_WIN)
    return NS_ERROR_NOT_IMPLEMENTED;
#else
    bool isIndexValid = false;
    uint32_t startOffset = Intl().AsProxy()->StartOffset(&isIndexValid);
    if (!isIndexValid) return NS_ERROR_FAILURE;

    *aStartIndex = startOffset;
#endif
  }

  return NS_OK;
}

NS_IMETHODIMP
xpcAccessibleHyperLink::GetEndIndex(int32_t* aEndIndex) {
  NS_ENSURE_ARG_POINTER(aEndIndex);
  *aEndIndex = 0;

  if (Intl().IsNull()) return NS_ERROR_FAILURE;

  if (Intl().IsAccessible()) {
    *aEndIndex = Intl().AsAccessible()->EndOffset();
  } else {
#if defined(XP_WIN)
    return NS_ERROR_NOT_IMPLEMENTED;
#else
    bool isIndexValid = false;
    uint32_t endOffset = Intl().AsProxy()->EndOffset(&isIndexValid);
    if (!isIndexValid) return NS_ERROR_FAILURE;

    *aEndIndex = endOffset;
#endif
  }

  return NS_OK;
}

NS_IMETHODIMP
xpcAccessibleHyperLink::GetAnchorCount(int32_t* aAnchorCount) {
  NS_ENSURE_ARG_POINTER(aAnchorCount);
  *aAnchorCount = 0;

  if (Intl().IsNull()) return NS_ERROR_FAILURE;

  if (Intl().IsAccessible()) {
    *aAnchorCount = Intl().AsAccessible()->AnchorCount();
  } else {
#if defined(XP_WIN)
    return NS_ERROR_NOT_IMPLEMENTED;
#else
    bool isCountValid = false;
    uint32_t anchorCount = Intl().AsProxy()->AnchorCount(&isCountValid);
    if (!isCountValid) return NS_ERROR_FAILURE;

    *aAnchorCount = anchorCount;
#endif
  }

  return NS_OK;
}

NS_IMETHODIMP
xpcAccessibleHyperLink::GetURI(int32_t aIndex, nsIURI** aURI) {
  NS_ENSURE_ARG_POINTER(aURI);

  if (Intl().IsNull()) return NS_ERROR_FAILURE;

  if (aIndex < 0) return NS_ERROR_INVALID_ARG;

  if (Intl().IsAccessible()) {
    if (aIndex >= static_cast<int32_t>(Intl().AsAccessible()->AnchorCount()))
      return NS_ERROR_INVALID_ARG;

    RefPtr<nsIURI>(Intl().AsAccessible()->AnchorURIAt(aIndex)).forget(aURI);
  } else {
#if defined(XP_WIN)
    return NS_ERROR_NOT_IMPLEMENTED;
#else
    nsCString spec;
    bool isURIValid = false;
    Intl().AsProxy()->AnchorURIAt(aIndex, spec, &isURIValid);
    if (!isURIValid) return NS_ERROR_FAILURE;

    nsCOMPtr<nsIURI> uri;
    nsresult rv = NS_NewURI(getter_AddRefs(uri), spec);
    NS_ENSURE_SUCCESS(rv, rv);

    uri.forget(aURI);
#endif
  }

  return NS_OK;
}

NS_IMETHODIMP
xpcAccessibleHyperLink::GetAnchor(int32_t aIndex, nsIAccessible** aAccessible) {
  NS_ENSURE_ARG_POINTER(aAccessible);
  *aAccessible = nullptr;

  if (Intl().IsNull()) return NS_ERROR_FAILURE;

  if (aIndex < 0) return NS_ERROR_INVALID_ARG;

  if (Intl().IsAccessible()) {
    if (aIndex >= static_cast<int32_t>(Intl().AsAccessible()->AnchorCount()))
      return NS_ERROR_INVALID_ARG;

    NS_IF_ADDREF(*aAccessible = ToXPC(Intl().AsAccessible()->AnchorAt(aIndex)));
  } else {
#if defined(XP_WIN)
    return NS_ERROR_NOT_IMPLEMENTED;
#else
    NS_IF_ADDREF(*aAccessible = ToXPC(Intl().AsProxy()->AnchorAt(aIndex)));
#endif
  }

  return NS_OK;
}

NS_IMETHODIMP
xpcAccessibleHyperLink::GetValid(bool* aValid) {
  NS_ENSURE_ARG_POINTER(aValid);
  *aValid = false;

  if (Intl().IsNull()) return NS_ERROR_FAILURE;

  if (Intl().IsAccessible()) {
    *aValid = Intl().AsAccessible()->IsLinkValid();
  } else {
#if defined(XP_WIN)
    return NS_ERROR_NOT_IMPLEMENTED;
#else
    *aValid = Intl().AsProxy()->IsLinkValid();
#endif
  }

  return NS_OK;
}
