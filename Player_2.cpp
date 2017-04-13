#include <bits/stdc++.h>
using namespace std;

const int dx[9] = {-1, -1, -1,  0, 0,  1, 1, 1, 0};
const int dy[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};
const string DIRECT_NAME[4] = { "RIGHT", "DOWN", "LEFT", "UP"};
const int dir_x[4] = { 0,  1,  0, -1};
const int dir_y[4] = { 1,  0, -1,  0};
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
	bool findACertainPath_inBound(const pair<int, int>&, int, int ,int, int);
	// playerId, state, mark, ret, steps, _d, _label, x, y, r, c;
	void findACertainPath_goAround(int,	const State&,const vector<vector<bool> >&, int&, queue<string>&,
								const vector<pair<int, int> >&,const vector<string>&, int, int, int, int);
} helper;

class Player
{
public:
	vector<State> states;
	int numberOfPlayer;
	int ownId;
	int turn;
	string lastStep;
	queue<string> steps;
//	Player(const Player&);
	/**
	* @param int _numberOfPlayer
	* @param int _ownId
	* @param const vector<string>& table
	* @param const vector<pair<int, int> >& palyersPos
	*/
	Player();
	Player(int, int, const vector<string>&, const vector<pair<int, int> >&);
	~Player();
	string getDecision();
	bool findACertainPath(int playerId, State state, queue<string> &steps);
	void updateState(const State&);
	int getScore(int);
};
Player::Player(){}
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
bool Player::findACertainPath(int playerId, State state, queue<string> &steps)
{
	if( state.playersPos[playerId-1] == make_pair(-1, -1) )
		return false;
	queue<pair<int, int> > q;
	int maxL = -1;
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
				maxL = max(maxL, d[newPos.first][newPos.second]);
				q.push(newPos);
			}
		}
	}
	// if(maxL == -1)maxL = 30;
	// for(int i = 0; i < 20; i++) {
	// 	for(int j = 0; j < 30; j++) {
	// 		cout << d[i][j] <<" ";
	// 	}
	// 	cout << endl;
	// }
	vector<vector<bool> > mark = vector<vector<bool> >(20, vector<bool>(30, true));
	string llabel[4] = {"RIGHT", "DOWN", "LEFT", "UP"};
	// "RIGHT", "DOWN", "LEFT", "UP";	
	vector<string> label_cw(llabel, llabel+4);
	pair<int, int> dd[4] = {make_pair(0,1), make_pair(1, 0), make_pair(0,-1), make_pair(-1, 0)};
	vector<pair<int, int> > d_cw(dd, dd+4);;
	// "LEFT", "DOWN", "RIGHT", "UP"
	vector<string> label_ccw = label_cw;
	swap(label_ccw[0], label_ccw[2]);
	vector<pair<int, int> > d_ccw = d_cw;;
	swap(d_ccw[0], d_ccw[2]);
	int ret = -1;
	// for(int l = 30; l >= 10; l--) 
		// if( d[state.playersPos[playerId-1].first][state.playersPos[playerId-1].second] >= l )
	{
		// mark = vector<vector<bool> >(20, vector<bool>(30, false));
		// for(int i = 0; i < 20; i++) 
		// {
			// for(int j = 0; j < 30; j++) mark[i][j] = d[i][j] >= l;
		// }
		// if( l == maxL)
		// for(int i = 0; i < 20; i++) 
		// {
		// 	for(int j = 0; j < 30; j++) cout << mark[i][j]; cout << endl;
		// }
		int rr[4] = {5, 5, 5, 4};
		int cc[4] = {8, 8, 8, 5};
		for(int r = 3; r <= rr[this->numberOfPlayer]; r++) 
		{
			for(int c = 3; c <= cc[this->numberOfPlayer]; c++) 
			{
				for(int x = 0; x <= 20 - r; x++) 
				{
					for(int y = 0; y <= 30 - c; y++)
					{
						if( helper.findACertainPath_inBound(state.playersPos[playerId-1], x, y, r, c) ) 
						{
							// cout <<state.playersPos[playerId-1].first<<" "<<state.playersPos[playerId-1].second<<" "<<x <<" "<<y<<" "<<r<<" "<<c<<endl;
							int tmp = -1;
							queue<string> tmpStep;
							helper.findACertainPath_goAround(
								playerId, 
								state, 
								mark,
								tmp,
								tmpStep,
								d_cw,
								label_cw,
								x,
								y,
								r,
								c);
							if( tmp > ret && !
								(
									(this->lastStep=="DOWN" && tmpStep.front()=="UP") ||
									(this->lastStep=="UP" && tmpStep.front()=="DOWN") ||
									(this->lastStep=="RIGHT" && tmpStep.front()=="LEFT") ||
									(this->lastStep=="LEFT" && tmpStep.front()=="RIGHT")
								))
							{
								ret = tmp;
								// queue<string> aa = tmpStep;
								// cout <<" PATH " << endl;
								// while(!aa.empty()) {
								// 	cout << aa.front() <<" ";
								// 	aa.pop();
								// }
								// cout << endl;
								// cout << "ret " << ret << endl;
								steps = tmpStep;
								// cout << x<<" "<<y<<" "<<r<<" "<<c<<endl;
							}
						}
					}
				}
			}
		}
	}
	// cout << "RETURN "<<(ret > 0)<<endl;
	return ret > 0;
}
bool Helper::findACertainPath_inBound(const pair<int, int> &pos, int x, int y, int r, int c)
{
	return (x == pos.first && pos.second >= y && pos.second < y + c) ||
	       (x + r - 1 == pos.first && pos.second >= y && pos.second < y + c) ||
	       (y == pos.second && pos.first >= x && pos.first < x + r) ||
	       (y + c - 1 == pos.second && pos.first >= x && pos.first < x + r);
}
void Helper::findACertainPath_goAround(int playerId,
									const State& _state, 
									const vector<vector<bool> >& mark, 
									int &ret, 
									queue<string >& steps,
									const vector<pair<int, int> >& _d,
									const vector<string>& _label,
									int x, int y, int r, int c) 
{
	State tmpState = _state;
	int idx;
	queue<string> tmpStep;
	int cnt = 0;
	pair<int, int> newPos, pos = _state.playersPos[playerId-1];
	for(int i = 0; i < 4; i++) 
	{
		if(!((i==0 && pos.first==x) || (i==1 && pos.second==y+c-1) || (i==2 && pos.first==x+r-1) || (i==3 && pos.second==y)))
			continue;
		newPos.first = pos.first + _d[i].first;
		newPos.second = pos.second + _d[i].second;
		if( helper.findACertainPath_inBound(newPos, x, y, r, c) && mark[newPos.first][newPos.second])
		{
			// cout << " first direction " << i << endl;
			idx = i;
			for(int loop = 0; loop < 4; loop++) 
			{
				while(cnt < 2 && helper.findACertainPath_inBound(newPos, x, y, r, c)) {
					// cout << " --- " << newPos.first <<" "<<newPos.second << endl;
					if( mark[newPos.first][newPos.second] == false ) return;
					tmpStep.push(_label[idx]);
					if( tmpState.table[newPos.first][newPos.second] != tmpState.table[pos.first][pos.second] && 
						(tmpState.table[newPos.first][newPos.second] == playerId * 2 - 1 + '0' || 
						 tmpState.table[pos.first][pos.second] == playerId * 2 - 1 + '0')
						)
						cnt++;
					if( tmpState.table[newPos.first][newPos.second] != playerId * 2 - 1 + '0' )
						tmpState.table[newPos.first][newPos.second] = playerId * 2 + '0';
					if( cnt == 2 ) break;
					pos = newPos;
					newPos.first = pos.first + _d[idx].first;
					newPos.second = pos.second + _d[idx].second;
				}
				idx = (idx + 1) % 4;
				newPos.first = pos.first + _d[idx].first;
				newPos.second = pos.second + _d[idx].second;
			}
			// cout << "Find a cetern path" << endl;
			// cout << "cnt "<< cnt << endl;
			// tmpState.printState();
			if(cnt == 2)
			{
				int tmp = helper.stablize(playerId, tmpState);
				if( tmp > ret ) 
				{
					ret = tmp;
					steps = tmpStep;
				}
			}

		}
	}
}
string Player::getDecision()
{
	if(this->steps.empty())
	{
		bool haveAPath = this->findACertainPath(this->ownId, this->states[this->states.size()-1], this->steps);
		if( !haveAPath)
		{
			pair<int, int> pos = this->states[this->states.size()-1].playersPos[this->ownId-1];
			int prevId = -1;
			for(int i = 0; i < 4; i++) {
				if( this->lastStep == DIRECT_NAME[i] ) {
					prevId = i;
					break;
				}
			}
			for(int i = 0; i < 4; i++) {
				if( prevId == -1 || (prevId - i) % 2 != 0) {
					pair<int, int> newPos;
					newPos.first = pos.first + dir_x[i];
					newPos.second = pos.second + dir_y[i];
					if( !helper.isOuter(newPos) ) {
						this->steps.push(DIRECT_NAME[i]);
						break;
					}
				}
			}
		}
	}
	// cout <<"Path "<<endl;
	// queue<string> tmp = this->steps;
	// while(!tmp.empty()) 
	// {
	// 	cout << tmp.front() << " ";
	// 	tmp.pop();
	// }
	// cout << endl;
	this->lastStep = this->steps.front(); this->steps.pop();
	return this->lastStep;
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
			cout << "Path " << endl;
			queue<string> _steps = this->players[i-1].steps;
			while(!_steps.empty())
			{
				cout <<_steps.front() <<" ";
				_steps.pop();
			}
			cout << endl;
		}
	}
};
int main()
{
	// freopen("input.txt", "r", stdin);
	// freopen("player.out", "w", stdout);
	int turn = 0;
	int numberOfPlayer, playerId;
	cin >> numberOfPlayer >> playerId;
	// cout << "number of players " << numberOfPlayer<<endl;
	// cout << "playerId "<<playerId<<endl;
	vector<string> table;
	vector<pair<int, int> > playersPos;
	Player myBot;
	string line;
	while( true) 
	{
		table = vector<string>();
		playersPos = vector<pair<int, int> >();
		// cout << " line 0 " << line<<endl;
		for(int i = 0; i < 20; i++) {
			cin >> line;
			// cout << "line "<<i+1<<" "<<line<<endl;
			table.push_back(line);
		}
		for(int i = 0; i < numberOfPlayer; i++)
		{
			pair<int, int> pos;
			cin >> pos.first >> pos.second;
			// cout <<"P " << i+1 <<" "<<pos.first <<" "<<pos.second <<endl;
			playersPos.push_back(pos);
		}
		if( turn == 0 )
			myBot = Player(numberOfPlayer, playerId, table, playersPos);
		else
			myBot.updateState(State(table, playersPos));
		// myBot.lastStep = "UP";
		// cout << "RIGHT" << endl;
		// cin.ignore(INT_MAX);
		cout << myBot.getDecision() << endl;
		turn++;
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
		// cout <<" Player "<< i +1 <<" decide in "<< dif<<" seconds."<<endl;
		printf("Player %d decide in %.2lf\n seconds.", i+1, dif);
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
	// cout <<"[STABLiZE] "<<ret << endl;
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
	// cout << "newPos " << newPos.first <<" "<<newPos.second << endl;
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
		// cout <<"Player "<<playerId<<" hit Player "<<x/2<<endl;
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