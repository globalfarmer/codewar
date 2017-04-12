#include <bits/stdc++.h>
using namespace std;

const int dx[9] = {-1, -1, -1,  0, 0,  1, 1, 1, 0};
const int dy[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};
const string DIRECT_NAME[4] = { "LEFT", "UP", "RIGHT", "DOWN"};
const int dir_x[4] = { 0, -1, 0, 1};
const int dir_y[4] = {-1,  0, 1, 0};
const string EMPTY_LINE = "000000000000000000000000000000";

class State
{
public:
	vector<string> table;
	vector<string> lastCellState;
	vector<pair<int, int> > playersPos;
	State();
	State(const vector<string>& _table, const vector<pair<int, int> >& _playersPos);
	void printState();
};

class Helper {
public:
	bool dfs(pair<int, int> pos, State &state, vector<string> &mark, int playerId);
	void stablize(int playerId, State &state);
	int getDirectID(int playerId,const vector<State>& states, const vector<string> &decisions);
	bool isOuter(const pair<int, int>& pos);
	/**
	/* @brief: check if playerId make a decision in law, if true update currentPos of playerId
	/* @return: playerId make a decision in law

	case1: Không đưa ra được nước đi nào (sau thời gian timeout là 1s)
	case2: Đưa ra các output không đúng với format 4 hướng di chuyển
	case3: Đi ra khỏi bàn đấu
	case4: Đi ngược lại với hướng đi của lượt trước đó
	case5: Đi vào ô unstable của chính mình
	*/

	int decisionInLaw(int playerId, const vector<State> &states, const vector<string> &decisions);
	int currentDirect(int playerId, const vector<State>& states);
} helper;

class Player
{
public:
	vector<State> states;
	int numberOfPlayer;
	int ownId;
	int turn;
//	Player(const Player&);
	/**
	* @param int _numberOfPlayer
	* @param int _ownId
	* @param const vector<string>& table
	* @param const vector<pair<int, int> >& palyersPos
	*/
	Player(int, int, const vector<string>&, const vector<pair<int, int> >&);
	~Player();
	string getDecision()
	{
		if(this->turn % 11 < 3 ) return "DOWN";
			else if(this->turn % 11 < 6) return "RIGHT";
				else if( this->turn % 11 < 9) return "UP";
					return "LEFT";
		// if( this->ownId == 1) return "RIGHT";
		// else
		// 	if(this->ownId == 2) return "LEFT";
		// return DIRECT_NAME[this->ownId-1];
	}
	void updateState(const State&);
	bool isValidDecision(string);
	int getScore(int);
	int minDistanceToOtherUnstable();
	int minDistanceToStable();
	void printPos(int);
};
Player::Player(int _numberOfPlayer,int _ownId,const vector<string> &_table, const vector<pair<int, int> > &_playersPos)
{
	this->turn = 0;
	this->ownId = _ownId;
	this->numberOfPlayer = _numberOfPlayer;
	this->states.push_back(State(_table, _playersPos));
}
void Player::updateState(const State& state)
{
	this->states.push_back(state);
	this->turn++;
}
int Player::getScore(int playerId)
{
	int score = 0;
	State state = this->states[this->states.size()-1];
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 30; j++) {
			if( state.table[i][j] == playerId * 2 - 1 + '0') score++;
		}
	}
	return score;
}
void Player::printPos(int playerId)
{
	// cout << this->playersPos[playerId].first <<" "<<this->playersPos[playerId].second << endl;
}
Player::~Player()
{

}
class Splix
{
public:
	// players (AI)
	vector<Player> players;
	vector<State> states;
	int numberOfPlayer;
	int turn;
	Splix(int _numberOfPlayer);
	/**
	/* @param int _numberOfPlayer
	/* @param const vector<string>& table
	/* @param const vector<pair<int, int> >& palyersPos
	*/
	Splix(int, const vector<string>&, const vector<pair<int, int> >&);
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
	/***
	/* @brief get each player decision alternatively
	/* @step
	/* - each players
	/*   + callPlayer
	/*   + check if decision is in law
	/* - find all loser if exist
	/*	 + updateTable
	*/
    void nextTurn(const vector<string>&);
	void updateGame(const vector<string>& decisions);
	void updateStatePlayer();
	void testConstructor()
	{
		cout << "Number of Player " << this->numberOfPlayer << endl;
		cout << "Turn " << this->turn << endl;
		this->states[0].printState();
	}
	void printPulse()
	{
		cout << "Number of Player " << this->numberOfPlayer << endl;
		cout << "Turn " << this->turn << endl;
		this->states[this->turn].printState();
		for(int i = 1; i <= this->numberOfPlayer; i++)
		{
			cout << "Score of Player " << i <<" is " << this->players[i-1].getScore(i) <<endl;
		}
	}
};
int main()
{
	Splix splix = Splix(2);
	// splix.testConstructor();
	for(int step = 0; step < 60; step++) {
		splix.updateStatePlayer();
		splix.printPulse();
		splix.nextTurn();
	}
	splix.updateStatePlayer();
	splix.printPulse();
	return 0;
}

