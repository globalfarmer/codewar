#include <bits/stdc++.h>
using namespace std;

const int dx[9] = {-1, -1, -1,  0, 0,  1, 1, 1, 0};
const int dy[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};
const string DIRECT_NAME[4] = { "LEFT", "UP", "RIGHT", "DOWN"};
const int dir_x[4] = { 0, -1, 0, 1};
const int dir_y[4] = {-1,  0, 1, 0};
const string EMPTY_LINE = "000000000000000000000000000000";
const int oo = 1e6;;
const int INF = 1e9;

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
	int stablize(int playerId, State &state);
	int getDirectID(int playerId,const vector<State>& states, const string &decision);
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

	int decisionInLaw(int playerId, const vector<State> &states, const string &decision);
	int currentDirect(int playerId, const vector<State>& states);
	int getScore(const int&, const State&);
	State removePlayer(int playerId, State lastState ); 
	State updateGame(int, const vector<State>& states, State, const string &decision);
	int tryingFill(const int&, State&, const vector<vector<int> >&, const vector<vector<int> >&, const int&);
	bool findPath(const pair<int, int>&, vector<string>&, int, State&, const vector<vector<int> >&, const vector<vector<int> >& g, const int&);
	int evaluateScore(int playerId, State state);
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
	string getDecision();
	pair<int, string> getDecision2(int depth, int playerId, State, State) ;
	pair<int, string> getDecision3();
	pair<int, string> getDecision4();
	void updateState(const State&);
	int getScore(int);
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
Player::~Player()
{

}
// string Player::minimax2(int depth, vector<State> states, vector<string> decisions) 
// {
// 	if( depth == 11 ) 
// 	{
// 		return (this->evaluateScore(this->ownId, states) + helper.getScore(this->ownId, states) )
// 		      -(this->evaluateScore(this->ownId % 2 + 1, states) + helper.getScore(this->ownId%2+1, states));
// 	}
// 	if( depth > 1 && depth % 2 == 1) 
// 	{

