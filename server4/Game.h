#pragma once

#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <sstream>
#include "Logger.h"
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <random>
#include <mutex>

struct Card {
	int value; // Значение карты (2-10, 11-13 - валет, дама, король, 14 - туз)
	char suit; // Масть карты (C - клубы, D - бубны, H - червы, S - пики)
};

class Game;

class User
{
private:
	SOCKET connection;
	Game* gameSession;
	thread* ClientThread;
	vector<Card> Cards;
	volatile bool EndStatus;

public:

	User(Game& gameSession, SOCKET& connection);

	int GetConnection();

	bool GetEndStatus();

	void Connect();

	void EndGame();

	int GetCardsSum();

	void Disconnect();

	int AddCard(Card card);

	void PlayerHandler();
};




const int MAX_PLAYERS = 2;

class Game {
private:
	enum GameState {
		PRE_GAME,
		IN_PROGRESS,
		GAME_OVER
	};

	vector<User> players;
	static int count;
	int game_id;
	GameState gameState;

public:
	Game();
	~Game();

	int addPlayer(User& user);

	int removePlayer(User& user);

	int startGame();

	int endGame();

	string getGameState() const;

	int getCountMaxPlayers();

	string GiveCard(User& user);

	int getCurrentCountPlayers();

	string CheckWin(User& user);

	Card GenerateCard();

	int PlayerEndGame(User& user);
};

#endif