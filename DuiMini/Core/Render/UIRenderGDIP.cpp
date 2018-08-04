/**
* Copyright (c) 2018-2050
* All rights reserved.
*
* @Author:MXWXZ
* @Date:2018/03/20
*/
#include "stdafx.h"
#include "UIRenderGDIP.h"

namespace DuiMini {
ULONG_PTR UIRenderGDIP::gdiplus_token = 0;

UIRenderGDIP::UIRenderGDIP() {}

UIRenderGDIP::~UIRenderGDIP() {
    DeleteDC(background_);
    DeleteObject(bg_bitmap_);
    background_ = NULL;
    bg_bitmap_ = NULL;
}

void UIRenderGDIP::GlobalInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    if (Gdiplus::GdiplusStartup(&gdiplus_token,
                                &gdiplusStartupInput, NULL) != Gdiplus::Ok)
        ErrorMsg_ThirdPartFail(GdiplusStartup);
}

void UIRenderGDIP::GlobalRelease() {
    Gdiplus::GdiplusShutdown(gdiplus_token);
}

void UIRenderGDIP::Paint(UIWindow* v_wnd) {
    assert(v_wnd);
    HWND hwnd = v_wnd->GetHWND();
    UIDialog* dlg = v_wnd->GetDialog();
    assert(dlg);
    UIRect rcwindow;
    GetWindowRect(hwnd, &(rcwindow.rect()));
    SIZE sizewindow = { rcwindow.width(),rcwindow.height() };
    HDC hdc = GetDC(hwnd);

    int bytes_perline = ((sizewindow.cx * 32 + 31) & (~31)) >> 3;
    BITMAPINFOHEADER bmp_infoheader = { 0 };
    bmp_infoheader.biSize = sizeof(BITMAPINFOHEADER);
    bmp_infoheader.biWidth = sizewindow.cx;
    bmp_infoheader.biHeight = sizewindow.cy;
    bmp_infoheader.biPlanes = 1;
    bmp_infoheader.biBitCount = 32;
    bmp_infoheader.biCompression = BI_RGB;
    bmp_infoheader.biClrUsed = 0;
    bmp_infoheader.biSizeImage = bytes_perline * sizewindow.cy;

    if (!background_) {  // redraw background
        background_ = CreateCompatibleDC(hdc);
        PVOID bits = NULL;
        bg_bitmap_ = CreateDIBSection(NULL, (PBITMAPINFO)&bmp_infoheader,
                                      DIB_RGB_COLORS, &bits, NULL, 0);
        assert(bg_bitmap_);
        tmpobj_ = SelectObject(background_, bg_bitmap_);

        graph_ = make_shared<Gdiplus::Graphics>(background_);
        dlg->Paint(true);
        graph_.reset();
    }

    // ctrl paint
    HDC hdctmp = CreateCompatibleDC(hdc);
    PVOID bits = NULL;
    HBITMAP bmptmp = CreateDIBSection(NULL, (PBITMAPINFO)&bmp_infoheader,
                                        DIB_RGB_COLORS, &bits, NULL, 0);
    HGDIOBJ bmpobj = SelectObject(hdctmp, bmptmp);
    POINT ptsrc = { 0, 0 };
    BLENDFUNCTION blendfunc;
    blendfunc.BlendOp = 0;
    blendfunc.BlendFlags = 0;
    blendfunc.AlphaFormat = 1;
    blendfunc.SourceConstantAlpha = 255;
    AlphaBlend(hdctmp, 0, 0, sizewindow.cx, sizewindow.cy,
               background_, 0, 0, sizewindow.cx, sizewindow.cy, blendfunc);

    graph_ = make_shared<Gdiplus::Graphics>(hdctmp);
    dlg->Paint();
    graph_.reset();

    POINT wndpos = { rcwindow.left, rcwindow.top };
    // dlg alpha will apply to all controls
    blendfunc.SourceConstantAlpha = v_wnd->GetDialog()->GetAlpha();
    
    // Mix background and ctrl
    if (!UpdateLayeredWindow(hwnd, hdc, &wndpos, &sizewindow, hdctmp,
                             &ptsrc, 0, &blendfunc, ULW_ALPHA))
        ErrorMsg_ThirdPartFail(UpdateLayeredWindow);

    SelectObject(hdctmp, bmpobj);
    DeleteDC(hdctmp);
    DeleteObject(bmptmp);
    ReleaseDC(hwnd, hdc);
}

