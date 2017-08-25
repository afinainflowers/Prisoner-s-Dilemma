// Dilemma_aquarium.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
using namespace std;

const bool BETRAY = 0;
const bool COOP = 1;
const int JOSS_BETR = 10; // Joss betrays 1 of this const times
const int FORG_RATE = 30;
const int AVG_MOVES = 500;
const int MOVES_PM = 20; // plus-minus this
const int BOTH_COOP_SUM = 3;
const int BOTH_BETR_SUM = 1;
const int BETRAYER_SUM = 5;
const int BETRAYED_SUM = 0;
const int CYCLES_GAME = 5;

class Strategy
{
	protected:
		vector<bool> moves;
		vector<bool> movesOpp; // 1 is coop, 0 is betrayal
		int points;

	public:
		Strategy():points(0){};
		void get_move(bool move)
		{
			movesOpp.push_back(move);// 1 is coop, 0 is betrayal
		}
		virtual bool do_move(){throw 0;};// will be owerwritten anyway
		virtual string give_name(){throw 0;};
		virtual Strategy* make_same_type(){throw 0;};
		int get_points()
		{
			return this->points;
		}
		int set_points( int set)
		{
			this->points = set;
			return this->points;
		}
		void reset_moves()
		{
			movesOpp.clear();
			moves.clear();
		}
		void inc_points_by_last_turn()
		{
			if ( (moves.empty())||(movesOpp.empty()))
				return;
			if ( COOP == moves.back())
			{
				if ( COOP == movesOpp.back())
					points += BOTH_COOP_SUM;
				else 
					points += BETRAYED_SUM;
			}
			else
			{
				if ( COOP == movesOpp.back())
					points += BETRAYER_SUM;
				else 
					points += BOTH_BETR_SUM;
			}
		}
};

class Grudger: public Strategy
{
	public:
		Grudger(): Strategy(){};
		string give_name(){return "Grudger";};
		Strategy* Grudger::make_same_type() { return new Grudger(); }
		bool do_move()
		{
			// coop until betrayal, then betray to an end
			if (this->movesOpp.empty())
			{
				moves.push_back(COOP);
				return COOP;
			}
			auto rezult = find(movesOpp.begin(), movesOpp.end(), BETRAY);
			if (rezult == movesOpp.end())
				{
					moves.push_back(COOP);
					return COOP;
				}
			else 
				{
					moves.push_back(BETRAY);
					return BETRAY;
				}
		}
};

class TitForTat: public Strategy
{
	public:
		TitForTat(): Strategy(){};
		string give_name(){return "Tit for tat";};
		Strategy* TitForTat::make_same_type() { return new TitForTat(); }
		bool do_move()
		{
			if (this->movesOpp.empty())
			{
				moves.push_back(COOP);
				return COOP;
			}
			moves.push_back(movesOpp.back());
			return movesOpp.back();
		}
};

class TitForTwoTat: public Strategy
{
	public:
		TitForTwoTat(): Strategy(){};
		string give_name(){return "Tit for two tat";};
		Strategy* TitForTwoTat::make_same_type() { return new TitForTwoTat(); }
		bool do_move()
		{
			// first coop. betray only if the opponent betrayed 2 times in a row
			/*if (this->movesOpp.empty())
			{
				return COOP;
			}*/
			if  (movesOpp.size() <= 1)
			{
				moves.push_back(COOP);
				return COOP;
			}
			if ( ( *(movesOpp.end()-1) == BETRAY)&&( *(movesOpp.end()-2) == BETRAY))
			{
				moves.push_back(BETRAY);
				return BETRAY;
			}
			moves.push_back(COOP);
			return COOP;
		}
};

class Joss: public Strategy
{
	public:
		Joss(): Strategy(){};
		string give_name(){return "Joss";};
		Strategy* Joss::make_same_type() { return new Joss(); }
		bool do_move()
		{
			// TitForTat, but sometimes betray for no reason
			srand(time(0));
			int ifbetr = rand() % JOSS_BETR;
			if (ifbetr == 0)
			{
				moves.push_back(BETRAY);
				return BETRAY;
			}
			if (this->movesOpp.empty())
			{
				moves.push_back(COOP);
				return COOP;
			}
			moves.push_back(movesOpp.back());
			return movesOpp.back();
		}
};

class ForgivingTitForTat: public Strategy
{
	public:
		ForgivingTitForTat(): Strategy(){};
		string give_name(){return " Forgiving tit for tat";};
		Strategy* ForgivingTitForTat::make_same_type() { return new ForgivingTitForTat(); }
		bool do_move()
		{
			// TitForTat, but sometimes betray for no reason
			srand(time(0));
			int ifforg = rand() % FORG_RATE;
			if (ifforg == 0)
			{
				moves.push_back(COOP);
				return COOP;
			}
			if (this->movesOpp.empty())
			{
				moves.push_back(COOP);
				return COOP;
			}
			moves.push_back(movesOpp.back());
			return movesOpp.back();
		}
};

