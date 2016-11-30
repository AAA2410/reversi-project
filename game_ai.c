/**
 * @file Lab7Part1.c
 * @author Anurag Agarwal
 * @date 8-Nov-2015
 * @brief Simulates the board game Reversi between a human user and very simplistic AI 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * @brief Sets all values in a 2-D array to be identical to another
 *
 * The function takes two 2-D arrays, and copies all values from the source to the destination, without modifying the pointers to either array. The function is used for copying the board from 
 * the game's memory to the AI's virtual "brain" for testing different locations and resetting it to original after finished
 *
 * @param boardFrom The source 2-D array
 * @param boardTo The destination 2-D array
 * @param size Both arrays are identically n x n sized, where n was taken as an input elsewhere in the function
 */
void copyBoard (char** boardFrom, char** boardTo, int size)  {
	//Runs through every element
	for (int i = 0; i < size; i++)  {
		for (int j = 0; j < size; j++)  {
			//Copies every element over
			boardTo[i][j] = boardFrom[i][j];
		}
	}
}

/**
 * @brief Prints out all the elements in an n x n array.
 * 
 * Used for printing out the game board when the game is being played; the board is always stored as an n x n. 
 *
 * @param toPrint Pointer to the array being printed
 * @param size The size n of the n x n array 
 */
void printBoard (char** toPrint, int size)  {
	//Prints a blank to allow even spacing
	printf("  ");	
	for (int i = 0; i < size; i++)  {
		//Prints column labels from a to the nth letter of the alphabet
		printf("%c", 'a'+i);
	}
	printf("\n");	
	for (int i = 0; i < size; i++)  {
		//Prints row labels from a to the nth letter of the alphabet
		printf("%c ", 'a'+i);
		for (int j = 0; j < size; j++)  {
			//Prints out every element for every corresponding row and column, counted by i and j respectively
			printf("%c", toPrint[i][j]);
		}
		printf("\n");
	}
}

/**
 * @brief Makes a move at any location on the n x n board
 * 
 * Used both for making changes to the board as the two players play their respective turns, and for storing any changes that the AI makes in it's own memory for deciding its best move
 *
 * @param changeBoard The board that is used for recording any changes made in this function call
 * @param originalBoard Stores a copy of the game board if the AI is making changes in its own memory; if both board parameters are identical, the changes passes through to the board without restoration
 * @param row The first index of the location of the change
 * @param col The second index of the location of the change
 * @param colour The colour of the player making/testing the move 
 * @param bounds The size n of the n x n board
 *
 * @return The change in score caused by the move that the function performed
 */
