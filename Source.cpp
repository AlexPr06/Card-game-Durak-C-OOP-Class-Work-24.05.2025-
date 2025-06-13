#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cstdlib> 
#include <ctime>
#include <chrono>
#include <thread>

using namespace std;

/**
ТЗ:
Створити масив з мастями карт
Створити масив із значенням карт
Створити функцію, яка створює колоду
Створити функцію, яка роздає карти
Створити функцію, яка виводить карти на екран
Створити функцію, яка виводить козир
Створити функцію, яка дозволяє користувачеві зробити хід (він вибирає карту, або по черзі, або за назвою карти)
Створити функцію, яка дозволить рандомно ходити боту
 */

#define WHITE   "\033[37m"
#define RED     "\033[31m"
#define BLACK   "\033[30m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

#define RESET   "\033[0m"


enum class Suit {
	Spades,   // ♠
	Hearts,   // ♥
	Diamonds, // ♦
	Clubs     // ♣
};

string suitToString(Suit suit) {
	switch (suit) {
	case Suit::Spades:   return string(WHITE) + "♠" + RESET;
	case Suit::Hearts:   return string(RED) + "♥" + RESET;
	case Suit::Diamonds: return string(RED) + "♦" + RESET;
	case Suit::Clubs:    return string(WHITE) + "♣" + RESET;
	default:             return "?";
	}
}




/* ------------------------------------------------------------------------------------ */
// CARD
/* ------------------------------------------------------------------------------------ */
struct Card {
	Suit suit;
	string value;

	string toString() const {
		return value + suitToString(suit);
	}

	bool operator==(const Card& other) const {
		return suit == other.suit && value == other.value;
	}
};

Card toCard(string& stringCard)
{
	Suit suit;
	string value;
	if (stringCard.find("♠") != string::npos) {
		suit = Suit::Spades;
	}
	else if (stringCard.find("♥") != string::npos) {
		suit = Suit::Hearts;
	}
	else if (stringCard.find("♦") != string::npos) {
		suit = Suit::Diamonds;
	}
	else if (stringCard.find("♣") != string::npos) {
		suit = Suit::Clubs;
	}
	else {
		throw invalid_argument("Invalid card suit.");
	}
	value = stringCard.substr(0, stringCard.find_first_of("♠♥♦♣"));
	return { suit, value };
}

int cardValueRank(const string& value) {
	if (value == "2") return 2;
	if (value == "3") return 3;
	if (value == "4") return 4;
	if (value == "5") return 5;
	if (value == "6") return 6;
	if (value == "7") return 7;
	if (value == "8") return 8;
	if (value == "9") return 9;
	if (value == "10") return 10;
	if (value == "J") return 11;
	if (value == "Q") return 12;
	if (value == "K") return 13;
	if (value == "A") return 14;
	return 0; // for invalid or unknown values
}

void sortByValue(vector<Card>& cards) {
	sort(cards.begin(), cards.end(), [](const Card& a, const Card& b) {
		return cardValueRank(a.value) < cardValueRank(b.value);
		});
}



/* ------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------ */

