/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Wojciech Mostowski (firstname.lastname@google.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "bsp/board_api.h"
#include "tusb.h"

/*
static inline bool is_logitech_dual(uint8_t dev_addr)
{
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  return (vid == 0x046d && pid == 0xc216);                 // Logitech GamePad Dual Action
}
*/

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
	(void)desc_report;
	(void)desc_len;
	//uint16_t vid, pid;
	//tuh_vid_pid_get(dev_addr, &vid, &pid);

	//printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
	//printf("VID = %04x, PID = %04x\r\n", vid, pid);

	tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
	(void)dev_addr;
	(void)instance;
	// printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
	bool fire = true;
	bool left = true;
	bool right = true;
	bool up = true;
	bool down = true;

	if(report[4] & 0xf0)
		fire = false;

	switch(report[4] & 0x0f) {
		case 0x0: up = false; break;
		case 0x1: up = false; right = false; break;
		case 0x2: right = false; break;
		case 0x3: down = false; right = false; break;
		case 0x4: down = false; break;
		case 0x5: down = false; left = false; break;
		case 0x6: left = false; break;
		case 0x7: up = false; left = false; break;
		default: break;
	}
	// up, down, left, right 5, 4, 3, 2
	// fire 6
	gpio_put(2, right);
	gpio_put(3, left);
	gpio_put(4, down);
	gpio_put(5, up);
	gpio_put(6, fire);
	// printf("Left:  %02X Up:    %02X Right: %02X Down:  %02X Fire:  %02X\r\n", left, up, right, down, fire);
	tuh_hid_receive_report(dev_addr, instance);
}

void led_blinking_task(void)
{
	const uint32_t interval_ms = 1000;
	static uint32_t start_ms = 0;

	static bool led_state = false;

	// Blink every interval ms
	if ( board_millis() - start_ms < interval_ms) return; // not enough time
	start_ms += interval_ms;

	board_led_write(led_state);
	led_state = 1 - led_state;
}

int main(void)
{
	board_init();
	for(int pin=2; pin <= 6; pin++) {
		gpio_init(pin);
		gpio_set_dir(pin, GPIO_OUT);
		gpio_put(pin, 1);
	}
	tuh_init(BOARD_TUH_RHPORT);

	if (board_init_after_tusb)
		board_init_after_tusb();

	while (1) {
		tuh_task();
		// led_blinking_task();
	}

	return 0;
}