int* moveAtLocation (char** changeBoard, char** originalBoard, int row, int col, char colour, int bounds)  {
	//Variable declaration 
	char colourCheck; 
	bool valid = false, lineOfOpp = false;
	int* moveScore = (int*) malloc(8*sizeof(int)); 
	int directionIndex, dump; 
	
	for (int i = 0; i < 8; i++)  {
		moveScore[i] = 0;
	}

	//Stores the colour opposite to the one that called the function i.e. stores black if colour is 'W', and white if 'B'
	if (colour == 'B')  {
		colourCheck = 'W';
	}
	else  {
		colourCheck = 'B';
	}

	//Checks if the location is unoccupied; a move cannot be made on a location already occupied by either colour 
	if (changeBoard[row][col] == 'U')  {
		/* Two levels of for loops, each iterating from [-1,1] ignoring the condition where both values are at 0 (to avoid infinite looping). Each value increments, decrements, or doesn't change
  		 * either vertically or horizontally; e.g. -1,-1 goes Northwest along the board; 0,1 goes East along the board
		 */
		for (int vertical = -1; vertical <= 1; vertical++)  {
			for (int horizontal = -1; horizontal <= 1; horizontal++)  {
				directionIndex = ((horizontal-vertical)+3*(vertical+1)+(vertical == 0||vertical == 1))*(!(vertical == 0 && horizontal == 0))-(horizontal == 1 && vertical == 0);
				//Counts from the location, using the direction vectors from above to traverse the board in one of 8 directions; loop does not iterate if the direction vectors are 0,0
				for (int i = row+vertical, j = col+horizontal, count = 0; i >= 0 && j >= 0 && i < bounds && j < bounds 
					&& !(horizontal == 0 && vertical == 0); i += vertical, j += horizontal, count++)  {
					//If an opposite colour position is detected, it sets this boolean to true
					if (changeBoard[i][j] == colourCheck)  {
						lineOfOpp = true;
					}
					//If it detects an adjacent equivalent colour or an unoccupied space, it falsifies all conditions and stops checking
					else if ((changeBoard[i][j] == colour && count == 0) || (changeBoard[i][j] == 'U'))  {
						lineOfOpp = false;
						valid = false;
						break;
					}
					//If it has already seen an opposite colour line, and then detects an equivalent colour, it decides that the direction is valid and stops checking further
					else  {
						valid = true;
						break;
					}
				}
				//Makes the appropriate changes to the board, only if it is decided that the move is valid in the current direction
				if (valid)  {
					//Changes location of actual move to the input colour
					changeBoard[row][col] = colour;
					//Changes all colours in the current direction to match the input colour
					for (int i = row+vertical, j = col+horizontal; changeBoard[i][j] != colour; i += vertical, j += horizontal)  {
						changeBoard[i][j] = colour; 
						//Increments the score for every colour change made
						moveScore[directionIndex]++;		
					}
					//Sets validity of direction to false before iterating over the next direction vector 
					valid = false;
				}
			}
		}
	}
	//If the move is detected to be in an unoccupied position, it sets the change in score to -1; this value is used outside of this function to determine if a user input an invalid move
	else  {
		moveScore[0] = -1;
	}
	
	/* Resets the original configuration of the board, to allow the AI to continue checking for moves; if the move is an actual change in the game master board, this function is called such that
 	 * both arguments passed to copyBoard are identical, ensuring that no reset is performed on a valid, input move
         */
	copyBoard(originalBoard, changeBoard, bounds);
	//Returns change in score caused by the move at location
	return moveScore;
}

/**
 * @brief Calculates the score from the move
 * 
 * Adds up the value of all the values in the vector returned by playing a move at any given location
 * 
 * @param movesVector The vector containing all the scores changed by any move
 *
 * @return Returns the score obtained from the move corresponding to the vector
 */
int scoreFromMove (int* movesVector)  {
	//Variable declaration
	int resultant = 0;
	for (int i = 0; i < 8; i++)  {
		resultant += movesVector[i];
	}
	if (resultant > 0)  {
		resultant++;
	}
	return resultant;
}

/**
 * @brief Checks if both the players have any moves available to make 
 * 
 * Iterates through every location on the board and makes a test move, returning true if even one move is available
 * 
 * @param gameBoard The board on which the game is played
 * @param testBoard The board used to test the various possible moves
 * @param size The size n of the n x n board
 *
 * @return Returns if a single legal move is available for either side or not
 */
