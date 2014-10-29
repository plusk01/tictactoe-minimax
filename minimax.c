/*
 * minimax.c
 *
 *  Created on: Oct 18, 2014
 *      Author: plusk01
 */

#include "minimax.h"

#define SCORES_MAX		(MINIMAX_BOARD_ROWS * MINIMAX_BOARD_COLUMNS)
#define MOVES_MAX		SCORES_MAX

#define DEBUG_PRINT			false
#define DEBUG_ALLOW_COLLOR	true

static bool playerIsX = false;

static minimax_move_t choice; // the move computed by minimax
static minimax_move_t tmpChoice; // the tested minimax choice

// Helper function prototypes
minimax_score_t minimax_recurse(minimax_board_t* board, bool player, uint8_t depth);
uint8_t minimax_getIndex(minimax_score_t *scores, bool getMax);
bool minimax_isBoardFull(minimax_board_t* board);
bool minimax_isVerticalWinnner(minimax_board_t* board, uint8_t* candidate);
bool minimax_isHorizontalWinnner(minimax_board_t* board, uint8_t* candidate);
bool minimax_isDiagonalWinnner(minimax_board_t* board, uint8_t* candidate);
void minimax_printBoard(minimax_board_t* board, minimax_move_t* move, bool mark, uint8_t depth);
void minimax_printTables(minimax_move_t* moves, minimax_score_t* scores, uint8_t index, uint8_t depth);

// ----------------------------------------------------------------------------

void minimax_initBoard(minimax_board_t* board) {
	// clear the board
}

// ----------------------------------------------------------------------------

