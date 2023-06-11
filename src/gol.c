#include <stdio.h>
#include <string.h>
#include <math.h>
#include <argp.h>

#include <raylib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBI_FAILURE_USERMSG

#include "gol.h"

void print_usage() {
  fputs("USAGE: gol [SUBCOMMAND] ...\n", stderr);
  fputs("SUBCOMMANDS:\n", stderr);
  fputs("    dim: specify width and height of board.\n", stderr);
  fputs("    file: take a grayscale image file as the starting state for the board.\n", stderr);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage();
    return 1;
  }

  uint64_t width, height;

  Board *board;

  if (strcmp(argv[1], "dim") == 0) {
    if (argc != 4) {
      print_usage();
      return 1;
    }

    width = strtol(argv[2], NULL, 0);
    height = strtol(argv[3], NULL, 0);

    board = board_create(width, height);
  } else if (strcmp(argv[1], "file") == 0) {
    if (argc != 3) {
      print_usage();
      return 1;
    }

    char *fpath = argv[2];
    int x, y, n;
    uint8_t *pixels = stbi_load(fpath, &x, &y, &n, 1);
    if (pixels == NULL) {
      fputs(stbi_failure_reason(), stderr);
      return 1;
    }
    
    width = x;
    height = y;

    board = board_create(width, height);

    for (uint64_t j = 0; j < y; j++) {
      for (uint64_t i = 0; i < x; i++) {
	if (pixels[j * y + i] >= 127) {
	  board_set_square(board, i, j, ALIVE);
	} else {
	  board_set_square(board, i, j, DEAD);
	}
      }
    }
  } else {
    print_usage();
    return 1;
  }

  const uint64_t window_width = 900;
  const uint64_t window_height = 900;
  const uint64_t square_width = round(window_width / width);
  const uint64_t square_height = round(window_height / height);

  uint64_t generation = 0;
  char *title = malloc(255 * sizeof(char));

  snprintf(title, 255, "Game of Life - generation %u", generation);
  InitWindow(window_width, window_height, title);

  Board *saved_board = NULL;
  uint64_t saved_generation = 0;

  while (!WindowShouldClose()) {
    // --- Update

    // check mouse click

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      // square mouse is in
      uint64_t x = floor(GetMouseX() / square_width);
      uint64_t y = floor(GetMouseY() / square_height);

      // toggle square
      if (board_get_square(board, x, y) == ALIVE) {
	board_set_square(board, x, y, DEAD);
      } else {
	board_set_square(board, x, y, ALIVE);
      }

      // set title to EDITED
      snprintf(title, 255, "Game of Life - generation %u [EDITED]", generation);
      SetWindowTitle(title);
    }

    // check if we should calculate next gen
    if (IsKeyPressed(KEY_SPACE)) {
      Board *old_board = board;
      Board *new_board = board_copy(old_board);
      board_next_generation(old_board, new_board);
      board_destroy(old_board);
      board = new_board;
      generation++;
      snprintf(title, 255, "Game of Life - generation %u", generation);
      SetWindowTitle(title);
    }

    // check for board clear
    if (IsKeyPressed(KEY_C)) {
      memset(board->ptr, DEAD, board->width * board->height);
    }

    // check for savestate load
    if (IsKeyPressed(KEY_L)) {
      if (saved_board != NULL) {
	board_destroy(board);
	board = board_copy(saved_board);
	generation = saved_generation;

	snprintf(title, 255, "Game of Life - generation %u [LOADED]", generation);
	SetWindowTitle(title);
      }
    }

    // check for savestate save
    if (IsKeyPressed(KEY_S)) {
      if (saved_board != NULL) {
	board_destroy(saved_board);
      }
      saved_board = board_copy(board);
      saved_generation = generation;
      snprintf(title, 255, "Game of Life - generation %u [SAVED]", generation);
      SetWindowTitle(title);
    }

    // --- Draw
    BeginDrawing();

    ClearBackground(BLACK);
    for (uint64_t y = 0; y < height; y++) {
      for (uint64_t x = 0; x < width; x++) {
	if (board_get_square(board, x, y) == ALIVE) {
	  DrawRectangle(x * square_width, y * square_height, square_width, square_height, RAYWHITE);
	}
      }
    }
    EndDrawing();
  }

  return 0;
}
