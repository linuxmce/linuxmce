#include "TransformBitmap.h"

/*
 * Before we BitBlt Bitmap to the screen, we BitBlt it to another in-memory bitmap.
 * This first BitBlt converts our Bitmap to a screen compatibile bitmap
 * This is worth it, since BitBlt is lightning fast when the bitmap is compatibile.
 * BitBlt takes a while to draw the image if it has to convert pixels from the bitmap format to that of the screen.
 * BitBlt transfers the pixels so fast when the pixel formats match, that it's instant.
 */

HBITMAP GetCompatibleBitmap(HWND hwnd, HBITMAP BitmapSrc)
{
	BITMAP BmpInfo;
	GetObject(BitmapSrc, sizeof(BITMAP), &BmpInfo);

	HDC windowDC = GetDC(hwnd);

	HBITMAP BitmapDst = CreateCompatibleBitmap(windowDC, BmpInfo.bmWidth, BmpInfo.bmHeight);
	HDC DCsrc = CreateCompatibleDC(windowDC);
	HDC DCdst = CreateCompatibleDC(windowDC);

	ReleaseDC(hwnd, windowDC);

	HBITMAP tmpBmpSrc = SelectObject(DCsrc, BitmapSrc);
	HBITMAP tmpBmpDst = SelectObject(DCdst, BitmapDst);

	// This converts the pixels from source bitmap format to destination bitmap format
	BitBlt(DCdst, 0, 0, BmpInfo.bmWidth, BmpInfo.bmHeight, DCsrc, 0, 0, SRCCOPY);

	// cleanup
	SelectObject(DCsrc, tmpBmpSrc);
	SelectObject(DCdst, tmpBmpDst);

	DeleteDC(DCsrc);
	DeleteDC(DCdst);

	return BitmapDst;
}
