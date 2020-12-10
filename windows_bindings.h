#pragma once
#include <windows.h>

struct screen_buffer {
	BITMAPINFO bitmap_info;
	void* bitmap_memory;
	int width;
	int height;
};

void setup_window();

bool is_key_down(int key_number);

void enable_key_capture();

// Returns the captured key number, or -1 if not captured yet
int get_capture_key();

void draw_to_screen();

screen_buffer get_screen_buffer();