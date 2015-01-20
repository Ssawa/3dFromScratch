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

void drawLine(BackBuffer* buffer, Vector2d point1, Vector2d point2) {
    double dist = distBetweenPoints(point1, point2);

    // If the distance is less than 1 pixel than we can just return
    if (dist < 1) {
        return;
    }

    // Recursivly find the mid point, draw the pixel, and call this function
    // again
    Vector2d midPoint = getMidPoint(point1, point2);
    drawPixel(buffer, midPoint.x, midPoint.y, 255, 255, 255);
    drawLine(buffer, point1, midPoint);
    drawLine(buffer, midPoint, point2);
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

    createBackBuffer(&backBuffer, windowWidth, windowHeight);
    //renderWeirdGradient(&backBuffer);
    clearBuffer(&backBuffer);

    Vector3d vertices[8] = {{.1, -.1, .1},
                            {.1, -.1, -.1},
                            {-.1, -.1, .1},
                            {-.1, -.1, -.1},
                            {.1, .1, .1},
                            {-.1, .1, .1},
                            {-.1, .1, -.1},
                            {.1, .1, -.1}};
    Mesh mesh = {8, vertices};
    Matrix rotatinYMatrix = MATRIX_BLANK(4, 4);
    Matrix rotatinZMatrix = MATRIX_BLANK(4, 4);
    Matrix rotatinXMatrix = MATRIX_BLANK(4, 4);

    // We're only setting this once but we are multiplying
    // the same point by it every frame
    getRotationYMatrix(&rotatinYMatrix, .02);
    getRotationZMatrix(&rotatinZMatrix, 45);
    getRotationXMatrix(&rotatinXMatrix, 10);
    multiplyVectorByMatrix(&mesh.vertices[0], rotatinZMatrix);
    multiplyVectorByMatrix(&mesh.vertices[1], rotatinZMatrix);
    multiplyVectorByMatrix(&mesh.vertices[2], rotatinZMatrix);
    multiplyVectorByMatrix(&mesh.vertices[3], rotatinZMatrix);
    multiplyVectorByMatrix(&mesh.vertices[4], rotatinZMatrix);
    multiplyVectorByMatrix(&mesh.vertices[5], rotatinZMatrix);
    multiplyVectorByMatrix(&mesh.vertices[6], rotatinZMatrix);
    multiplyVectorByMatrix(&mesh.vertices[7], rotatinZMatrix);

    multiplyVectorByMatrix(&mesh.vertices[0], rotatinXMatrix);
    multiplyVectorByMatrix(&mesh.vertices[1], rotatinXMatrix);
    multiplyVectorByMatrix(&mesh.vertices[2], rotatinXMatrix);
    multiplyVectorByMatrix(&mesh.vertices[3], rotatinXMatrix);
    multiplyVectorByMatrix(&mesh.vertices[4], rotatinXMatrix);
    multiplyVectorByMatrix(&mesh.vertices[5], rotatinXMatrix);
    multiplyVectorByMatrix(&mesh.vertices[6], rotatinXMatrix);
    multiplyVectorByMatrix(&mesh.vertices[7], rotatinXMatrix);

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
        yRotate += .0001;
        //y++;
        //y = y % 360;
        //printf("%f\n", yRotate * y);
        multiplyVectorByMatrix(&mesh.vertices[0], rotatinYMatrix);
        multiplyVectorByMatrix(&mesh.vertices[1], rotatinYMatrix);
        multiplyVectorByMatrix(&mesh.vertices[2], rotatinYMatrix);
        multiplyVectorByMatrix(&mesh.vertices[3], rotatinYMatrix);
        multiplyVectorByMatrix(&mesh.vertices[4], rotatinYMatrix);
        multiplyVectorByMatrix(&mesh.vertices[5], rotatinYMatrix);
        multiplyVectorByMatrix(&mesh.vertices[6], rotatinYMatrix);
        multiplyVectorByMatrix(&mesh.vertices[7], rotatinYMatrix);
        Vector2d pixel1 = project3dToScreen(mesh.vertices[0], backBuffer.width, backBuffer.height);
        Vector2d pixel2 = project3dToScreen(mesh.vertices[1], backBuffer.width, backBuffer.height);
        Vector2d pixel3 = project3dToScreen(mesh.vertices[2], backBuffer.width, backBuffer.height);
        Vector2d pixel4 = project3dToScreen(mesh.vertices[3], backBuffer.width, backBuffer.height);
        Vector2d pixel5 = project3dToScreen(mesh.vertices[4], backBuffer.width, backBuffer.height);
        Vector2d pixel6 = project3dToScreen(mesh.vertices[5], backBuffer.width, backBuffer.height);
        Vector2d pixel7 = project3dToScreen(mesh.vertices[6], backBuffer.width, backBuffer.height);
        Vector2d pixel8 = project3dToScreen(mesh.vertices[7], backBuffer.width, backBuffer.height);

        drawPixel(&backBuffer, pixel1.x, pixel1.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel2.x, pixel2.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel3.x, pixel3.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel4.x, pixel4.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel5.x, pixel5.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel6.x, pixel6.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel7.x, pixel7.y, 255, 255, 255);
        drawPixel(&backBuffer, pixel8.x, pixel8.y, 255, 255, 255);

        drawLine(&backBuffer, pixel1, pixel2);
        drawLine(&backBuffer, pixel1, pixel3);
        drawLine(&backBuffer, pixel3, pixel4);
        drawLine(&backBuffer, pixel4, pixel2);

        drawLine(&backBuffer, pixel5, pixel6);
        drawLine(&backBuffer, pixel6, pixel7);
        drawLine(&backBuffer, pixel7, pixel8);
        drawLine(&backBuffer, pixel8, pixel5);

        drawLine(&backBuffer, pixel1, pixel5);
        drawLine(&backBuffer, pixel2, pixel8);
        drawLine(&backBuffer, pixel3, pixel6);
        drawLine(&backBuffer, pixel4, pixel7);

        //drawPixel(&backBuffer, 1, backBuffer.height - 1, 255, 255, 255);
        blitBuffer(&backBuffer, context, windowWidth, windowHeight);
    }
    return 0;
}