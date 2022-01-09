#ifndef _GdlBtn_
#define _GdlBtn_

#include "./gdl.h"

#pragma pack(push, 1)
union btn {
  uint8_t raw;
  struct {
/*
   uint8_t down:1;
   uint8_t left:1;
   uint8_t right:1;
   uint8_t up:1;
   uint8_t a:1;
   uint8_t b:1;
   uint8_t menu:1;
   uint8_t home:1;
*/
   uint8_t left:1; // left
   uint8_t right:1; // right
   uint8_t up:1; // up
   uint8_t a:1; // a
   uint8_t b:1; // b
   uint8_t menu:1; // menu
   uint8_t down:1; // ?
   uint8_t home:1;
  };
};
#pragma pack(pop)

struct buttons {
  union btn current;
  union btn last;
};

extern struct buttons buttons;

void initButtons(void);
void readButtons(void);

#define keyUp (!buttons.current.up)
#define keyDown (!buttons.current.down)
#define keyLeft (!buttons.current.left)
#define keyRight (!buttons.current.right)
#define keyA (!buttons.current.a)
#define keyB (!buttons.current.b)
#define keyMenu (!buttons.current.menu)
#define keyHome (!buttons.current.home)

#endif
