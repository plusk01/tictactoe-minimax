/*
 * minimax.c
 *
 *  Created on: Oct 18, 2014
 *      Author: plusk01
 */

#include "minimax.h"

#define DEBUG_END_PRINT		true
#define DEBUG_PLAY_SELF		true

// Winning scenario products
#define PRODUCT_PLAYER_WIN		(8) // 2 * 2 * 2
#define PRODUCT_OPPONENT_WIN	(1) // 1 * 1 * 1

#define SCORES_MAX		(MINIMAX_BOARD_ROWS * MINIMAX_BOARD_COLUMNS)
#define MOVES_MAX		SCORES_MAX

static minimax_move_t choice; // the move computed by minimax

// Helper function prototypes
minimax_score_t minimax_recurse(minimax_board_t* board, bool player);
uint8_t minimax_getIndex(minimax_score_t *scores, uint8_t length, bool getMax);
bool minimax_isBoardFull(minimax_board_t* board);
bool minimax_isVerticalWinnner(minimax_board_t* board, uint8_t* candidate);
bool minimax_isHorizontalWinnner(minimax_board_t* board, uint8_t* candidate);
bool minimax_isDiagonalWinnner(minimax_board_t* board, uint8_t* candidate);
void minimax_printBoard(minimax_board_t* board, minimax_move_t* move, bool mark);

// ----------------------------------------------------------------------------

void minimax_initBoard(minimax_board_t* board) {
	// clear the board
	memset(board->squares, MINIMAX_EMPTY_SQUARE, \
			sizeof(board->squares[0][0]) * MINIMAX_BOARD_ROWS * MINIMAX_BOARD_COLUMNS);
}

// ----------------------------------------------------------------------------

void minimax_computeNextMove(minimax_board_t* board, bool player, uint8_t* row, uint8_t* column) {
	// call the recursive algorithm
	minimax_score_t score = minimax_recurse(board, player);

#if DEBUG_END_PRINT
	printf("Chosen score: %d\r\n", score);
	minimax_printBoard(board, &choice, true);
#endif

	// minimax_recurse() sets the global var, choice, so now return the
	// row and column from the choice that has been calculated by the algorithm.
	*row = choice.row;
	*column = choice.column;

#if DEBUG_PLAY_SELF
	board->squares[choice.row][choice.column] = (player) ? \
					MINIMAX_PLAYER_SQUARE : MINIMAX_OPPONENT_SQUARE;
#endif
}

// ----------------------------------------------------------------------------

bool minimax_isGameOver(minimax_score_t score) {
	return (score != MINIMAX_NOT_ENDGAME);
}

// ----------------------------------------------------------------------------

int16_t minimax_computeBoardScore(minimax_board_t* board, bool player) {
	// go get the candidate, figure out who the player is, +/- based on that.
	uint8_t winner = MINIMAX_EMPTY_SQUARE;
	minimax_score_t score = (player) ? MINIMAX_PLAYER_WINNING_SCORE : MINIMAX_OPPONENT_WINNING_SCORE;

	// find a winner
	if (!winner) minimax_isVerticalWinnner(board, &winner);
	if (!winner) minimax_isHorizontalWinnner(board, &winner);
	if (!winner) minimax_isDiagonalWinnner(board, &winner);

	// figure out score
	if (winner) {
		return score;
	} else if (minimax_isBoardFull(board)) {
		return MINIMAX_DRAW_SCORE;
	} else {
		return MINIMAX_NOT_ENDGAME;
	}

}

// ----------------------------------------------------------------------------
// Helper Functions
// ----------------------------------------------------------------------------

minimax_score_t minimax_recurse(minimax_board_t* board, bool player) {
	// Compute this board's score.
	// This checks if there is a winner and returns appropriate score
	minimax_score_t scoreTest = minimax_computeBoardScore(board, !player);

	if (minimax_isGameOver(scoreTest)) {
		// Base case
		return scoreTest;
	}

	minimax_score_t scores[SCORES_MAX] = { 0 };
	minimax_move_t moves[MOVES_MAX] = { 0 };

	uint8_t row, col, counter = 0;
	for (row=0; row<MINIMAX_BOARD_ROWS; row++) {

		// go through each of this row's columns
		for (col=0; col<MINIMAX_BOARD_COLUMNS; col++) {
			// We only care about places we can move, ie, empty squares
			if (board->squares[row][col] != MINIMAX_EMPTY_SQUARE) continue;

			// Mark the board with an appropriate mark
			if (player) {
				board->squares[row][col] = MINIMAX_PLAYER_SQUARE;
			} else {
				board->squares[row][col] = MINIMAX_OPPONENT_SQUARE;
			}

			// create a move based on the current (row, col)
			minimax_move_t move = { row, col };

			// minimax to find the score of that move
			minimax_score_t score = minimax_recurse(board, !player);

			// add the score to the move-score table
			scores[counter] = score;
			// add the move to the move-score table
			moves[counter] = move;

			// Undo the change to the board (return the square to empty)
			// This is because board is a pointer, and we don't want to
			// change the original board
			board->squares[row][col] = MINIMAX_EMPTY_SQUARE;

			counter++;
		}
	}

	// Now that I'm here, the while-loop has completed and all the scores
	// in the move-score table for this level have been computed.

	// Now I need to return the score depending on whether you are
	// computing min or max (based on the player flag)

	// if player, get max
	uint8_t index = minimax_getIndex(scores, counter, player);
	choice = moves[index];
	minimax_score_t finalScore = scores[index];

	return finalScore;
}

