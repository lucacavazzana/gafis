#include <Optimizing.h>
#include <sstream>

using namespace std;

string Optimizing::getScript(int num){
	stringstream tmp;
	int i;
	string numTxt, script = "v";

	//script += " v0";
	//script += " r0 p0 y0";

	for (i=1; i<num; i++){
		tmp << i;
		numTxt = tmp.str();
		script +=  + " v" + numTxt + " r" + numTxt + " p" + numTxt + " y" + numTxt + " a" + numTxt + " b" + numTxt + " c" + numTxt;
		tmp.str("");
	}

	return script;
}
