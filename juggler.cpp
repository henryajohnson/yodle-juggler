//Juggler coded by Henry Johnson
//April 27th, 2015 

#include <iostream>
#include <vector>
#include <queue>

using namespace std;
struct Juggler{
	int number;
	int pref_index;
	vector<int> preferences;
	vector<int> skills;
	vector<int> skillset; 
};

class JuggleBrackets{
	private:
		vector< vector<Juggler *> > competitionRounds;
		vector< vector<int> > rounds;
		vector<Juggler *> overflow;
		int maxjuggler, round_cap;
		int dot_product(vector<int> vec1, vector<int> vec2){
			return ( vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2] );
		}
	public:
		JuggleBrackets(){}; 
		~JuggleBrackets();

		void prepareComp();
		//Resize competition rounds

		void addRound(vector<int> values);
		//Takes a new set of aspects for a round and adds them to the JuggleBrackets rounds

		void insertJuggler(Juggler *newjuggler, int round_num);
		//Inserts a juggler into a given round number. This function also sorts the jugglers on insert
		//based on how suitable it is for the round

		void addJuggler(vector<int> preferences, vector<int> skillset, int number);
		//Adds a new juggler to the juggle brackets and calls insertJuggler to assign them to their top preference round

		void shrink_round(int round_num);
		//Goes through a specific round number, finds the least suitable player that can be removed,
		//reassigns them to a different round and, if that target round needs to be resized, calls itself
		//on the target round. If the least suitable player is not in the top number for any of their preferences,
		//they get put in the "Overflow" list to be added later

		void evenRounds();
		//Iterates through all rounds and shrinks ones that are larger than allowed
		//After the initial sorting, it goes through all the circuits that have open spots and assigns the 
		//best fit from the overflow list for that circuit (we want to see the best fit juggler for the circuit).

		void printJugglerVec();
		//Prints all the rounds, jugglers assigned to them and their preferences
};

JuggleBrackets::~JuggleBrackets(){
	for(int i=0; i<competitionRounds.size(); i++){
		while(competitionRounds.at(i).size() > 0){
			Juggler *thisjuggler = competitionRounds.at(i).back();
			competitionRounds.at(i).pop_back();
			delete thisjuggler;
		}
	}
};

void JuggleBrackets::prepareComp(){
	competitionRounds.resize(rounds.size());
	return;
}

void JuggleBrackets::addRound(vector<int> values){
	rounds.push_back(values);
	return;
}

void JuggleBrackets::insertJuggler(Juggler *newjuggler, int round_num){
	if(competitionRounds.at(round_num).empty()){
		competitionRounds.at(round_num).push_back(newjuggler);
	}else{
		vector<Juggler *>::iterator it = competitionRounds.at(round_num).begin();	
		while((it !=competitionRounds.at(round_num).end()) && 
			(newjuggler->skills.at(newjuggler->pref_index) < (*it)->skills.at((*it)->pref_index))){
			it++;
		}
		competitionRounds.at(round_num).insert(it, newjuggler);
	}
	return;
}

void JuggleBrackets::addJuggler(vector<int> preferences, vector<int> skillset, int number){
	Juggler *newjuggler = new Juggler;
	newjuggler->preferences = preferences;
	for(int i=0; i<preferences.size(); i++){
		newjuggler->skills.push_back(dot_product(skillset, rounds.at(preferences.at(i))));
	}
	newjuggler->pref_index = 0;
	newjuggler->skillset = skillset;
	newjuggler->number = number;
	insertJuggler(newjuggler, preferences.front() );
	maxjuggler = number;
	return;
}

void JuggleBrackets::shrink_round(int round_num){
	vector<Juggler *>::iterator victim = competitionRounds.at(round_num).end();
	victim --;
	if( ((*victim)->pref_index+1) < (*victim)->preferences.size() ){
		(*victim)->pref_index ++;
		Juggler *newjuggler = (*victim);
		insertJuggler(newjuggler, newjuggler->preferences.at(newjuggler->pref_index));
		competitionRounds.at(round_num).erase(victim);
		if(competitionRounds.at(newjuggler->preferences.at(newjuggler->pref_index)).size() > round_cap){
			shrink_round(newjuggler->preferences.at(newjuggler->pref_index));
		}
		return;
	}else{
		Juggler *newjuggler = (*victim);
		overflow.push_back(newjuggler);
		competitionRounds.at(round_num).erase(victim);
	}	
	return;
}

void JuggleBrackets::evenRounds(){
	round_cap = (maxjuggler+1)/(rounds.size());
	for(int i=0; i<rounds.size(); i++){
		while(competitionRounds.at(i).size() > round_cap){
			shrink_round(i);
		}
	}
	for(int i=0; i<rounds.size(); i++){
		while(competitionRounds.at(i).size() < round_cap){
			int bestfit = 0, max_score = 0;
			for(int j=0; j<overflow.size(); j++){
				if(dot_product(overflow.at(j)->skillset, rounds.at(i)) > max_score){
					max_score = dot_product(overflow.at(j)->skillset, rounds.at(i));
					bestfit = j;
				}
			}
			insertJuggler(overflow.at(bestfit), i);
			overflow.erase(overflow.begin()+bestfit);

		}
	}
	return;
}

void JuggleBrackets::printJugglerVec(){
	for(int i=competitionRounds.size()-1; i>=0 ; i--){
		cout << "C" << i;
		for(int j=0; j<competitionRounds.at(i).size(); j++){
			if(j > 0){ cout << ",";}
			cout << " J" << competitionRounds.at(i).at(j)->number;
			for(int k=0; k<competitionRounds.at(i).at(j)->preferences.size(); k++){
				
				cout << " C" << competitionRounds.at(i).at(j)->preferences.at(k) << ":"
					<< competitionRounds.at(i).at(j)->skills.at(k);
			}
		}
		cout << endl;
	}
	return;
}


int main(int argc, char * argv){
	char type, extrachar;
	int intval;

	JuggleBrackets *jugglecomp = new JuggleBrackets;

	cin >> type;
	while(type == 'C'){
		cin >> extrachar >> intval;
		vector<int> values;
		for(int i=0; i<3; i++){
			cin >> extrachar >> extrachar >> intval;
			values.push_back(intval);
		}
		jugglecomp->addRound(values);
		cin >> type;
	}

	int jugglernum, numpreferences = -1;
	jugglecomp->prepareComp();

	while(type == 'J'){
		cin >> extrachar >> jugglernum;
		vector<int> skillset;
		for(int i=0; i<3; i++){
			cin >> extrachar >> extrachar >> intval;
			skillset.push_back(intval);
		}
		
		vector<int> preferences;
		cin >> type;
		int prefs = numpreferences;
		while(type == 'C' && (prefs != 0)){
			cin >> intval >> type;
			preferences.push_back(intval);
			prefs --;
			if(type==','){
				cin >> type;
			}
		}
		numpreferences = preferences.size();
		jugglecomp->addJuggler(preferences, skillset, jugglernum);
	}

	jugglecomp->evenRounds();
	jugglecomp->printJugglerVec();

	delete jugglecomp;

	return 0;
}