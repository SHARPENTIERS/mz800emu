
/*
 * Autor: Ing. Jaroslav Svehla
 * 
 */


#include "memory/memory.h"


const Z80EX_BYTE c_ROM_JSS108C_CGROM [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x24, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x00,
	0x3e, 0x48, 0x48, 0x38, 0x48, 0x48, 0x3e, 0x00, 0x38, 0x44, 0x02, 0x02, 0x02, 0x44, 0x38, 0x00,
	0x3e, 0x48, 0x48, 0x48, 0x48, 0x48, 0x3e, 0x00, 0x7e, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x7e, 0x00,
	0x7e, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x02, 0x00, 0x38, 0x44, 0x02, 0x72, 0x42, 0x44, 0x38, 0x00,
	0x42, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x42, 0x00, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00,
	0x70, 0x20, 0x20, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x42, 0x22, 0x12, 0x0e, 0x12, 0x22, 0x42, 0x00,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x7e, 0x00, 0x42, 0x66, 0x5a, 0x5a, 0x42, 0x42, 0x42, 0x00,
	0x42, 0x46, 0x4a, 0x52, 0x62, 0x42, 0x42, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00,
	0x3e, 0x42, 0x42, 0x3e, 0x02, 0x02, 0x02, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x52, 0x62, 0x7c, 0x00,
	0x3e, 0x42, 0x42, 0x3e, 0x12, 0x22, 0x42, 0x00, 0x3c, 0x42, 0x02, 0x3c, 0x40, 0x42, 0x3c, 0x00,
	0x7c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00,
	0x42, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18, 0x00, 0x42, 0x42, 0x42, 0x5a, 0x5a, 0x66, 0x42, 0x00,
	0x42, 0x42, 0x24, 0x18, 0x24, 0x42, 0x42, 0x00, 0x44, 0x44, 0x44, 0x38, 0x10, 0x10, 0x10, 0x00,
	0x7e, 0x40, 0x20, 0x18, 0x04, 0x02, 0x7e, 0x00, 0x30, 0x48, 0x08, 0x1c, 0x08, 0x08, 0x7c, 0x00,
	0x10, 0x10, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x00, 0x00, 0x00,
	0x10, 0x10, 0x10, 0x10, 0xf0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0x00, 0x00, 0x00,
	0x3c, 0x42, 0x62, 0x5a, 0x46, 0x42, 0x3c, 0x00, 0x10, 0x18, 0x14, 0x10, 0x10, 0x10, 0x7c, 0x00,
	0x3c, 0x42, 0x40, 0x30, 0x0c, 0x02, 0x7e, 0x00, 0x3c, 0x42, 0x40, 0x38, 0x40, 0x42, 0x3c, 0x00,
	0x20, 0x30, 0x28, 0x24, 0x7e, 0x20, 0x20, 0x00, 0x7e, 0x02, 0x1e, 0x20, 0x40, 0x22, 0x1c, 0x00,
	0x38, 0x04, 0x02, 0x3e, 0x42, 0x42, 0x3c, 0x00, 0x7e, 0x42, 0x20, 0x10, 0x08, 0x08, 0x08, 0x00,
	0x3c, 0x42, 0x42, 0x3c, 0x42, 0x42, 0x3c, 0x00, 0x3c, 0x42, 0x42, 0x7c, 0x40, 0x20, 0x1c, 0x00,
	0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x10, 0x08, 0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x08,
	0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff,
	0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x08, 0x10, 0x10, 0x20, 0x10, 0x10, 0x08, 0x00, 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0x38, 0x7c, 0x00,
	0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x00, 0x08, 0x04, 0xfe, 0x04, 0x08, 0x00,
	0x38, 0x38, 0xd6, 0xfe, 0xd6, 0x10, 0x38, 0x00, 0x00, 0x3c, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c, 0x00,
	0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00, 0x3c, 0x42, 0x40, 0x30, 0x08, 0x00, 0x08, 0x00,
	0xff, 0xc3, 0x81, 0x81, 0x81, 0x81, 0xc3, 0xff, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0x10, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x08, 0x08, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff,
	0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 0x00,
	0x00, 0x10, 0x38, 0x54, 0x10, 0x10, 0x10, 0x00, 0x70, 0x18, 0x0c, 0x06, 0x0c, 0x18, 0x70, 0x00,
	0x3c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x3c, 0x00, 0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00,
	0x3c, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x00, 0x38, 0x44, 0x52, 0x6a, 0x32, 0x04, 0x78, 0x00,
	0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x0e, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0e, 0x00,
	0x00, 0x10, 0x10, 0x10, 0x54, 0x38, 0x10, 0x00, 0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00,
	0x00, 0x00, 0x20, 0x40, 0xfe, 0x40, 0x20, 0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10,
	0x00, 0x00, 0x80, 0x7c, 0x2a, 0x28, 0x28, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x00,
	0x24, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x24, 0x7e, 0x24, 0x7e, 0x24, 0x24, 0x00,
	0x10, 0x78, 0x14, 0x38, 0x50, 0x38, 0x10, 0x00, 0x00, 0x46, 0x26, 0x10, 0x08, 0x64, 0x62, 0x00,
	0x0c, 0x12, 0x12, 0x0c, 0x52, 0x22, 0x5c, 0x00, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00, 0x04, 0x08, 0x10, 0x10, 0x10, 0x08, 0x04, 0x00,
	0x00, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x00, 0x00, 0x10, 0x54, 0x38, 0x7c, 0x38, 0x54, 0x10, 0x00,
	0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f, 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81,
	0x08, 0x08, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x20, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
	0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0,
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x1c, 0x20, 0x3c, 0x22, 0x5c, 0x00,
	0x02, 0x02, 0x3a, 0x46, 0x42, 0x46, 0x3a, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x02, 0x42, 0x3c, 0x00,
	0x40, 0x40, 0x5c, 0x62, 0x42, 0x62, 0x5c, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x7e, 0x02, 0x3c, 0x00,
	0x30, 0x48, 0x08, 0x3e, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x5c, 0x62, 0x62, 0x5c, 0x40, 0x3c,
	0x02, 0x02, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x00, 0x10, 0x00, 0x18, 0x10, 0x10, 0x10, 0x38, 0x00,
	0x20, 0x00, 0x30, 0x20, 0x20, 0x20, 0x22, 0x1c, 0x02, 0x02, 0x22, 0x12, 0x0a, 0x16, 0x22, 0x00,
	0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 0x00, 0x6e, 0x92, 0x92, 0x92, 0x92, 0x00,
	0x00, 0x00, 0x3a, 0x46, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x3c, 0x00,
	0x00, 0x00, 0x3a, 0x46, 0x46, 0x3a, 0x02, 0x02, 0x00, 0x00, 0x5c, 0x62, 0x62, 0x5c, 0x40, 0x40,
	0x00, 0x00, 0x3a, 0x46, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x7c, 0x02, 0x3c, 0x40, 0x3e, 0x00,
	0x08, 0x08, 0x3e, 0x08, 0x08, 0x48, 0x30, 0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x62, 0x5c, 0x00,
	0x00, 0x00, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00, 0x00, 0x82, 0x92, 0x92, 0x92, 0x6c, 0x00,
	0x00, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, 0x42, 0x42, 0x62, 0x5c, 0x40, 0x3c,
	0x00, 0x00, 0x7e, 0x20, 0x18, 0x04, 0x7e, 0x00, 0x24, 0x00, 0x1c, 0x20, 0x3c, 0x22, 0x5c, 0x00,
	0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0xc0, 0x38, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x1c, 0x60, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x08, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x0c, 0x30, 0xc0, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
	0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0xff, 0x22, 0x22, 0x22, 0xff, 0x22, 0x22,
	0x04, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x32, 0x00, 0x00, 0x00,
	0x55, 0x22, 0x55, 0x88, 0x55, 0x22, 0x55, 0x88, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x0c, 0x03,
	0xc0, 0x30, 0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0c, 0x30, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x1c, 0x22, 0x22, 0x52, 0x42, 0x4a, 0x32, 0x00, 0x00, 0x44, 0x00, 0x44, 0x44, 0x64, 0x58, 0x00,
	0x00, 0x44, 0x00, 0x38, 0x44, 0x44, 0x38, 0x00, 0x42, 0x00, 0x42, 0x42, 0x42, 0x42, 0x3c, 0x00,
	0x42, 0x18, 0x24, 0x42, 0x7e, 0x42, 0x42, 0x00, 0x42, 0x18, 0x24, 0x42, 0x42, 0x24, 0x18, 0x00,
	0x08, 0x04, 0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x80, 0x60, 0x18, 0x04, 0x04, 0x02, 0x02, 0x01,
	0x01, 0x06, 0x18, 0x20, 0x20, 0x40, 0x40, 0x80, 0x10, 0x20, 0x20, 0x40, 0x40, 0x40, 0x80, 0x80,
	0x01, 0x01, 0x02, 0x02, 0x02, 0x04, 0x04, 0x08, 0x01, 0x02, 0x02, 0x04, 0x04, 0x18, 0x60, 0x80,
	0x80, 0x40, 0x40, 0x20, 0x20, 0x18, 0x06, 0x01, 0x80, 0x80, 0x40, 0x40, 0x40, 0x20, 0x20, 0x10,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0x38, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x1c, 0x03, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00,
	0x10, 0x08, 0x08, 0x04, 0x08, 0x08, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10,
	0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00,
	0x38, 0x38, 0x7c, 0x38, 0x10, 0x00, 0x7c, 0x00, 0xff, 0xef, 0xef, 0xef, 0xab, 0xc7, 0xef, 0xff,
	0xff, 0xef, 0xc7, 0xab, 0xef, 0xef, 0xef, 0xff, 0xff, 0xff, 0xef, 0xdf, 0x81, 0xdf, 0xef, 0xff,
	0xff, 0xff, 0xf7, 0xfb, 0x81, 0xfb, 0xf7, 0xff, 0xbd, 0xbd, 0xbd, 0x81, 0xbd, 0xbd, 0xbd, 0xff,
	0xc7, 0xbb, 0xfd, 0xfd, 0xfd, 0xbb, 0xc7, 0xff, 0x18, 0x24, 0x7e, 0xff, 0x5a, 0x24, 0x00, 0x00,
	0x07, 0xe2, 0x42, 0x7e, 0x42, 0xe2, 0x07, 0x00, 0x44, 0x7c, 0x54, 0x10, 0x10, 0x92, 0xfe, 0x82,
	0x38, 0x38, 0x10, 0x7c, 0x10, 0x10, 0x28, 0x44, 0x00, 0x88, 0x4b, 0x3f, 0x4b, 0x88, 0x00, 0x00,
	0x00, 0x11, 0xd2, 0xfc, 0xd2, 0x11, 0x00, 0x00, 0x44, 0x28, 0x10, 0x10, 0x7c, 0x10, 0x38, 0x38,
	0x3c, 0x7e, 0xff, 0xdb, 0xff, 0xe7, 0x7e, 0x3c, 0x3c, 0x42, 0x81, 0xa5, 0x81, 0x99, 0x42, 0x3c,
	0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x50, 0xa0, 0x50, 0xa0, 0x50, 0xa0, 0x50, 0xa0,
	0x05, 0x0a, 0x05, 0x0a, 0x05, 0x0a, 0x05, 0x0a, 0x55, 0xaa, 0x55, 0xaa, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0x2a, 0x15, 0x0a, 0x05, 0x02, 0x01, 0x00,
	0x55, 0xaa, 0x54, 0xa8, 0x50, 0xa0, 0x40, 0x80, 0x01, 0x02, 0x05, 0x0a, 0x15, 0x2a, 0x55, 0xaa,
	0x00, 0x80, 0x40, 0xa0, 0x50, 0xa8, 0x54, 0xaa, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08,
	0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x1c, 0x14, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x2a, 0x54, 0x2a, 0x54, 0x2a, 0x54, 0x00, 0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10,
	0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01, 0x00, 0x03, 0x13, 0x2a, 0x2a, 0xaa, 0x44, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xff, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xe0, 0x40,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xff, 0x40, 0x00, 0x00, 0x04, 0x0a, 0x11, 0xa0, 0x40, 0x00,
	0x00, 0x70, 0x88, 0x44, 0x23, 0x20, 0x40, 0x80, 0x88, 0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11,
	0x00, 0x0e, 0x11, 0x22, 0xc4, 0x04, 0x02, 0x01, 0x00, 0x23, 0x25, 0x29, 0xf1, 0x29, 0x25, 0x23,
	0x00, 0xc4, 0xa4, 0x94, 0x8f, 0x94, 0xa4, 0xc4, 0x11, 0x09, 0x05, 0x03, 0x03, 0x15, 0x19, 0x1d,
	0x15, 0x0d, 0x1d, 0x03, 0x03, 0x05, 0x09, 0x11, 0x01, 0x02, 0x04, 0x08, 0xf8, 0x04, 0x02, 0x01,
	0x00, 0x00, 0x24, 0x24, 0xe7, 0x24, 0x24, 0x00, 0x10, 0x10, 0x7c, 0x00, 0x00, 0x7c, 0x10, 0x10,
	0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x40, 0x20, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xee, 0xee, 0xee, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x0e, 0x0e, 0x0e,
	0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0xee, 0xee, 0xee, 0x00, 0x0e, 0x0e, 0x0e,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0xe0, 0xe0, 0xe0,
	0x00, 0xe0, 0xe0, 0xe0, 0x00, 0xe0, 0xe0, 0xe0, 0x00, 0xee, 0xee, 0xee, 0x00, 0xe0, 0xe0, 0xe0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xee, 0xee, 0xee, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0xee, 0xee, 0xee,
	0x00, 0xe0, 0xe0, 0xe0, 0x00, 0xee, 0xee, 0xee, 0x00, 0xee, 0xee, 0xee, 0x00, 0xee, 0xee, 0xee,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x63, 0x5d, 0x5d, 0x41, 0x5d, 0x55, 0x77,
	0x3f, 0x61, 0x5d, 0x21, 0x5d, 0x5d, 0x61, 0x3f, 0x7e, 0x43, 0x7d, 0x05, 0x05, 0x7d, 0x43, 0x7e,
	0x1f, 0x31, 0x6d, 0x55, 0x55, 0x6d, 0x31, 0x1f, 0x7f, 0x41, 0x7d, 0x11, 0x11, 0x7d, 0x41, 0x7f,
	0x7f, 0x41, 0x7d, 0x11, 0x1d, 0x05, 0x05, 0x07, 0x7e, 0x41, 0x7d, 0x05, 0x75, 0x5d, 0x41, 0x7e,
	0x77, 0x55, 0x5d, 0x41, 0x5d, 0x55, 0x55, 0x77, 0x7f, 0x41, 0x77, 0x14, 0x14, 0x77, 0x41, 0x7f,
	0xf8, 0x88, 0xd8, 0x50, 0x57, 0x5d, 0x63, 0x3e, 0x67, 0x55, 0x2d, 0x11, 0x11, 0x2d, 0x55, 0x67,
	0x07, 0x05, 0x05, 0x05, 0x05, 0x7d, 0x41, 0x7f, 0x7f, 0x41, 0x55, 0x55, 0x5d, 0x55, 0x55, 0x77,
	0x77, 0x59, 0x51, 0x45, 0x4d, 0x55, 0x55, 0x77, 0x3e, 0x63, 0x5d, 0x55, 0x55, 0x5d, 0x63, 0x3e,
	0x3f, 0x61, 0x5d, 0x5d, 0x61, 0x3d, 0x05, 0x07, 0x3e, 0x63, 0x5d, 0x5d, 0x55, 0x4d, 0x43, 0x3e,
	0x3f, 0x61, 0x5d, 0x5d, 0x21, 0x2d, 0x55, 0x67, 0x7e, 0x43, 0x7d, 0x23, 0x5e, 0x5f, 0x61, 0x3f,
	0x7f, 0x41, 0x77, 0x14, 0x14, 0x14, 0x14, 0x1c, 0x77, 0x55, 0x55, 0x55, 0x55, 0x5d, 0x63, 0x3e,
	0x77, 0x55, 0x55, 0x55, 0x55, 0x2a, 0x14, 0x08, 0x77, 0x55, 0x55, 0x5d, 0x55, 0x55, 0x41, 0x7f,
	0x63, 0x55, 0x2a, 0x14, 0x14, 0x2a, 0x55, 0x63, 0x77, 0x55, 0x49, 0x22, 0x14, 0x14, 0x14, 0x1c,
	0x7f, 0x41, 0x5f, 0x28, 0x14, 0x7a, 0x41, 0x7f, 0x00, 0x02, 0x05, 0x09, 0xff, 0x7e, 0x00, 0x00,
	0x00, 0x40, 0xa0, 0x90, 0xff, 0x7e, 0x00, 0x00, 0x00, 0x3e, 0x6b, 0x3e, 0x1c, 0x2a, 0x49, 0x00,
	0x49, 0x2a, 0x1c, 0x7f, 0x1c, 0x2a, 0x49, 0x00, 0x00, 0x00, 0x1c, 0x2a, 0x7f, 0x00, 0x00, 0x00,
	0x3e, 0x41, 0x4d, 0x55, 0x55, 0x59, 0x41, 0x3e, 0x1c, 0x12, 0x16, 0x14, 0x14, 0x36, 0x22, 0x3e,
	0x3e, 0x41, 0x5d, 0x53, 0x28, 0x74, 0x42, 0x7f, 0x3f, 0x41, 0x5f, 0x44, 0x44, 0x5f, 0x41, 0x3f,
	0x30, 0x28, 0x24, 0x2a, 0x6d, 0x41, 0x6f, 0x38, 0x7f, 0x41, 0x7d, 0x21, 0x5e, 0x5f, 0x61, 0x3f,
	0x7e, 0x41, 0x7d, 0x3d, 0x41, 0x5d, 0x41, 0x3e, 0x7f, 0x41, 0x5f, 0x28, 0x14, 0x0a, 0x0a, 0x0e,
	0x3e, 0x41, 0x5d, 0x3e, 0x41, 0x5d, 0x41, 0x3e, 0x3e, 0x41, 0x5d, 0x41, 0x5e, 0x5f, 0x41, 0x3f,
	0x1f, 0x11, 0x7d, 0x55, 0x5f, 0x44, 0x7c, 0x00, 0xf8, 0x88, 0xbe, 0xaa, 0xfa, 0x22, 0x3e, 0x00,
	0x3c, 0x5a, 0xff, 0xe7, 0x7e, 0x24, 0x42, 0x81, 0x3c, 0x5a, 0xff, 0xe7, 0x7e, 0x24, 0x24, 0x66,
	0x10, 0x38, 0x54, 0xfe, 0xee, 0x7c, 0x6c, 0xc6, 0x10, 0x38, 0x54, 0xfe, 0xee, 0x7c, 0x6c, 0x28,
	0x82, 0x45, 0x3c, 0x5a, 0x7e, 0xff, 0x42, 0xc6, 0x41, 0xa2, 0x3c, 0x5a, 0x7e, 0xff, 0x42, 0x63,
	0x00, 0x5a, 0xbd, 0x99, 0x24, 0x42, 0x24, 0x00, 0x81, 0xa5, 0x5a, 0x18, 0x18, 0x24, 0xc3, 0x00,
	0x00, 0x24, 0x7e, 0xbd, 0x7e, 0x24, 0x24, 0xe7, 0x24, 0x7e, 0xbd, 0x7e, 0x24, 0x42, 0x42, 0xc3,
	0x3c, 0x5a, 0xff, 0xd5, 0xab, 0xff, 0xbb, 0x91, 0x3c, 0x5a, 0xff, 0xd5, 0xab, 0xff, 0xee, 0x44,
	0x3c, 0x42, 0xa5, 0x81, 0x99, 0x81, 0xab, 0x55, 0x3c, 0x42, 0xa5, 0x81, 0x99, 0x81, 0xd5, 0xaa,
	0x42, 0x42, 0x66, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x38, 0x7f, 0xfc, 0xf0, 0xf0, 0xfc, 0x7f, 0x38,
	0x3c, 0x7e, 0xff, 0xff, 0xe7, 0x66, 0x42, 0x42, 0x1c, 0xfe, 0x3f, 0x0f, 0x0f, 0x3f, 0xfe, 0x1c,
	0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x08, 0x1c, 0x14, 0x14, 0x14, 0x3e, 0x7f, 0x6b,
	0x00, 0xc0, 0xe0, 0x7e, 0xe3, 0x7e, 0xe0, 0xc0, 0xd6, 0xfe, 0x7c, 0x28, 0x28, 0x28, 0x38, 0x10,
	0x00, 0x03, 0x07, 0x7e, 0xc7, 0x7e, 0x07, 0x03, 0x3c, 0x30, 0x3c, 0x18, 0x3c, 0x6e, 0x6e, 0x62,
	0x3c, 0x24, 0x3c, 0x18, 0x3c, 0x5a, 0x5a, 0x7e, 0x3c, 0x0c, 0x3c, 0x18, 0x3c, 0x76, 0x76, 0x46,
	0x7e, 0x7e, 0x24, 0x24, 0x24, 0x24, 0x24, 0x36, 0x7e, 0x7e, 0x24, 0x24, 0x24, 0x24, 0x24, 0x66,
	0x7e, 0x7e, 0x24, 0x24, 0x24, 0x24, 0x24, 0x6c, 0x44, 0xc6, 0xef, 0xed, 0xff, 0x7e, 0x3c, 0x3c,
	0x1c, 0x36, 0xff, 0xfc, 0xf0, 0xfc, 0x3f, 0x1c, 0x3c, 0x3c, 0x7e, 0xff, 0xb7, 0xf7, 0x63, 0x22,
	0x38, 0x6c, 0xff, 0x3f, 0x0f, 0x3f, 0xfc, 0x78, 0x3c, 0x7e, 0xff, 0xfd, 0xff, 0x7e, 0x3c, 0x3c,
	0x3c, 0x3c, 0x7e, 0xff, 0xbf, 0xff, 0x7e, 0x3c, 0x38, 0x6c, 0xff, 0xff, 0xff, 0xff, 0x7c, 0x38,
	0x1c, 0x36, 0xff, 0xff, 0xff, 0xff, 0x3e, 0x1c, 0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x18, 0x3c, 0x3c,
	0x00, 0x00, 0xde, 0xff, 0xff, 0xde, 0x00, 0x00, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x18,
	0x00, 0x00, 0x7b, 0xff, 0xff, 0x7b, 0x00, 0x00, 0x04, 0x06, 0x04, 0x04, 0x0c, 0x14, 0x3c, 0x3c,
	0x00, 0x02, 0xff, 0xd0, 0xe0, 0xc0, 0x00, 0x00, 0x3c, 0x3c, 0x28, 0x30, 0x20, 0x20, 0x60, 0x20,
	0x00, 0x40, 0xff, 0x0b, 0x07, 0x03, 0x00, 0x00, 0x08, 0x08, 0x1c, 0x3e, 0x49, 0x08, 0x08, 0x1c,
	0x00, 0x10, 0x08, 0x8c, 0xff, 0x8c, 0x08, 0x10, 0x1c, 0x08, 0x08, 0x49, 0x3e, 0x1c, 0x08, 0x08,
	0x00, 0x08, 0x10, 0x31, 0xff, 0x31, 0x10, 0x08, 0x00, 0x1e, 0x06, 0x0a, 0x12, 0x20, 0x40, 0x00,
	0x00, 0x40, 0x20, 0x12, 0x0a, 0x06, 0x1e, 0x00, 0x00, 0x02, 0x04, 0x48, 0x50, 0x60, 0x78, 0x00,
	0x00, 0x78, 0x60, 0x50, 0x48, 0x04, 0x02, 0x00, 0x18, 0x7e, 0x7e, 0xff, 0xc3, 0x81, 0x81, 0x81,
	0xf8, 0x1e, 0x0e, 0x0f, 0x0f, 0x0e, 0x1e, 0xf8, 0x81, 0x81, 0x81, 0xc3, 0xff, 0x7e, 0x7e, 0x18,
	0x1f, 0x78, 0x70, 0xf0, 0xf0, 0x70, 0x78, 0x1f, 0xfd, 0x85, 0xb5, 0xa5, 0xa5, 0xbd, 0x81, 0xff,
	0xff, 0x81, 0xbd, 0xa5, 0xa1, 0xbf, 0x80, 0xff, 0xff, 0x81, 0xbd, 0xa5, 0xa5, 0xad, 0xa1, 0xbf,
	0xff, 0x01, 0xfd, 0x85, 0xa5, 0xbd, 0x81, 0xff, 0x00, 0x18, 0x00, 0x3c, 0x00, 0x7e, 0x00, 0xff,
	0x80, 0xa0, 0xa8, 0xaa, 0xaa, 0xa8, 0xa0, 0x80, 0xff, 0x00, 0x7e, 0x00, 0x3c, 0x00, 0x18, 0x00,
	0x01, 0x05, 0x15, 0x55, 0x55, 0x15, 0x05, 0x01, 0x00, 0x10, 0x38, 0x7c, 0x00, 0x10, 0x38, 0x7c,
	0x00, 0x00, 0x88, 0xcc, 0xee, 0xcc, 0x88, 0x00, 0x00, 0x7c, 0x38, 0x10, 0x00, 0x7c, 0x38, 0x10,
	0x00, 0x00, 0x22, 0x66, 0xee, 0x66, 0x22, 0x00, 0x00, 0x00, 0xe7, 0xa5, 0xe7, 0x00, 0x00, 0x00,
	0x08, 0x1c, 0x2a, 0x08, 0x08, 0x2a, 0x1c, 0x08, 0x00, 0x00, 0x24, 0x42, 0xff, 0x42, 0x24, 0x00,
	0xfe, 0x82, 0x44, 0x38, 0x10, 0x10, 0x10, 0xfe, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xa5, 0x42, 0xa5, 0x00, 0x00, 0xa5, 0x42, 0xa5,
	0x24, 0x42, 0x81, 0x00, 0x00, 0x81, 0x42, 0x24, 0xff, 0x01, 0xf9, 0x05, 0x05, 0x05, 0x05, 0x05,
	0xff, 0x80, 0xa7, 0x88, 0xa8, 0x88, 0xa8, 0x88, 0x00, 0x00, 0x00, 0xff, 0x05, 0xf5, 0x05, 0xff,
	0x00, 0x00, 0x00, 0xff, 0x82, 0x82, 0xaa, 0xff, 0x05, 0xf9, 0x01, 0xff, 0x0c, 0x0c, 0x0c, 0x1e,
	0x88, 0x87, 0x80, 0xff, 0x30, 0x30, 0x30, 0x78, 0x01, 0x55, 0x01, 0xa9, 0x01, 0xf1, 0x01, 0xff,
	0x80, 0x95, 0x80, 0x8a, 0x80, 0x87, 0x80, 0xff, 0x3c, 0x42, 0xd5, 0xab, 0x08, 0x08, 0x28, 0x10,
	0x00, 0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x24, 0x18, 0x00,
	0x3c, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x00,
	0x3c, 0x42, 0xb9, 0x85, 0x85, 0xb9, 0x42, 0x3c, 0xff, 0xff, 0xff, 0xe7, 0xe7, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xff, 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff,
	0x04, 0x0c, 0x04, 0x04, 0xff, 0x7e, 0x3c, 0x00, 0x3c, 0x42, 0x81, 0xff, 0xff, 0x81, 0x42, 0x3c,
	0x3c, 0x5a, 0x99, 0x99, 0x99, 0x99, 0x5a, 0x3c, 0x3c, 0x5a, 0x99, 0xff, 0xff, 0x99, 0x5a, 0x3c,
	0x00, 0x14, 0x7f, 0x55, 0x7f, 0x2a, 0x1c, 0x08, 0xf0, 0x0c, 0x02, 0x72, 0x51, 0x71, 0x01, 0x81,
	0x0f, 0x30, 0x40, 0x4e, 0x8a, 0x8e, 0x80, 0x81, 0xf0, 0x0c, 0x02, 0x02, 0x71, 0x01, 0x01, 0x81,
	0x0f, 0x30, 0x40, 0x40, 0x8e, 0x80, 0x80, 0x81, 0x81, 0x01, 0x11, 0x21, 0xc2, 0x02, 0x0c, 0xf0,
	0x81, 0x80, 0x88, 0x84, 0x43, 0x40, 0x30, 0x0f, 0x81, 0x01, 0x01, 0xe1, 0x02, 0x02, 0x0c, 0xf0,
	0x81, 0x80, 0x80, 0x87, 0x40, 0x40, 0x30, 0x0f, 0x81, 0x01, 0xc1, 0x21, 0xc2, 0x02, 0x0c, 0xf0,
	0x81, 0x80, 0x83, 0x84, 0x43, 0x40, 0x30, 0x0f, 0x81, 0x01, 0xe1, 0x11, 0x12, 0x02, 0x0c, 0xf0,
	0x81, 0x80, 0x87, 0x88, 0x48, 0x40, 0x30, 0x0f, 0x10, 0x08, 0x2a, 0x7f, 0x7f, 0x7f, 0x7f, 0x3e,
	0x00, 0x60, 0x10, 0x08, 0x0c, 0x1e, 0x1e, 0x0c, 0x00, 0x4a, 0x2c, 0x60, 0x06, 0x34, 0x52, 0x00,
	0x89, 0x4a, 0x00, 0xc0, 0x03, 0x00, 0x52, 0x91, 0x01, 0x03, 0x07, 0x0f, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x80, 0x40, 0xff, 0xc3, 0xc3, 0xff, 0x00, 0x00, 0x01, 0x02, 0xff, 0xc3, 0xc3, 0xff,
	0x00, 0x03, 0x04, 0x08, 0x3f, 0x7f, 0xff, 0x3f, 0x80, 0xc0, 0xe0, 0xf0, 0xff, 0xff, 0xff, 0xff,
	0x40, 0x28, 0x14, 0x10, 0x28, 0x28, 0x10, 0x00, 0x00, 0x7f, 0x42, 0x04, 0x08, 0x04, 0x42, 0x7f,
	0x00, 0xc0, 0x20, 0x10, 0xfc, 0xfe, 0xff, 0xfc, 0x00, 0x04, 0x08, 0x08, 0x08, 0x14, 0x12, 0x61,
	0x00, 0x3c, 0x42, 0x42, 0x42, 0x24, 0xa5, 0xe7, 0x00, 0x22, 0x41, 0x41, 0x49, 0x36, 0x00, 0x00,
	0x00, 0x00, 0x36, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00, 0x40, 0x36, 0x09, 0x09, 0x76, 0x00, 0x00,
	0x00, 0x78, 0x08, 0x0a, 0x0a, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x3e, 0x00, 0x08, 0x00,
	0x00, 0x8f, 0xda, 0xaa, 0xaa, 0x8a, 0x8a, 0x00, 0xff, 0x91, 0x89, 0xa3, 0xc5, 0x91, 0x89, 0xff,
	0xff, 0xc3, 0xa5, 0x99, 0x99, 0xa5, 0xc3, 0xff, 0x00, 0x49, 0x2a, 0x1c, 0x77, 0x1c, 0x2a, 0x49,
	0xff, 0x99, 0x99, 0xff, 0xff, 0x99, 0x99, 0xff, 0x49, 0x2a, 0x1c, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x1c, 0x08, 0x1c, 0x08, 0x1c, 0x08, 0x1c, 0x08, 0x00, 0x00, 0x00, 0x55, 0xff, 0x55, 0x00, 0x00,
	0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x3e, 0x7e, 0x42, 0x7e, 0x42, 0x7e, 0x42, 0x7e, 0x42,
	0x00, 0xff, 0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0d, 0x31, 0xc1, 0xff,
	0x00, 0x00, 0x00, 0xc0, 0xb0, 0x8c, 0x83, 0xff, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff,
	0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x03,
	0xc0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xe0, 0xc0, 0xc0, 0x30, 0xfc, 0xfc, 0xff, 0xfe, 0xec, 0xf8,
	0x03, 0x0c, 0x1d, 0x3b, 0x77, 0x6f, 0xdf, 0xdf, 0x70, 0x70, 0x50, 0x20, 0x80, 0x80, 0xc0, 0xf0,
	0x5e, 0x2e, 0x2f, 0x2f, 0x2f, 0x5f, 0xbf, 0xbf, 0x20, 0x72, 0x27, 0x62, 0xf6, 0xfe, 0x06, 0xfc,
	0x04, 0x4e, 0xe4, 0x46, 0x6f, 0x7f, 0x60, 0x3f, 0xdc, 0x8c, 0xd8, 0xf8, 0x08, 0xf8, 0xf0, 0xe0,
	0x3b, 0x31, 0x1b, 0x1f, 0x10, 0x1f, 0x0f, 0x07, 0x80, 0xc0, 0xe0, 0x60, 0x70, 0x7c, 0x0e, 0x0c,
	0x01, 0x03, 0x07, 0x06, 0x0e, 0x3e, 0x70, 0x30, 0x78, 0x70, 0x60, 0xe0, 0xc0, 0xec, 0xfe, 0xd1,
	0x1e, 0x0e, 0x06, 0x07, 0x03, 0x37, 0x7f, 0x8b, 0x80, 0xcc, 0xde, 0x9a, 0x31, 0xfb, 0xfe, 0xfc,
	0x01, 0x33, 0x7b, 0x59, 0x8c, 0xdf, 0x7f, 0x3f, 0xfc, 0xf8, 0xf8, 0xf0, 0xf0, 0xfe, 0x00, 0xff,
	0x3f, 0x1f, 0x1f, 0x0f, 0x0f, 0x7f, 0x00, 0xff, 0x00, 0x80, 0x40, 0x20, 0x40, 0x80, 0xf8, 0xf8,
	0x00, 0x01, 0x02, 0x04, 0x02, 0x01, 0x1f, 0x1f, 0x40, 0x40, 0x40, 0x40, 0xf8, 0x04, 0xfe, 0x00,
	0x02, 0x02, 0x02, 0x02, 0x1f, 0x20, 0x7f, 0x00, 0xce, 0xce, 0xce, 0xfe, 0xfc, 0xf8, 0xf0, 0xf0,
	0x73, 0x73, 0x73, 0x7f, 0x3f, 0x1f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0x18, 0xfe, 0x02, 0xfe, 0xff,
	0x0f, 0x0f, 0x0f, 0x18, 0x7f, 0x40, 0x7f, 0xff, 0x1f, 0x22, 0x42, 0x84, 0x84, 0x42, 0x22, 0x1f,
	0xff, 0xa0, 0xe0, 0x00, 0x00, 0xe0, 0xa0, 0xff, 0x3f, 0x61, 0x41, 0x81, 0x81, 0x41, 0x61, 0x3f,
	0x00, 0x00, 0x01, 0x02, 0xfe, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0xff, 0x00, 0x10, 0x30, 0x50, 0x9f, 0x50, 0x30, 0x10,
	0x00, 0x10, 0x30, 0x5c, 0x97, 0x5c, 0x30, 0x10, 0xf8, 0x14, 0x12, 0x7f, 0x11, 0x11, 0xf1, 0x00,
	0x02, 0x03, 0x02, 0x67, 0x90, 0x40, 0x20, 0xf0, 0x02, 0x03, 0x02, 0x47, 0x60, 0x50, 0xf8, 0x40,
	0x02, 0x03, 0x02, 0xf7, 0x80, 0xe0, 0x80, 0xf0, 0x02, 0x05, 0x04, 0xf2, 0x87, 0xe0, 0x80, 0xf0,
	0x03, 0x06, 0x18, 0x60, 0x18, 0x06, 0x01, 0x7f, 0x80, 0x60, 0x18, 0x06, 0x18, 0x60, 0x80, 0xfe,
	0x00, 0x80, 0x60, 0xb8, 0x54, 0x54, 0x54, 0xf8, 0xd8, 0xf1, 0xa6, 0x88, 0x93, 0x95, 0x8d, 0xcf,
	0x32, 0xef, 0x0f, 0x18, 0xe0, 0x40, 0x7c, 0x7f, 0xfe, 0xf9, 0x8c, 0x82, 0x81, 0x81, 0x9f, 0xbf,
	0x11, 0x40, 0x02, 0x00, 0x11, 0x82, 0x00, 0x89, 0x02, 0x80, 0x11, 0x00, 0x02, 0x20, 0x01, 0x88,
	0x00, 0x0c, 0x1a, 0xbf, 0xff, 0x9e, 0x0c, 0x00, 0x00, 0x30, 0x58, 0xfd, 0xff, 0x79, 0x30, 0x00,
	0x00, 0x0c, 0x1a, 0xbf, 0xfc, 0x9f, 0x0c, 0x00, 0x00, 0x30, 0x58, 0xfd, 0x3f, 0xf9, 0x30, 0x00,
	0x08, 0x14, 0x16, 0x3d, 0x3f, 0x1e, 0x08, 0x1c, 0x5d, 0x77, 0x55, 0x1c, 0x1c, 0x5d, 0x7f, 0x5d,
	0x5d, 0x7f, 0x5d, 0x1c, 0x1c, 0x55, 0x77, 0x5d, 0x00, 0xe7, 0x42, 0xff, 0xf9, 0xff, 0x42, 0xe7,
	0x00, 0xe7, 0x42, 0xff, 0x9f, 0xff, 0x42, 0xe7, 0x00, 0x00, 0x3f, 0x38, 0xfe, 0xc6, 0x7c, 0x00,
	0x00, 0x00, 0xfc, 0x1c, 0x7f, 0x63, 0x3e, 0x00, 0xff, 0x81, 0xa5, 0x81, 0x81, 0xa5, 0x81, 0xff,
	0xe7, 0x81, 0x81, 0x00, 0x00, 0x81, 0x81, 0xe7, 0x00, 0x20, 0x10, 0x7f, 0x08, 0x7f, 0x04, 0x02,
	0x18, 0x24, 0x24, 0x04, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x24, 0x24, 0x18
};