bool movesAvailable (char** gameBoard, char** testBoard, int size)  {
	for (int i = 0; i < size; i++)  {
		for (int j = 0; j < size; j++)  {
			if (scoreFromMove(moveAtLocation(testBoard, gameBoard, i, j, 'W', size)) > 0 || scoreFromMove(moveAtLocation(testBoard, gameBoard, i, j, 'B', size)) > 0)  {
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief Determines the optimal move for the AI to make; 
 * 
 * The AI checks all positions from Northwest to Southeast, and records the Northwesternmost, highest scoring position that is valid as per the game rules
 *
 * @param testBoard Pointer to the board that the AI is testing its moves on
 * @param originalBoard Pointer to the game master board so the AI can reset the board as required; never used to make a change to the original board in this function
 * @param dimensions The size n of the n x n board
 * @param colour The colour of the player that is checking for its best move is passed here
 *
 * @return The location that recorded the highest possible score the player could make in this move
 */
char* highestScoringMove (char** testBoard, char** originalBoard, int dimensions, char colour)  {
	//Variable declaration
	int highestScore = 0, temp = 0;
	char* locations = (char*) malloc(2*sizeof(char));
	int* movesVector;

	//Iterates through every tile on the board that is being tested
	for (int i = 0; i < dimensions; i++)  {
		for (int j = 0; j < dimensions; j++)  {
			//Calls function to test in all 8 directions at the specific point, and temporarily stores the sum of the scores in the stack
			temp = scoreFromMove(moveAtLocation(testBoard, originalBoard, i, j, colour, dimensions));
			//If the temporary variable is higher than the current highest score, it stores both the score and the position (as two characters like "aa" or "cd") of that move
			if (temp > highestScore)  {
				highestScore = temp;
				//The addition of 'a' translates the numerical value within the matrices (i.e. 0-n, n <= 25) to character labels ('a'-'z')
				locations[0] = i+'a';
				locations[1] = j+'a';
			}
		}
	}
	//If highest score is returned to be 0, the player has no valid moves; returns '0' as the position to the stack that called this function
	if (highestScore == 0)  {
		locations[0] = '0';
		locations[1] = '0';
	}
	return locations;
}

/**
 * @brief Sets up a board to play the game with
 *
 * Declares an n x n matrix on heap memory to use as the board as required by the program
 *
 * @param dim Size n of the n x n matrix, decided in a separate function
 *
 * @return Returns a pointer to the heap memory that stores the array that was created
 */
char** setBoard (int dim)  {
	//Variable declaration
	char** board;
	char* boardCol;
	
	//Creates a 2D-array of pointers of the input size n
	board = (char**) malloc(dim*sizeof(char*));
	//Creates a 1D-array of pointers of the size n*n
	boardCol = (char*) malloc(dim*dim*sizeof(char));

	//Takes every ith element of the n-sized 2-D pointers, and points them to every ith memory block; each memory block stores n number of chars, hence the n pointers point to ever contiguous block
	for (int i = 0; i < dim; i++)  {
		board[i] = boardCol + (i*sizeof(char)*dim);
	}

	//Initializes the board to have all values set to unoccupied
	for (int i = 0; i < dim; i++)  {	
		for (int j = 0; j < dim; j++)  {
				board[i][j] = 'U';
		}
	}
	//Sets the centre four pieces of the game to white and black 	
	board[(dim/2)-1][(dim/2)-1] = 'W';
	board[dim/2][(dim/2)-1] = 'B';
	board[(dim/2)-1][dim/2] = 'B';
	board[dim/2][dim/2] = 'W';
	return board;
}

/**
 * @brief Runs a recursive sequence to calculate the best possible move for the AI to play
 *
 * The function looks at the current location (the location is set by another function that calls this one) and it predicts the opponent's possible responses and assigns a weight to playing a move in the current location. For the 
 * sake of simplicity, the algorithm assumes that the opponent will always make their highest scoring move, calling a naive checker to predict future moves. It calculates and assigns weight by taking a dot product of the number of 
 * directions that are flipped by the move with the score in each of those directions, multiplied by a weight that prioritizes having more directions instead of higher points. Each move is also multiplied by a weight that lightens
 * the effect of predictions that are far in the future
 *
 * @param testBoard The AI's memory of a board on which it tests various possible moves
 * @param originalBoard The board on which the game is actually played; used both to make appropriate changes and to reset the memory board when necessary
 * @param dimensions The size n of the n x n board
 * @param colour The AI player's colour; necessary because validity is unique to the colour and locations occupied on the board
 * @param accuracyFactor The number of moves into the future that the algorithm predicts and counters moves for; also used to calculate weight of each prediction in the formula
 * @param distanceFromPresent Used to check if the algorithm has reached the number of moves it needs to or not
 *
 * @return Returns weighted score of the move
 */
double predictionSequence (char** testBoard, char** originalBoard, int dimensions, char colour, int accuracyFactor, int distanceFromPresent)  {
	//Variable declaration 
	char passColour;
	char* locations = (char*) malloc(2*sizeof(char));
	char** newTestBoard = setBoard(dimensions);
	int* movesVector = (int*) malloc(8*sizeof(int)); 
	//Initializes a 0 vector for the moves; necessary to avoid memory leakage if no valid moves are available
	for (int i = 0; i < 8; i++)  {
		movesVector[i] = 0;
	}
	int directionWeight = 0;
	double weightedMoveScores = 0.0, predictionAccuracy = 1.0-(distanceFromPresent*1.0/accuracyFactor); 
	
	//Every other prediction is for the opponent's move, causing the vaue to be subtracted from the weighted predictions
	if (accuracyFactor%2 == 1)  {
		predictionAccuracy *= -1;
	}
	
	//The colour that's passed to the next iteration, allowing it to predict the opponent's move; always opposite of current colour
	if (colour == 'B')  {
		passColour = 'W';
	}
	else  {
		passColour = 'B';
	}
	
	//Base case; if it detects it's predicted as far into the future as available, or if both players have 0 valid moves, it returns 0 and stops predicting into the future
	if (distanceFromPresent == accuracyFactor || !movesAvailable(originalBoard, testBoard, dimensions))  {
		return 0;
		
	}
	//If the base case hasn't been reached
	else  {
		//Stores the highest scoring move for the current board state 
		locations = highestScoringMove(testBoard, originalBoard, dimensions, colour);
		//Ensures the move is valid
		if (locations[0] != '0')  {
			//Frees the 0 vector from memory, and assigns the vector containing possible scores from the move only if the move is valid to begin with 
			free(movesVector);
			//Contains a vector with point change in all directions stored
			movesVector = moveAtLocation(testBoard, testBoard, locations[0]-'a', locations[1]-'a', colour, dimensions);
		}
		//Creates a new state of the test board to pass it to the next call of the function
		copyBoard(testBoard, newTestBoard, dimensions);
		//Frees up heap memory storing the locations
		free(locations);
		//Creates a denominator to weigh the moves in each direction with
		for (int i = 0; i < 8; i++)  {
			if (movesVector[i] > 0)  {
				directionWeight++;
			}
		}
		directionWeight = 8 - directionWeight;
		
		//Multiplies the vector containing score with the scalar weights, and adds it to the weighted score
		for (int i = 0; i < 8; i++)  {
			weightedMoveScores += (1.0/directionWeight)*movesVector[i];
		}
		//Function calls itself for one move into the future
		return (weightedMoveScores + predictionSequence (newTestBoard, testBoard, dimensions, passColour, accuracyFactor, distanceFromPresent + 1));
	}
}


/**
 * @brief Calculates the best possible move for the AI to make
 * 
 * Calls the prediction sequence for each location on the board, and scores both validity of moves and the weights, deciding what move is the best possible
 *
 * @param testBoard The AI's memory of the board on which it tests moves 
 * @param originalBoard The board on which the game is played
 * @param dimensions Size n of the n x n board
 * @param colour The computer player's colour
 */
char* bestMove (char** testBoard, char** originalBoard, int dimensions, char colour)  {
	//Variable declaration
	int* movesVector;
	double highestProbMove = 0.0, temp = 0.0;
	char* locations = (char*) malloc(2*sizeof(char));
	
	//Iterates through every element on the board
	for (int i = 0; i < dimensions; i++)  {
		for (int j = 0; j < dimensions; j++)  {
			//Initializes values as required for the check
			temp = 0.0;
			movesVector = moveAtLocation(testBoard, originalBoard, i, j, colour, dimensions);
			//If the move is valid, it sets the temporary value to the weighted score obtained from the prediction sequence
			if (scoreFromMove(movesVector) && movesVector[0] != -1)  {
				temp = predictionSequence(testBoard, originalBoard, dimensions, colour, 15, 0);
			}
			//Compares the temporary value to the highest recorded values and stores it
			if (temp > highestProbMove || (temp < 0 && highestProbMove == 0))  {
				highestProbMove = temp;
				locations[0] = i+'a';
				locations[1] = j+'a';
			}
		}
	}
	
	//Returns an invalid move if the highest weighted score remains 0.0
	if (highestProbMove == 0.0)  {
		locations[0] = '0';
		locations[1] = '0';
	}
	return locations; 
}

/**
 * @brief Sets up the game to play
 * 
 * Takes an input from the user to define the size of the board and an input to decide which colour the opposite player will be, modifying all the required values in the stack that calls this function
 * 
 * @param sizePtr Pointer to the variable storing the size n of the n x n array
 * @param compPtr Pointer to the variable storing the colour of the computer player's tokens
 * @param playerPtr Pointer to the variable storing the colour of the user's tokens
 */
void setGame (int* sizePtr, char* compPtr, char* playerPtr)  {
	//Takes the size of the nxn board as an input 
	printf("Enter the board dimension: ");
	scanf("%d", sizePtr);
	
	//Takes in computer player's colour as an input
	printf("Computer plays (B/W) : ");
	scanf(" %c", compPtr);
	
	//Sets the player's colour to be opposite of the computer's 
	*playerPtr = *compPtr == 'B' ? 'W' : 'B';
}



/**
 * @brief Plays a single move within the game
 * 
 * Decides whether to call the AI's algorithm for checking moves or to let the user input their move (depending on whose turn it is) and validates and performs the move as required using the aboe    
 * helper functions
 *
 * @param gameBoard The board on which the game is being played
 * @param compBoard The board in the AI's "brain" on which it predicts the best move
 * @param size The size n of the n x n board
 * @param currentTurnColour The colour of the player who is currently playing
 * @param colourAI The colour of the AI as determined before this function is called
 * @param compScore Pointer to the computer's score in memory
 * @param plrScore Pointer to the player's score in memory
 *
 * @returns Whether or not a change was made in the current function call
 */
bool playMove (char** gameBoard, char** compBoard, int size, char currentTurnColour, char colourAI, int* compScore, int* plrScore)  {
	//Variable declaration
	int changeInScore;
	int *movesVector;
	char* playerMove = malloc(2*sizeof(char));
	bool changeMade = false;	
	
	//If the AI is current playing, it calls the AI's prediction algorithm
	if (currentTurnColour == colourAI)  {
		//Stores the optimal move returned by helper function 
		playerMove = bestMove(compBoard, gameBoard, size, colourAI);
		//If the optimal move is within the range of the matrix, it performs an actual change
		if (playerMove[0] >= 'a' && playerMove[0] <= 'z' && playerMove[1] >= 'a' && playerMove[1] <= 'z')  {	
			//Performs change on board and stores the score
			changeInScore = scoreFromMove(moveAtLocation(gameBoard, gameBoard, playerMove[0]-'a', playerMove[1]-'a', colourAI, size));
			//Adds change to the computer player's score; subtracts from user, and restores 1 point because the computer got that from occupying a 'U' position
			*compScore += changeInScore;
			*plrScore -= (changeInScore - 1);
			//Prints out where the change was made, allowing user to track any moves made by the computer player
			printf("Computer places %c at %c%c.\n", colourAI, playerMove[0], playerMove[1]);
			changeMade = true; 
		}
	}
	//If the user is playing, it checks if the user has any valid moves, by using the AI's algorithm, without indicating to the user what the optimal move is 
	else  {
		//Stores optimal move for user
		playerMove = bestMove(compBoard, gameBoard, size, currentTurnColour);
		//If an optimal move exists, it allows the user to override its value with their own decided move
		if (playerMove[0] >= 'a' && playerMove[0] <= 'z' && playerMove[1] >= 'a' && playerMove[1] <= 'z')  {
			//Stores user's input move
			printf("Enter move for colour %c (RowCol): ", currentTurnColour);
			for (int i = 0; i < 2; i++)  {
				scanf(" %c", &playerMove[i]);
			}
			//Performs the input move and stores the change in score
			movesVector = moveAtLocation(gameBoard, gameBoard, playerMove[0]-'a', playerMove[1]-'a', currentTurnColour, size);
			changeInScore = scoreFromMove(movesVector);
			//Modifies the user's and computer's score as dictated by the above change in score
			if (changeInScore != 0 && movesVector[0] != -1)  {
				*compScore -= (changeInScore - 1);
				*plrScore += changeInScore;
			}
			//If the function returns a value <= 0, the user input was invalid and sets the player's score to -1 
			else  {
				*plrScore = -1;
			}	
			changeMade = true;
		}
	}
	return changeMade; 
}



/**
 * @brief Simulates a game of reversi
 * 
 * Iterates through every successive turn, keeping track of scores and turn number until a the lack of valid moves causes the game to end
 * 
 * @param gameMasterBoard The board that the game is played on
 * @param internalBoard The board that the AI's optimization algorithm uses 
 * @param dim The size n of the n x n board
 * @param plrColour The colour of the user's tokens
 * @param compColour The colour of the CPU player's tokens 
 * @param compScore Pointer to the computer player's score 
 * @param plrScore Pointer to the user's score
 */
void playGame (char** gameMasterBoard, char** internalBoard, int dim, char plrColour, char compColour, int* compScore, int* plrScore)  {
	//Variable declaration
	int countOfNoChange = 0; 
	int numOfTurns = 0; 
	char previousChangedColour;
	bool boardChangedThisTurn = false, boardChangedLastTurn = false;

	//Keeps iterating while the user hasn't input an invalid move and at least one of the players has a valid move to play
	while (countOfNoChange < 2 && *plrScore > -1)  {
		boardChangedLastTurn = boardChangedThisTurn;
		//All even numbered turns, numbered upwards from 0 are black token moves
		if (numOfTurns%2 == 0)  {
			//Counts how many moves went without a valid change from black player
			boardChangedThisTurn = playMove(gameMasterBoard, internalBoard, dim, 'B', compColour, compScore, plrScore);
			if (boardChangedThisTurn)  {
				countOfNoChange = 0;
				previousChangedColour = 'W';
			}
			else  {
				countOfNoChange++;
			}
		}
		//All odd numbered turns labelled upwards from 1 are white token moves
		else  {
			//Counts how many moves went without a valid change from white player
			boardChangedThisTurn = playMove(gameMasterBoard, internalBoard, dim, 'W', compColour, compScore, plrScore);
			if (boardChangedThisTurn)  {
				countOfNoChange = 0;
				previousChangedColour = 'B';
			}
			else  {
				countOfNoChange++;
			}
		}
		//Increments the turn tracker, prints out the current board state and recopies the AI's algorithm to recognize the newly modified board
		numOfTurns++;	
		if (!boardChangedThisTurn && *plrScore > -1 && movesAvailable(internalBoard, gameMasterBoard, dim))  {
			printf("%c player has no valid move.", previousChangedColour);
		}
		if (*plrScore > -1 && countOfNoChange < 1)  {
			printBoard(gameMasterBoard, dim);
		}
		
		copyBoard(gameMasterBoard, internalBoard, dim);
	}
	//Automatically makes the CPU player win if the user inputs an incorrect move
	if (*plrScore == -1)  {
		printf("Invalid move.\n%c player wins.\n", compColour);
	}
	//Prints who won depending on whose score is higher
	else if (*plrScore > *compScore)  {
		printf("%c player wins.\n", plrColour);
	}
	else if (*plrScore < *compScore) {
		printf("%c player wins.\n", compColour);
	}
	else if (*plrScore == *compScore)  {
		printf("Draw!");
	}
}

/**
 * @brief Plays Reversi
 *
 * Calls the function to simulate a full game of reversi, linking all variables and function calls 
 */
int main (void)  {
	//Variable declaration
	int boardSize, computerScore = 2, playerScore = 2;
	char computerColour, playerColour;

	
	//Takes inputs of colours and board size from user
	setGame(&boardSize, &computerColour, &playerColour);

	//Sets two identical boards, one for the AI's optimization algorithm, and the other for the game master
	char** boardGameMaster = setBoard(boardSize);
	char** boardAI = setBoard(boardSize);
	//Prints out original configuration of board
	printBoard(boardGameMaster, boardSize);
	//Calls the function to play the game and passes all requisite values and pointers to it
	playGame(boardGameMaster, boardAI, boardSize, playerColour, computerColour, &computerScore, &playerScore);
	//Frees heap memory after game ends
	free(boardGameMaster);
	free(boardAI);
}
