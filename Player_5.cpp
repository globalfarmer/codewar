/**
/ (1) if have other cell(not own stable or unstable) that also next to a stable and this stable cell and all unstable cell cant reach by enemy in x go to this unstable ( x <= 2 emergency > 2 is not)

/ if in bound
/    + (1)
/    +
/ if in unstable
/    + (1)
/    + else go to stable cloest
*/

#include <iostream>
#include <cstdio>
#include <vector>
#include <queue>
#include <cmath>
#include <time.h>
using namespace std;
const string DIRECT_NAME_STATIC[4] = {"RIGHT", "DOWN", "LEFT", "UP"};
const pair<int, int> DIRECT_XY_STATIC[4] = {make_pair(0,1), make_pair(1, 0), make_pair(0,-1), make_pair(-1,0)};
string DIRECT_NAME[4] = {"RIGHT", "DOWN", "LEFT", "UP"};
pair<int, int> DIRECT_XY[4] = {make_pair(0,1), make_pair(1, 0), make_pair(0,-1), make_pair(-1,0)};
const int oo = 1000000;
int cnt = 0;
class State
{
public:
	vector<string> table;
	vector<pair<int, int> > playersPos;
	State();
	// State(const State&);
	State(const vector<string>&, const vector<pair<int, int> >&);
	void printState() const;
};
class Helper
{
public:
	bool inStable(const pair<int, int>&, const int&, const State&);
	bool inUnstable(const pair<int, int>&, const int&, const State&);
	bool isBound(const pair<int, int>&, const int&, const State&);
	bool inBoard(const pair<int, int>&);
	pair<int, int> nextPos(const pair<int, int>&, const pair<int, int>&);
	int getMyTime(const int&, const State& , const vector<vector<int> >&);
	vector<pair<int, int> > getNextSteps(const pair<int, int>&, const int&, const State&);

} helper;
class Player
{
public:
	int turn;
	int numberOfPlayer;
	int ownID;
	State state;
	vector<pair<int, int> > nextSteps;
	vector<vector<int> > _dist;
	vector<vector<int> > min_d;
	vector<vector<pair<int, int> > > trace;
	vector<vector<int> > sum;
	int lastStepId;
	Player();
	Player(const int&, const int&);
	string getEmergencyDecision();
	void updateDistance();
	void updateSum();
	void updateState(const State&);
	pair<int, int> getCurPos();
};
class Decision2Player
{
public:
	queue<string> emergencySteps;
	bool emergency;
	Decision2Player();
	string getDecisionAsFirstPlayer(Player, Player);
	string getEmergencyDecision();
	pair<int, string> tryPath(const Player&, const Player& ,const int&, const int&, const int&, const int&, const int&);
	bool canKillEnemy(Player, Player);
	string inUnstableOrBoundCase(Player, Player);
	string inStableCase(Player, Player);
} decision2Player;
int main()
{
// time_t start,end;
// time (&start);

	// freopen("in1.txt", "r", stdin);
	// freopen("output.txt", "w", stdout);
	int numberOfPlayer, playerId, turn = 0;
	string line;
	cin >> numberOfPlayer;
	cin >> playerId;
	vector<string> table(20);
	vector<pair<int, int> > playersPos(numberOfPlayer);
	Player myBot(numberOfPlayer, playerId);
	Player enemy(numberOfPlayer, 3 - playerId);
	while(true)
	// for(int loop = 0; loop < 2; loop++)
	{
		for(int i = 0; i < 20; i++)
		{
			cin >> line;
			table[i] = line;
			// cout << "line " <<i<<" "<<line<<endl;
		}
		// cout <<"OK"<<endl;
		for(int i = 0; i < numberOfPlayer; i++)
		{
			cin >> playersPos[i].first >> playersPos[i].second;
			// cout << "Player "<<i<<" "<<playersPos[i].first <<" "<<playersPos[i].second << endl;
		}
		// cout << "OK" << endl;
		myBot.updateState(State(table, playersPos));
		enemy.updateState(State(table, playersPos));
		// cout << "OK" << endl;
		cout << decision2Player.getDecisionAsFirstPlayer(myBot, enemy) << endl;
	}
// time (&end);
// double dif = difftime (end,start);
// printf ("Elasped time is %.4lf seconds.", dif );
	return 0;
}
State::State(const vector<string>& _table, const vector<pair<int, int> >& _playersPos)
{
	this->table = _table;
	this->playersPos = _playersPos;
	// for(int i = 0; i < this->table.size(); i++) {
	// 	cout <<"line "<<i<<" "<<this->table[i]<<endl;
	// }
	// for(int i = 0; i < this->playersPos.size(); i++){
	// 	cout <<"Player "<<i<<" "<<this->playersPos[i].first <<" "<<this->playersPos[i].second<<endl;
	// }
}
State::State() {}
void State::printState() const
{
	cout << "[printState]" << endl;
	for(int i = 0; i < 20; i++)
	{
		cout << this->table[i] << endl;
	}
	for(int i = 0; i < this->playersPos.size(); i++)
	{
		cout << this->playersPos[i].first <<" "<<this->playersPos[i].second << endl;
	}
}
Player::Player(const int& _numberOfPlayer, const int& _playerId)
{
	this->numberOfPlayer = _numberOfPlayer;
	this->ownID = _playerId;
	this->turn = 0;
	this->nextSteps = vector<pair<int, int> >(4);
	this->lastStepId = -1;
}
pair<int, int> Player::getCurPos()
{
	return this->state.playersPos[this->ownID-1];
}
void Player::updateDistance()
{
	vector<vector<int> > dist = vector<vector<int> >(20, vector<int>(30, oo));
	queue<pair<int, int> > q;
	// cout << this->numberOfPlayer << endl;
	// for(int i = 0; i < this->numberOfPlayer; i++) cout << this.state.playersPos[i].first <<" "<<this.state.playersPos[i].second << endl;
	pair<int, int> curPos = this->getCurPos();
	q.push(curPos);
	dist[curPos.first][curPos.second] = 0;
	while(!q.empty())
	{
		pair<int, int> u = q.front(); q.pop();
		// cout <<" stay "<<u.first.first <<" "<<u.first.second <<" "<<dist[u.first.first][u.first.second] << endl;
		vector<pair<int, int> > steps = helper.getNextSteps(u, this->ownID, this->state);
		for(int i = 0; i < 4; i++)
		{
			// cout <<" to "<<steps[i].first <<" "<<steps[i].second << endl;
			if(steps[i] != make_pair(-1, -1) && dist[steps[i].first][steps[i].second] == oo)
			{
				dist[steps[i].first][steps[i].second] = dist[u.first][u.second] + 1;
				q.push(steps[i]);
			}
		}
	}
	// cout <<"Player " << this->ownID << endl;
	// cout <<"DIST"<<endl;
	// for(int i = 0; i < 20; i++) 
	// {
	// 	for(int j = 0; j < 30; j++) cout << dist[i][j] << " ";cout << endl;
	// }
	this->_dist = dist;
	this->trace = vector<vector<pair<int, int> > >(20, vector<pair<int, int> >(30, make_pair(-1, -1)));
	this->min_d = this->_dist;
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 30; j++) if( helper.inStable(make_pair(i, j), this->ownID, this->state) )
		{
			for(int x = 0; x < 20; x++)
			{
				for(int y = 0; y < 30; y++)
				{
					if( this->min_d[x][y] > this->_dist[i][j] + abs(i-x) + abs(j-y))
					{
						this->min_d[x][y] = this->_dist[i][j] + abs(i-x) + abs(j-y);
						this->trace[x][y] = make_pair(i, j);
					}
				}
			}
		}
	}
	// cout <<"MIN d"<<endl;
	// for(int i = 0; i < 20; i++) 
	// {
	// 	for(int j = 0; j < 30; j++) cout << this->min_d[i][j] << " ";cout << endl;
	// }
}
void Player::updateSum()
{
	this->sum = vector<vector<int> >(21, vector<int>(31, 0));
	for(int i = 0; i < 20; i++) 
	{
		for(int j = 0; j < 30; j++)
		{
			int tmp = (helper.inStable(make_pair(i, j), this->ownID, this->state) ? -1: 
				      (this->state.table[i][j]=='0' ? 1 : 2));
			sum[i+1][j+1] = sum[i][j+1] + sum[i+1][j] - sum[i][j] + tmp;
		}
	}
}
void Player::updateState(const State& newState)
{
	// newState.printState();
	// cout <<"[PLAYER::updateState] OK"<<endl;
	// cout <<"[PlAYER::updateState] Player " << this->ownID <<" is in "<<this->state.playersPos[this->ownID-1].first <<" "<<
	this->state = newState;
	this->nextSteps = helper.getNextSteps(newState.playersPos[this->ownID-1], this->ownID, newState);
	this->turn++;
	this->updateDistance();
	this->updateSum();
}
string Decision2Player::getEmergencyDecision()
{
	string ret = this->emergencySteps.front(); this->emergencySteps.pop();
	this->emergency = !this->emergencySteps.empty();
	return ret;
}
string Decision2Player::inUnstableOrBoundCase(Player myBot, Player enemy)
{
	int myTime = helper.getMyTime(myBot.ownID, myBot.state, enemy.min_d);
	queue<pair<int, int> > q;
	vector<vector<int> > g = vector<vector<int> >(20, vector<int>(30, oo));
	for(int i = 0; i < 20; i++) 
	{
		for(int j = 0; j < 30; j++) if(helper.inStable(make_pair(i, j), myBot.ownID, myBot.state))
		{
			q.push(make_pair(i,j));
			g[i][j] = 0;
		}
	}
	while(!q.empty())
	{
		pair<int, int> u = q.front(); q.pop();
		vector<pair<int, int> > steps = helper.getNextSteps(u, myBot.ownID, myBot.state);
		for(int i = 0; i < 4; i++) if( steps[i] != make_pair(-1, -1) && g[steps[i].first][steps[i].second] == oo)
		{
			g[steps[i].first][steps[i].second] = g[u.first][u.second] + 1;
			q.push(steps[i]);
		}
	}
	// for(int i = 0; i < 20; i++)
	// {
	// 	for(int j = 0; j < 30; j++) cout << g[i][j]<<" ";cout << endl;
	// }
	// inStable
	if(myTime == oo) 
	{
		// cout <<"OK"<<endl;
		pair<int, int> curPos = myBot.getCurPos();
		for(int i = 0; i < 4; i++)
		{
			pair<int, int> nextPos = myBot.nextSteps[i];
			if( nextPos != make_pair(-1, -1) && 
				!helper.inStable(nextPos, myBot.ownID, myBot.state) &&
				enemy.min_d[nextPos.first][nextPos.second] >= 3
				)
			{
				vector<pair<int, int> > steps = helper.getNextSteps(nextPos, myBot.ownID, myBot.state);
				for(int ii = 0; ii < 4; ii++) if( steps[ii] != make_pair(-1, -1) && steps[ii] != curPos && helper.inStable(steps[ii], myBot.ownID, myBot.state))
				{
					// cout <<"2 steps"<<endl;
					return DIRECT_NAME[i];
				}
			}
		}
		// cout <<"out of 2 steps"<<endl;
		for(int i = 0; i < 4; i++)
		{
			pair<int, int> nextPos = myBot.nextSteps[i];
			if( nextPos != make_pair(-1, -1) && 
				!helper.inStable(nextPos, myBot.ownID, myBot.state) &&
				enemy.min_d[nextPos.first][nextPos.second] >= 4
				)
			{
				// cout << "OK 3 steps"<<endl;
				vector<pair<int, int> > steps = helper.getNextSteps(nextPos, myBot.ownID, myBot.state);
				for(int ii = 0; ii < 4; ii++) if( steps[ii] != make_pair(-1, -1) && !helper.inStable(steps[ii], myBot.ownID, myBot.state))
				{
					vector<pair<int, int> > steps2 = helper.getNextSteps(steps[ii], myBot.ownID, myBot.state);
					for(int i2 = 0; i2 < 4; i2++)  if( steps2[i2] != make_pair(-1, -1) && helper.inStable(steps2[i2], myBot.ownID, myBot.state))
					{
						// cout <<"3 steps"<<endl;

						return DIRECT_NAME[i];
					}
				}
			}
			
		}
		for(int i = 0; i < 4; i++)
			if( myBot.nextSteps[i] != make_pair(-1, -1) && helper.inStable(myBot.nextSteps[i], myBot.ownID, myBot.state) )
				return DIRECT_NAME[i];
	}
	else
	{
		int tmpMyTime = (myTime + 1 ) / 2;
		// cout <<"OK "<<myTime<<" "<<tmpMyTime<<endl;
		int max_g = -oo;
		string decision = "";
		for(int i = 0; i < 4; i++) if( myBot.nextSteps[i] != make_pair(-1, -1)) 
		{
			pair<int, int> nextPos = myBot.nextSteps[i];
			if(g[nextPos.first][nextPos.second] < tmpMyTime ) 
			{
				if( max_g < g[nextPos.first][nextPos.second] )
				{
					max_g = g[nextPos.first][nextPos.second];
					decision = DIRECT_NAME[i];
				}
			}
		}
		return decision;
	}
	
	// cout <<"RET "<<ret<<" "<<::cnt << endl;
	// cout << " OK " << endl;
	for(int i = 0; i < 4; i++)
		if( myBot.nextSteps[i] != make_pair(-1, -1))
			return DIRECT_NAME[i];
	// this case when bot can go into a cell that not own territory(not stable and not unstable) in 2 or 3 so to a stable
	return "ERROR";
}
string Decision2Player::inStableCase(Player myBot, Player enemy)
{
	// cout << "[Player::inStableCase]" << endl;
	vector<pair<int, int> > bounds;
	string decision = "";
	int minDist = oo;
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 30; j++)
		{
			if( helper.inStable(make_pair(i, j), myBot.ownID, myBot.state) && 
				helper.isBound(make_pair(i, j), myBot.ownID, myBot.state))
			{
				// cout << i <<" "<<j<<endl;
				int oldDist = abs(myBot.getCurPos().first - i) + abs(myBot.getCurPos().second-j);
				// cout << oldDist << endl;
				for(int kk=0; kk < 4; kk++) if(myBot.nextSteps[kk] != make_pair(-1, -1))
				{
					pair<int, int> nextStep = myBot.nextSteps[kk];
					int newDist = abs(nextStep.first-i) + abs(nextStep.second-j);
					// cout <<"To "<< nextStep.first <<" "<<nextStep.second << endl;
					// cout <<" newDist " << newDist<<endl;
					if( oldDist > newDist && minDist > newDist )
					{
						decision = DIRECT_NAME[kk];
						minDist = newDist;
					}
				}
			}
		}
	}
	return decision;
}
Decision2Player::Decision2Player()
{
	this->emergency = false;
	this->emergencySteps = queue<string>();
}
bool Decision2Player::canKillEnemy(Player myBot, Player enemy)
{
	return false;
}
string Decision2Player::getDecisionAsFirstPlayer(Player myBot, Player enemy)
{
	// in bound or unstable
	string ret;
	pair<int, int> curPos = myBot.getCurPos();
	if( decision2Player.emergency || decision2Player.canKillEnemy(myBot, enemy) )
	{
		ret = decision2Player.getEmergencyDecision();
	}
	else if( helper.inUnstable(curPos, myBot.ownID, myBot.state) ||
		     helper.isBound(curPos, myBot.ownID, myBot.state))
	{
		ret = decision2Player.inUnstableOrBoundCase(myBot, enemy);
	}
	// in stable
	ret = decision2Player.inStableCase(myBot, enemy);
	return ret;
}
vector<pair<int, int> > Helper::getNextSteps(const pair<int, int>& curPos, const int& playerId, const State& state)
{
	vector<pair<int, int> > nextSteps;
	for(int j = 0; j < 4; j++)
	{
		// cout <<"[Helper::getNextSteps] "<<j<<endl;
		// cout << DIRECT_XY[j].first <<" "<<DIRECT_XY[j].second << endl;
		// cout << curPos.first <<" "<<curPos.second<<endl;
		pair<int, int> nextPos = helper.nextPos(curPos, DIRECT_XY[j]);
		// cout << nextPos.first <<" "<<nextPos.second<<endl;
		if(
			helper.inBoard(nextPos) &&
			!helper.inUnstable(nextPos, playerId, state)
		  )
		{
			nextSteps.push_back(nextPos);
		}
		else
		{
			nextSteps.push_back(make_pair(-1, -1));
		}
	}
	return nextSteps;
}
bool Helper::isBound(const pair<int, int>& curPos, const int& playerId, const State& state)
{
	vector<pair<int, int> > steps = helper.getNextSteps(curPos, playerId, state);
	for(int i = 0; i < steps.size(); i++)
		if( steps[i] != make_pair(-1, -1) && !helper.inStable(steps[i], playerId, state) )
		{
			return true;
		}
	return false;
}
bool Helper::inBoard(const pair<int, int>& curPos)
{
	return !(curPos.first < 0 || curPos.first >= 20 || curPos.second < 0 || curPos.second >= 30);
}
bool Helper::inStable(const pair<int, int>& curPos, const int& playerId, const State& state)
{
	return (state.table[curPos.first][curPos.second] - '0') == playerId * 2 - 1;
}
bool Helper::inUnstable(const pair<int, int>& curPos, const int& playerId, const State& state)
{
	return (state.table[curPos.first][curPos.second] - '0') == playerId * 2;
}
pair<int, int> Helper::nextPos(const pair<int, int>& curPos, const pair<int, int>& add)
{
	return make_pair(curPos.first + add.first, curPos.second + add.second);
}
int Helper::getMyTime(const int& playerId, const State& state, const vector<vector<int> >& dist)
{
	int ret = oo;
	for(int i = 0; i < 20; i++)
		for(int j = 0; j < 30; j++)
			if( state.table[i][j] - '0' == playerId * 2 )
			{
				ret = min(ret, dist[i][j]);
			}
	return ret;
}
