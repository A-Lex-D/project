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
*	Compile with:  gcc -o mybfs mybfs.c -lrt
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define VERTICES 80000
#define MAX_EDGES 50000
#define GIG 1000000000
#define CPG 2.533 

typedef struct Queue
{
	int capacity;
	int size;
	int front;
	int rear;
	int *elements;
}Queue;

int main()
{
	void Bfs(int** graph, int *size, int presentVertex,int *visited);
	struct timespec diff(struct timespec start, struct timespec end);
  	struct timespec time1, time2, fintime;
	
	srand(time(NULL));
	int *visited;
	if ((visited=(int*)calloc(VERTICES, sizeof(int)))==NULL)
	{
		printf("Could not allocate memory for visited array\n");
		exit(1);
	}
	int size[VERTICES]={0};
	
	//Graph is 2 dimensional array of pointers
	int **graph;

	//printf("Total vertices = %d, Max edges = %d\n", VERTICES, MAX_EDGES);

	//Generate dynamic array
	if ((graph=(int**)malloc(sizeof(int *) * VERTICES))==NULL)
	{
		printf("Could not allocate memory for graph\n");
		exit(1);
	}


	//Generate space for edges
	int vertexCounter = 0;
	//Generate space for vertices
	int edgeCounter = 0;

	//Link the graph
	for(vertexCounter=0; vertexCounter<VERTICES; vertexCounter++)
	{
		size[vertexCounter] = rand()%MAX_EDGES;
		
		if((graph[vertexCounter]=(int*)malloc(sizeof(int)*size[vertexCounter]))==NULL)
		{
			printf("Could not allocate memory for edges\n");
			exit(1);
		}
		
		//printf("%d:\t", vertexCounter);
		
		for(edgeCounter=0; edgeCounter<size[vertexCounter]; edgeCounter++)
		{
				int linkedVertex = rand()%VERTICES;
				graph[vertexCounter][edgeCounter]=linkedVertex;
				//printf("%d, ", linkedVertex);
		}

		//printf("\n");

	}

	
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);

	int presentVertex;
    for(presentVertex=0;presentVertex<VERTICES;presentVertex++)
    {
    	if(!visited[presentVertex])
    	{   		
			Bfs(graph,size,presentVertex,visited);
			//memset(visited, 0, (VERTICES*sizeof(int))); 
        }
    }


    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);


	fintime = diff(time1,time2);

    printf("%f, \n", (double)(CPG)*(double)(GIG*fintime.tv_sec
				+fintime.tv_nsec));
	return 0;
}


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



void Bfs(int** graph, int *size, int presentVertex,int *visited)
{
        visited[presentVertex] = 1;
        /* Iterate through all the vertices connected to the presentVertex and perform bfs on those
           vertices if they are not visited before */
        Queue *Q = CreateQueue(VERTICES);
        Enqueue(Q,presentVertex);
        while(Q->size)
        {
                presentVertex = Front(Q);
                //printf("Now visiting vertex %d\n",presentVertex);
                Dequeue(Q);
                int iter;
                for(iter=0;iter<size[presentVertex];iter++)
                {
					//if(graph[presentVertex][iter]<VERTICES)
					//{
                        if(!visited[graph[presentVertex][iter]])
                        {
                                visited[graph[presentVertex][iter]] = 1;
                                Enqueue(Q,graph[presentVertex][iter]);
                        }
					//}
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