class Randomer: public Strategy
{
	public:
		Randomer(): Strategy(){};
		string give_name(){return "Randomer";};
		Strategy* Randomer::make_same_type() { return new Randomer(); }
		bool do_move()
		{
			srand(time(0));
			bool rez = bool(rand() % 2);
			moves.push_back(rez);
			return (rez);
		}
};

class Betrayer: public Strategy
{
	public:
		Betrayer(): Strategy(){};
		string give_name(){return "Betrayer";};
		Strategy* Betrayer::make_same_type() { return new Betrayer(); }
		bool do_move()
		{
			moves.push_back(BETRAY);
			return BETRAY;
		}
};

class Pushover: public Strategy
{
	public:
		Pushover(): Strategy(){};
		string give_name(){return "Pushover";};
		Strategy* Pushover::make_same_type() { return new Pushover(); }
		bool do_move()
		{
			moves.push_back(COOP);
			return COOP;
		}
};

void onegame( Strategy* a, Strategy* b, int turns_quant)
{
	a->reset_moves();
	b->reset_moves();
	for (int i = 1; i <= turns_quant; i++)
	{
		bool a_turn = a->do_move();
		bool b_turn = b->do_move();
		a->get_move(b_turn);
		b->get_move(a_turn);
		a->inc_points_by_last_turn();
		b->inc_points_by_last_turn();
	}
}

vector<int> cycle_game( vector<Strategy*> agents)
{
	for (unsigned int i = 0; i<=agents.size() - 1; i++)
	{
		agents[i]->reset_moves();
		agents[i]->set_points(0);
	}
	srand(time(0));
	int moves = AVG_MOVES + rand()%(MOVES_PM *2) - MOVES_PM;
	for (unsigned int i = 1; i<=agents.size() - 1; i++) 
	{
		for (unsigned int j = 0; j< i; j++)
		{
			onegame(agents[i], agents[j], moves);
		}
	}
	vector<int> rez;
	for (unsigned int i = 0; i<=agents.size() - 1; i++)
	{
		int rezone = agents[i]->get_points();
		rez.push_back(rezone);
	}
	return rez;
}

int numless (vector<Strategy*> vec)
{
	Strategy* temp = new Strategy();
	temp->set_points(INT_MAX);
	int tempi = 0;
	for (unsigned i = 0; i<vec.size(); i++)
	{
		if (vec[i]->get_points() < temp->get_points())
		{
			temp = vec[i];
			tempi = i;
		}
	}
	return tempi;
}

int numbig (vector<Strategy*> vec)
{
	Strategy* temp = new Strategy();
	int tempi = 0;
	for (unsigned i = 0; i<vec.size(); i++)
	{
		if (vec[i]->get_points() > temp->get_points())
		{
			temp = vec[i];
			tempi = i;
		}
	}
	return tempi;
}

int _tmain(int argc, _TCHAR* argv[])
{
	vector <Strategy*> agents;
	for (int i = 1; i<= 3; i++)
	{
		agents.push_back(new Grudger());
		agents.push_back(new TitForTat());
		agents.push_back(new TitForTwoTat());
		agents.push_back(new ForgivingTitForTat());
		agents.push_back(new Joss());
		agents.push_back(new Betrayer());
		agents.push_back(new Pushover());
		agents.push_back(new Randomer());
	}
	for (int j = 1; j<= CYCLES_GAME; j++)
	{
		cout<<"The circle begins \n";
		vector<int> rez = cycle_game(agents);
		for (int i = 0; i < rez.size(); i++)
		{
			cout<<rez[i]<<" ";
			string name = agents[i]->give_name();
			cout<<name.c_str()<<" ";
			//- cout<<agents_names[i].c_str();
			cout<<"\n";
		}
		cout<<"End of this circle, next to begin.\n";
		vector<Strategy*>::iterator temp = agents.begin()+ numless(agents) - 1;
		cout<<"One of "<<agents[numless(agents)]->give_name().c_str()<<" goes out with "<<agents[numless(agents)]->get_points()<<" points\n";
		agents.erase(temp);
		cout<<"A copy of "<<agents[numbig(agents)]->give_name().c_str()<<" which got "<<agents[numbig(agents)]->get_points()<<" points, goes in\n";
		agents.push_back(agents[numbig(agents)]->make_same_type());
	}
	return 0;
}

