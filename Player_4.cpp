/**
/ (1) if have other cell(not own stable or unstable) that also next to a stable and this stable cell and all unstable cell cant reach by enemy in x go to this unstable ( x <= 2 emergency > 2 is not)
/ if in stable find road to bound ( find shorstest path to the broadest bound)
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
using namespace std;
const string DIRECT_NAME[4] = {"RIGHT", "DOWN", "LEFT", "UP"};
const pair<int, int> DIRECT_XY[4] = {make_pair(0,1), make_pair(1, 0), make_pair(0,-1), make_pair(-1,0)};
const int oo = 1000000;
class State
{
public:
	vector<string> table;
	vector<pair<int, int> > playersPos;
	State();
	// State(const State&);
	State(const vector<string>&, const vector<pair<int, int> >&);
	void printState();
};
class Helper
{
public:
	bool inStable(const pair<int, int>&, const int&, const State&);
	bool inUnstable(const pair<int, int>&, const int&, const State&);
	bool isBound(const pair<int, int>&, const int&, const State&);
	bool inBoard(const pair<int, int>&);
	bool opositeDirection(const string&, const string&);
	pair<int, int> nextPos(const pair<int, int>&, const pair<int, int>&);
	int getMyTime(const int&, const State& , const vector<vector<int> >&);
	vector<pair<int, int> > getNextSteps(const pair<int, int>&, const int&, const State&, const string&);

} helper;
class Player
{
public:
	int turn;
	int numberOfPlayer;
	int ownID;
	bool emergency;
	string emergencyStep;
	State state;
	vector<pair<int, int> > nextSteps;
	vector<int> dirId;
	vector<string> dirStr;
	Player();
	Player(const int&, const int&);
	string inUnstableOrBoundCase();
	string inStableCase();
	string getDecision();
	string getEmergencyDecision();
	void updateState(const State&);
};
int main() 
{
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	int numberOfPlayer, playerId, turn = 0;
	string line;
	cin >> numberOfPlayer;
	cin >> playerId;
	vector<string> table(20);
	vector<pair<int, int> > playersPos(numberOfPlayer);
	Player myBot(numberOfPlayer, playerId);
	while(true) 
	{

		bool isReturned = false;
		for(int i = 0; i < 20; i++) 
		{
			cin >> line;
			if( myBot.emergency ) 
			{
				cout << myBot.getEmergencyDecision() << endl;
				isReturned = true;
			}
			table[i] = line;
		}
		for(int i = 0; i < numberOfPlayer; i++)
		{
			cin >> playersPos[i].first >> playersPos[i].second;
		}
		myBot.updateState(State(table, playersPos));
		if( !isReturned )
			cout << myBot.getDecision() << endl;
	}
	return 0;
}
State::State(const vector<string>& _table, const vector<pair<int, int> >& _playersPos)
{
	this->table = _table;
	this->playersPos = _playersPos;
}
State::State() {}
Player::Player(const int& _numberOfPlayer, const int& _playerId)
{
	this->numberOfPlayer = _numberOfPlayer;
	this->ownID = _playerId;
	this->turn = 0;
	this->emergency = false;
	this->nextSteps = vector<pair<int, int> >(4);
	this->dirId = vector<int>(_numberOfPlayer, -1);
	this->dirStr = vector<string>(_numberOfPlayer, "");
}
void Player::updateState(const State& newState)
{
	if( this->turn > 0 )
	{
		for(int i = 0; i < this->numberOfPlayer; i++) if(newState.playersPos[i] != make_pair(-1, -1))
		{
			pair<int, int> detal = make_pair
									(
										newState.playersPos[i].first -this->state.playersPos[i].first, 
										newState.playersPos[i].second-this->state.playersPos[i].second
									);
			for(int j = 0; j < 4; j++) if( DIRECT_XY[j] == detal )
			{
				dirId[i] = j;
				dirStr[i] = DIRECT_NAME[j];
			}
		}
	}
	this->nextSteps = helper.getNextSteps(this->state.playersPos[this->ownID-1], this->ownID, newState, dirStr[this->ownID-1]);
	this->state = newState;
	this->turn++;
}
string Player::getEmergencyDecision()
{
	this->emergency = false;
	return this->emergencyStep;
}
string Player::inUnstableOrBoundCase()
{
	queue<pair<pair<int, int>, int> > q;
	vector<vector<int> > dist = vector<vector<int> >(20, vector<int>(30, oo));
	for(int i = 0; i < this->numberOfPlayer; i++) if(this->ownID != i+1 && this->state.playersPos[i] == make_pair(-1, -1))
	{
		q.push(make_pair(this->state.playersPos[i], i+1));
		dist[this->state.playersPos[i].first][this->state.playersPos[i].second] = 0;
	}
	while(!q.empty())
	{
		pair<pair<int, int>, int> u = q.front(); q.pop();
		if( dist[u.first.first][u.first.second] == 2 )
			continue;
		vector<pair<int, int> > steps = helper.getNextSteps(u.first, u.second, this->state, "");
		for(int i = 0; i < 4; i++) 
			if(steps[i] != make_pair(-1, -1) && dist[steps[i].first][steps[i].second] == oo)
			{
				dist[steps[i].first][steps[i].second] = dist[u.first.first][u.first.second] + 1;
				q.push(make_pair(steps[i],u.second));
			}
	}
	// get my time
	int myTime = helper.getMyTime(this->ownID, this->state, dist);

	for(int ii = 0; ii < 4; ii++) if( this->nextSteps[ii] != make_pair(-1, -1))
	{
		pair<int, int> nextPos = this->nextSteps[ii];
		if( !helper.inStable(nextPos, this->ownID, this->state))
		{
			vector<pair<int, int> > steps = helper.getNextSteps(nextPos, this->ownID, this->state, DIRECT_NAME[ii]);
			for(int j2 = 0; j2 < 4; j2++) if( steps[j2] != make_pair(-1, -1) && helper.inStable(steps[j2], this->ownID, this->state))
			{
				if(myTime <= 2 )
				{
					this->emergency = true;
					this->emergencyStep = DIRECT_NAME[j2];
				}
				return DIRECT_NAME[j2];
			}
		}
	}
	return "ERROR";
}
string Player::inStableCase()
{
	vector<pair<int, int> > bounds;
	string decision = "";
	int minDist = oo;
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 30; j++) 
		{
			if( helper.inStable(make_pair(i, j), this->ownID, this->state) && helper.isBound(make_pair(i, j), this->ownID, this->state)) 
			{
				int oldDist = abs(this->state.playersPos[this->ownID-1].first - i) + abs(this->state.playersPos[this->ownID-1].second-j);
				for(int kk; kk < 4; kk++) if(this->nextSteps[kk] != make_pair(-1, -1))
				{
					pair<int, int> nextStep = this->nextSteps[kk];
					int newDist = abs(nextStep.first-i) + abs(nextStep.second-j);
					if( oldDist > newDist && minDist > newDist )
					{
						decision = DIRECT_NAME[kk];
					}
				}
			}
		}
	}
	return decision;
}
string Player::getDecision()
{
	// in bound or unstable
	if( helper.inUnstable(this->state.playersPos[this->ownID-1], this->ownID, this->state) || 
		helper.isBound(this->state.playersPos[this->ownID-1], this->ownID, this->state))
	{
		return this->inUnstableOrBoundCase();
	}
	// in stable
	return this->inStableCase();
}
vector<pair<int, int> > Helper::getNextSteps(const pair<int, int>& curPos, const int& playerId, const State& state, const string& preDirStr)
{
	vector<pair<int, int> > nextSteps;
	for(int j = 0; j < 4; j++)
	{
		pair<int, int> nextPos = helper.nextPos(curPos, DIRECT_XY[j]);
		if( 
			helper.inBoard(nextPos) && 
			!helper.opositeDirection(preDirStr, DIRECT_NAME[j]) &&
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
	vector<pair<int, int> > steps = helper.getNextSteps(curPos, playerId, state, "");
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
bool Helper::opositeDirection(const string& direct_0, const string& direct_1)
{
	return (direct_0 == "RIGHT" && direct_1 == "LEFT")||
	       (direct_0 == "LEFT" && direct_1 == "RIGHT")||
	       (direct_0 == "UP" && direct_1 == "DOWN")||
	       (direct_0 == "DOWN" && direct_1 == "UP");
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