void UIRenderGDIP::RedrawBackground() {
    if (background_) {
        SelectObject(background_, tmpobj_);
        DeleteDC(background_);
        DeleteObject(bg_bitmap_);
        background_ = NULL;
        bg_bitmap_ = NULL;
        tmpobj_ = NULL;
    }
}

void UIRenderGDIP::DrawImage(UIRenderImage* v_img, const UIRect& v_destrect,
                             const UIRect& v_srcrect, ALPHA v_alpha/* = 255*/,
                             ImageMode v_mode/* = kIM_Extrude*/) {
    assert(graph_ && v_img);
    Gdiplus::Image* img = (Gdiplus::Image*)v_img->GetInterface();
    Gdiplus::ColorMatrix matrix = {
        1,0,0,0,0,
        0,1,0,0,0,
        0,0,1,0,0,
        0,0,0,(Gdiplus::REAL)v_alpha / 255,0,
        0,0,0,0,1 };
    Gdiplus::ImageAttributes imgattr;
    imgattr.SetColorMatrix(&matrix);
    if (v_mode == kIM_Extrude) {
        if (graph_->DrawImage(img,
                              Gdiplus::Rect(v_destrect.left, v_destrect.top,
                                            v_destrect.width(),
                                            v_destrect.height()),
                              v_srcrect.left, v_srcrect.top,
                              v_srcrect.width(), v_srcrect.height(),
                              Gdiplus::UnitPixel, &imgattr) != Gdiplus::Ok)
            ErrorMsg_ThirdPartFail(DrawImage);
    } else {
        Gdiplus::TextureBrush brush(img, Gdiplus::Rect(v_srcrect.left,
                                                       v_srcrect.top,
                                                       v_srcrect.width(),
                                                       v_srcrect.height()),
                                    &imgattr);
        brush.SetWrapMode(Gdiplus::WrapModeTile);
        if (graph_->FillRectangle(&brush, v_destrect.left, v_destrect.top,
                                  v_destrect.width(), v_destrect.height())
            != Gdiplus::Ok)
            ErrorMsg_ThirdPartFail(FillRectangle);
    }
}

