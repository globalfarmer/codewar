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
	bool oppositeDirection(const string&, const string&);
	pair<int, int> nextPos(const pair<int, int>&, const pair<int, int>&);
	int getMyTime(const int&, const State& , const vector<vector<int> >&);
	vector<pair<int, int> > getNextSteps(const pair<int, int>&, const int&, const State&, const string&);
	int ccw(const pair<int, int>&, const pair<int, int> &);
	bool niceDecision(const pair<int, int>&, const vector<pair<int, int> >&);
	pair<int, int> getVector(const pair<int, int>&, const pair<int, int>&);

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
	vector<vector<int> > g;
	vector<vector<int> > sum;
	int lastStepId;
	int curUnstable;
	string dirId;
	string dirStr;
	vector<pair<int, int> > tracePos;
	pair<int, int> lastStable;
	Player();
	Player(const int&, const int&);
	string getEmergencyDecision();
	void updateDistance();
	void updateSum();
	void updateG();
	void updateState(const State&);
	pair<int, int> getCurPos();
};
class Decision2Player
{
public:
	queue<string> emergencySteps;
	bool emergency;
	Decision2Player();
	string getDecisionAsFirstPlayer(Player&, Player&);
	string getEmergencyDecision();
	bool canKillEnemy(Player, Player);
	string inUnstableOrBoundCase(Player, Player);
	string inStableCase(Player, Player);
} decision2Player;
int main()
{
// vector<pair<int, int> > ps;
// ps.push_back(make_pair(0,18));
// ps.push_back(make_pair(10, 18));
// ps.push_back(make_pair(9, 18));
// // ps.push_back(make_pair(9, 19));
// cout << helper.niceDecision(make_pair(9, 19), ps) << endl;
// return 0;
// time_t start,end;
// time (&start);

	// freopen("input.txt", "r", stdin);
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
	// for(int loop = 0; loop < 1; loop++)
	{
		bool isReturned = false;
		for(int i = 0; i < 20; i++)
		{
			cin >> line;
			if( !isReturned && decision2Player.emergency)
			{
				isReturned = true;
				cout << decision2Player.getEmergencyDecision() << endl;
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
		enemy.updateState(State(table, playersPos));
		// cout << "OK" << endl;
		if( !isReturned)
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
	this->dirId = -1;
	this->dirStr = "";
	this->tracePos = vector<pair<int, int> >();
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
		vector<pair<int, int> > steps = (u == curPos ? this->nextSteps : helper.getNextSteps(u, this->ownID, this->state, ""));
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
void Player::updateG()
{
	queue<pair<int, int> > q;
	this->g = vector<vector<int> >(20, vector<int>(30, oo));
	for(int i = 0; i < 20; i++) 
	{
		for(int j = 0; j < 30; j++) if(helper.inStable(make_pair(i, j), this->ownID, this->state))
		{
			q.push(make_pair(i,j));
			this->g[i][j] = 0;
		}
	}
	while(!q.empty())
	{
		pair<int, int> u = q.front(); q.pop();
		vector<pair<int, int> > steps = helper.getNextSteps(u, this->ownID, this->state, "");
		for(int i = 0; i < 4; i++) if( steps[i] != make_pair(-1, -1) && this->g[steps[i].first][steps[i].second] == oo)
		{
			this->g[steps[i].first][steps[i].second] = this->g[u.first][u.second] + 1;
			q.push(steps[i]);
		}
	}
}
void Player::updateState(const State& newState)
{
	// newState.printState();
	if( this->turn > 0 )
	{
		if(newState.playersPos[this->ownID-1] != make_pair(-1, -1))
		{
			pair<int, int> detal = make_pair
									(
										newState.playersPos[this->ownID-1].first -this->state.playersPos[this->ownID-1].first, 
										newState.playersPos[this->ownID-1].second-this->state.playersPos[this->ownID-1].second
									);
			for(int j = 0; j < 4; j++) if( DIRECT_XY[j] == detal )
			{
				dirId = j;
				dirStr = DIRECT_NAME[j];
				// cout <<"Dir str"<< dirStr<<endl;
			}
		}
	}
	// cout <<"[PLAYER::updateState] OK"<<endl;
	// cout <<"[PlAYER::updateState] Player " << this->ownID <<" is in "<<this->state.playersPos[this->ownID-1].first <<" "<<
	this->state = newState;
	this->turn++;
	if(helper.inStable(this->getCurPos(), this->ownID, this->state)) 
	{
		this->tracePos = vector<pair<int, int> >();
		this->nextSteps = helper.getNextSteps(newState.playersPos[this->ownID-1], this->ownID, newState, "");
		this->curUnstable = 0;
	}
	else
	{
		this->nextSteps = helper.getNextSteps(newState.playersPos[this->ownID-1], this->ownID, newState, dirStr);
	}
	// cout <<"[updateState] nextSteps"<<endl;
	// for(int i = 0; i < 4; i++)
	// 	cout << this->nextSteps[i].first<<" "<<this->nextSteps[i].second << endl;
	while(this->tracePos.size() >= 3 ) {
		pair<int, int> v1 = helper.getVector(*(this->tracePos.begin()), *(this->tracePos.end()-2));
		pair<int, int> v2 = helper.getVector(*(this->tracePos.begin()), *(this->tracePos.end()-1));
		pair<int, int> v3 = helper.getVector(*(this->tracePos.begin()), this->getCurPos());
		int product_1 = helper.ccw(v1, v2);
		int product_2 = helper.ccw(v2, v3);
		if( product_1 * product_2 > 0 || (product_1 == 0 && product_2 == 0) )
			break;
		this->tracePos.erase(this->tracePos.end()-2);
	}
	this->tracePos.push_back(newState.playersPos[this->ownID-1]);
	this->updateDistance();
	this->updateG();
	this->updateSum();
	// cout <<"[updateState] Turn "<<this->turn<<endl;
	// cout <<"Player "<<this->ownID<<endl;
	// cout <<"trace Pos"<<endl;
	// for(int i = 0; i < this->tracePos.size(); i++)
	// 	cout << this->tracePos[i].first <<" "<<this->tracePos[i].second << endl;
	// cout << "lastStable"<<endl;
	// cout << this->lastStable.first <<" "<<this->lastStable.second << endl;
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
	// cout <<"[inUnstableOrBoundCase] "<< myBot.turn <<" OK "<<endl;
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
				vector<pair<int, int> > steps = helper.getNextSteps(nextPos, myBot.ownID, myBot.state, "");
				for(int ii = 0; ii < 4; ii++) if( steps[ii] != make_pair(-1, -1) && steps[ii] != curPos && helper.inStable(steps[ii], myBot.ownID, myBot.state))
				{
					if(enemy.min_d[nextPos.first][nextPos.second] == 3)
					{
						decision2Player.emergency = true;
						decision2Player.emergencySteps.push(DIRECT_NAME[ii]);
					}
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
				enemy.min_d[nextPos.first][nextPos.second] >= 4 && 
				enemy.min_d[curPos.first][curPos.second] >= 4
				)
			{
				// cout << "OK 3 steps"<<" "<<nextPos.first<<" "<<nextPos.second<<endl;
				vector<pair<int, int> > steps = helper.getNextSteps(nextPos, myBot.ownID, myBot.state, "");
				for(int ii = 0; ii < 4; ii++) if( steps[ii] != make_pair(-1, -1) && !helper.inStable(steps[ii], myBot.ownID, myBot.state))
				{
					vector<pair<int, int> > steps2 = helper.getNextSteps(steps[ii], myBot.ownID, myBot.state, "");
					for(int i2 = 0; i2 < 4; i2++)  if( steps2[i2] != make_pair(-1, -1) && helper.inStable(steps2[i2], myBot.ownID, myBot.state))
					{
						// cout <<"3 steps"<<" "<<DIRECT_NAME[i]<<" "<<enemy.min_d[nextPos.first][nextPos.second]<<endl;
						if( enemy.min_d[nextPos.first][nextPos.second] == 4) 
						{
							decision2Player.emergency = true;
							// cout << DIRECT_NAME[i] <<" "<<DIRECT_NAME[ii]<<" "<<DIRECT_NAME[i2]<<endl;
							decision2Player.emergencySteps.push(DIRECT_NAME[ii]);
							decision2Player.emergencySteps.push(DIRECT_NAME[i2]);
						}
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
		int tmpMyTime = myTime / 2;
		if( tmpMyTime == 1 && myTime == 2) tmpMyTime = 0;
		// if( myTime <= 3 ) 
		// {
		// 	for(int i = 0; i < 4; i++) if( myBot.nextSteps[i] != make_pair(-1, -1)) 
		// 	{
		// 		vector<pair<int, int> > steps = helper.getNextSteps(myBot.nextSteps[i], myBot.ownID, myBot.state, "");
		// 		for(int i2 = 0; i2 < 4; i2++) if( steps[i2] != make_pair(-1, -1))
		// 	}
		// }
		// cout <<"[inUnstableOrBound] OK "<<myTime<<" "<<tmpMyTime<<endl;
		int max_g = -oo, nice_max_g = -oo;
		string decision = "";
		string niceDecision = "";
		for(int i = 0; i < 4; i++) 
			if( myBot.nextSteps[i] != make_pair(-1, -1) && 
				myBot.curUnstable > 10 && 
				helper.inStable(myBot.nextSteps[i], myBot.ownID, myBot.state))
		{
			return DIRECT_NAME[i];
		}
		for(int i = 0; i < 4; i++) if( myBot.nextSteps[i] != make_pair(-1, -1)) 
		{
			pair<int, int> nextPos = myBot.nextSteps[i];
			if(myBot.g[nextPos.first][nextPos.second] <= tmpMyTime) 
			{
				if( max_g < myBot.g[nextPos.first][nextPos.second] )
				{
					max_g = myBot.g[nextPos.first][nextPos.second];
					decision = DIRECT_NAME[i];	
				}
				if( nice_max_g < myBot.g[nextPos.first][nextPos.second] && helper.niceDecision(nextPos, myBot.tracePos))
				{
					nice_max_g = myBot.g[nextPos.first][nextPos.second];
					niceDecision = DIRECT_NAME[i];
				}
			}
		}
		// cout<<"[inUnstableOrBound] " << niceDecision <<" "<<decision<<" "<<myTime<<endl;

		if( niceDecision != "")
			return niceDecision;
		else if( decision != "")
			return decision;
		for(int i = 0; i < 4; i++) if( myBot.nextSteps[i] != make_pair(-1, -1)) 
		{
			pair<int, int> nextPos = myBot.nextSteps[i];
			if(myBot.g[nextPos.first][nextPos.second] <= myTime) 
			{
				if( max_g < myBot.g[nextPos.first][nextPos.second] )
				{
					max_g = myBot.g[nextPos.first][nextPos.second];
					decision = DIRECT_NAME[i];	
				}
				if( nice_max_g < myBot.g[nextPos.first][nextPos.second] && helper.niceDecision(nextPos, myBot.tracePos))
				{
					nice_max_g = myBot.g[nextPos.first][nextPos.second];
					niceDecision = DIRECT_NAME[i];
				}
			}
		}
		// cout<<"[inUnstableOrBound] " << niceDecision <<" "<<decision<<" "<<myTime<<endl;
		if( niceDecision != "")
			return niceDecision;
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
	int enemyTime = helper.getMyTime(enemy.ownID, enemy.state, myBot.min_d);
	int fastestTime = oo;
	for(int i = 0; i < 4; i++) if( enemy.nextSteps[i] != make_pair(-1, -1) ) {
		pair<int, int> nextPos = enemy.nextSteps[i];
		fastestTime = min(fastestTime, enemy.g[nextPos.first][nextPos.second]+1);
	}
	if( enemyTime < fastestTime ) 
	{
		this->emergency = true;
	}
	return false;
}
string Decision2Player::getDecisionAsFirstPlayer(Player& myBot, Player& enemy)
{
	// in bound or unstable
	// cout <<"[getDecisionAsFirstPlayer] Turn "<<myBot.turn<<endl;
	string ret;
	pair<int, int> curPos = myBot.getCurPos();
	if( decision2Player.canKillEnemy(myBot, enemy) )
	{
		ret = decision2Player.getEmergencyDecision();
	}
	else if( helper.inUnstable(curPos, myBot.ownID, myBot.state) ||
		     helper.isBound(curPos, myBot.ownID, myBot.state))
	{
		ret = decision2Player.inUnstableOrBoundCase(myBot, enemy);
		myBot.curUnstable++;
	}
	// in stable
	else
		ret = decision2Player.inStableCase(myBot, enemy);
	return ret;
}
vector<pair<int, int> > Helper::getNextSteps(const pair<int, int>& curPos, const int& playerId, const State& state, const string& dir)
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
			!helper.inUnstable(nextPos, playerId, state) &&
			!helper.oppositeDirection(dir, DIRECT_NAME[j])
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
bool Helper::oppositeDirection(const string& dir_1, const string& dir_2)
{
	return (dir_1 == "DOWN" && dir_2 == "UP") ||
			(dir_1 == "UP" && dir_2 == "DOWN") ||
			(dir_1 == "RIGHT" && dir_2 == "LEFT") ||
			(dir_1 == "LEFT" && dir_2 == "RIGHT");
}
int Helper::ccw(const pair<int, int>& p1, const pair<int, int>& p2)
{
	return p1.first * p2.second - p1.second * p2.first;
}
bool Helper::niceDecision(const pair<int, int>& pos, const vector<pair<int, int> >& tracePos)
{
	// cout << tracePos.size() << endl;
	if(tracePos.size() < 3)
		return true;
	// cout <<" "<<(*tracePos.begin()).first<<" "<<(*tracePos.begin()).second<<endl;
	pair<int, int> p1 = *(tracePos.end()-2);
	// cout << p1.first <<" - "<<p1.second<<endl;
	pair<int, int> p2 = *(tracePos.end()-1);
	// cout << p2.first <<" - "<<p2.second<<endl;
	pair<int, int> v1 = helper.getVector(*(tracePos.begin()), *(tracePos.end()-2));
	pair<int, int> v2 = helper.getVector(*(tracePos.begin()), *(tracePos.end()-1));
	pair<int, int> v3 = helper.getVector(*(tracePos.begin()), pos);
	// cout <<v1.first<<" v1 "<<v1.second<<endl;
	// cout <<v2.first<<" v2 "<<v2.second<<endl;
	// cout <<v3.first<<" v3 "<<v3.second<<endl;
	int product_1 = helper.ccw(v1, v2);
	int product_2 = helper.ccw(v2, v3);
	// cout <<"[niceDecision] "<<pos.first <<" "<<pos.second<<" "<< helper.ccw(v1, v2) <<" "<<helper.ccw(v2, v3) << endl;
	return (product_1 * product_2 > 0 || (product_1 == 0 && product_2==0));
}
pair<int, int> Helper::getVector(const pair<int, int>& startPoint, const pair<int, int>& endPoint)
{
	return make_pair(endPoint.first - startPoint.first, endPoint.second - startPoint.second);
}