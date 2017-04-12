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
class State
{
public:
	string table[25];
	vector<pair<int, int> > playersPos;
	void printState() 
	{
		for(int i = 0; i < 20; i++) 
		{
			// cout << this->table[i] << endl;
			for(int j = 0; j < 30; j++) 
			{
				if( this->table[i][j] == '0')
					cout << ".";
				else
					cout << this->table[i][j];
			}
			cout << endl;
		}
		for(int i = 0; i < this->playersPos.size(); i++) 
		{
			cout << this->playersPos[i].first <<" "<<this->playersPos[i].second<<endl;
		}
	}
};
class Splix
{
public:
	// players (AI)
	vector<Player> players;
	vector<State> states;
	int numberOfPlayer;
	int turn;
	Splix(int _numberOfPlayer);
	Splix(int _numberOfPlayer, string _table[25], const vector<pair<int, int> > &_playersPos);
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
    void nextTurn(const vector<string> &decisions);
	/***
	/* @brief: update table
	/* @step:
	/*  - remove losers
	/*  - update stable cell
    */
	void updateTable(const vector<string> &decisions);
	void testConstructor()
	{
		cout << "Number of Player " << this->numberOfPlayer << endl;
		cout << "Turn " << this->turn << endl;
		this->states[0].printState();
	}
};
int main() 
{
	Splix splix = Splix(3);
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
	this->numberOfPlayer = _numberOfPlayer;
	this->turn = 0;
	pair<int, int> pos[4] = {
		make_pair(5,5), 
		make_pair(5, 24), 
		make_pair(14, 5), 
		make_pair(14, 24)
	};
	State state;
	state.playersPos = vector<pair<int, int> >(pos, pos+_numberOfPlayer);
	for(int i = 0; i < 20; i++) {
		state.table[i] = "000000000000000000000000000000";
	}
	for(int i = 1; i <= _numberOfPlayer; i++) {
		for(int j = 0; j < 9; j++) {
			state.table[pos[i-1].first + dx[j]][pos[i-1].second + dy[j]] = i * 2 - 1 + '0';
		}
	}
	this->states.push_back(state);
}
Splix::Splix(int _numberOfPlayer, string _table[25], const vector<pair<int, int> > &_playersPos) 
{
	this->numberOfPlayer = _numberOfPlayer;
	this->turn = 0;
	State state;
	copy(_table, _table+25, state.table);
	state.playersPos = _playersPos;
	this->states.push_back(state);
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
	// start game
	// if( this->turn == 0 ) 
	// {
	// 	for(int i = 1; i <= this->numberOfPlayer; i++) 
	// 	{
	// 		Player player = Player
	// 		(
	// 			_numberOfPlayer,
	// 			i, 
	// 			this->states[this->turn].table, 
	// 			this->states[this->turn].playersPos
	// 		);
	// 		this->players.push_back(player);
	// 	}
	// }
	// else
	// {
	// 	for(int i = 0; i < this->players.size(); i++) 
	// 	{
	// 		players[i].updateSate(this->states[this->turn].table, this.states[this->turn.playersPos]);
	// 	}
	// }
	// vector<string> decision = vector<string>(this->numberOfPlayer);
	// for(int i = 0; i < this->numberOfPlayer; i++) {
	// 	decision[i] = this->players[i].getDecision();
	// }
	// cout << "Decisions: "<<endl;
	// for(int i = 0; i < this->players.size(); i++) cout << decision[i]<<" "; cout << endl;
	// updateTable(decision);
}
/***
/* @brief: update table
/* @step:
/*  - remove losers
/*  - update stable cell
*/
void Splix::updateTable(const vector<string> &decisions)
{
}
