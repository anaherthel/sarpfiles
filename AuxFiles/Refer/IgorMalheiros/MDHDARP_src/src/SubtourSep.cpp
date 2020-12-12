#include <float.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "SubtourSep.h"
#include <algorithm>

using namespace std;

vector<vector<int>> SubtourSep::separate(std::vector<std::vector<double>> &solX, int n)
{
	int i, j, k, m, numCutsMB;

	int ndepth;
	int numCols;
	double eps = 0.00000001;

	//Max-Back variables
	vector<double> b;
	vector<int> Sinit;
	vector<int> S;
	vector<int> Smin;
	double cutmin;
	double cutval;
	double max;
	int vertex;
	//-------------------

	//Min-Cut variables
	vector<vector<double>> arcs;
	vector<double> aux;
	vector<vector<int>> nodes;
	vector<int> nAux;
	vector<int> cutSet;
	vector<vector<int>> poolCutSet;

	int a;
	int flag;
	double l, cutphase, cut;
	//-------------------

	std::vector<bool> isSingleton(n, true);
	//preenche isSingleton
	for (int ii = 0; ii < n; ii++)
	{
		for (int jj = ii + 1; jj < n; jj++)
		{
			if (solX[ii][jj] > 0)
			{
				isSingleton[ii] = isSingleton[jj] = false;
			}
		}
	}

	//determinas os ids dos vertices nao singletons
	int n_vertices = 0;
	std::vector<int> vertexIndex;
	for (i = 0; i < n; i++)
	{
		if (not isSingleton[i])
		{
			vertexIndex.push_back(i);
			n_vertices++;
		}
	}

	// Max-Back algorithm ------------------------------------//
	list<int> LC;

	for (i = 0; i < n_vertices; i++)
		LC.push_back(i);

	b = vector<double>(n_vertices, 0.0);
	//for(i = 0; i < n_vertices; i++)
	// b.push_back(0.0);

	numCutsMB = 0;
	do
	{
		vertex = LC.front();
		Sinit.push_back(vertex); //Adding vertex 0 to S0

		cutmin = 0.0;
		for (i = 0; i < n_vertices; i++)
		{
			for (j = 0; j < Sinit.size(); j++)
				cutmin += solX[vertexIndex[Sinit[j]]][vertexIndex[i]];
		}

		for (i = 0; i < n_vertices; i++)
		{
			for (j = 0; j < Sinit.size(); j++)
			{
				if (i != Sinit[j])
					b[i] = solX[vertexIndex[Sinit[j]]][vertexIndex[i]];
			}
		}

		S = Sinit;
		Smin = Sinit;
		cutval = cutmin;

		while (S.size() < LC.size())
		{
			max = -1.0;
			int flag = 0;
			for (i = 0; i < b.size(); i++)
			{
				for (j = 0; j < S.size(); j++)
				{
					if (i == S[j])
					{
						flag = 1;
						break;
					}
				}
				if ((flag == 0) && (b[i] > max))
				{
					max = b[i];
					vertex = i;
				}
				flag = 0;
			}
			S.push_back(vertex);
			cutval += 2 - (2 * b[vertex]);

			flag = 0;
			for (i = 0; i < n_vertices; i++)
			{
				for (j = 0; j < S.size(); j++)
				{
					if (i == S[j])
					{
						flag = 1;
						break;
					}
				}
				if ((flag == 0))
				{
					if (vertexIndex[vertex] < vertexIndex[i])
					{
						//  b[i] += solX[vertex][i];
						b[i] += solX[vertexIndex[i]][vertexIndex[vertex]];
					}
					else
					{
						// b[i] += solX[i][vertex];
						b[i] += solX[vertexIndex[i]][vertexIndex[vertex]];
					}
				}
				flag = 0;
			}

			if (cutval < (cutmin - eps))
			{
				cutmin = cutval;
				Smin.clear();
				for (j = 0; j < S.size(); j++)
				{
					Smin.push_back(S[j]);
				}
			}
		}

		if (Smin.size() < n_vertices && Smin.size() > 2)
		{
			vector<int> cutSet(Smin.size());
			for (int i = 0; i < Smin.size(); i++)
				cutSet[i] = vertexIndex[Smin[i]];
			poolCutSet.push_back(cutSet);
			numCutsMB++;

			if (std::find(cutSet.begin(), cutSet.end(), 11) != Smin.end())
			{
				double val = 0;
				for (int vvv : cutSet)
					for (int uuu : cutSet)
						if (vvv < uuu)
							val += solX[vvv][uuu];

				/*cout << cutmin << " found wrong set: n_v:" << n_vertices << " val:" << val <<endl;
	    for(int vvv : Smin)
	       cout << vvv + 1 << " ";
	    cout << "\n";*/
			}
		}

		Sinit.clear();
		S.clear();
		for (i = 0; i < n_vertices; i++)
		{
			b[i] = 0.0;
		}

		for (i = 0; i < Smin.size(); i++)
		{
			LC.remove(Smin[i]);
		}
		Smin.clear();
		//cout << "Size LC: " << LC.size() << endl;
		//getchar();
	} while (LC.size() > 0);
	// Max-Back algorithm ------------------------------------//

	// Min-Cut algorithm ------------------------------------//
	//cout << "Depth: " << ndepth << endl;
	if (1 /*ndepth <= 7*/)
	{
		if (numCutsMB == 0)
		{
			//cout << "min cut\n";
			nAux.push_back(0);
			for (i = 0; i < n_vertices; i++)
			{
				aux.push_back(0.0);
			}

			for (i = 0; i < n_vertices; i++)
			{
				arcs.push_back(aux);
				nAux[0] = i;
				nodes.push_back(nAux);
			}

			for (i = 0; i < n_vertices; i++)
			{
				for (j = i + 1; j < n_vertices; j++)
				{
					//   arcs[i][j] = solX[i][j];
					//  arcs[j][i] = solX[i][j];
					arcs[i][j] = arcs[j][i] = solX[vertexIndex[i]][vertexIndex[j]];
					//arcs[j][i] =  solX[vertexIndex[j]][vertexIndex[i]];
				}
			}

			//MinCutPhase
			a = 0;
			cutphase = DBL_MAX;
			while (nodes.size() > 1)
			{
				nAux.clear();
				nAux.push_back(a);
				while (nAux.size() < nodes.size())
				{
					l = -1.0;
					for (j = 0; j < nodes.size(); j++)
					{
						flag = 0;
						for (i = 0; i < nAux.size(); i++)
						{
							if (nAux[i] == nodes[j][0])
							{
								flag = 1;
								break;
							}
						}

						if (!flag)
						{
							for (i = 0; i < nAux.size(); i++)
							{
								if (arcs[nAux[i]][nodes[j][0]] > l)
								{
									l = arcs[nAux[i]][nodes[j][0]];
									k = nodes[j][0];
								}
							}
						}
					}
					nAux.push_back(k);
					//cout << "k = " << k;
					//getchar();
				}

				cut = 0.0;
				for (i = 0; i < nodes.size(); i++)
				{
					if (arcs[nAux[nAux.size() - 1]][nodes[i][0]] > 0)
						cut += arcs[nAux[nAux.size() - 1]][nodes[i][0]];

					if ((arcs[nAux[nAux.size() - 2]][nodes[i][0]] > 0) && (arcs[nAux[nAux.size() - 1]][nodes[i][0]] > 0))
					{
						arcs[nAux[nAux.size() - 2]][nodes[i][0]] += arcs[nAux[nAux.size() - 1]][nodes[i][0]];
						arcs[nodes[i][0]][nAux[nAux.size() - 2]] += arcs[nAux[nAux.size() - 1]][nodes[i][0]];
					}
					else if ((arcs[nAux[nAux.size() - 1]][nodes[i][0]] > 0) && (nAux[nAux.size() - 2] != nodes[i][0]))
					{
						arcs[nAux[nAux.size() - 2]][nodes[i][0]] += arcs[nAux[nAux.size() - 1]][nodes[i][0]];
						arcs[nodes[i][0]][nAux[nAux.size() - 2]] += arcs[nAux[nAux.size() - 1]][nodes[i][0]];
					}
				}

				j = 0;
				while ((j < nodes.size()) && (nodes[j][0] != nAux[nAux.size() - 2]))
				{
					j++;
				}

				k = 0;
				while ((k < nodes.size()) && (nodes[k][0] != nAux[nAux.size() - 1]))
				{
					k++;
				}

				if (cut < cutphase)
				{
					cutphase = cut;
					cutSet = nodes[k];
				}

				vector<int> cutAux;
				if (nodes[k].size() <= (n_vertices / 2))
					cutAux = nodes[k];
				else
				{
					for (i = 0; i < n_vertices; i++)
					{
						m = 0;
						while ((m < nodes[k].size()) && (nodes[k][m] != i))
						{
							m++;
						}
						if (m == nodes[k].size())
						{
							cutAux.push_back(i);
						}
					}
				}

				cutval = 0.0;
				for (i = 0; i < (cutAux.size() - 1); i++)
				{
					for (m = i + 1; m < cutAux.size(); m++)
					{
						//		  if(cutAux[i] < cutAux[m])
						//		  {
						//		     cutval += solX[cutAux[i]][cutAux[m]];
						//		  }else
						//		  {
						//		     cutval += solX[cutAux[m]][cutAux[i]];
						//		  }

						cutval += solX[vertexIndex[cutAux[m]]][vertexIndex[cutAux[i]]];
						//cutval += solX[vertexIndex[cutAux[i]]][vertexIndex[cutAux[m]]];
					}
				}

				if ((cutval > (cutAux.size() - 1 + eps)) && cutAux.size() > 2)
				{
					for (int i = 0; i < cutAux.size(); i++)
						cutAux[i] = vertexIndex[cutAux[i]];
					poolCutSet.push_back(cutAux);
				}

				for (i = 0; i < nodes[k].size(); i++)
				{
					nodes[j].push_back(nodes[k][i]);
				}
				nodes.erase(nodes.begin() + k);
			}
		}
	}
	// Min-Cut algorithm ------------------------------------//

	return poolCutSet;
}

