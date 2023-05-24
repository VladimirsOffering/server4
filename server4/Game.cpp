#include "Game.h"

Game::Game()
{
	setlocale(LC_ALL, "rus");
	gameState = PRE_GAME;
	game_id = count;
	count++;
	Logger::logGame("Создана игровая сессия");
}

Game::~Game()
{
	//Logger::logGame("Освобождение памяти, веделенной под массив значений игрового поля");
	Logger::logGame("Закрыта игровая сессия");
}

int game_end = 0;
std::vector<Card> deck; // Колода с картами

int Game::addPlayer(User& user) {
	if (gameState == IN_PROGRESS) {
		Logger::logGame("Игра уже началась, нельзя добавлять игроков");
		return -1;
	}
	std::lock_guard<std::mutex> guard(myMutex);
	if (players.size() > MAX_PLAYERS) {
		Logger::logGame("Достигнуто максимальное количество игроков");
		return -2;
	}

	Logger::logGame("Игрок " + to_string(user.GetConnection()) + " успешно добавлен в игру №" + to_string(game_id));
	players.push_back(user);

	if (players.size() == MAX_PLAYERS) {
		gameState = IN_PROGRESS;
		Logger::logGame("Игра начата");
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
			Logger::logGame("Игрок " + to_string(user.GetConnection()) + " удален из игры №" + to_string(game_id));
			players.erase(it);
			endGame();
			return 0;
			break;
		}
	}
	Logger::logGame("Игрок не существует в данной игре");
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
	Logger::logGame("Игра окончена");
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

	// Если колода пуста, то создаем новую колоду
	if (deck.empty()) {
		// Создаем колоду из 52 карт
		for (int i = 2; i <= 14; i++) {
			for (char suit : {'C', 'D', 'H', 'S'}) {
				deck.push_back({ i, suit });
			}
		}
	}

	// Выбираем случайную карту из колоды и удаляем ее из колоды
	std::uniform_int_distribution<> dis(0, deck.size() - 1);
	int index = dis(gen);
	Card card = deck[index];
	deck.erase(deck.begin() + index);

	Logger::logGame("Сгенерирована карта: " + to_string(card.value) + " " + (card.suit));
	return card;
}


