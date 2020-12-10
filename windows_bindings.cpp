/*
This windows code is based on the first few episodes of the 'Handmade Hero' series
by Casey Muratori, and is not commented as it is pretty much just dealing with
Windows' APIs and its oddities.
*/

#include <Windows.h>
#include <stdexcept>
#include <cstdint>

#include "windows_bindings.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

static HDC window_device_context;
static screen_buffer screen_buff;
static bool key_states[16] = {};
static bool key_capture = false;
static int last_key = -1;

void setup_drawbuffer() {
	screen_buff.bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFO);
	screen_buff.bitmap_info.bmiHeader.biWidth = WINDOW_WIDTH;
	screen_buff.bitmap_info.bmiHeader.biHeight = -WINDOW_HEIGHT;
	screen_buff.bitmap_info.bmiHeader.biPlanes = 1;
	screen_buff.bitmap_info.bmiHeader.biBitCount = 32;
	screen_buff.bitmap_info.bmiHeader.biCompression = BI_RGB;
	screen_buff.bitmap_info.bmiHeader.biSizeImage = 0;
	screen_buff.bitmap_info.bmiHeader.biXPelsPerMeter = 0;
	screen_buff.bitmap_info.bmiHeader.biYPelsPerMeter = 0;
	screen_buff.bitmap_info.bmiHeader.biClrUsed = 0;
	screen_buff.bitmap_info.bmiHeader.biClrImportant = 0;

	screen_buff.width = WINDOW_WIDTH;
	screen_buff.height = WINDOW_HEIGHT;

	screen_buff.bitmap_memory = VirtualAlloc(0, 4 * WINDOW_WIDTH * WINDOW_HEIGHT, MEM_COMMIT, PAGE_READWRITE);
	if (!screen_buff.bitmap_memory) {
		throw std::runtime_error("Failed to allocate bitmap memory");
	}
}

void flip_drawbuffer(HDC device_context) {
	StretchDIBits(device_context, 0, 0, screen_buff.width, screen_buff.height, 0, 0, screen_buff.width,
		screen_buff.height, screen_buff.bitmap_memory, &screen_buff.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}

void draw_to_screen() {
	flip_drawbuffer(window_device_context);
}

screen_buffer get_screen_buffer() {
	return screen_buff;
}

// returns -1 if the keycode doesn't map to a chip-8 key number
int get_chip8_key_number(int VK_code) {
	switch (VK_code) {
	case 'X':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case 'Q':
		return 4;
	case 'W':
		return 5;
	case 'E':
		return 6;
	case 'A':
		return 7;
	case 'S':
		return 8;
	case 'D':
		return 9;
	case 'Z':
		return 0xA;
	case 'C':
		return 0xB;
	case '4':
		return 0xC;
	case 'R':
		return 0xD;
	case 'F':
		return 0xE;
	case 'V':
		return 0xF;
	default:
		return -1;
	}
}

bool is_key_down(int key_number) {
	return key_states[key_number];
}

void enable_key_capture() {
	key_capture = true;
}

// Returns the captured key number, or -1 if not captured yet
int get_capture_key() {
	if (key_capture) {
		return -1;
	} else {
		return last_key;
	}
}

LRESULT CALLBACK WindowCallback(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	LRESULT result = 0;
	switch (uMsg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: {
		int VK_code = wParam;
		int key_number = get_chip8_key_number(VK_code);
		if (key_number != -1) {
			key_states[key_number] = true;
			if (key_capture) {
				last_key = key_number;
				key_capture = false;
			}
		}
		//debug_print_keyboard();
	} break;

	case WM_KEYUP:
	case WM_SYSKEYUP: {
		int VK_code = wParam;
		int key_number = get_chip8_key_number(VK_code);
		if (key_number != -1) {
			key_states[key_number] = false;
		}
		//debug_print_keyboard();
	} break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	} break;

	case WM_CLOSE: {
		DestroyWindow(hwnd);
	} break;

	case WM_PAINT: {
		PAINTSTRUCT paint;
		HDC device_context = BeginPaint(hwnd, &paint);
		flip_drawbuffer(device_context);
		EndPaint(hwnd, &paint);
	} break;

	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	} break;
	}

	return result;
}

void setup_window() {
	WNDCLASS window_class = {};
	window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = WindowCallback;
	window_class.hInstance = GetModuleHandle(0);
	window_class.lpszClassName = L"CHIP8EmuWindowClass";
	if (!RegisterClass(&window_class)) {
		throw std::runtime_error("Failed to register window class");
	}

	setup_drawbuffer();

	DWORD window_style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	RECT desired_client_rect = {};
	desired_client_rect.right = WINDOW_WIDTH;
	desired_client_rect.bottom = WINDOW_HEIGHT;
	AdjustWindowRectEx(&desired_client_rect, window_style, 0, 0);

	int window_width = desired_client_rect.right - desired_client_rect.left;
	int window_height = desired_client_rect.bottom - desired_client_rect.top;

	HWND window_handle = CreateWindowEx(0, window_class.lpszClassName, L"CHIP8 Emulator", window_style,
		CW_USEDEFAULT, CW_USEDEFAULT, window_width, window_height, 0, 0, window_class.hInstance, 0);
	if (!window_handle) {
		throw std::runtime_error("Failed to create window");
	}

	window_device_context = GetDC(window_handle);
}

void debug_print_keyboard() {
	OutputDebugStringA("\n==========\n");
	int keymap[] = { 1,2,3,12,4,5,6,13,7,8,9,14,10,0,11,15 };
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (key_states[keymap[y * 4 + x]]) {
				OutputDebugStringA("O ");
			} else {
				OutputDebugStringA("_ ");
			}
		}
		OutputDebugStringA("\n");
	}
}