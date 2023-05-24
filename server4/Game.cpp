#include "Game.h"

Game::Game()
{
	setlocale(LC_ALL, "rus");
	gameState = PRE_GAME;
	game_id = count;
	count++;
	Logger::logGame("������� ������� ������");
}

Game::~Game()
{
	//Logger::logGame("������������ ������, ���������� ��� ������ �������� �������� ����");
	Logger::logGame("������� ������� ������");
}

int game_end = 0;
std::vector<Card> deck; // ������ � �������

int Game::addPlayer(User& user) {
	if (gameState == IN_PROGRESS) {
		Logger::logGame("���� ��� ��������, ������ ��������� �������");
		return -1;
	}
	std::lock_guard<std::mutex> guard(myMutex);
	if (players.size() > MAX_PLAYERS) {
		Logger::logGame("���������� ������������ ���������� �������");
		return -2;
	}

	Logger::logGame("����� " + to_string(user.GetConnection()) + " ������� �������� � ���� �" + to_string(game_id));
	players.push_back(user);

	if (players.size() == MAX_PLAYERS) {
		gameState = IN_PROGRESS;
		Logger::logGame("���� ������");
	}
	return 0;
}

int Game::removePlayer(User& user) {
	if (gameState == IN_PROGRESS) {
		endGame();
	}
	std::lock_guard<std::mutex> guard(myMutex);
	for (auto it = players.begin(); it != players.end(); ++it) {
		if (it->GetConnection() == user.GetConnection()) {
			Logger::logGame("����� " + to_string(user.GetConnection()) + " ������ �� ���� �" + to_string(game_id));
			players.erase(it);
			endGame();
			return 0;
			break;
		}
	}
	Logger::logGame("����� �� ���������� � ������ ����");
	return -1;
}


int Game::startGame() {

	std::lock_guard<std::mutex> guard(myMutex);
	if (players.size() == MAX_PLAYERS && gameState == IN_PROGRESS) {
		return 0;
	}
	return -1;
}

int Game::endGame() {
	gameState = GAME_OVER;
	deck.clear();
	game_end = 0;
	Logger::logGame("���� ��������");
	return 0;
}




int Game::getCountMaxPlayers()
{
	return MAX_PLAYERS;
}

string Game::GiveCard(User& user)
{
	if (user.GetCardsSum() >= 21) return "-1";
	auto next = GenerateCard();
	user.AddCard(next);
	return to_string(next.value) + "|" + next.suit;
}

int Game::PlayerEndGame(User& user)
{
	game_end++;
	std::lock_guard<std::mutex> guard(myMutex);
	if (game_end == players.size())
	{
		endGame();
	}
	user.EndGame();
	return 0;
}


int Game::getCurrentCountPlayers()
{
	std::lock_guard<std::mutex> guard(myMutex);
	return players.size();
}

string Game::CheckWin(User& user)
{
	int current_sum = user.GetCardsSum();
	if (gameState != GAME_OVER) return "-1";
	std::lock_guard<std::mutex> guard(myMutex);
	if (current_sum > 21)
	{
		for (auto it = players.begin(); it != players.end(); it++) {
			if (it->GetConnection() == user.GetConnection()) continue;
			int sum = it->GetCardsSum();
			if (sum < current_sum) return "lose";
		}
		return "win";
	}
	else {
		for (auto it = players.begin(); it != players.end(); it++) {
			if (it->GetConnection() == user.GetConnection()) continue;
			int sum = it->GetCardsSum();
			if (sum > current_sum && sum < 22) return "lose";
		}
		return "win";
	}
	gameState = PRE_GAME;
}

Card Game::GenerateCard() {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	// ���� ������ �����, �� ������� ����� ������
	if (deck.empty()) {
		// ������� ������ �� 52 ����
		for (int i = 2; i <= 14; i++) {
			for (char suit : {'C', 'D', 'H', 'S'}) {
				deck.push_back({ i, suit });
			}
		}
	}

	// �������� ��������� ����� �� ������ � ������� �� �� ������
	std::uniform_int_distribution<> dis(0, deck.size() - 1);
	int index = dis(gen);
	Card card = deck[index];
	deck.erase(deck.begin() + index);

	Logger::logGame("������������� �����: " + to_string(card.value) + " " + (card.suit));
	return card;
}