Splix::Splix(int _numberOfPlayer)
{
	this->numberOfPlayer = _numberOfPlayer;
	this->turn = 0;
	pair<int, int> pos[4] = {
		make_pair(5,5),
		make_pair(5, 15),
		make_pair(9, 10),
		make_pair(14, 24)
	};
	// pair<int, int> pos[4] = {
	// 	make_pair(5,5),
	// 	make_pair(5, 24),
	// 	make_pair(14, 5),
	// 	make_pair(14, 24)
	// };
	State state;
	for(int i = 1; i <= _numberOfPlayer; i++) {
		for(int j = 0; j < 9; j++) {
			state.table[pos[i-1].first + dx[j]][pos[i-1].second + dy[j]] = i * 2 - 1 + '0';
		}
	}
	state.playersPos = vector<pair<int, int> >(pos, pos+_numberOfPlayer);
	this->states.push_back(state);
}
Splix::Splix(int _numberOfPlayer, const vector<string> &_table, const vector<pair<int, int> > &_playersPos)
{
	this->numberOfPlayer = _numberOfPlayer;
	this->turn = 0;
	State state;
	state.table = _table;
	state.playersPos = _playersPos;
	this->states.push_back(state);
}
Splix::~Splix()
{
}
void Splix::updateStatePlayer() 
{
	// start game
	if( this->turn == 0 )
	{
		for(int i = 1; i <= this->numberOfPlayer; i++)
		{
			Player player = Player
			(
				this->numberOfPlayer,
				i,
				this->states[this->turn].table,
				this->states[this->turn].playersPos
			);
			player.turn = this->turn;
			this->players.push_back(player);
		}
	}
	else // inGame
	{
		for(int i = 0; i < this->players.size(); i++)
		{
			players[i].updateState(this->states[this->turn]);
			players[i].turn = this->turn;
		}
	}
}
/***
/* @brief get each player decision alternatively
/* @step
/* - each players
/*   + getDecisions respectively
/*   + updateGame
*/
void Splix::nextTurn()
{
	vector<string> decision = vector<string>(this->numberOfPlayer);
	for(int i = 0; i < this->numberOfPlayer; i++)
	{
		decision[i] = this->players[i].getDecision();
	}
	cout << "Decisions: "<<endl;
	for(int i = 0; i < this->players.size(); i++) cout << decision[i]<<" "; cout << endl;
	this->updateGame(decision);
	this->turn++;
}
/***
/* @brief: update Game(last state)
/* @step:
/*  - check if players take a decision in law
/*  - find loser who was hit by another
/*  - update cell
*/
void Splix::updateGame(const vector<string> &decisions)
{
	State lastState = this->states[this->turn];
	vector<bool> illegalDecision = vector<bool>(this->numberOfPlayer, false);
	for(int i = 0; i < this->numberOfPlayer; i++)
	{
		int directID = helper.decisionInLaw(i+1, this->states, decisions);
		if(  directID != -1)
		{
			lastState.playersPos[i].first += dir_x[directID];
			lastState.playersPos[i].second += dir_y[directID];
		}
		else
		{
			illegalDecision[i] = true;
		}
	}
	for(int i = 0; i < illegalDecision.size(); i++)	cout << illegalDecision[i] << " ";
	cout << endl;
	vector<bool> isHit = vector<bool>(this->numberOfPlayer, false);
	for(int i = 0; i < this->numberOfPlayer; i++) if(lastState.playersPos[i] != make_pair(-1, -1))
	{
		int cell = lastState.table[lastState.playersPos[i].first][lastState.playersPos[i].second] - '0';
		if( cell > 0 && cell % 2 == 0 ) isHit[cell/2-1] = true;
	}
	for(int i = 0; i < this->numberOfPlayer; i++) if (lastState.playersPos[i] != make_pair(-1, -1))
		for(int j = i+1; j < this->numberOfPlayer; j++) if (lastState.playersPos[j] != make_pair(-1, -1))
			if( lastState.playersPos[i] == lastState.playersPos[j] )
			{
				isHit[i] = isHit[j] = true;
			}
	for(int i = 0; i < isHit.size(); i++) cout << isHit[i] <<" ";
	cout << endl;
	// remove player who make a wrong decision or is hit by another player
	// remove unstable cell
	for(int id = 1; id <= this->numberOfPlayer; id++)
	{
		if( isHit[id-1] || illegalDecision[id-1] )
		{
			lastState.playersPos[id-1] = make_pair(-1, -1);
			for(int i = 0; i < 20; i++) {
				for(int j = 0; j < 30; j++) {
					if( lastState.table[i][j] == id * 2 + '0') {
						lastState.table[i][j] = lastState.lastCellState[i][j];
						lastState.lastCellState[i][j] = '0';
					}
				}
			}
		}
	}
	// remove stable cell
	for(int id = 1; id <= this->numberOfPlayer; id++)
	{
		if( isHit[id-1] || illegalDecision[id-1] )
		{
			for(int i = 0; i < 20; i++) {
				for(int j = 0; j < 30; j++) {
					if( lastState.table[i][j] == id * 2 - 1 + '0') {
						lastState.table[i][j] = lastState.lastCellState[i][j];
						lastState.lastCellState[i][j] = '0';
					}
					if( (lastState.lastCellState[i][j] - '0' + 1) / 2 == id )
						lastState.lastCellState[i][j] = '0';
				}
			}
		}
		else if(lastState.playersPos[id-1] != make_pair(-1, -1))
		{
			pair<int, int> pos = lastState.playersPos[id-1];
			if( lastState.table[pos.first][pos.second] != id * 2 - 1 + '0') 
			{
				// cout <<" position " <<" "<<id*2-1<<" "<<pos.first <<" "<<pos.second << endl;
				lastState.lastCellState[pos.first][pos.second] = lastState.table[pos.first][pos.second];
				lastState.table[pos.first][pos.second] = id * 2 + '0';
			}
		}
	}
	for(int i = 0; i < this->numberOfPlayer; i++) 
	{
		pair<int, int> pos = lastState.playersPos[i];
		if( pos != make_pair(-1, -1) && lastState.table[pos.first][pos.second] == (i+1) * 2 - 1 + '0') {
			cout <<"Stablize "<<i+1<<endl;
			helper.stablize(i+1, lastState);
		}
	}
	this->states.push_back(lastState);
}