void UIRenderGDIP::DrawString(LPCTSTR v_text, const UIFont &v_font,
                              const UIStringFormat &v_format,
                              const UIRect &v_rect) {
    assert(graph_);
    Gdiplus::FontFamily fontfamily(v_font.font_);
    // font style
    Gdiplus::FontStyle fontstyle = Gdiplus::FontStyleRegular;
    if (v_font.bold_ && !v_font.italic_)
        fontstyle = Gdiplus::FontStyleBold;
    if (!v_font.bold_ && v_font.italic_)
        fontstyle = Gdiplus::FontStyleItalic;
    if (v_font.bold_ && v_font.italic_)
        fontstyle = Gdiplus::FontStyleBoldItalic;
    if (v_font.underline_)
        fontstyle = (Gdiplus::FontStyle)(fontstyle | Gdiplus::FontStyleUnderline);
    if (v_font.strikeout_)
        fontstyle = (Gdiplus::FontStyle)(fontstyle | Gdiplus::FontStyleStrikeout);
    Gdiplus::Font font(&fontfamily, v_font.size_, fontstyle, Gdiplus::UnitPixel);
    
    // string format
    Gdiplus::StringFormat format;
    int formatflag = 0;
    format.SetTrimming((Gdiplus::StringTrimming)v_format.trimming_);
    if (!v_format.autowrap_)
        formatflag |= Gdiplus::StringFormatFlagsNoWrap;
    if (v_format.vertical_)
        formatflag |= Gdiplus::StringFormatFlagsDirectionVertical;
    if (formatflag)
        format.SetFormatFlags(formatflag);

    if (v_format.align_ % 3 == 0)         // horizontal left
        format.SetAlignment(Gdiplus::StringAlignmentNear);
    else if (v_format.align_ % 3 == 1)    // horizontal mid
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
    else                                  // horizontal right
        format.SetAlignment(Gdiplus::StringAlignmentFar);
    if (v_format.align_ < 3)              // vertical top
        format.SetLineAlignment(Gdiplus::StringAlignmentNear);
    else if (v_format.align_ < 6)         // vertical mid
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    else                                  // vertical bottom
        format.SetLineAlignment(Gdiplus::StringAlignmentFar);

    Gdiplus::RectF rect(v_rect.left, v_rect.top,
                        v_rect.width(), v_rect.height());
    Gdiplus::SolidBrush color(Gdiplus::Color(v_format.color_.a,
                                             v_format.color_.r,
                                             v_format.color_.g,
                                             v_format.color_.b));
    
    if (graph_->DrawString(v_text, -1, &font, rect, &format,
                           &color) != Gdiplus::Ok)
        ErrorMsg_ThirdPartFail(DrawString);
}

void UIRenderGDIP::DrawRect(const UIRect &v_rect, const UIColor &v_color,
                            long v_border) {
    assert(graph_);
    // make sure draw exactly in the rect so we need offset
    long offset = v_border / 2;
    Gdiplus::Pen pen(Gdiplus::Color(v_color.a, v_color.r, v_color.g, v_color.b),
                     v_border);
    if (graph_->DrawRectangle(&pen, v_rect.left + offset, v_rect.top + offset,
                              v_rect.width() - 2 * offset, v_rect.height() - 2 * offset)
        != Gdiplus::Ok)
        ErrorMsg_ThirdPartFail(DrawRectangle);
}

void UIRenderGDIP::DrawFillRect(const UIRect & v_rect, const UIColor &v_color) {
    assert(graph_);
    Gdiplus::SolidBrush brush(Gdiplus::Color(v_color.a, v_color.r,
                                             v_color.g, v_color.b));
    if (graph_->FillRectangle(&brush, v_rect.left, v_rect.top, v_rect.width(),
                              v_rect.height()) != Gdiplus::Ok)
        ErrorMsg_ThirdPartFail(FillRectangle);
}

////////////////////////////////////////

UIRenderImageGDIP::UIRenderImageGDIP() {}

UIRenderImageGDIP::UIRenderImageGDIP(LPCTSTR v_path) {
    Load(v_path);
}

UIRenderImageGDIP::~UIRenderImageGDIP() {}

void UIRenderImageGDIP::Load(LPCTSTR v_path) {
    FILESIZE buflen= UIResource::GetFileSize(v_path);
    HGLOBAL mem = GlobalAlloc(GHND, buflen);
    BYTE* buffer = (BYTE*)GlobalLock(mem);
    UIResource::GetFile(v_path, buffer, buflen);
    GlobalUnlock(mem);
    IStream *stream = nullptr;
    if (CreateStreamOnHGlobal(buffer, TRUE, &stream) != S_OK)
        ErrorMsg_ThirdPartFail(CreateStreamOnHGlobal);
    img_.reset(Gdiplus::Image::FromStream(stream));
    stream->Release();
    // no need to call GlobalFree here
}

LPVOID UIRenderImageGDIP::GetInterface() const {
    assert(img_);
    return (LPVOID)img_.get();
}

UINT UIRenderImageGDIP::GetWidth() const {
    assert(img_);
    return img_->GetWidth();
}

UINT UIRenderImageGDIP::GetHeight() const {
    assert(img_);
    return img_->GetHeight();
}
}    // namespace DuiMini
