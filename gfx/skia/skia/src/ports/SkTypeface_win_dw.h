/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkTypeface_win_dw_DEFINED
#define SkTypeface_win_dw_DEFINED

#include "SkAdvancedTypefaceMetrics.h"
#include "SkDWrite.h"
#include "SkHRESULT.h"
#include "SkLeanWindows.h"
#include "SkTScopedComPtr.h"
#include "SkTypeface.h"
#include "SkTypefaceCache.h"

#include <dwrite.h>
#include <dwrite_1.h>
#include <dwrite_2.h>

class SkFontDescriptor;
struct SkScalerContextRec;

static SkFontStyle get_style(IDWriteFont* font) {
    int weight = font->GetWeight();
    int width = font->GetStretch();
    SkFontStyle::Slant slant = SkFontStyle::kUpright_Slant;
    switch (font->GetStyle()) {
        case DWRITE_FONT_STYLE_NORMAL: slant = SkFontStyle::kUpright_Slant; break;
        case DWRITE_FONT_STYLE_OBLIQUE: slant = SkFontStyle::kOblique_Slant; break;
        case DWRITE_FONT_STYLE_ITALIC: slant = SkFontStyle::kItalic_Slant; break;
        default: SkASSERT(false); break;
    }
    return SkFontStyle(weight, width, slant);
}

class DWriteFontTypeface : public SkTypeface {
private:
    DWriteFontTypeface(const SkFontStyle& style,
                       IDWriteFactory* factory,
                       IDWriteFontFace* fontFace,
                       IDWriteFont* font = nullptr,
                       IDWriteFontFamily* fontFamily = nullptr,
                       IDWriteFontFileLoader* fontFileLoader = nullptr,
                       IDWriteFontCollectionLoader* fontCollectionLoader = nullptr)
        : SkTypeface(style, false)
        , fFactory(SkRefComPtr(factory))
        , fDWriteFontCollectionLoader(SkSafeRefComPtr(fontCollectionLoader))
        , fDWriteFontFileLoader(SkSafeRefComPtr(fontFileLoader))
        , fDWriteFontFamily(SkSafeRefComPtr(fontFamily))
        , fDWriteFont(SkSafeRefComPtr(font))
        , fDWriteFontFace(SkRefComPtr(fontFace))
        , fRenderingMode(DWRITE_RENDERING_MODE_DEFAULT)
        , fGamma(2.2f)
        , fContrast(1.0f)
    {
        if (!SUCCEEDED(fDWriteFontFace->QueryInterface(&fDWriteFontFace1))) {
            // IUnknown::QueryInterface states that if it fails, punk will be set to nullptr.
            // http://blogs.msdn.com/b/oldnewthing/archive/2004/03/26/96777.aspx
            SkASSERT_RELEASE(nullptr == fDWriteFontFace1.get());
        }
        if (!SUCCEEDED(fDWriteFontFace->QueryInterface(&fDWriteFontFace2))) {
            SkASSERT_RELEASE(nullptr == fDWriteFontFace2.get());
        }
        if (!SUCCEEDED(fFactory->QueryInterface(&fFactory2))) {
            SkASSERT_RELEASE(nullptr == fFactory2.get());
        }
    }

public:
    SkTScopedComPtr<IDWriteFactory> fFactory;
    SkTScopedComPtr<IDWriteFactory2> fFactory2;
    SkTScopedComPtr<IDWriteFontCollectionLoader> fDWriteFontCollectionLoader;
    SkTScopedComPtr<IDWriteFontFileLoader> fDWriteFontFileLoader;
    SkTScopedComPtr<IDWriteFontFamily> fDWriteFontFamily;
    SkTScopedComPtr<IDWriteFont> fDWriteFont;
    SkTScopedComPtr<IDWriteFontFace> fDWriteFontFace;
    SkTScopedComPtr<IDWriteFontFace1> fDWriteFontFace1;
    SkTScopedComPtr<IDWriteFontFace2> fDWriteFontFace2;

    static sk_sp<DWriteFontTypeface> Make(
        IDWriteFactory* factory,
        IDWriteFontFace* fontFace,
        IDWriteFont* font,
        IDWriteFontFamily* fontFamily,
        IDWriteFontFileLoader* fontFileLoader = nullptr,
        IDWriteFontCollectionLoader* fontCollectionLoader = nullptr)
    {
        return sk_sp<DWriteFontTypeface>(
            new DWriteFontTypeface(get_style(font), factory, fontFace, font, fontFamily,
                                   fontFileLoader, fontCollectionLoader));
    }

    static DWriteFontTypeface* Create(IDWriteFactory* factory,
                                      IDWriteFontFace* fontFace,
                                      SkFontStyle aStyle,
                                      DWRITE_RENDERING_MODE aRenderingMode,
                                      float aGamma,
                                      float aContrast) {
        DWriteFontTypeface* typeface =
                new DWriteFontTypeface(aStyle, factory, fontFace,
                                       nullptr, nullptr,
                                       nullptr, nullptr);
        typeface->fRenderingMode = aRenderingMode;
        typeface->fGamma = aGamma;
        typeface->fContrast = aContrast;
        return typeface;
    }

    bool ForceGDI() const { return fRenderingMode == DWRITE_RENDERING_MODE_GDI_CLASSIC; }
    DWRITE_RENDERING_MODE GetRenderingMode() const { return fRenderingMode; }

protected:
    void weak_dispose() const override {
        if (fDWriteFontCollectionLoader.get()) {
            HRV(fFactory->UnregisterFontCollectionLoader(fDWriteFontCollectionLoader.get()));
        }
        if (fDWriteFontFileLoader.get()) {
            HRV(fFactory->UnregisterFontFileLoader(fDWriteFontFileLoader.get()));
        }

        //SkTypefaceCache::Remove(this);
        INHERITED::weak_dispose();
    }

    std::unique_ptr<SkStreamAsset> onOpenStream(int* ttcIndex) const override;
    SkScalerContext* onCreateScalerContext(const SkScalerContextEffects&,
                                           const SkDescriptor*) const override;
    void onFilterRec(SkScalerContextRec*) const override;
    void getGlyphToUnicodeMap(SkUnichar* glyphToUnicode) const override;
    std::unique_ptr<SkAdvancedTypefaceMetrics> onGetAdvancedMetrics() const override;
    void onGetFontDescriptor(SkFontDescriptor*, bool*) const override;
    int onCharsToGlyphs(const void* chars, Encoding encoding,
                        uint16_t glyphs[], int glyphCount) const override;
    int onCountGlyphs() const override;
    int onGetUPEM() const override;
    void onGetFamilyName(SkString* familyName) const override;
    SkTypeface::LocalizedStrings* onCreateFamilyNameIterator() const override;
    int onGetVariationDesignPosition(SkFontArguments::VariationPosition::Coordinate coordinates[],
                                     int coordinateCount) const override;
    int onGetVariationDesignParameters(SkFontParameters::Variation::Axis parameters[],
                                       int parameterCount) const override;
    int onGetTableTags(SkFontTableTag tags[]) const override;
    size_t onGetTableData(SkFontTableTag, size_t offset, size_t length, void* data) const override;

private:
    typedef SkTypeface INHERITED;
    DWRITE_RENDERING_MODE fRenderingMode;
    float fGamma;
    float fContrast;
};

#endif
