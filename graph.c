/*
*
*	Graph builder from:
*	sanfoundry.com/c-program-generate-random-undirected-graph-given-number-edges/
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VERTICES 30
#define MAX_EDGES 5

typedef unsigned char vertex;

int main()
{
	srand(time(NULL));
	
	//Number of nodes in graph
	int numberOfVertices = MAX_VERTICES;
	//Max number of vertices each node can have
	int maxNumberOfEdges = MAX_EDGES;
	//Graph is 2 dimensional array of pointers
	vertex ***graph;

	printf("Total vertices = %d, Max edges = %d\n", numberOfVertices, maxNumberOfEdges);

	//Generate dynamic array
	if ((graph=(vertex***)malloc(sizeof(vertex **) * numberOfVertices))==NULL)
	{
		printf("Could not allocate memory for graph\n");
		exit(1);
	}


	//Generate space for edges
	int vertexCounter = 0;
	//Generate space for vertices
	int edgeCounter = 0;

	for(vertexCounter=0; vertexCounter<numberOfVertices; vertexCounter++)
	{
		if((graph[vertexCounter]=(vertex**)malloc(sizeof(vertex*)*maxNumberOfEdges))==NULL)
		{
			printf("Could not allocate memory for edges\n");
			exit(1);
		}

		for(edgeCounter=0; edgeCounter<maxNumberOfEdges; edgeCounter++)
		{
			if((graph[vertexCounter][edgeCounter]=(vertex*)malloc(sizeof(vertex)))==NULL)
			{
				printf("Could not allocate memory for vertex\n");
				exit(1);
			}
		}
	}

	//Link the graph
	for(vertexCounter=0; vertexCounter<numberOfVertices; vertexCounter++)
	{
		printf("%d:\t", vertexCounter);
		
		for(edgeCounter=0; edgeCounter<maxNumberOfEdges; edgeCounter++)
		{
			if(rand()%2 == 1)//link vertices
			{
				int linkedVertex = rand()%numberOfVertices;
				graph[vertexCounter][edgeCounter]=graph[linkedVertex];
				printf("%d, ", linkedVertex);
			}
			
			else //make link null
			{
				graph[vertexCounter][edgeCounter]=NULL;
			}
		}

		printf("\n");
	}

	return 1;
}
