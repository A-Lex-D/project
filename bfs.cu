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
*	Compile with:  nvcc -o bfs bfs.cu 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <cuda.h>
#include "cuPrintf.cuh"
#include "cuPrintf.cu"

#define VERTICES 20
#define MAX_EDGES 2
#define MAX_THREADS_PER_BLOCK 256

typedef struct Queue
{
	int capacity;
	int size;
	int front;
	int rear;
	int *elements;
}Queue;

struct Node
{
	int start;
	int no_of_edges;
};

// Assertion to check for errors

#define CUDA_SAFE_CALL(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, char *file, int line, bool abort=true)
{
	if (code != cudaSuccess) 
	{
		fprintf(stderr,"CUDA_SAFE_CALL: %s %s %d\n", cudaGetErrorString(code), file, line);
		if (abort) exit(code);
	}
}


__global__ void
Kernel2( bool* g_graph_mask, bool *g_updating_graph_mask, bool* g_graph_visited, bool *g_over )
{
	int tid = blockIdx.x*MAX_THREADS_PER_BLOCK + threadIdx.x;
	if( tid<VERTICES && g_updating_graph_mask[tid])
	{

		g_graph_mask[tid]=true;
		g_graph_visited[tid]=true;
		*g_over=true;
		//cuPrintf("visting branches in kernal2 %d,", tid);
		g_updating_graph_mask[tid]=false;
	}
}


__global__ void
Kernel( Node* g_graph, int *g_edge, bool* g_graph_mask, bool* g_updating_graph_mask, bool *g_graph_visited ) 
{
	int tid = blockIdx.x*MAX_THREADS_PER_BLOCK + threadIdx.x;
	if( tid<VERTICES && g_graph_mask[tid])
	{
		g_graph_mask[tid]=false;
		//cuPrintf("now visiting%d, %d\n", tid, g_graph[tid].start);
		for(int i=g_graph[tid].start; i<(g_graph[tid].no_of_edges+g_graph[tid].start); i++)
			{
			int id = g_edge[i];
			if(!g_graph_visited[id])
				{
			//cuPrintf("branches %d,", id);
				g_updating_graph_mask[id]=true;
				}
			}
	//cuPrintf("\n");
	}
}
void Bfs(Node* graph_nodes, int *graph_edge, int presentVertex,bool *visited);
int main()
{

	// GPU Timing variables
	cudaEvent_t start, stop, start1, stop1;
	float elapsed_gpu, elapsed_gpu1;
	//int i;
	

	printf("Total vertices = %d, Max edges = %d\n", VERTICES, MAX_EDGES);

	Node* graph_nodes = (Node*) malloc(sizeof(Node)*VERTICES);
	
	int* graph_edge; //= (int *) malloc(sizeof(int)*VERTICES*MAX_EDGES);
	
	//Generate Varibles to check
	bool *graph_mask;
	if ((graph_mask = (bool*) malloc(sizeof(bool)*VERTICES))==NULL)
	{
		printf("Could not allocate memory for graph_mask\n");
		exit(1);
	}

	bool *updating_graph_mask;
	if ((updating_graph_mask = (bool*) malloc(sizeof(bool)*VERTICES))==NULL)
	{
		printf("Could not allocate memory for graph_mask\n");
		exit(1);
	}
	bool *graph_visited;
	if ((graph_visited = (bool*) malloc(sizeof(bool)*VERTICES))==NULL)
	{
		printf("Could not allocate memory for graph_mask\n");
		exit(1);
	}
	bool *h_graph_visited;
	if ((h_graph_visited = (bool*) malloc(sizeof(bool)*VERTICES))==NULL)
	{
		printf("Could not allocate memory for graph_mask\n");
		exit(1);
	}
	//for(i=0; i<VERTICES*MAX_EDGES; i++)
	//graph_edge[i] = -1;

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
				printf("Could not allocate memory for graph_edge : %d\n", vertexCounter);
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
				printf("Could not reallocate memory for graph_edge: %d\n", vertexCounter);
				exit(1);
			} 
		} 
		
		
		
	//	printf("%d:\t", vertexCounter);
		graph_mask[vertexCounter] = false;
		updating_graph_mask[vertexCounter] = false;
		graph_visited[vertexCounter] = false;
		h_graph_visited[vertexCounter] = false;
		for(edgeCounter=graph_nodes[vertexCounter].start; edgeCounter<(graph_nodes[vertexCounter].no_of_edges+graph_nodes[vertexCounter].start); edgeCounter++)
		{
				int linkedVertex = rand()%VERTICES;
				graph_edge[edgeCounter] = rand()%VERTICES;
				//printf("%d, ", graph_edge[edgeCounter]);
		}

	//	printf("\n");

	}

	// Create the cuda events
	cudaEventCreate(&start1);
	cudaEventCreate(&stop1);
	// Record event on the default stream
	cudaEventRecord(start1, 0);

	int presentVertex;
	    for(presentVertex=0;presentVertex<VERTICES;presentVertex++)
	    {
	    	if(!h_graph_visited[presentVertex])
	    	{   		
				Bfs(graph_nodes,graph_edge,presentVertex,h_graph_visited);
				
		}
	    }

	

	// Stop and destroy the timer
	cudaEventRecord(stop1,0);
	cudaEventSynchronize(stop1);
	cudaEventElapsedTime(&elapsed_gpu1, start1, stop1);
	printf("\nCPU time: %f (msec)\n", elapsed_gpu1);
	cudaEventDestroy(start1);
	cudaEventDestroy(stop1);
	

	int source=0;

	//set the source node as true in the mask
	graph_mask[source]=true;
	graph_visited[source]=true;

	//Copy the Graph to device memory
	Node *d_graph;
	CUDA_SAFE_CALL(cudaMalloc( (void**) &d_graph, sizeof(Node) *VERTICES)) ;
	CUDA_SAFE_CALL(cudaMemcpy( d_graph, graph_nodes, sizeof(Node) *VERTICES, cudaMemcpyHostToDevice));

	//Copy the Edge List to device Memory
	int* d_edge;
	cudaMalloc( (void**) &d_edge, sizeof(int)*(len)) ;
	cudaMemcpy( d_edge, graph_edge, sizeof(int)*(len), cudaMemcpyHostToDevice) ;


	//Copy the Mask to device memory
	bool* d_graph_mask;
	CUDA_SAFE_CALL(cudaMalloc( (void**) &d_graph_mask, sizeof(bool)*VERTICES)) ;
	CUDA_SAFE_CALL(cudaMemcpy( d_graph_mask, graph_mask, sizeof(bool)*VERTICES, cudaMemcpyHostToDevice));

	bool* d_updating_graph_mask;
	CUDA_SAFE_CALL(cudaMalloc( (void**) &d_updating_graph_mask, sizeof(bool)*VERTICES)) ;
	CUDA_SAFE_CALL(cudaMemcpy( d_updating_graph_mask, updating_graph_mask, sizeof(bool)*VERTICES, cudaMemcpyHostToDevice)) ;

	//Copy the Visited nodes array to device memory
	bool* d_graph_visited;
	CUDA_SAFE_CALL(cudaMalloc( (void**) &d_graph_visited, sizeof(bool)*VERTICES)) ;
	CUDA_SAFE_CALL(cudaMemcpy( d_graph_visited, graph_visited, sizeof(bool)*VERTICES, cudaMemcpyHostToDevice)) ;


	//make a bool to check if the execution is over
	bool *d_over;
	CUDA_SAFE_CALL(cudaMalloc( (void**) &d_over, sizeof(bool)));

	printf("Copied Everything to GPU memory\n");
	
	int num_of_blocks = 1;
	int num_of_threads_per_block = VERTICES;

	//Make execution Parameters according to the number of nodes
	//Distribute threads across multiple Blocks if necessary
	if(VERTICES>MAX_THREADS_PER_BLOCK)
	{
		num_of_blocks = (int)ceil(VERTICES/(double)MAX_THREADS_PER_BLOCK); 
		num_of_threads_per_block = MAX_THREADS_PER_BLOCK; 
	}
	// setup execution parameters
	dim3  grid( num_of_blocks, 1, 1);
	dim3  threads( num_of_threads_per_block, 1, 1);

	int k=0;
	printf("Start traversing the tree\n");
	bool over;
	//Call the Kernel untill all the elements of Frontier are not false

	// Create the cuda events
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	// Record event on the default stream
	cudaEventRecord(start, 0);
	