class Deck
{
private:
	vector<Suit> suits = { Suit::Spades, Suit::Hearts, Suit::Diamonds, Suit::Clubs };
	vector<string> values = { "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
	vector<Card> deck;
	string trump = "";
	int amount = 0;
public:

	vector<Suit>& getSuits() { return suits; };
	vector<string>& getValues() { return values; };
	vector<Card>& getDeck() { return deck; };
	int& getAmount() { return amount; };

	void setAmount(int newAmount) { amount = newAmount; };
	void setTrumpIndex(string newTrump) { trump = newTrump; }
	string getTrump() { return trump; }

	Deck() = default;

	void createDeck() {
		getDeck().clear();
		for (const auto& suit : getSuits()) {
			for (const auto& value : getValues()) {
				getDeck().push_back({ suit, value });
			}
		}
	}

	void printDeck() {
		for (const auto& card : getDeck()) {
			cout << card.toString() << " ";
		}
		cout << endl;
	}

	void dealCards(vector<Card>& player, vector<Card>& bot, int count = 6) {
		for (int i = 0; i < count; ++i) {
			player.push_back(getDeck().back());
			getDeck().pop_back();
			bot.push_back(getDeck().back());
			getDeck().pop_back();
		}
	}

	string generateTrump() {
		if (getSuits().empty()) {
			cout << "No suits available to generate a trump." << endl;
			return "";
		}
		Suit generatedTrump = getSuits()[rand() % getSuits().size()];
		cout << "Trump suit is: " << suitToString(generatedTrump) << endl;
		return suitToString(generatedTrump);
	}

	void shuffleDeck() {
		std::random_device rd;  // реальне випадкове зерно
		std::mt19937 g(rd());   // Mersenne Twister генератор
		shuffle(getDeck().begin(), getDeck().end(), g);
		cout << "Deck shuffled." << endl;
	}
};

class Player : public Deck
{
private:
	string name;
	vector<Card> hand;
	int amountOfCardsInHand;

public:
	Player(const string& name) : name(name)
	{
		amountOfCardsInHand = 6;
	}

	string& getName() {
		return name;
	}

	vector<Card>& getHand() {
		return hand;
	}

	int& getAmountOfCardsInHand() {
		return amountOfCardsInHand;
	}

	void setName(const string& newName) {
		name = newName;
	}
	void setHand(const vector<Card>& newHand) {
		hand = newHand;
	}
	void setAmountOfCardsInHand(int newAmount) {
		amountOfCardsInHand = newAmount;
	}


	void addCard(const Card& card) {
		getHand().push_back(card);
	}

	void showHand() {
		cout << getName() << "'s hand: ";
		for (const auto& card : getHand()) {
			cout << card.toString() << " ";
		}
		cout << endl;
	}

	void fullfillHand(vector<Card>& deck) {
		while (getHand().size() < 6) {
			if (deck.empty()) {
				cout << "Deck is empty, cannot draw more cards." << endl;
				break;
			}
			getHand().push_back(deck.back());
			deck.pop_back();
			setAmountOfCardsInHand(getAmountOfCardsInHand()++);
			cout << getName() << " drew a card: " << getHand().back().toString() << endl;
		}
	}

	Card playerMove()
	{
		showHand();
		int input;
		while (true)
		{
			cout << "Enter the card value (e.g., 1 or 5): ";
			cin >> input;
			input--; // Convert to 0-based index

			if (cin.fail() || input < 0 || input >= getHand().size()) {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Invalid input. Please enter a valid card index." << endl;
				continue;
			}
			break;
		}


		Card playerCard = getHand()[input];
		cout << "You played: " << playerCard.toString() << endl;
		return playerCard;
	}

	bool cardAbleToBeatAnotherCard(Card& card, Card& opponentCard, Card& trump)
	{
		if (card.suit == opponentCard.suit && cardValueRank(card.value) > cardValueRank(opponentCard.value)) {
			return true;
		}
		if (card.suit == trump.suit) {
			if (opponentCard.suit != trump.suit || cardValueRank(card.value) > cardValueRank(opponentCard.value)) {
				return true;
			}
		}
		return false;
	}

	bool canBeat(Card& opponentCard, Card& trump) {
		bool able = false;
		for (Card& card : getHand()) {
			if (cardAbleToBeatAnotherCard(card, opponentCard, trump)) {
				able = true;
				break;
			}
		}
		return able;
	}

	void getRidOfCard(Card& playerCard)
	{
		auto it = find(getHand().begin(), getHand().end(), playerCard);
		if (it != getHand().end()) {
			getHand().erase(it);
			setAmountOfCardsInHand(getAmountOfCardsInHand() - 1);
		}
	}


