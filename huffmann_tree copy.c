//
// Created by Cameron Brewster on 2024-11-20.
//
#include "huffmann_tree.h"



HuffmanNode *createLeafNode(char data, int frequency) {
    HuffmanNode * newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode)); // Allocating memory
    if(newNode == NULL) {
        printf("Memory allocation failed");
        exit(EXIT_FAILURE); //Edge case if memory allocation fails
    }

    newNode -> frequency = frequency;
    newNode -> data = data;
    newNode -> left = NULL;
    newNode -> right =NULL; // Initializing leaf node
    return newNode;

}


HuffmanNode *createInternalNodes(HuffmanNode *left, HuffmanNode *right) {
    HuffmanNode *newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode)); // allocating memory for internal node
    if(newNode == NULL) {
        printf("Memory allocation failed");
        exit(EXIT_FAILURE);  //Edge case if memory allocation fails
    }
    newNode -> frequency = left->frequency + right->frequency; // Setting the frequency to the sum of both its child nodes
    newNode -> data = '\0'; // Internal nodes do not hold a character
    newNode -> left = left; // Point to children
    newNode -> right = right;

    return newNode;
}


HuffmanNode* buildHuffmanTree(Queue* queue) {
    while (queue->front != NULL && queue->front->next != NULL) {  // While more than one node in the queue

        // Dequeue the two nodes with the smallest frequencies
        HuffmanNode* left = priorityDequeue(queue);
        HuffmanNode* right = priorityDequeue(queue);

        // Create a new internal node
        HuffmanNode* internalNode = createInternalNodes(left, right);

        // Enqueue the new internal node back into the queue
        priorityEnqueue(queue, internalNode);
    }

    // The final node left in the queue is the root of the Huffman tree
    return priorityDequeue(queue);
}

void freeHuffmanTree(HuffmanNode* root) {
    if(root == NULL) {
        return; // Edge case if tree DNE
    }
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right); // Recursive calls

    free(root);
}
void getCode(HuffmanNode *root, char codes[256][MAX], char* currentCode, int depth) {
    if (root == NULL) {
        return;
    }

    // Base case: if the node is a leaf node (no children)
    if (root->left == NULL && root->right == NULL) {
        currentCode[depth] = '\0';  // Null-terminate the code string
        strcpy(codes[(unsigned char)root->data], currentCode);  // Store the code for this character
        printf("Character: '%c', Code: %s\n", root->data, currentCode);  // Debug print
        return;
    }

    // Traverse the left subtree
    currentCode[depth] = '0';
    getCode(root->left, codes, currentCode, depth + 1);

    // Traverse the right subtree
    currentCode[depth] = '1';
    getCode(root->right, codes, currentCode, depth + 1);
}

void HuffmanCodes(HuffmanNode *root, char codes[256][MAX]) {
    char currentCode[MAX] = {0};  // Initialize currentCode array to store the current code being generated

    // Special case for when there is only one character (tree with only one node)
    if (root != NULL && root->left == NULL && root->right == NULL) {
        currentCode[0] = '0';  // Assign the code '0' for the single character
        codes[(unsigned char)root->data][0] = '0';
        codes[(unsigned char)root->data][1] = '\0';  // Null-terminate the string
        printf("Character: '%c', Code: %s\n", root->data, currentCode);  // Debug print
    } else {
        getCode(root, codes, currentCode, 0);  // Call getCode to generate the codes
    }
}


void countFrequencies(const char *filename, node_t *nodes, int *uniqueCharCount) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        exit(1);
    }

    // Initialize frequency array
    int frequencies[256] = {0};

    // Read characters and count occurrences
    int c;
    while ((c = fgetc(file)) != EOF) {
        frequencies[c]++;
    }

    // Populate the nodes array with non-zero frequencies
    int nodeIndex = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            nodes[nodeIndex].index = i;
            nodes[nodeIndex].weight = frequencies[i];
            nodeIndex++;
        }
    }

    *uniqueCharCount = nodeIndex;
    fclose(file);

    // Debug print the frequency table
    printf("Frequency Table:\n");
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            printf("Character: '%c', Frequency: %d\n", i, frequencies[i]);
        }
    }
}


