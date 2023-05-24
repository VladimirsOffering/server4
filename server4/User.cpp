#include "Game.h"

std::mutex mutex_user;

User::User(Game& gameSession, SOCKET& connection)
{
	this->connection = connection;
	this->gameSession = &gameSession;
	Connect();
}

int User::GetConnection() { return this->connection; }

bool User::GetEndStatus()
{
	return EndStatus;
}

void User::Connect()
{
	Logger::log("Подключился клиент " + to_string(connection));
	EndStatus = false;
	this->ClientThread = new std::thread(&User::PlayerHandler, this);
}

void User::EndGame()
{
	EndStatus = true;
}

int User::GetCardsSum()
{
	std::lock_guard<std::mutex> guard(mutex_user);
	int sum = 0;
	for (Card card : Cards) {
		sum += card.value;
	}
	return sum;
}

void User::Disconnect()
{
	Logger::log("Отключился клиент " + to_string(connection));
	gameSession->removePlayer(*this);
	closesocket(connection);
}

int User::AddCard(Card card)
{
	//std::lock_guard<std::mutex> guard(mutex_user);
	Cards.push_back(card);
	return GetCardsSum();
}

void User::PlayerHandler()
{
	std::vector<char> requestBuffer(1024);
	while (1)
	{

		auto bytesRead = recv(connection, requestBuffer.data(), requestBuffer.size(), 0);
		if (bytesRead == SOCKET_ERROR) {
			Logger::errorLog("Ошибка при получении данных от клиента");
			break;
		}
		if (bytesRead == 0) {
			break;
		}
		std::string request(requestBuffer.data(), bytesRead);
		// Удаляем символы перевода строки из строки request
		request.erase(std::remove(request.begin(), request.end(), '\r'), request.end());
		request.erase(std::remove(request.begin(), request.end(), '\n'), request.end());


		Logger::logGame("Пришла команда " + request + " от клиента " + to_string(connection));

		string response = "";
		if (request == "add_player") {
			response = to_string(gameSession->addPlayer(*this));
		}
		else if (request == "start_game") {
			response = to_string(gameSession->startGame());
		}
		else if (request == "give_card") {
			response = (gameSession->GiveCard(*this));
		}
		else if (request == "i_end_game") {
			response = to_string(gameSession->PlayerEndGame(*this));
		}
		else if (request == "result") {

			response = (gameSession->CheckWin(*this));
		}
		send(connection, response.c_str(), response.size(), 0);
		Logger::logGame("Отправлено клиенту " + to_string(connection) + " " + response);

	}
	Disconnect();
};


