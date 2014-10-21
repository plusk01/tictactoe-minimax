/*
 * minimax.c
 *
 *  Created on: Oct 18, 2014
 *      Author: plusk01
 */

#include "minimax.h"

#define SCORES_MAX		(MINIMAX_BOARD_ROWS * MINIMAX_BOARD_COLUMNS)
#define MOVES_MAX		SCORES_MAX

static minimax_move_t choice; // the move computed by minimax

// Helper function prototypes
minimax_score_t minimax_recurse(minimax_board_t* board, bool player);


// ----------------------------------------------------------------------------

void minimax_initBoard(minimax_board_t* board) {
	// clear the board
}

// ----------------------------------------------------------------------------

void minimax_computeNextMove(minimax_board_t* board, bool player, uint8_t* row, uint8_t* column) {
	// call the recursive algorithm
	minimax_recurse(board, player);

	// minimax_recurse() sets the global var, choice, so now return the
	// row and column from the choice that has been calculated by the algorithm.
	*row = choice.row;
	*column = choice.column;
}

// ----------------------------------------------------------------------------

bool minimax_isGameOver(minimax_score_t score) {
	return (score != MINIMAX_NOT_ENDGAME);
}

// ----------------------------------------------------------------------------

int16_t minimax_computeBoardScore(minimax_board_t* board, bool player) {
	// go get the candidate, figure out who the player is, +/- based on that.
}

// ----------------------------------------------------------------------------
// Helper Functions
// ----------------------------------------------------------------------------

minimax_score_t minimax_recurse(minimax_board_t* board, bool player) {
	if (minimax_checkForWinner(board)) {
		return minimax_computeBoardScore(board, !player);
	}

	minimax_score_t scores[SCORES_MAX] = { 0 };
	minimax_move_t moves[MOVES_MAX] = { 0 };

	uint8_t counter = 0;
	while (minimax_thereAreMoreMoves(board)) {
		minimax_move_t move = minimax_getNextMove(board);

		if (player) {
			board->squares[move.row][move.column] = MINIMAX_PLAYER_SQUARE;
		} else {
			board->squares[move.row][move.column] = MINIMAX_OPPONENT_SQUARE;
		}

		minimax_score_t score = minimax_recurse(board, !player);

		// add the score to the move-score table
		scores[counter] = score;
		// add the move to the move-score table
		moves[counter] = move;

		// Undo the change to the board (return the square to empty)
		board->squares[move.row][move.column] = MINIMAX_EMPTY_SQUARE;

		counter++;
	}

	// Now that I'm here, the while-loop has completed and all the scores
	// in the move-score table for this level have been computed.

	// Now I need to return the score depending on whether you are
	// computing min or max (based on the player flag)
	minimax_score_t finalScore = 0;
	if (player) {
		// max
		choice = 0;
		finalScore = 0;
	} else {
		// min
		choice = 0;
		finalScore = 0;
	}

	return score;
}

// ----------------------------------------------------------------------------

bool minimax_thereAreMoreMoves(minimax_board_t* board) {
	uint8_t row = 0, col = 0;

	for (row=0; row<MINIMAX_BOARD_ROWS; row++) {
		for (col=0; col<MINIMAX_BOARD_COLUMNS; col++) {
			// TODO: I don't think this is right. There could be no
			// more moves and still have empty squares
			if (board->squares[row][col] == MINIMAX_EMPTY_SQUARE) {
				return true;
			}
		}
	}
}

// ----------------------------------------------------------------------------

minimax_move_t minimax_getNextMove(minimax_board_t* board) {

}

// ----------------------------------------------------------------------------

bool minimax_checkForWinner(minimax_board_t* board) {
	uint8_t candidate;
	return (minimax_isVerticalWinner(board, &candidate) \
			|| minimax_isHorizontalWinner(board, &candidate) \
			|| minimax_isDiagonalWinnder(board, &candidate));
}

// ----------------------------------------------------------------------------

bool minimax_isVerticalWinnner(minimax_board_t* board, uint8_t* candidate) {
	for (col=0; col<MINIMAX_BOARD_COLUMNS; col++) {
		*candidate = board->squares[0][col];

		// start iterating on the next row
		uint8_t winner = true;
		for (row=1; row<MINIMAX_BOARD_ROWS; row++) {
			if (board->squares[row][col] != *candidate) {
				winner = false;
				break;
			}
		}

		if (winner) return true;
	}

	return false;
}

// ----------------------------------------------------------------------------

bool minimax_isHorizontalWinnner(minimax_board_t* board, uint8_t* candidate) {
	for (row=0; row<MINIMAX_BOARD_ROWS; row++) {
		*candidate = board->squares[row][0];

		// start iterating on the next column
		uint8_t winner = true;
		for (col=1; col<MINIMAX_BOARD_COLUMNS; col++) {
			if (board->squares[row][col] != *candidate) {
				winner = false;
				break;
			}
		}
		if (winner) return true;
	}
}

// ----------------------------------------------------------------------------

bool minimax_isDiagonalWinnner(minimax_board_t* board, uint8_t* candidate) {
	uint8_t candidate1 = board->squares[0][0];
	uint8_t candidate2 = board->squares[2][0];

	bool candidate1_win = ((candidate1 == board->squares[1][1]) \
						(candidate1 == board->squares[2][2]));

	bool candidate2_win = ((candidate2 == board->squares[1][1]) \
						(candidate2 == board->squares[0][2]));

	if (candidate1) *candidate = candidate1;
	if (candidate2) *candidate = candidate2;

	return (candidate1_win || candidate2_win);
}

// ----------------------------------------------------------------------------

void minimax_printBoard(minimax_board_t* board) {
	uint8_t row = 0, col = 0;

	for (row=0; row<MINIMAX_BOARD_ROWS; row++) {
		for (col=0; col<MINIMAX_BOARD_COLUMNS; col++) {
			char c;

			switch(board->squares[row][col]) {
				case MINIMAX_EMPTY_SQUARE:
					c = '-';
					break;
				case MINIMAX_OPPONENT_SQUARE:
					c = 'O';
					break;
				case MINIMAX_PLAYER_SQUARE:
					c = 'X';
					break;
				case MINIMAX_USED_SQUARE:
					c = 'U';
					break;
				default:
					c = 'E';
			}

			printf("%c ", c);
		}
		printf("\n\r");
	}
}