// ----------------------------------------------------------------------------

uint8_t minimax_getIndex(minimax_score_t *scores, uint8_t length, bool getMax) {
	// Set score to something out of range, depending on what you're looking for
	// if getMax is true, use a lower bound; if getMax is false, use an upper bound
	minimax_score_t score = (getMax) ? \
				(MINIMAX_OPPONENT_WINNING_SCORE - 1) : \
				(MINIMAX_PLAYER_WINNING_SCORE + 1);
	uint8_t index = 0;

	uint8_t i;
	for (i=0; i<length; i++) {
		if (getMax) {
			if (scores[i] > score) {
				score = scores[i];
				index = i;
			}
		} else {
			if (scores[i] < score) {
				score = scores[i];
				index = i;
			}
		}
	}

	return index;
}

// ----------------------------------------------------------------------------

bool minimax_isBoardFull(minimax_board_t* board) {
	uint8_t row, col;

	for (col=0; col<MINIMAX_BOARD_COLUMNS; col++) {

		for (row=0; row<MINIMAX_BOARD_ROWS; row++) {
			if (board->squares[row][col] == MINIMAX_EMPTY_SQUARE) {
				return false;
			}
		}
	}

	return true;
}

// ----------------------------------------------------------------------------

bool minimax_isVerticalWinnner(minimax_board_t* board, uint8_t* candidate) {
	uint8_t col;

	for (col=0; col<MINIMAX_BOARD_COLUMNS; col++) {
		// find product of this column
		uint8_t colScore = (board->squares[0][col] * \
				board->squares[1][col] * board->squares[2][col]);

		if (colScore == PRODUCT_PLAYER_WIN) {
			*candidate = MINIMAX_PLAYER_SQUARE;
			return true;
		}

		if (colScore == PRODUCT_OPPONENT_WIN) {
			*candidate = MINIMAX_OPPONENT_SQUARE;
			return true;
		}

	}

	// there was no win, so set *candidate to empty
	*candidate = MINIMAX_EMPTY_SQUARE;
	return false;
}

// ----------------------------------------------------------------------------

bool minimax_isHorizontalWinnner(minimax_board_t* board, uint8_t* candidate) {
	uint8_t row;

	for (row=0; row<MINIMAX_BOARD_COLUMNS; row++) {
		// find product of this column
		uint8_t rowScore = (board->squares[row][0] * \
				board->squares[row][1] * board->squares[row][2]);

		if (rowScore == PRODUCT_PLAYER_WIN) {
			*candidate = MINIMAX_PLAYER_SQUARE;
			return true;
		}

		if (rowScore == PRODUCT_OPPONENT_WIN) {
			*candidate = MINIMAX_OPPONENT_SQUARE;
			return true;
		}

	}

	// there was no win, so set *candidate to empty
	*candidate = MINIMAX_EMPTY_SQUARE;
	return false;
}

// ----------------------------------------------------------------------------

bool minimax_isDiagonalWinnner(minimax_board_t* board, uint8_t* candidate) {
	uint8_t candidate1 = board->squares[0][0];
	uint8_t candidate2 = board->squares[2][0];

	bool candidate1_win = ((candidate1 == board->squares[1][1]) && \
						(candidate1 == board->squares[2][2]));

	bool candidate2_win = ((candidate2 == board->squares[1][1]) && \
						(candidate2 == board->squares[0][2]));

	// if no winner
	*candidate = MINIMAX_EMPTY_SQUARE;

	if (candidate1_win) *candidate = candidate1;
	if (candidate2_win) *candidate = candidate2;

	return (candidate1_win || candidate2_win);
}

// ----------------------------------------------------------------------------

void minimax_printBoard(minimax_board_t* board, minimax_move_t* move, bool mark) {

	uint8_t row, col;
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
			if (row == move->row && col == move->column && mark) {
				printf("%c*", c);
			} else {
				printf("%c ", c);
			}

		}
		printf("\n\r");
	}
}
