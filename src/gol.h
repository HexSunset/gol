#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Board Board;
struct Board {
  uint64_t width;
  uint64_t height;
  uint8_t *ptr;
};

typedef enum State {
  ALIVE = 0,
  DEAD = 1,
} State;

Board *board_create(uint64_t width, uint64_t height) {
  uint8_t *ptr = malloc(width * height * sizeof(uint8_t));
  memset(ptr, DEAD, width * height);

  Board *b = malloc(sizeof(Board));
  b->width = width;
  b->height = height;
  b->ptr = ptr;

  return b;
}

Board *board_copy(Board *b) {
  Board *new_board = board_create(b->width, b->height);
  memcpy(new_board->ptr, b->ptr, b->width * b->height);
  return new_board;
}

void board_destroy(Board *board) {
  free(board->ptr);
  free(board);
}

uint64_t board_get_linear_coordinate(Board *b, uint64_t x, uint64_t y) {
  return y * b->width + x;
}

// returns 0 on success
uint8_t board_set_square(Board *board, uint64_t x, uint64_t y, State s) {
  if (x >= board->width || x < 0 || y >= board->height || y < 0) {
    // out of bounds.
    return -1;
  }

  uint64_t coord = board_get_linear_coordinate(board, x, y);
  board->ptr[coord] = s;

  return 0;
}

State board_get_square(Board *board, uint64_t x, uint64_t y) {
  uint64_t coord = board_get_linear_coordinate(board, x, y);
  return board->ptr[coord];
}

void board_next_generation(Board *b, Board *new_board) {
  // iterate over squares.
  for (uint64_t y = 0; y < b->height; y++) {
    for (uint64_t x = 0; x < b->width; x++) {
      uint8_t live_neighbors = 0;

      // nw neighbor
      if (x > 0 && y > 0) {
	if (board_get_square(b, x - 1, y - 1) == ALIVE) {
	  live_neighbors++;
	}
      }
      // n neighbor
      if (y > 0) {
	if (board_get_square(b, x, y - 1) == ALIVE) {
	  live_neighbors++;
	}
      }
      // ne neighbor
      if (x < (b->width - 1) && y > 0) {
	if (board_get_square(b, x + 1, y - 1) == ALIVE) {
	  live_neighbors++;
	}
      }
      // w neighbor
      if (x > 0) {
	if (board_get_square(b, x - 1, y) == ALIVE) {
	  live_neighbors++;
	}
      }
      // e neighbor
      if (x < (b->width - 1)) {
	if (board_get_square(b, x + 1, y) == ALIVE) {
	  live_neighbors++;
	}
      }
      // sw neighbor
      if (x > 0 && y < b->height - 1) {
	if (board_get_square(b, x - 1, y + 1) == ALIVE) {
	  live_neighbors++;
	}
      }
      // s neighbor
      if (y < b->height - 1) {
	if (board_get_square(b, x, y + 1) == ALIVE) {
	  live_neighbors++;
	}
      }
      // se neighbor
      if (x < b->width - 1 && y < b->height - 1) {
	if (board_get_square(b, x + 1, y + 1) == ALIVE) {
	  live_neighbors++;
	}
      }

      bool is_alive = board_get_square(b, x, y) == ALIVE;
      if (is_alive && live_neighbors < 2) {
	board_set_square(new_board, x, y, DEAD);
      }
      if (is_alive && live_neighbors >= 4) {
	board_set_square(new_board, x, y, DEAD);
      }
      if (!is_alive && live_neighbors == 3) {
	board_set_square(new_board, x, y, ALIVE);
      }
    }
  }
}

#endif
