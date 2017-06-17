// Zombies.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "Ville.h"
using namespace std;

string pathm = "ville.txt";
vector<Ville> villes;
int* bestSolution;
int* starting_town;
int* ending_town;
float best_eval;
int* bestSolutionFinal;
/**
* print a solution
*/
void print_solution(int* sol, float eval)
{
	int i;
	
	cout << "(" <<eval<<")";
	for (i = 0; i<villes.size(); i++)
		cout <<  " " <<sol[i];
	cout << "\n";
}

/**
* evaluation of a solution
*/
double evaluation_solution(int* sol, float** dist)
{

	double eval = 0;
	int i;
	for (i = 0; i<villes.size() - 1; i++)
	{
		/* printf ("from %d -> %d : %f\n", sol[i], sol[i+1], dist[sol[i]][sol[i+1]]) ; */
		eval += dist[sol[i]][sol[i + 1]];
	}
	/* printf ("from %d -> %d : %f\n", sol[NBR_TOWNS-1], sol[0], dist[sol[NBR_TOWNS-1]][sol[0]]) ; */
	eval += dist[sol[villes.size() - 1]][sol[0]];

	return eval;

}

/**
* nearest neighbour solution
*/
double build_nearest_neighbour(float ** dist)
{
	/* solution of the nearest neighbour */
	int i;
	int* sol = new int[villes.size()];

	/* evaluation of the solution */
	double eval = 0;

	sol[0] = 0;
	for (i = 1; i<villes.size(); i++)
	{
		int j;
		int shortest_town = -1;
		double shortest_dist = -1;
		for (j = 1; j<villes.size(); j++)
		{
			int j_used = 0;
			int k = 0;
			while (k<i && !j_used)
			{
				if (sol[k] == j) j_used = 1;
				k++;
			}
			if (!j_used)
			{
				double d = dist[sol[i - 1]][j];
				if ((shortest_dist<0) || (d<shortest_dist))
				{
					shortest_dist = d;
					shortest_town = j;
				}
			}
		}
		sol[i] = shortest_town;
	}
	eval = evaluation_solution(sol, dist);

	printf("Nearest neighbour ");
	print_solution(sol, eval);
	
	/* initialisation of the best solution and best_eval */
	for (i = 0; i<villes.size(); i++) 
		bestSolution[i] = sol[i];

	best_eval = eval;

	return eval;
}


/**
*  Build final solution
*/
void build_solution(float** dist)
{
	int* solution = new int[villes.size()];
	int i;

	int indiceCour = 0;
	int villeCour = 0;

	while (indiceCour < villes.size())
	{

		solution[indiceCour] = villeCour;

		// Test si le cycle est hamiltonien
		for (i = 0; i < indiceCour; i++)
		{
			if (solution[i] == villeCour)
			{
				/* printf ("cycle non hamiltonien\n") ; */
				return;
			}
		}
		// Recherche de la ville suivante
		int trouve = 0;
		int i = 0;
		while ((!trouve) && (i < villes.size()))
		{
			if (starting_town[i] == villeCour)
			{
				trouve = 1;
				villeCour = ending_town[i];
			}
			i++;
		}
		indiceCour++;
	}

	double eval = evaluation_solution(solution, dist);

	if (best_eval<0 || eval < best_eval)
	{
		best_eval = eval;
		for (i = 0; i<villes.size(); i++)
			bestSolution[i] = solution[i];
		printf("New best solution ");
		print_solution(solution, best_eval);
	}
	return;
}