void minimax_computeNextMove(minimax_board_t* board, bool player, uint8_t* row, uint8_t* column) {
	playerIsX = player;

	// call the recursive algorithm
	minimax_recurse(board, player, 0);

#ifdef DEBUG_PRINT
	minimax_printBoard(board, &choice, true, 1);
#endif

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

int16_t minimax_computeBoardScore(minimax_board_t* board, bool player, uint8_t depth) {
	// go get the candidate, figure out who the player is, +/- based on that.
	// bool choosePlayerValue = (!player && playerIsX);
	uint8_t winner = MINIMAX_EMPTY_SQUARE;
	uint8_t playerValue = (playerIsX) ? MINIMAX_PLAYER_SQUARE : MINIMAX_OPPONENT_SQUARE;
	uint8_t opponentValue = (!playerIsX) ? MINIMAX_PLAYER_SQUARE : MINIMAX_OPPONENT_SQUARE;

#if DEBUG_PRINT
	// char *playerStr = (playerIsX) ? "\033[92mplayer\033[0m" : "\033[93mopponent\033[0m";
	// 	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	// printf("$$ Computing Score from %s's perspective.\r\n\t(playerVal: %u, opponentVal: %u)\r\n", playerStr, playerValue, opponentValue);
#endif
	
	// find a winner
	if (!winner) minimax_isVerticalWinnner(board, &winner);
#if DEBUG_PRINT
	// printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	// printf("winner after v: %u\r\n", winner);
#endif
	if (!winner) minimax_isHorizontalWinnner(board, &winner);
#if DEBUG_PRINT
	// printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	// printf("winner after h: %u\r\n", winner);
#endif
	if (!winner) minimax_isDiagonalWinnner(board, &winner);
#if DEBUG_PRINT
	// printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	// printf("winner after d: %u\r\n", winner);
#endif

	// figure out score
	if (winner == playerValue) {
#if DEBUG_PRINT
		// printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
		// printf("+++ Winner is player; winner: %d, player: %d\r\n", winner, playerValue);
#endif
		return MINIMAX_PLAYER_WINNING_SCORE;
	} else if (winner == opponentValue) {
#if DEBUG_PRINT
		// printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
		// printf("+++ Winner is opponent; winner: %d, opponent: %d\r\n", winner, opponentValue);
#endif
		return MINIMAX_OPPONENT_WINNING_SCORE;
	} else if (minimax_isBoardFull(board)) {
#if DEBUG_PRINT
		// printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
		// printf("+++ draw\r\n");
#endif
		return MINIMAX_DRAW_SCORE;
	} else {
#if DEBUG_PRINT
		// printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
		// printf("+++ not end game\r\n");
#endif
		return MINIMAX_NOT_ENDGAME;
	}

}

// ----------------------------------------------------------------------------
// Helper Functions
// ----------------------------------------------------------------------------

minimax_score_t minimax_recurse(minimax_board_t* board, bool player, uint8_t depth) {
	depth++;

#if DEBUG_PRINT
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf("********* MINIMAX [%u] *********\r\n", depth);
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	if (player) printf("-- \033[92mPLAYER (X)\033[0m --\r\n\r\n");
	else printf("-- \033[93mOPPONENT (O)\033[0m --\r\n\r\n");
	
	minimax_printBoard(board, &tmpChoice, (depth != 1), depth);

#endif

	// Compute this board's score.
	// This checks if there is a winner and returns appropriate score
	minimax_score_t scoreTest = minimax_computeBoardScore(board, !player, depth);

#if DEBUG_PRINT
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf("scoreTest: %d\r\n", scoreTest);
#endif

	if (minimax_isGameOver(scoreTest)) {
		// Base case
#if DEBUG_PRINT
		printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
		printf("Base case score: \033[91m%d\033[0m\r\n", scoreTest);
#endif
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

			// for debugging
			tmpChoice = move;

			// minimax to find the score of that move
			minimax_score_t score = minimax_recurse(board, !player, depth);

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
	uint8_t index = minimax_getIndex(scores, player);
	choice = moves[index];
	minimax_score_t finalScore = scores[index];

#if DEBUG_PRINT
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf(" *** level %d ***\r\n", depth);
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf("finalScore: %d (from index: %u)\r\n", finalScore, index);
	minimax_printTables(moves, scores, index, depth);
	char *playerStr = (player) ? "player" : "opponent";
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf("Adding %s at (%d, %d)\r\n", playerStr, choice.row, choice.column);
#endif

	return finalScore;
}

// ----------------------------------------------------------------------------

uint8_t minimax_getIndex(minimax_score_t *scores, bool getMax) {
	minimax_score_t score = 0;
	uint8_t index = 0;

	uint8_t i;
	for (i=0; i<SCORES_MAX; i++) {
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

		// start iterating on the next row
		for (row=1; row<MINIMAX_BOARD_ROWS; row++) {
			if (board->squares[row][col] == MINIMAX_EMPTY_SQUARE) {
				return false;
			}
		}
	}

	return true;
}

// ----------------------------------------------------------------------------

bool minimax_isVerticalWinnner(minimax_board_t* board, uint8_t* candidate) {
	uint8_t row, col;

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

	// if no winner
	*candidate = MINIMAX_EMPTY_SQUARE;

	return false;
}

// ----------------------------------------------------------------------------

bool minimax_isHorizontalWinnner(minimax_board_t* board, uint8_t* candidate) {
	uint8_t row, col;
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

	// if no winner
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

void minimax_printBoard(minimax_board_t* board, minimax_move_t* move, bool mark, uint8_t depth) {
	
	uint8_t row, col;
	for (row=0; row<MINIMAX_BOARD_ROWS; row++) {
		printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
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
#if DEBUG_ALLOW_COLLOR
				printf("\033[94m%c\033[0m ", c);
#else
				printf("%c*", c);
#endif
			} else {
				printf("%c ", c);
			}
			
		}
		printf("\n\r");
	}
}

// ----------------------------------------------------------------------------

void minimax_printTables(minimax_move_t* moves, minimax_score_t* scores, uint8_t index, uint8_t depth) {
	
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf("-----------------------------\r\n");
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf("|    MOVES    |    SCORES   |\r\n");
	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf("-----------------------------\r\n");

	uint8_t i;
	for (i=0; i<SCORES_MAX; i++) {
		printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
		printf("|    ");
		printf("(%d,%d)", moves[i].row, moves[i].column);
		printf("    |");

		printf("     ");
		printf("%+3d", scores[i]);
		printf("     |");

		if (index == i) {
			printf(" \033[94m<--\033[0m\r\n");
		} else {
			printf("\r\n");
		}
	}

	printf("%.*s", (depth-1), "\t\t\t\t\t\t\t");
	printf("-----------------------------\r\n");
}
