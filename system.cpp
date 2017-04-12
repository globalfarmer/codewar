#include <bits/stdc++.h>
using namespace std;

const int dx[9] = {-1, -1, -1,  0, 0,  1, 1, 1, 0};
const int dy[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};
const string DIR[4] = { "LEFT", "RIGHT", "UP", "DOWN"};
const int dir_x[4] = { 0, 0, -1, 1};
const int dir_y[4] = {-1, 1,  0, 0};

class Player
{
public:
	Player *players[4];
	int numberOfPlayer;
	string table[25];
	int ownId;
	vector<pair<int, int> > playersPos;
	Player(int _numberOfPlayer,int _ownId, string _table[25], const vector<pair<int, int> > &playerPos);
	~Player();
	string bestChoice();
	bool isValidDecision();
	int getScore();
	int minDistanceToOtherUnstable();
	int minDistanceToStable();
	void printPos(int);
};
class Splix
{
public:
	// players (AI)
	vector<Player> players;
	string table[25];
	Splix(int _numberOfPlayer);
	Splix(int _numberOfPlayer, string table[25], const vector<pair<int, int> > &playerPos);
	~Splix();
	/***
	/* @brief get each player decision alternatively
	/* @step 
	/* - each players
	/*   + callPlayer
	/*   + check if decision is in law
	/* - find all loser if exist
	/*	 + updateTable
	*/
	void nextTurn();
    void nextTurn(vector<string> decisionns);
	/***
	/* @brief: update table
	/* @step:
	/*  - remove losers
	/*  - update stable cell
    */
	void updateTable();
	void testConstructor()
	{
		this->printTable();
		cout << "Number of Players "<<this->players.size()<<endl;
		for(int i = 0; i < this->players.size(); i++) {
			cout << "Player " << i+1 << " is at ";
			players[i].printPos(players[i].ownId-1);
		}
	}
	void printTable() 
	{
		for(int i = 0; i < 20; i++) {
			cout << this->table[i] << endl;
		}
	}
};
int main() 
{
	Splix splix = Splix(4);
	splix.testConstructor();
	return 0;
}
Player::Player(int _numberOfPlayer,int _ownId, string _table[25], const vector<pair<int, int> > &_playersPos)
{
	this->ownId = _ownId;
	this->playersPos = _playersPos;
}
void Player::printPos(int playerId)
{
	cout << this->playersPos[playerId].first <<" "<<this->playersPos[playerId].second << endl;
}
Player::~Player()
{

}
Splix::Splix(int _numberOfPlayer)
{
	this->players.clear();
	pair<int, int> pos[4] = {
		make_pair(5,5), 
		make_pair(5, 24), 
		make_pair(14, 5), 
		make_pair(14, 24)
	};
	for(int i = 0; i < 20; i++) {
		table[i] = "000000000000000000000000000000";
	}
	for(int i = 1; i <= _numberOfPlayer; i++) {
		for(int j = 0; j < 9; j++) {
			table[pos[i-1].first + dx[j]][pos[i-1].second + dy[j]] = i * 2 - 1 + '0';
		}
	}
	for(int i = 1; i <= _numberOfPlayer; i++) {
		Player player = Player
		(
			_numberOfPlayer, 
			i, 
			this->table, 
			vector<pair<int, int> >(pos, pos+_numberOfPlayer)
		);
		this->players.push_back(player);
	}
}
Splix::Splix(int _numberOfPlayer, string _table[25], const vector<pair<int, int> > &playerPos) 
{

}
Splix::~Splix() 
{
}
/***
/* @brief get each player decision alternatively
/* @step 
/* - each players
/*   + callPlayer
/*   + check if decision is in law
/* - find all loser if exist
/*	 + updateTable
*/
void Splix::nextTurn()
{
	vector<string> decision = vector<string>(this->players.size());
	for(int i = 0; i < this->players.size(); i++) {
		decision[i] = this->players.getDecision();
	}
	cout << "Decisions: "<<endl;
	for(int i = 0; i < this->players.size(); i++) cout << decision[i]<<" "; cout << endl;
	updateTable(decision);
}
/***
/* @brief: update table
/* @step:
/*  - remove losers
/*  - update stable cell
*/
void Splix::updateTable(const vector<string> &decision);
{

}
