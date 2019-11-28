#ifndef _DATA_H_
#define _DATA_H_

#include <vector>

#define NMAX 1000

using namespace std;

typedef pair<int,int> arc;

class Data
{
public:

	const char* filePath;
	int n, v, m;
	double matG[NMAX][NMAX];
	vector<int> P, D, N, V, M_o, M_e, K, U, id;
	vector<double>L, X, Y, d, e, l;
	vector<vector<int> > C, q;
	vector<arc> Amp, Apd, Adm, A;
	vector<vector<arc> > Graph;
	vector<vector<double> > t, c;
	vector<vector<bool> > A_;
	vector<vector<vector<arc> > > delta_minus, delta_plus;
	vector<vector<vector<int> > > K_aux;

	Data(const char* filePath);
	Data(int problem, const char* filePath);

	bool check_RT(int n0, int n1, int n2, int n3);

	void print();

};

#endif
