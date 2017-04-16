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
using namespace std;
const string DIRECT_NAME[4] = {"RIGHT", "DOWN", "LEFT", "UP"};
const pair<int, int> DIRECT_XY[4] = {make_pair(0,1), make_pair(1, 0), make_pair(0,-1), make_pair(-1,0)};
const int oo = 9;
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
	queue<string> emergencyStep;
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
	// freopen("in1.txt", "r", stdin);
	// freopen("output.txt", "w", stdout);
	int numberOfPlayer, playerId, turn = 0;
	string line;
	cin >> numberOfPlayer;
	cin >> playerId;
	vector<string> table(20);
	vector<pair<int, int> > playersPos(numberOfPlayer);
	Player myBot(numberOfPlayer, playerId);
	while(true) 
	// for(int loop = 0; loop < 2; loop++)
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
		// cout << "OK" << endl;
		if( !isReturned )
			cout << myBot.getDecision() << endl;
	}
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
	this->emergency = false;
	this->emergencyStep = queue<string>();
	this->nextSteps = vector<pair<int, int> >(4);
	this->dirId = vector<int>(_numberOfPlayer, -1);
	this->dirStr = vector<string>(_numberOfPlayer, "");
}
void Player::updateState(const State& newState)
{
	// newState.printState();
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
	// cout <<"[PLAYER::updateState] OK"<<endl;
	// cout <<"[PlAYER::updateState] Player " << this->ownID <<" is in "<<this->state.playersPos[this->ownID-1].first <<" "<<
	this->state = newState;
	this->nextSteps = helper.getNextSteps(newState.playersPos[this->ownID-1], this->ownID, newState, dirStr[this->ownID-1]);
	this->turn++;
}
string Player::getEmergencyDecision()
{
	string ret = this->emergencyStep.front(); this->emergencyStep.pop();
	this->emergency = !this->emergencyStep.empty();
	return ret;
}
string Player::inUnstableOrBoundCase()
{
	// cout <<"[Player::inUnstableOrBoundCase]"<<endl;
	queue<pair<pair<int, int>, pair<int, string> > > q;
	vector<vector<int> > dist = vector<vector<int> >(20, vector<int>(30, oo));
	// cout << this->numberOfPlayer << endl;
	// for(int i = 0; i < this->numberOfPlayer; i++) cout << this.state.playersPos[i].first <<" "<<this.state.playersPos[i].second << endl;
	for(int i = 0; i < this->numberOfPlayer; i++) if(this->ownID != i+1 && this->state.playersPos[i] != make_pair(-1, -1))
	{
		q.push(make_pair(this->state.playersPos[i], make_pair(i+1, this->dirStr[i])));
		dist[this->state.playersPos[i].first][this->state.playersPos[i].second] = 0;
	}
	while(!q.empty())
	{
		pair<pair<int, int>, pair<int, string> > u = q.front(); q.pop();
		if( dist[u.first.first][u.first.second] == 3 )
			continue;
		// cout <<" stay "<<u.first.first <<" "<<u.first.second <<" "<<dist[u.first.first][u.first.second] << endl;
		vector<pair<int, int> > steps = helper.getNextSteps(u.first, u.second.first, this->state, u.second.second);
		for(int i = 0; i < 4; i++) 
		{
			// cout <<" to "<<steps[i].first <<" "<<steps[i].second << endl;
			if(steps[i] != make_pair(-1, -1) && dist[steps[i].first][steps[i].second] == oo)
			{
				dist[steps[i].first][steps[i].second] = dist[u.first.first][u.first.second] + 1;
				q.push(make_pair(steps[i],make_pair(u.second.first, DIRECT_NAME[i])));
			}
		}
	}
	// for(int i = 0; i < 20; i++) {
	// 	for(int j = 0; j < 30; j++) cout << dist[i][j];
	// 	cout << endl;
	// }
	// for(int i = 0; i < 4; i++) {
		// cout << this->nextSteps[i].first <<" "<<this->nextSteps[i].second << endl;
	// }
	// get my time
	int myTime = helper.getMyTime(this->ownID, this->state, dist);
	// cout << myTime << endl;

	for(int ii = 0; ii < 4; ii++) if( this->nextSteps[ii] != make_pair(-1, -1))
	{
		pair<int, int> nextPos = this->nextSteps[ii];
		// cout << "nextPos "<<nextPos.first <<" "<<nextPos.second << endl;
		if( !helper.inStable(nextPos, this->ownID, this->state))
		{
			vector<pair<int, int> > steps = helper.getNextSteps(nextPos, this->ownID, this->state, DIRECT_NAME[ii]);
			// for(int j2 = 0; j2 < 4; j2++) 
			// {
				// cout << steps[j2].first <<" "<<steps[j2].second << endl;
			// }
			for(int j2 = 0; j2 < 4; j2++) if( steps[j2] != make_pair(-1, -1) && helper.inStable(steps[j2], this->ownID, this->state) && myTime >= 2)
			{
				if(myTime <= 2 )
				{
					// cout <<"OK emergency "<<" "<<DIRECT_NAME[j2]<<endl;
					this->emergency = true;
					this->emergencyStep.push(DIRECT_NAME[j2]);
				}
				return DIRECT_NAME[ii];
			}
			for(int j2 = 0; j2 < 4; j2++) if(steps[j2] != make_pair(-1, -1) && dist[nextPos.first][nextPos.second] >= 3 && myTime >= 3)
			{
				// cout << "next pos "<<ii<<endl;
				vector<pair<int, int> >steps2 = helper.getNextSteps(steps[j2], this->ownID, this->state, DIRECT_NAME[j2]);
				for(int j3 = 0; j3 < 4; j3++) if( steps2[j3] != make_pair(-1, -1) && helper.inStable(steps2[j3], this->ownID, this->state))
				{
					// cout <<" OK "<<endl;
					if(myTime == 3 )
					{
						this->emergency = true;
						this->emergencyStep.push(DIRECT_NAME[j2]);
						this->emergencyStep.push(DIRECT_NAME[j3]);
					}
					return DIRECT_NAME[ii];
				}
			}
		}
	}
	// cout << " OK " << endl;
	for(int i = 0; i < 4; i++) 
		if( this->nextSteps[i] != make_pair(-1, -1) && helper.inStable(this->nextSteps[i], this->ownID, this->state) )
			return DIRECT_NAME[i];
	// this case when bot can go into a cell that not own territory(not stable and not unstable) in 2 or 3 so to a stable
	return "ERROR";
}
string Player::inStableCase()
{
	// cout << "[Player::inStableCase]" << endl;
	vector<pair<int, int> > bounds;
	string decision = "";
	int minDist = oo;
	for(int i = 0; i < 20; i++)
	{
		for(int j = 0; j < 30; j++) 
		{
			if( helper.inStable(make_pair(i, j), this->ownID, this->state) && helper.isBound(make_pair(i, j), this->ownID, this->state)) 
			{
				// cout << i <<" "<<j<<endl;
				int oldDist = abs(this->state.playersPos[this->ownID-1].first - i) + abs(this->state.playersPos[this->ownID-1].second-j);
				// cout << oldDist << endl;
				for(int kk=0; kk < 4; kk++) if(this->nextSteps[kk] != make_pair(-1, -1))
				{
					pair<int, int> nextStep = this->nextSteps[kk];
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
		// cout <<"[Helper::getNextSteps] "<<j<<endl;
		// cout << DIRECT_XY[j].first <<" "<<DIRECT_XY[j].second << endl;
		// cout << curPos.first <<" "<<curPos.second<<endl;
		pair<int, int> nextPos = helper.nextPos(curPos, DIRECT_XY[j]);
		// cout << nextPos.first <<" "<<nextPos.second<<endl;
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