// 	}
// }
pair<int, string> Player::getDecision2(int depth, int playerId, State prevState, State curState) 
{
	// cout << "depth "<<depth << endl;
	if( depth == 3) 
	{
		return make_pair(helper.getScore(playerId, curState) + helper.evaluateScore(playerId, curState), "");
	}
	if( curState.playersPos[playerId-1] == make_pair(-1, -1) )
		return make_pair(-INF, "");
	int optValue = (depth % 2 ? -INF : INF);
	string decision;
	vector<State> states;
	if( depth > 1)
	{
		states.push_back(prevState);
		states.push_back(curState);
	}
	else
	{
		states.push_back(curState);
	}
	// cout << "depth "<<depth << endl;

	for(int j = 0; j < 4; j++) 
	{
		// cout << j << endl;
		State newState = helper.updateGame(playerId, states, curState, DIRECT_NAME[j]);
		// for(int ii = 0; ii < newState.playersPos.size(); ii++)
			// cout << newState.playersPos[ii].first <<" "<<newState.playersPos[ii].second << endl;
		pair<int, string> tmp = this->getDecision2(depth+1, playerId % 2 + 1, curState, newState);
		if( depth % 2 && tmp.first > optValue)
		{
			optValue = tmp.first;
			decision = DIRECT_NAME[j];
		}
		else if( depth % 2 == 0 && tmp.first < optValue)
		{
			optValue = tmp.first;
			decision = DIRECT_NAME[j];
		}
	}
	return make_pair(optValue, decision);
}
pair<int, string> Player::getDecision3()
{
	return make_pair(0, "RIGHT");
}
pair<int, string> Player::getDecision4() 
{
	return make_pair(0, "RIGHT");
}
string Player::getDecision()
{
	pair<int, string> ret;
	if( this->numberOfPlayer == 2) {
		cout << "Game for two"<<endl;
		ret = this->getDecision2(1, this->ownId, State(), this->states[this->states.size()-1]);
	}
	else if( this->numberOfPlayer == 3)
		ret = this->getDecision3();
	else if( this->numberOfPlayer == 4)
		ret = this->getDecision4();
	return ret.second;
	// State lastState = this->states[this->states.size()-1];
	// int maxStat = 1;
	// for(int i = 0; i < this->numberOfPlayer; i++) maxStat *= 4;
	// if( lastState.table[lastState.playersPos[this->ownId-1].first][lastState.playersPos[this->ownId-1].second] - '0' == this->ownId*2-1) {
	// 	vector<string> decisions = vector<string>(this->numberOfPlayer, "");
	// 	for(int stat = 0; stat <= maxStat; stat++) {
	// 		for(int j = 0, tmp=stat; j < this->numberOfPlayer; j++) 
	// 		{
	// 			decision[j] = DIRECT_NAME[tmp%4];
	// 			tmp/=4;
	// 		}
	// 	}
	// 	return decision;
	// }
	return DIRECT_NAME[this->ownId-1];
	// return DIRECT_NAME[rand() % 4];//"RIGHT";
}
int Helper::evaluateScore(int playerId, State state) 
{
	if( state.playersPos[playerId-1] == make_pair(-1, -1) )
		return -oo;
	vector<pair<int, int> > myUnstable, myStable;
	for(int i = 0; i < 20; i++) 
	{
		for(int j = 0; j < 30; j++) 
			if( state.table[i][j] != '0' )
			{
				if( (state.table[i][j] - '0' + 1) / 2 != playerId )
				{
					if( (state.table[i][j] - '0') % 2 ) 
						state.table[i][j] = 's';
					else
						state.table[i][j] = 'u';
				}
				else 
				{
					if( ( (state.table[i][j] - '0') % 2 == 0) )
						myUnstable.push_back(make_pair(i, j));
					else
						myStable.push_back(make_pair(i, j));
				}
				
			}

	}
	// cout <<" OK "<<endl;
	// for(int i = 0; i < 20; i++) cout << state.table[i]<<endl;
	if( myUnstable.size() > 0) 
	{
		queue<pair<int, int> > q;
		// find d[i][j] is minimum turn in Which an enemy can reach to [i,j]
		vector<vector<int> > d = vector<vector<int> >(20, vector<int>(30, oo));
		q = queue<pair<int, int> >();
		for(int i = 0; i < state.playersPos.size(); i++) 
			if( i+1 != playerId && state.playersPos[i] != make_pair(-1, -1))
		{
			d[state.playersPos[i].first][state.playersPos[i].second] = 0;
			q.push(state.playersPos[i]);
		}
		while(!q.empty()) 
		{
			pair<int, int> pos = q.front(); q.pop();
			// cout << pos.first <<" "<<pos.second << endl;
			for(int j = 0; j < 4; j++) 
			{
				pair<int, int> newPos = make_pair(pos.first + dir_x[j], pos.second + dir_y[j]);
				if( !helper.isOuter(newPos) && d[newPos.first][newPos.second] == oo) 
				{
					d[newPos.first][newPos.second] = d[pos.first][pos.second] + 1;
					q.push(newPos);
				}
			}
		}
		cout <<" OK "<<endl;
		// for(int i = 0; i < 20; i++) {
		// 	for(int j = 0; j < 30; j++) {
		// 		cout << d[i][j] <<" ";
		// 	}
		// 	cout << endl;
		// }
		// find g[i][j] is minimum turn in which I can reach to a my stable ( but my stable still be changed in game ????)
		vector<vector<int> > g = vector<vector<int> >(20, vector<int>(30, oo));
		q = queue<pair<int, int> >();
		for(int i = 0; i < myStable.size(); i++) 
		{
			g[myStable[i].first][myStable[i].second] = 0;
			q.push(myStable[i]);
		}
		while(!q.empty())
		{
			pair<int, int> pos = q.front(); q.pop();
			for(int j = 0; j < 4; j++)
			{
				pair<int, int> newPos = make_pair(pos.first + dir_x[j], pos.second + dir_y[j]);
				if( !helper.isOuter(newPos) && 
					g[newPos.first][newPos.second] == oo  &&
					state.table[newPos.first][newPos.second] - '0' != playerId * 2)
				{
					g[newPos.first][newPos.second] = g[pos.first][pos.second] + 1;
					q.push(newPos);
				}
			}
		}
		// for(int i = 0; i < 20; i++) {
		// 	for(int j = 0; j < 30; j++) {
		// 		cout << g[i][j] <<" ";
		// 	}
		// 	cout << endl;
		// }
		int minD = oo;
		for(int i = 0; i < myUnstable.size(); i++) {
			minD = min(minD, d[myUnstable[i].first][myUnstable[i].second]);
			// cout << myUnstable[i].first <<" "<<myUnstable[i].second <<" "<<d[myUnstable[i].first][myUnstable[i].second]<<" "<<minD<< endl;

		}
		cout << "minD "<< minD<<endl;
		return this->tryingFill(playerId, state, d, g, minD);
	}
	return 0;
}
bool Helper::findPath(const pair<int, int>& pos, 
					vector<string> &mark, 
					int playerId, 
					State& state,
					const vector<vector<int> >& d, 
					const vector<vector<int> >& g, 
					const int& minD)
{
	// cout << "visit "<<pos.first<<" "<<pos.second<<endl;
	mark[pos.first][pos.second] = '1';
	if( state.table[pos.first][pos.second] - '0' == playerId * 2 - 1) return true;
	bool ret = false;
	for(int j = 0; j < 4; j++) 
	{
		pair<int, int> newPos = make_pair(pos.first + dir_x[j], pos.second + dir_y[j]);
		// cout <<"TO "<< newPos.first <<" "<<newPos.second<<endl;
		// cout <<"Sum " << d[newPos.first][newPos.second] + g[newPos.first][newPos.second] << endl;
		// cout <<"    " << g[newPos.first][newPos.second] <<" "<< g[pos.first][pos.second] << endl;
		if( !helper.isOuter(newPos) && 	
			mark[newPos.first][newPos.second] == '0' &&
			d[newPos.first][newPos.second] + g[newPos.first][newPos.second] < 2*minD &&
			(state.playersPos[playerId-1] == pos ||
			g[newPos.first][newPos.second] == g[pos.first][pos.second] - 1) &&
			state.table[newPos.first][newPos.second] - '0' != playerId * 2
			)
			ret |= this->findPath(newPos, mark, playerId, state, d, g, minD);
	}
	if( ret )
		state.table[pos.first][pos.second] = playerId * 2 + '0';
	return ret;
}
int Helper::tryingFill(const int& playerId, State& state,const vector<vector<int> >& d, const vector<vector<int> >& g, const int& minD)
{
	vector<string> mark = vector<string>(20, EMPTY_LINE);
	bool isAlive = this->findPath(state.playersPos[playerId-1], mark, playerId, state, d, g, minD);
	// cout << "-----------------" << endl;
	// for(int i = 0; i < 20; i++) cout << state.table[i] << endl;
	if( isAlive)
		return helper.stablize(playerId, state);
	return -oo;
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
	srand (time(NULL));
	Splix splix = Splix(2);
	// splix.testConstructor();
	for(int step = 0; step < 100; step++) {
		splix.updateStatePlayer();
		splix.printPulse();
		splix.nextTurn();
	}
	splix.updateStatePlayer();
	splix.printPulse();
	for(int i = 1; i <= splix.numberOfPlayer; i++)
	{
		cout << "evaluate Score of Player " << i << " is ";
		cout << helper.evaluateScore(i, splix.states[splix.states.size()-1]) << endl;
	}
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
	string decision;
	State lastState = this->states[this->states.size()-1];
	for(int i = 0; i < this->numberOfPlayer; i++)
	{
		time_t start,end;
		time (&start);
		decision = this->players[i].getDecision();
		time (&end);
		double dif = difftime (end,start);
		cout <<" Player "<< i +1 <<" decide in "<< dif<<" seconds."<<endl;
		cout << "Player "<< i+1<<" decide go: "<<decision<<endl;
		lastState = helper.updateGame(i+1, this->states, lastState, decision);
	}
	this->states.push_back(lastState);
	this->turn++;
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
int Helper::stablize(int playerId, State &state) 
{
	int ret = 0;
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
							ret++;
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
				ret++;
				state.table[i][j] = playerId * 2 - 1 + '0';
				state.lastCellState[i][j] = '0';
			}
		}
	}
	return ret;
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
int Helper::getDirectID(int playerId, const vector<State>& states, const string &decision)
{
	int directId = -1;
	for(int j = 0; j < 4; j++)
	{
		if( decision == DIRECT_NAME[j] )
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

int Helper::decisionInLaw(int playerId,const vector<State>& states, const string &decision)
{
	State state = states[states.size()-1];
	if( state.playersPos[playerId-1] == make_pair(-1, -1) ) return -1;
	int directId = this->getDirectID(playerId, states, decision);
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
int Helper::getScore(const int& playerId, const State& state)
{
	int score = 0;
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 30; j++) {
			if( state.table[i][j] == playerId * 2 - 1 + '0') score++;
		}
	}
	return score;
}
State Helper::removePlayer(int playerId, State lastState ) 
{
	int id = playerId;
	lastState.playersPos[id-1] = make_pair(-1, -1);
	for(int i = 0; i < 20; i++) {
		for(int j = 0; j < 30; j++) {
			if( lastState.table[i][j] == id * 2 + '0') {
				lastState.table[i][j] = lastState.lastCellState[i][j];
				lastState.lastCellState[i][j] = '0';
			}
		}
	}
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
	return lastState;
}
/***
/* @brief: update Game(last state)
/* @step:
/*  - check if players take a decision in law
/*  - find loser who was hit by another
/*  - update cell
*/
State Helper::updateGame(int playerId, const vector<State>& states, State lastState, const string &decision)
{
	if( lastState.playersPos[playerId-1] == make_pair(-1, -1))
		return lastState;
	int directID = this->decisionInLaw(playerId, states, decision);
	// cout <<" get directID successfully "<<decision<<" "<<directID<<endl;
	if(  directID == -1)
		return this->removePlayer(playerId, lastState);;
	pair<int, int> newPos, pos = lastState.playersPos[playerId-1];
	newPos.first = pos.first + dir_x[directID];
	newPos.second = pos.second + dir_y[directID];
	int x = lastState.table[newPos.first][newPos.second] - '0';
	if( x > 0 && x % 2 == 0) {
		cout <<"Player "<<playerId<<" hit Player "<<x/2<<endl;
		lastState = this->removePlayer(x/2, lastState);
	}
	if( lastState.table[newPos.first][newPos.second] == playerId * 2 - 1 + '0')
	{
		this->stablize(playerId, lastState);
	}
	else
	{
		lastState.lastCellState[newPos.first][newPos.second] = lastState.table[newPos.first][newPos.second];
		lastState.table[newPos.first][newPos.second] = playerId * 2 + '0';
	}
	lastState.playersPos[playerId-1] = newPos;
	return lastState;
}