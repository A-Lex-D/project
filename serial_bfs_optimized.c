/*
*
*	Graph builder from:
*	sanfoundry.com/c-program-generate-random-undirected-graph-given-number-edges/
*
*
*	BFS Traversal from
*   http://www.thelearningpoint.net/computer-science/algorithms-graph-traversal---breadth-first-search-with-c-program-source-code
*
*	
*	Modifications done to source code to get them to work together
*
*
*	
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>


#define VERTICES 50
#define MAX_EDGES 50
#define GIG 1000000000
#define CPG 2.53


typedef struct Queue
{
	int capacity;
	int size;
	int front;
	int rear;
	int *elements;
}Queue;

typedef struct Node
{
	int start;
	int no_of_edges;
}Node;

int main()
{
void Bfs(Node* graph_nodes, int *graph_edge, int presentVertex,bool *visited);
struct timespec diff(struct timespec start, struct timespec end);
struct timespec time1, time2, fintime;
	
	

	printf("Total vertices = %d, Max edges = %d\n", VERTICES, MAX_EDGES);

	Node* graph_nodes = (Node*) malloc(sizeof(Node)*VERTICES);
	
	int* graph_edge;
	
	
	bool *graph_visited;
	if ((graph_visited = (bool*) malloc(sizeof(bool)*VERTICES))==NULL)
	{
		printf("Could not allocate memory for graph_mask\n");
		exit(1);
	}
	
	

	//Generate space for edges
	int vertexCounter = 0;
	//Generate space for vertices
	int edgeCounter = 0;
	int len;
	//Link the graph
	for(vertexCounter=0; vertexCounter<VERTICES; vertexCounter++)
	{
		
		//for gpu transfer

		graph_nodes[vertexCounter].no_of_edges = (rand()%(MAX_EDGES))+1;

		if(vertexCounter ==0)
		{
		graph_nodes[vertexCounter].start= vertexCounter;
		len = graph_nodes[vertexCounter].no_of_edges;
		graph_edge = (int *) malloc(sizeof(int)*len);
				if ((graph_edge = (int *) malloc(sizeof(int)*len))==NULL)
			{
				printf("Could not allocate memory for graph_edge\n");
				exit(1);
			} 

		}
		else
		{
		graph_nodes[vertexCounter].start= graph_nodes[vertexCounter-1].start+graph_nodes[vertexCounter-1].no_of_edges;
		len += graph_nodes[vertexCounter].no_of_edges;
		graph_edge = (int *) realloc(graph_edge, sizeof(int)*len);
		if ((graph_edge = (int *) realloc(graph_edge, sizeof(int)*len))==NULL)
			{
				printf("Could not reallocate memory for graph_edge\n");
				exit(1);
			} 
		} 
		
		
		
		//printf("%d:\t", vertexCounter);

		graph_visited[vertexCounter] = false;
		for(edgeCounter=graph_nodes[vertexCounter].start; edgeCounter<(graph_nodes[vertexCounter].no_of_edges+graph_nodes[vertexCounter].start); edgeCounter++)
		{
				int linkedVertex = rand()%VERTICES;
				graph_edge[edgeCounter] = rand()%VERTICES;
				//printf("%d, ", graph_edge[edgeCounter]);
		}

		//printf("\n");

	}


clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);	
	int presentVertex;
	    for(presentVertex=0;presentVertex<VERTICES;presentVertex++)
	    {
	    	if(!graph_visited[presentVertex])
	    	{   		
				Bfs(graph_nodes,graph_edge,presentVertex,graph_visited);
				
		}
	    }

	
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
fintime = diff(time1,time2);
printf("\n Cycles:%f \n", (double)(CPG)*(double)(GIG*fintime.tv_sec+fintime.tv_nsec));		

	// cleanup memory
	free( graph_nodes);
	free( graph_edge);
	
	free( graph_visited);
	
return 0;
}

// Host Implementation
/* crateQueue function takes argument the maximum number of elements the Queue can hold, creates
   a Queue according to it and returns a pointer to the Queue. */
Queue * CreateQueue(int maxElements)
{
        /* Create a Queue */
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));
        /* Initialise its properties */
        Q->elements = (int *)malloc(sizeof(int)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = 0;
        Q->rear = -1;
        /* Return the pointer */
        return Q;
}
void Dequeue(Queue *Q)
{
        /* If Queue size is zero then it is empty. So we cannot pop */
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                return;
        }
        /* Removing an element is equivalent to incrementing index of front by one */
        else
        {
                Q->size--;
                Q->front++;
                /* As we fill elements in circular fashion */
                if(Q->front==Q->capacity)
                {
                        Q->front=0;
                }
        }
        return;
}
int Front(Queue *Q)
{
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                exit(0);
        }
        /* Return the element which is at the front*/
        return Q->elements[Q->front];
}
void Enqueue(Queue *Q,int element)
{
        /* If the Queue is full, we cannot push an element into it as there is no space for it.*/
        if(Q->size == Q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
                Q->size++;
                Q->rear = Q->rear + 1;
                /* As we fill the queue in circular fashion */
                if(Q->rear == Q->capacity)
                {
                        Q->rear = 0;
                }
                /* Insert the element in its rear side */ 
                Q->elements[Q->rear] = element;
        }
        return;
}



void Bfs(Node* graph_nodes, int* graph_edge, int presentVertex, bool* visited)
{
        visited[presentVertex] = true;
        /* Iterate through all the vertices connected to the presentVertex and perform bfs on those
           vertices if they are not visited before */
        Queue *Q = CreateQueue(VERTICES);
        Enqueue(Q,presentVertex);
        while(Q->size)
        {
                presentVertex = Front(Q);
               // printf("Now visiting vertex %d\n",presentVertex);
                Dequeue(Q);
                int iter;
                for(iter=0;iter<graph_nodes[presentVertex].no_of_edges;iter++)
                {
					
                        if(!visited[graph_edge[graph_nodes[presentVertex].start+iter]])
                        {
                                visited[graph_edge[graph_nodes[presentVertex].start+iter]] = 1;
                                Enqueue(Q,graph_edge[graph_nodes[presentVertex].start+iter]);
                        }
					
                }
        }
        return;
        

}

struct timespec diff(struct timespec start, struct timespec end)
{
struct timespec temp;
if ((end.tv_nsec-start.tv_nsec)<0) {
temp.tv_sec = end.tv_sec-start.tv_sec-1;
temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
} else {
temp.tv_sec = end.tv_sec-start.tv_sec;
temp.tv_nsec = end.tv_nsec-start.tv_nsec;
}
return temp;
}
