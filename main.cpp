#include <cstdlib>
#include <ctime>
#include <windows.h>

#include "chip8.h"
#include "windows_bindings.h"

// The clock speed of CHIP-8 is not formally defined, and it seems that the clock
// speed changed depending on which computer the game was intended to run on, so 
// allowing the users to change it based on the game might be needed.
#define CLOCK_SPEED_HZ 540

int main(int argc, char** argv) {
	// Seed PRNG
	srand(time(0));

	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " ROM_FILE" << std::endl;
		return 1;
	}

	try {
		Chip8 emu(argv[1]);

		setup_window();

		int instructions_per_60hz = CLOCK_SPEED_HZ / 60;
		LARGE_INTEGER perf_count_freq;
		QueryPerformanceFrequency(&perf_count_freq);
		LARGE_INTEGER last_perf_count;
		QueryPerformanceCounter(&last_perf_count);

		// We sleep to yield time to the cpu, so we set the clock precision so
		// we can be sure we don't over-sleep.
		timeBeginPeriod(1);

		bool running = true;
		MSG message;
		while (running) {
			// Handle Windows message loop
			while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
				if (message.message == WM_QUIT) {
					running = false;
				}

				TranslateMessage(&message);
				DispatchMessage(&message);
			}

			// Execute number of interpreter instructions to simulate the relevant clock speed
			// We also remember if any draw instructions were executed so we can update the screen
			bool screen_dirty = false;
			for (int i = 0; i < instructions_per_60hz; i++) {
				emu.step();
				if (emu.is_screen_dirty()) {
					screen_dirty = true;
					// A few websites say that the original interpreter blocked when drawing until
					// the next vertical blank.
					break;
				}
			}

			// If drawing instructions were run, we need to update the real window.
			if (screen_dirty) {
				screen_buffer buff = get_screen_buffer();
				int pixel_width = buff.width / SCREEN_WIDTH;
				int pixel_height = buff.height / SCREEN_HEIGHT;

				for (int y = 0; y < SCREEN_HEIGHT; y++) {
					for (int x = 0; x < SCREEN_WIDTH; x++) {
						for (int py = 0; py < pixel_height; py++) {
							for (int px = 0; px < pixel_width; px++) {
								int screen_x = x * pixel_width + px;
								int screen_y = y * pixel_height + py;
								int* pixel = ((int*)buff.bitmap_memory) + screen_y * buff.width + screen_x;
								int pixel_color;
								if (emu.get_pixel_value(x, y)) {
									pixel_color = 0x00FFFFFF;
								} else {
									pixel_color = 0x00000000;
								}
								*pixel = pixel_color;
							}
						}
					}
				}
				draw_to_screen();
			}

			// We check how much time we spent since the last time we got here, and sleep so we can
			// essentially draw to screen at 60FPS.
			LARGE_INTEGER cur_perf_count;
			QueryPerformanceCounter(&cur_perf_count);
			DWORD ms_elapsed = (1000 * (cur_perf_count.QuadPart - last_perf_count.QuadPart)) / perf_count_freq.QuadPart;
			if (ms_elapsed < 16) {
				Sleep(16 - ms_elapsed);
			}
			last_perf_count = cur_perf_count;
			emu.step_clocks(); // Update internal clocks at 60HZ
		}
	}
	catch (const std::runtime_error& err) {
		std::cerr << "ERROR: " << err.what() << std::endl;
	}

	timeEndPeriod(1);
}