	~Player() {
		cout << "Player " << name << " is destroyed." << endl;
	};
};

class Bot : public Player
{
private:
	string name;
public:
	Bot(string name) : Player(name), name(name) {
		cout << "Bot " << this->getName() << " is created." << endl;
	};

	Card botMove()
	{
		int randomIndex = rand() % getHand().size();
		Card botCard = getHand()[randomIndex];
		cout << "Bot plays: " << botCard.toString() << endl;
		return botCard;
	}
	// This function finds a card in the bot's hand that can beat the opponent's card
	// REASONABLY
	Card findCardToBeat(Card& opponentCard, Card& trump)  
	{
		Card chosenCard;
		vector <Card> notTrumps;
		vector<Card> trumps;
		for (Card& card : getHand()) {
			if (card.suit != trump.suit) notTrumps.push_back(card);
			else trumps.push_back(card);
		}
		sortByValue(notTrumps);
		sortByValue(trumps);
		cout << endl;
		for (Card& card : notTrumps) cout << card.toString() << " ";
		cout << "   /   ";
		for (Card& card : trumps) cout << card.toString() << " ";
		cout << endl;
		if (!notTrumps.empty())
		{
			sortByValue(notTrumps);
			for (Card& card : notTrumps) {
				if (cardAbleToBeatAnotherCard(card, opponentCard, trump)) {
					chosenCard = card;
					return chosenCard;
				}
			}
		}
		
		if (!trumps.empty())
		{
			sortByValue(trumps);
			for (Card& card : trumps) {
				if (cardAbleToBeatAnotherCard(card, opponentCard, trump)) {
					chosenCard = card;
					return chosenCard;
				}
			}
		}
	}