vector<vector<int>> SubtourSep::separateInteger(std::vector<std::vector<int>> &solX, int n)
{
	vector<vector<int>> poolCutSet;

	vector<bool> reached(n, false);

	for (int i = 0; i < n; i++)
	{
		//componente conexa que contem o i
		if (!reached[i])
		{
			vector<int> component;
			int sumDegrees = 0;
			vector<int> dfs_stack;
			dfs_stack.push_back(i);
			reached[i] = true;

			while (dfs_stack.size() > 0)
			{
				int v = dfs_stack.back();
				dfs_stack.pop_back();
				component.push_back(v);

				for (int j = 0; j < n; j++)
				{
					if ((j != v) && (solX[v][j] == 1))
					{
						sumDegrees++;
						if (!reached[j])
						{
							dfs_stack.push_back(j);
							reached[j] = true;
						}
					}
				}
			}

			if (sumDegrees / 2 > component.size() - 1)
				poolCutSet.push_back(component);
		}
	}

	return poolCutSet;
}

vector<vector<int>> SubtourSep::maxback(std::vector<std::vector<double>> &solX, int n)
{
	double eps = 0.00000001;

	vector<double> b;
	vector<double> degrees;
	vector<int> Sinit;
	vector<int> S;
	vector<int> Smin;
	double cutmin;
	double cutval;
	double max_bi;
	int vertex;
	vector<vector<int>> poolCutSet;
	//-------------------

	//graus dos vertices
	degrees = vector<double>(n, 0);
	for (int i = 0; i < n; i++)
	{
		for (int j = i + 1; j < n; j++)
		{
			degrees[i] += solX[i][j];
			degrees[j] += solX[i][j];
		}
	}

	//preenche isSingleton
	std::vector<bool> isSingleton(n, false);
	for (int i = 0; i < n; i++)
		isSingleton[i] = (degrees[i] == 0);

	//determinas os ids dos vertices nao singletons
	int n_vertices = 0;
	std::vector<int> vertexIndex;
	for (int i = 0; i < n; i++)
	{
		if (!isSingleton[i])
		{
			vertexIndex.push_back(i);
			n_vertices++;
		}
	}

	// Max-Back algorithm ------------------------------------//
	list<int> LC;
	for (int i = 0; i < n_vertices; i++)
		LC.push_back(i);
	b = vector<double>(n_vertices, 0.0);
	vector<bool> inS(n_vertices, false);

	do
	{
		vertex = LC.front();
		Sinit.push_back(vertex); //Adding vertex 0 to S0
		inS[vertex] = true;

		cutmin = 0.0;
		for (int i = 0; i < n_vertices; i++)
			if (i != vertex)
				cutmin += solX[vertexIndex[vertex]][vertexIndex[i]];

		for (int i = 0; i < n_vertices; i++)
		{
			if (!inS[i])
				b[i] = solX[vertexIndex[vertex]][vertexIndex[i]];
		}

		S = Sinit;
		Smin = Sinit;
		cutval = cutmin;

		while (S.size() < n_vertices - 1)
		{
			//encontra o vertice que possui o maior bi
			max_bi = -1.0;
			for (int i = 0; i < n_vertices; i++)
			{
				if (!inS[i] && (b[i] > max_bi))
				{
					max_bi = b[i];
					vertex = i;
				}
			}

			//adiciona o novo vertice a S
			S.push_back(vertex);
			inS[vertex] = true;
			cutval += degrees[vertexIndex[vertex]] - (2 * b[vertex]);

			//atualizando bi
			for (int i = 0; i < n_vertices; i++)
				if (!inS[i])
					b[i] += solX[vertexIndex[i]][vertexIndex[vertex]];

			//atualiza Smin
			if (cutval < (cutmin - eps))
			{
				cutmin = cutval;
				Smin = S;
			}
		}

		//verifica se Smin esta violado
		double sminval = 0;
		for (int v : Smin)
			for (int u : Smin)
				if (u < v)
					sminval += solX[vertexIndex[u]][vertexIndex[v]];
		if (sminval > Smin.size() - 1 + eps)
		{
			vector<int> cutSet(Smin.size());
			for (int i = 0; i < Smin.size(); i++)
				cutSet[i] = vertexIndex[Smin[i]];
			poolCutSet.push_back(cutSet);
		}

		//verifica se o complemento de Smin esta violado
		double smincomplval = 0;
		vector<int> SminCompl;
		for (int i = 0; i < n_vertices; i++)
			if (std::find(Smin.begin(), Smin.end(), i) == Smin.end())
				SminCompl.push_back(i);
		for (int v : SminCompl)
			for (int u : SminCompl)
				if (u < v)
					smincomplval += solX[vertexIndex[u]][vertexIndex[v]];
		if (smincomplval > SminCompl.size() - 1 + eps)
		{
			vector<int> cutSet(SminCompl.size());
			for (int i = 0; i < SminCompl.size(); i++)
				cutSet[i] = vertexIndex[SminCompl[i]];
			poolCutSet.push_back(cutSet);
		}

		Sinit.clear();
		S.clear();
		for (int i = 0; i < n_vertices; i++)
			b[i] = 0.0;

		for (int i = 0; i < Smin.size(); i++)
			LC.remove(Smin[i]);
		Smin.clear();

	} while (LC.size() > 0);
	// Max-Back algorithm ------------------------------------//

	return poolCutSet;
}