/**
*  Algorithme de Little
*/
void little_algorithm(float** d0, int iteration, double eval_node_parent)
{

	if (iteration == villes.size())
	{
		build_solution(d0);
		return;
	}

	/* Do the modification on a copy of the distance matrix */

	double **d = new double*[villes.size()];
	for (int i = 0; i <villes.size(); i++)
		d[i] = new double[villes.size()];
	

	int i, j;
	double eval_node_child = eval_node_parent;

	/**
	* substract the min of the rows and the min of the columns
	* and update the evaluation of the current node
	*/
	double somme = 0;

	/* Soustraction du min de chaque ligne */
	for (i = 0; i < villes.size(); i++)
	{
		double min = -1;
		for (j = 0; j < villes.size(); j++)
			if ((d[i][j] >= 0) && ((min < 0) || (d[i][j] < min)))
				min = d[i][j];

		if (min > 0)
		{
			for (j = 0; j < villes.size(); j++)
			{
				if (d[i][j] >= 0)
					d[i][j] -= min;
			}
			somme += min;
		}
	}

	/* Soustraction du min de chaque colonne */
	for (j = 0; j < villes.size(); j++)
	{
		double min = -1;
		for (i = 0; i < villes.size(); i++)
			if ((d[i][j] >= 0) && ((min < 0) || (d[i][j] < min)))
				min = d[i][j];
		if (min > 0)
		{
			for (i = 0; i < villes.size(); i++)
				if (d[i][j] >= 0)
					d[i][j] -= min;
			somme += min;
		}
	}
	eval_node_child += somme;

	/* Cut : stop the exploration of this node */
	if (best_eval >= 0 && eval_node_child >= best_eval)
		return;

	/* Compute the penalities to identify the zero with max penalty */
	int i0, j0, izero = -1, jzero = -1;
	double maxPenalite = 0;
	for (i = 0; i < villes.size(); i++)
	{
		for (j = 0; j < villes.size(); j++)
			if (d[i][j] == 0)
			{
				if (izero<0)
				{
					izero = i;
					jzero = j;
				}
				double penalite = 0;
				double min0 = -1;
				for (i0 = 0; i0 < villes.size(); i0++)
				{
					if ((i != i0) && (d[i0][j] >= 0))
					{
						if ((min0 < 0) || (d[i0][j] < min0)) {
							min0 = d[i0][j];
						}
					}
				}

				if (min0 > 0) penalite += min0;
				min0 = -1;
				for (j0 = 0; j0 < villes.size(); j0++)
				{
					if ((j != j0) && (d[i][j0] >= 0))
					{
						if ((min0 < 0) || (d[i][j0] < min0))
							min0 = d[i][j0];
					}
				}
				if (min0 > 0) penalite += min0;
				/*printf ("%d, %d : %f \n", i, j, penalite) ;*/
				if (penalite > maxPenalite)
				{
					maxPenalite = penalite;
					izero = i;
					jzero = j;
				}
			}
	}

	if (izero<0) return;

	starting_town[iteration] = izero;
	ending_town[iteration] = jzero;

	/*
	printf ("izero=%d ; jzero=%d\n", izero, jzero) ;
	getchar() ;
	*/

	/* Do the modification on a copy of the distance matrix */
	float **d2 = new float*[villes.size()];
	for (int i = 0; i <villes.size(); i++)
		d2[i] = new float[villes.size()];

	//memcpy(d2, d, villes.size()*villes.size() * sizeof(double));

	for (i = 0; i < villes.size(); i++)
	{
		d2[izero][i] = -1;
		d2[i][jzero] = -1;
	}

	d2[jzero][izero] = -1;

	/* Explore left child node according to given choice */
	little_algorithm(d2, iteration + 1, eval_node_child);

	/* Do the modification on a copy of the distance matrix */
	memcpy(d2, d, villes.size()*villes.size() * sizeof(double));

	d2[izero][jzero] = -1;

	/* Explore right child node according to non-choice */
	little_algorithm(d2, iteration, eval_node_child);
	delete d;
	delete d2;

}



float decimal(float entier, float dec) {
	while (dec > 1) {
		dec = dec / 10;
	}
	return entier + dec;
}

void creerVilles() {
	int id;
	int xcoordEntier;
	int xcoordDec;
	int ycoordEntier;
	int ycoordDec;
	char point;
	ifstream fichier;
	fichier.open("../" + pathm);

	if (!fichier.is_open()) {
		cout << "Le fichier n'a pas pu etre ouvert. Verifiez son existence et sa disponibilite.\n";
		return;
	}

	while (fichier>> id)
	{
		cout << "id " << id;
		fichier >> xcoordEntier;
		cout << " int  " << xcoordEntier;
		fichier >> point;
		fichier >> xcoordDec;
		cout << "." << xcoordDec;
		fichier >> ycoordEntier;
		cout << " int  " << ycoordEntier;
		fichier >> point;
		fichier >> ycoordDec;
		cout << "." << ycoordDec << endl;

		villes.push_back(Ville(decimal(xcoordEntier, xcoordDec), decimal(ycoordEntier, ycoordDec), id));
	}
	fichier.close();
}

void creerDistance(float** distances) {
	for (int i = 0; i<villes.size(); i++)
	{
		for (int j = 0; j<villes.size(); j++)
		{
			if (i == j)
			{
				distances[i][j] = -1;
				//distances.at(i).at(j) = -1.0;
			}
			else
			{
				double dx = villes.at(i).xCoord - villes.at(j).xCoord;
				double dy = villes.at(i).yCoord - villes.at(j).yCoord;
				distances[i][j] = sqrt(dx*dx + dy*dy);
			}
		}
	}
}

float notHamilton(float** dist) {
	int j = 0;
	float tempMax = dist[bestSolution[0]][bestSolution[1]];

	for (int i = 1; i < villes.size()-1; i++) {
		if (tempMax < dist[bestSolution[i]][bestSolution[i + 1]]) {
			tempMax = dist[bestSolution[i]][bestSolution[i + 1]];
			j = i;
		}
	}
	int k = 0;
	for (int i = j; i < villes.size(); i++) {
		bestSolutionFinal[k] = bestSolution[i];
		k++;
	}
	for (int i = 0; i < j; i++) {
		bestSolutionFinal[k] = bestSolution[i];
		k++;
	}

	return tempMax;
}

int main()
{
	creerVilles();

	float **ary = new float*[villes.size()];
	for (int i = 0; i <villes.size(); i++)
		ary[i] = new float[villes.size()];
	bestSolution = new int[villes.size()];
	starting_town = new int[villes.size()];
	ending_town = new int[villes.size()];
	creerDistance(ary);
	
	double nearest_neighbour = build_nearest_neighbour(ary);

	int iteration = 0;
	double lowerbound = 0.0;

	little_algorithm(ary, iteration, lowerbound);

	cout <<"Best Hamilton solution ";
	print_solution(bestSolution, best_eval);

	bestSolutionFinal = new int[villes.size()];
	float d = notHamilton(ary);
	cout << "Best solution ";
	print_solution(bestSolutionFinal, best_eval-d);


	delete[] bestSolution;
	delete[] starting_town;
	delete[] ending_town;
	
	system("PAUSE");
    return 0;
}