	~Bot() {
		cout << "Bot " << this->getName() << " is destroyed." << endl;
	};
};

int main() {
	cout << GREEN << R"(
 __    __    ___  _        __   ___   ___ ___    ___      ______   ___       ______  __ __    ___       ____   ____  ___ ___    ___  __ 
|  |__|  |  /  _]| |      /  ] /   \ |   |   |  /  _]    |      | /   \     |      ||  |  |  /  _]     /    | /    ||   |   |  /  _]|  |
|  |  |  | /  [_ | |     /  / |     || _   _ | /  [_     |      ||     |    |      ||  |  | /  [_     |   __||  o  || _   _ | /  [_ |  |
|  |  |  ||    _]| |___ /  /  |  O  ||  \_/  ||    _]    |_|  |_||  O  |    |_|  |_||  _  ||    _]    |  |  ||     ||  \_/  ||    _]|__|
|  `  '  ||   [_ |     /   \_ |     ||   |   ||   [_       |  |  |     |      |  |  |  |  ||   [_     |  |_ ||  _  ||   |   ||   [_  __ 
 \      / |     ||     \     ||     ||   |   ||     |      |  |  |     |      |  |  |  |  ||     |    |     ||  |  ||   |   ||     ||  |
  \_/\_/  |_____||_____|\____| \___/ |___|___||_____|      |__|   \___/       |__|  |__|__||_____|    |___,_||__|__||___|___||_____||__|
                                                                                                                                        
)" << RESET << endl;
	system("chcp 65001 > nul");
	srand(time(0));
	Deck* deck = new Deck();
	Player* player = new Player("Player");
	Bot* bot = new Bot("Bot");

	/*cout << "Available suits: ";
	for (const auto& suit : deck->getSuits()) {
		cout << suitToString(suit) << " ";
	}
	cout << endl;*/

	/*cout << "Available values: ";
	for (const auto& value : deck->getValues()) {
		cout << value << " ";
	}
	cout << endl;*/

	deck->setAmount(36);
	deck->createDeck();
	deck->shuffleDeck();

	/*cout << "Initial deck: ";
	deck->printDeck();*/
	string stringTrump = deck->generateTrump();
	Card cardTrump = toCard(stringTrump);
	deck->setTrumpIndex(stringTrump);


	player->fullfillHand(deck->getDeck());
	
	cout << "Player's hand after fulfilling: ";
	player->showHand();
	bot->fullfillHand(deck->getDeck());
	cout << "Bot's hand after fulfilling: ";
	bot->showHand();

	/*cout << "Deck: ";
	deck->printDeck();*/
	bool gameIsOn = true;
	int whoMovesFirst = rand() % 2; // Randomly decide who moves first (0 for player, 1 for bot)
	bool turn;
	if (whoMovesFirst == 0) {
		cout << GREEN << "Player moves first." << RESET << endl;
		turn = true; // Player's turn
	}
	else {
		cout << RED << "Bot moves first." << RESET << endl;
		turn = false; // Bot's turn
	}
	cout << "Game started!" << endl;
	while (gameIsOn)
	{
		// Game loop


		cout << endl << endl;
		if (player->getAmountOfCardsInHand() <= 0)
		{
			cout << GREEN << "You have no cards left. You won!" << RESET << endl;
			gameIsOn = false;
			break;
		}

		if (bot->getAmountOfCardsInHand() <= 0)
		{
			cout << RED << "Bot has no cards left. Bot won!" << RESET << endl;
			gameIsOn = false;
			break;
		}


		if (turn)
		{
			// Player's turn
			vector <string> validCardsInRound;
			cout << "Player's turn." << endl;
			Card playerCard = player->playerMove();
			validCardsInRound.push_back(playerCard.value);
			if (bot->canBeat(playerCard, cardTrump))
			{
				cout << "Bot can beat your card." << endl;
				Card botCard = bot->findCardToBeat(playerCard, cardTrump);

				cout << "Bot plays beats your card " << playerCard.toString() << " with "<< botCard.toString() << endl;
				validCardsInRound.push_back(botCard.value);
				bot->getRidOfCard(botCard); // Remove played card from bot's hand
				turn = !turn; // Switch to bot's turn
			}
			else {
				cout << "Bot cannot beat your card. You win this round!" << endl;
				bot->addCard(playerCard); // Player keeps the card
				bot->setAmountOfCardsInHand(bot->getAmountOfCardsInHand() + 1);
			}
			player->getRidOfCard(playerCard); // Remove played card from player's hand
			
		}
		else {
			// Bot's turn
			vector <string> validCardsInRound;
			cout << "Bot's turn." << endl;
			cout << "Bot is thinking..." << endl;
			this_thread::sleep_for(chrono::seconds(1)); // Simulate bot thinking time
			Card botCard = bot->botMove();
			validCardsInRound.push_back(botCard.value);
			if (player->canBeat(botCard, cardTrump))
			{

				cout << "You can beat the bot's card." << endl;
				Card playerCard;
				while (true)
				{
					cout << "Choose a card to beat the bot's card: ";
					playerCard = player->playerMove();
					if (!player->cardAbleToBeatAnotherCard(playerCard, botCard, cardTrump))
					{
						cout << "You cannot beat the bot's card. Try again." << endl;
						continue; 
					}
					break;
				}
				cout << "You beat bot's card " << botCard.toString() << " with " << playerCard.toString() << endl;
				player->getRidOfCard(playerCard); // Remove played card from player's hand
				validCardsInRound.push_back(playerCard.value);
				turn = !turn; // Switch to bot's turn
			}
			else {
				cout << "You cannot beat the bot's card. Bot wins this round!" << endl;
				player->addCard(botCard); // Bot keeps the card
				player->setAmountOfCardsInHand(player->getAmountOfCardsInHand() + 1);
			}
			bot->getRidOfCard(botCard); // Remove played card from bot's hand
			this_thread::sleep_for(chrono::seconds(1)); // Simulate bot thinking time
		}
		cout << endl << endl;
	}


	delete player;
	delete bot;
	delete deck;
	cout << "Game over. All resources cleaned up." << endl;

	return 0;
}
