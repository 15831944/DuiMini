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

bool UIRenderGDIP::GlobalInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    if (Gdiplus::GdiplusStartup(&gdiplus_token,
                                &gdiplusStartupInput, NULL) != Gdiplus::Ok) {
        UISetError(kLL_Warning, kEC_ThirdPartFail,
                   _T("GDI+ GdiplusStartup fail!"));
        return false;
    }
    return true;
}

bool UIRenderGDIP::GlobalRelease() {
    Gdiplus::GdiplusShutdown(gdiplus_token);
    return true;
}

bool UIRenderGDIP::Paint(UIWindow* v_wnd) {
    if (!v_wnd)
        return false;
    HWND hwnd = v_wnd->GetHWND();
    UIDialog* dlg = v_wnd->GetDialog();
    UIRect rcwindow;
    GetWindowRect(hwnd, &(rcwindow.rect()));
    SIZE sizewindow;
    sizewindow.cx = rcwindow.width();
    sizewindow.cy = rcwindow.height();
    HDC hdc = GetDC(hwnd);

    int nBytesPerLine = ((sizewindow.cx * 32 + 31) & (~31)) >> 3;
    BITMAPINFOHEADER bmp_infoheader = { 0 };
    bmp_infoheader.biSize = sizeof(BITMAPINFOHEADER);
    bmp_infoheader.biWidth = sizewindow.cx;
    bmp_infoheader.biHeight = sizewindow.cy;
    bmp_infoheader.biPlanes = 1;
    bmp_infoheader.biBitCount = 32;
    bmp_infoheader.biCompression = BI_RGB;
    bmp_infoheader.biClrUsed = 0;
    bmp_infoheader.biSizeImage = nBytesPerLine * sizewindow.cy;
    if (background_ == NULL) {
        background_ = CreateCompatibleDC(hdc);
        PVOID bits = NULL;
        bg_bitmap_ = CreateDIBSection(NULL, (PBITMAPINFO)&bmp_infoheader,
                                      DIB_RGB_COLORS, &bits, NULL, 0);
        if (bg_bitmap_ == NULL)
            return false;
        tmpobj_ = SelectObject(background_, bg_bitmap_);

        graph_ = new Gdiplus::Graphics(background_);
        dlg->Paint(true);
        delete graph_;
        graph_ = nullptr;
    }
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

    graph_ = new Gdiplus::Graphics(hdctmp);
    dlg->Paint();
    delete graph_;
    graph_ = nullptr;

    POINT wndpos = { rcwindow.left, rcwindow.top };
    blendfunc.SourceConstantAlpha = v_wnd->GetDialog()->GetAlpha();
    
    if (!UpdateLayeredWindow(hwnd, hdc, &wndpos, &sizewindow, hdctmp,
                             &ptsrc, 0, &blendfunc, ULW_ALPHA))
        return false;

    SelectObject(hdctmp, bmpobj);
    DeleteDC(hdctmp);
    DeleteObject(bmptmp);
    ReleaseDC(hwnd, hdc);
    return true;
}

bool UIRenderGDIP::RedrawBackground() {
    if (background_) {
        SelectObject(background_, tmpobj_);
        DeleteDC(background_);
        DeleteObject(bg_bitmap_);
        background_ = NULL;
        bg_bitmap_ = NULL;
        tmpobj_ = NULL;
    }
    return true;
}

bool UIRenderGDIP::DrawImage(UIRenderImage* v_img, const UIRect& v_destrect,
                             const UIRect& v_srcrect) {
    if (!graph_ || !v_img)
        return false;
    if (graph_->DrawImage((Gdiplus::Image*)v_img->GetInterface(),
                          Gdiplus::Rect(v_destrect.left, v_destrect.top,
                                        v_destrect.width(),
                                        v_destrect.height()),
                          v_srcrect.left, v_srcrect.top,
                          v_srcrect.width(), v_srcrect.height(),
                          Gdiplus::UnitPixel) != Gdiplus::Ok) {
        UISetError(kLL_Warning, kEC_ThirdPartFail,
                   _T("GDI+ DrawImage fail!"));
        return false;
    }
    return true;
}

bool UIRenderGDIP::DrawString(LPCTSTR v_text, const UIFont &v_font,
                              const UIRect &v_rect) {
    if (!graph_)
        return false;
    Gdiplus::FontFamily fontfamily(v_font.font_);
    Gdiplus::Font font(&fontfamily, v_font.size_, Gdiplus::FontStyleRegular,
                       Gdiplus::UnitPixel);
    Gdiplus::StringFormat format;
    if (graph_->DrawString(v_text, -1, &font,
                           Gdiplus::RectF((Gdiplus::REAL)v_rect.left, (Gdiplus::REAL)v_rect.top,
                           (Gdiplus::REAL)v_rect.width(), (Gdiplus::REAL)v_rect.height()),
                           &format, &Gdiplus::SolidBrush(Gdiplus::Color::White)) != Gdiplus::Ok) {
        UISetError(kLL_Warning, kEC_ThirdPartFail,
                   _T("GDI+ DrawString fail!"));
        return false;
    }
    return true;
}

////////////////////////////////////////

UIRenderImageGDIP::UIRenderImageGDIP() {}

UIRenderImageGDIP::UIRenderImageGDIP(LPCTSTR v_path) {
    Load(v_path);
}

UIRenderImageGDIP::~UIRenderImageGDIP() {
    Release();
}

bool UIRenderImageGDIP::Load(LPCTSTR v_path) {
    long buflen= UIResource::GetFileSize(v_path);
    HGLOBAL mem = GlobalAlloc(GHND, buflen);
    BYTE* buffer = (BYTE*)GlobalLock(mem);
    UIResource::GetFile(v_path, buffer, buflen);
    GlobalUnlock(mem);
    IStream *stream = nullptr;
    if (CreateStreamOnHGlobal(buffer, TRUE, &stream) != S_OK) {
        UISetError(kLL_Warning, kEC_ThirdPartFail,
                   _T("GDI+ CreateStreamOnHGlobal fail!"));
        return false;
    }
    img_ = Gdiplus::Image::FromStream(stream);
    stream->Release();
    // no need to call GlobalFree here
    return true;
}

bool UIRenderImageGDIP::Release() {
    delete img_;
    img_ = nullptr;
    return true;
}

LPVOID UIRenderImageGDIP::GetInterface() const {
    return (LPVOID)img_;
}

long UIRenderImageGDIP::GetWidth() const {
    if (!img_)
        return 0;
    return img_->GetWidth();
}

long UIRenderImageGDIP::GetHeight() const {
    if (!img_)
        return 0;
    return img_->GetHeight();
}
}    // namespace DuiMini
