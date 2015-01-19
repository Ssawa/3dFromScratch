#include "windows.h"
#include "stdint.h"
#include <stdio.h>

#include "matrix.h"

typedef struct _backBuffer {
    BITMAPINFO header;
    void* bufferLoc;
    int width;
    int height;

    // Because there are multiple bytes per pixel
    // we need to know how many bytes are in each row
    // (Essentially just bytesPerPixel * width)
    int pitch;
} BackBuffer;

BackBuffer backBuffer;

void createBackBuffer(BackBuffer* buffer, int width, int height) {
    int bytesPerPixel = 4;
    buffer->height = height;
    buffer->width = width;
    buffer->pitch = width * bytesPerPixel;

    buffer->header.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    buffer->header.bmiHeader.biWidth = width;
    // Negative so pixels go from top left to bottom right
    buffer->header.bmiHeader.biHeight = -height;
    buffer->header.bmiHeader.biPlanes = 1;
    buffer->header.bmiHeader.biBitCount = bytesPerPixel * 8; // 32 bits
    buffer->header.bmiHeader.biCompression = BI_RGB;

    // Maybe we should just use malloc but I'd rather stay as low level as possible
    buffer->bufferLoc = VirtualAlloc(0, width * height * bytesPerPixel, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void blitBuffer(BackBuffer* buffer, HDC context, int destWidth, int destHeight) {
    int i = StretchDIBits(context, 0, 0, destWidth, destHeight,
        0, 0, buffer->width, buffer->height, buffer->bufferLoc,
        &buffer->header, DIB_RGB_COLORS, SRCCOPY);
}

void clearBuffer(BackBuffer* buffer) {
     uint32_t* pixel = (uint32_t*)buffer->bufferLoc;
     for (int y = 0; y < buffer->height; y++) {
        for (int x = 0; x < buffer->width; x++) {
            *pixel = 0;
            pixel++;
        }
     }
}

// Taken from Casey Muratori's "Handmade Hero"
void renderWeirdGradient(BackBuffer* buffer) {
    uint8_t* row = (uint8_t *)buffer->bufferLoc;    
    for(int y = 0;
        y < buffer->height;
        ++y)
    {
        uint32_t* pixel = (uint32_t *)row;
        for(int x = 0;
            x < buffer->width;
            ++x)
        {
            uint8_t blue = (x);
            uint8_t green = (y);

            *pixel++ = ((green << 8) | blue);
        }
        
        row += buffer->pitch;
    }
}

void drawPixel(BackBuffer* buffer, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    // TODO - ADD BOUNDS CHECK
    // We should probably do this the way we do it in "renderWeirdGradient"
    // where we move the row by the pitch, but for this simple experiment, we know
    // each pixel is 32 bits, so we can just move by that.
    uint32_t* pixel = (uint32_t*)buffer->bufferLoc;
    pixel += y * buffer->width + x;
    *pixel = ((r << 16) | (g << 8) | (b << 0)); 
}

LRESULT CALLBACK WindowHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE: {
            DestroyWindow(hwnd);
        }
        break;
        case WM_DESTROY: {
            PostQuitMessage(0);
        }
        break;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC context = BeginPaint(hwnd, &paint);
            RECT windowRect;
            GetClientRect(hwnd, &windowRect);
            int windowWidth = windowRect.right - windowRect.left;
            int windowHeight = windowRect.bottom - windowRect.top;
            blitBuffer(&backBuffer, context, windowWidth, windowHeight);
            EndPaint(hwnd, &paint);
        } break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = "MainWindow";
    wc.lpfnWndProc = WindowHandler;
    wc.hInstance = hInstance;

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Failed to register MainWindow", "Error", MB_OKCANCEL | MB_ICONEXCLAMATION);
        return 0;
    }

    HWND hwnd = CreateWindowEx(NULL, wc.lpszClassName, "Main Window", WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
        hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Failed to create MainWindow", "Error", MB_OKCANCEL | MB_ICONEXCLAMATION);
        return 0;
    }

    RECT windowRect;
    GetClientRect(hwnd, &windowRect);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    createBackBuffer(&backBuffer, windowWidth / 5, windowHeight / 5);
    //renderWeirdGradient(&backBuffer);
    clearBuffer(&backBuffer);

    Vector3d point1 = {.1, -.1, .1};
    Vector3d point2 = {.1, -.1, -.1};
    Vector3d point3 = {-.1, -.1, .1};
    Vector3d point4 = {-.1, -.1, -.1};
    Vector3d point5 = {.1, .1, .1};
    Vector3d point6 = {-.1, .1, .1};
    Vector3d point7 = {-.1, .1, -.1};
    Vector3d point8 = {.1, .1, -.1};
    Matrix rotatinYMatrix = MATRIX_BLANK(4, 4);
    Matrix rotatinZMatrix = MATRIX_BLANK(4, 4);
    getRotationZMatrix(&rotatinZMatrix, 45);
    multiplyVectorByMatrix(&point1, rotatinZMatrix);
    multiplyVectorByMatrix(&point2, rotatinZMatrix);
    multiplyVectorByMatrix(&point3, rotatinZMatrix);
    multiplyVectorByMatrix(&point4, rotatinZMatrix);
    multiplyVectorByMatrix(&point5, rotatinZMatrix);
    multiplyVectorByMatrix(&point6, rotatinZMatrix);
    multiplyVectorByMatrix(&point7, rotatinZMatrix);
    multiplyVectorByMatrix(&point8, rotatinZMatrix);

    HDC context = GetDC(hwnd);
    blitBuffer(&backBuffer, context, windowWidth, windowHeight);
    ShowWindow(hwnd, nCmdShow);
    double yRotate = .0001;
    int y = 0;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        clearBuffer(&backBuffer);
        y++;
        y = y % 360;
        //printf("%f\n", yRotate * y);
        getRotationYMatrix(&rotatinYMatrix, yRotate * y);
        multiplyVectorByMatrix(&point1, rotatinYMatrix);
        multiplyVectorByMatrix(&point2, rotatinYMatrix);
        multiplyVectorByMatrix(&point3, rotatinYMatrix);
        multiplyVectorByMatrix(&point4, rotatinYMatrix);
        multiplyVectorByMatrix(&point5, rotatinYMatrix);
        multiplyVectorByMatrix(&point6, rotatinYMatrix);
        multiplyVectorByMatrix(&point7, rotatinYMatrix);
        multiplyVectorByMatrix(&point8, rotatinYMatrix);
        Vector2d pixel1 = project3dToScreen(point1, backBuffer.width, backBuffer.height);
        Vector2d pixel2 = project3dToScreen(point2, backBuffer.width, backBuffer.height);
        Vector2d pixel3 = project3dToScreen(point3, backBuffer.width, backBuffer.height);
        Vector2d pixel4 = project3dToScreen(point4, backBuffer.width, backBuffer.height);
        Vector2d pixel5 = project3dToScreen(point5, backBuffer.width, backBuffer.height);
        Vector2d pixel6 = project3dToScreen(point6, backBuffer.width, backBuffer.height);
        Vector2d pixel7 = project3dToScreen(point7, backBuffer.width, backBuffer.height);
        Vector2d pixel8 = project3dToScreen(point8, backBuffer.width, backBuffer.height);

        drawPixel(&backBuffer, pixel1.x, pixel1.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel2.x, pixel2.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel3.x, pixel3.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel4.x, pixel4.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel5.x, pixel5.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel6.x, pixel6.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel7.x, pixel7.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel8.x, pixel8.y, 255, 255, 255);
        //drawPixel(&backBuffer, 1, backBuffer.height - 1, 255, 255, 255);
        blitBuffer(&backBuffer, context, windowWidth, windowHeight);
    }
    return 0;
}