//	cudaPrintfInit ();
	do
	{
		
		//if no thread changes this value then the loop stops
		over=false;
		cudaMemcpy( d_over, &over, sizeof(bool), cudaMemcpyHostToDevice) ;
		
		
		Kernel<<< grid, threads, 0 >>>( d_graph,d_edge, d_graph_mask, d_updating_graph_mask, d_graph_visited);
		// check if kernel execution generated and error
		

		Kernel2<<< grid, threads, 0 >>>( d_graph_mask, d_updating_graph_mask, d_graph_visited, d_over);
		// check if kernel execution generated and error
		

		cudaMemcpy( &over, d_over, sizeof(bool), cudaMemcpyDeviceToHost) ;
		k++;
	}
	while(over);

	//cudaPrintfDisplay (stdout, true);
//	cudaPrintfEnd ();


	
	
	
	// Stop and destroy the timer
	cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&elapsed_gpu, start, stop);
	printf("\nGPU time: %f (msec)\n", elapsed_gpu);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);

	printf("Kernel Executed %d times\n",k); 
	
	
	
	// cleanup memory
	free( graph_nodes);
	free( graph_edge);
	free( graph_mask);
	free( updating_graph_mask);
	free( graph_visited);
	cudaFree(d_graph);
	cudaFree(d_edge);
	cudaFree(d_graph_mask);
	cudaFree(d_updating_graph_mask);
	cudaFree(d_graph_visited);

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
                printf("Now visiting vertex %d\n",presentVertex);
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