HuffmanNode* priorityDequeue(Queue* q) {
    if (q->front == NULL) {
        fprintf(stderr, "Queue is empty! Cannot dequeue.\n");
        return NULL;  // Return NULL if the queue is empty
    }

    // Store the data of the front node to return
    QueueNode* temp = q->front;
    HuffmanNode* dequeuedNode = temp->node;

    // Move the front pointer to the next node
    q->front = q->front->next;

    // If the queue is now empty, set rear to NULL
    if (q->front == NULL) {
        q->rear = NULL;
    }

    // Free the memory of the dequeued node
    free(temp);

    return dequeuedNode;
}




void freeQueue(Queue* q){
    QueueNode* current = q->front;
    while (current != NULL) {
        QueueNode* temp = current;
        current = current->next;
        free(temp);
    }
    q->front = NULL;
    q->rear = NULL;
}

void priorityEnqueue(Queue* q, HuffmanNode* newNode) {
    // Allocate memory for the new QueueNode
    QueueNode* newQueueNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (!newQueueNode) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);  // Exit if memory allocation fails
    }
    newQueueNode->node = newNode;
    newQueueNode->next = NULL;

    // If the queue is empty, insert at the front
    if (q->front == NULL) {
        q->front = q->rear = newQueueNode;
        return;
    }

    // If the new node has a smaller frequency than the front node, insert at the front
    if (newNode->frequency < q->front->node->frequency) {
        newQueueNode->next = q->front;
        q->front = newQueueNode;
        return;
    }

    // Traverse to find the correct insertion point (based on frequency)
    QueueNode* current = q->front;
    while (current->next != NULL && current->next->node->frequency <= newNode->frequency) {
        current = current->next;
    }

    // Insert the new node
    newQueueNode->next = current->next;
    current->next = newQueueNode;

    // If inserted at the end, update the rear pointer
    if (newQueueNode->next == NULL) {
        q->rear = newQueueNode;
    }
}



void initQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}
void compress(const char *filename, char *compressed, HuffmanNode *root, char codes[256][MAX]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file '%s'\n", filename);
        exit(1);
    }

    int j = 0;  // Index for storing bits in the compressed array
    int c;
    while ((c = fgetc(file)) != EOF) {
        // Get the Huffman code for the current character
        char *code = codes[(unsigned char)c];  // Get the Huffman code for the current character

        // Append the Huffman code to the compressed array bit by bit
        for (int k = 0; code[k] != '\0'; k++) {
            compressed[j++] = code[k];  // Store each bit as a character ('0' or '1') in the compressed array
        }
    }

    // Ensure the string is null-terminated after the last bit (not adding extra bits)
    compressed[j] = '\0';  // Null-terminate the compressed data to indicate the end of the bit string

    fclose(file);
}






void decompress(const char *compressed, char *decompressed, HuffmanNode *root) {
    if (root == NULL) {
        printf("Error: Huffman Tree is empty!\n");
        return;
    }

    // Check if the tree has only one node
    if (root->left == NULL && root->right == NULL) {
        // Handle the special case for a single character tree
        int index = 0;
        while (compressed[index] != '\0') {
            decompressed[index] = root->data;  // Just append the character
            index++;
        }
        decompressed[index] = '\0';  // Null-terminate the string
        printf("Decompressed Data: %s\n", decompressed);
        return;
    }

    // General case: Traverse the tree
    int index = 0;  // Pointer to the current position in the compressed string
    int decompressedIndex = 0;  // Pointer to the current position in the decompressed string
    HuffmanNode *currentNode = root;  // Start from the root of the tree

    // Traverse the compressed binary string
    while (compressed[index] != '\0') {
        // Traverse the tree based on the current bit (0 or 1)
        if (compressed[index] == '0') {
            currentNode = currentNode->left;  // Go to left child
        } else if (compressed[index] == '1') {
            currentNode = currentNode->right;  // Go to right child
        } else {
            printf("Error: Invalid compressed data!\n");
            return;
        }

        // If a leaf node is reached (i.e., we have a character)
        if (currentNode->left == NULL && currentNode->right == NULL) {
            decompressed[decompressedIndex++] = currentNode->data;  // Add the character to the decompressed string
            currentNode = root;  // Reset to the root of the tree to decode the next character
        }

        index++;  // Move to the next bit in the compressed string
    }

    decompressed[decompressedIndex] = '\0';  // Null-terminate the decompressed string
}