State::State()
{
	this->table = this->lastCellState = vector<string>(20, EMPTY_LINE);
}
State::State(const vector<string>& _table, const vector<pair<int, int> >& _playersPos)
{
	this->lastCellState = vector<string>(20, EMPTY_LINE);
	this->table = _table;
	this->playersPos = _playersPos;
}
void State::printState()
{
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 30; j++) {
			if( this->lastCellState[i][j] == '0')
				cout << ".";
			else
				cout << this->lastCellState[i][j];
		}
		cout << endl;
	}
	cout << "----------------------------------" << endl;
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

bool Helper::dfs(pair<int, int> pos, State &state, vector<string> &mark, int playerId) 
{
	mark[pos.first][pos.second] = '1';
	bool ret = true;
	for(int j = 0; j < 4; j++) {
		pair<int, int> newPos = make_pair(pos.first + dir_x[j], pos.second + dir_y[j]);
		if( !this->isOuter(newPos) ) 
		{
			if( mark[newPos.first][newPos.second] == '0' && 
			   ((state.table[newPos.first][newPos.second] - '0' + 1)/2 != playerId))
					ret &= dfs(newPos, state, mark, playerId);
		}
		else
		{
			ret = false;
		}
	}
	return ret;
}
void Helper::stablize(int playerId, State &state) 
{
	vector<string> mark = vector<string>(20, EMPTY_LINE);
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 30; j++) if( (state.table[i][j] -'0' + 1)/2 != playerId && mark[i][j] == '0') {
			if( dfs(make_pair(i, j), state, mark, playerId) )
			{	
				// cout << " DFS_------" << i <<" "<<j<< " true"<<endl;
				for(int i = 0; i < 20; i++) {
					for(int j = 0; j < 30; j++)
						if(mark[i][j]=='1') 
						{
							state.table[i][j] = playerId * 2 - 1 + '0';
							state.lastCellState[i][j] = '0';
						}
				}
			}
			else
			{
				// cout << " DFS_------" << i <<" "<<j<< " false"<<endl;
			}
			for(int i = 0; i < 20; i++) {
				for(int j = 0; j < 30; j++)
					if(mark[i][j] == '1' ) mark[i][j] = '2';
			}
		}
	}
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 30; j++) {
			if( state.table[i][j] == playerId * 2 + '0' ) {
				state.table[i][j] = playerId * 2 - 1 + '0';
				state.lastCellState[i][j] = '0';
			}
		}
	}
}
int Helper::currentDirect(int playerId, const vector<State>& states)
{
    if( states.size() <= 1) return -1;
    pair<int, int> lastPos = states[states.size()-1].playersPos[playerId-1];
    pair<int, int> prevPos = states[states.size()-2].playersPos[playerId-1];
    pair<int, int> delta = make_pair(lastPos.first - prevPos.first, lastPos.second - prevPos.second);
    for(int i = 0; i < 4; i++)
        if( delta.first == dir_x[i] && delta.second == dir_y[i] )
            return i;
    return 555;
}
int Helper::getDirectID(int playerId,const vector<State>& states, const vector<string> &decisions)
{
	int directId = -1;
	for(int j = 0; j < 4; j++)
	{
		if( decisions[playerId-1] == DIRECT_NAME[j] )
		{
			directId = j;
			break;
		}
	}
	if( directId == -1) return -1;
	int previousDirect = this->currentDirect(playerId, states);
	// const string DIRECT_NAME[4] = { "LEFT", "UP", "RIGHT", "DOWN"};
	if( previousDirect != -1 && directId != previousDirect && (directId - previousDirect) % 2 == 0 )
		return -1;
	return directId;
}
bool Helper::isOuter(const pair<int, int>& pos)
{
	return (pos.first < 0 || pos.first >= 20 || pos.second < 0 || pos.second >= 30);
}
/**
/* @brief: check if playerId make a decision in law, if true update currentPos of playerId
/* @return: playerId make a decision in law

case1: Không đưa ra được nước đi nào (sau thời gian timeout là 1s)
case2: Đưa ra các output không đúng với format 4 hướng di chuyển
case3: Đi ra khỏi bàn đấu
case4: Đi ngược lại với hướng đi của lượt trước đó
case5: Đi vào ô unstable của chính mình
*/

int Helper::decisionInLaw(int playerId,const vector<State>& states, const vector<string> &decisions)
{
	State state = states[states.size()-1];
	if( state.playersPos[playerId-1] == make_pair(-1, -1) ) return -1;
	int directId = this->getDirectID(playerId, states, decisions);
	cout << "directId " << playerId <<" "<<directId << endl;
	if( directId == -1) return -1;
	pair<int, int> newPos;
	newPos.first = state.playersPos[playerId-1].first + dir_x[directId];
	newPos.second = state.playersPos[playerId-1].second + dir_y[directId];
	cout << "newPos " << newPos.first <<" "<<newPos.second << endl;
	bool isOuter = this->isOuter(newPos);
	if( isOuter ) return -1;
	bool goInOwnUnstable = state.table[newPos.first][newPos.second] == playerId * 2 + '0';
	if( goInOwnUnstable ) return -1;
	return directId;
}