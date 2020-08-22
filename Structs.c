/**
 * @file Structs.c
 * @author  Ayelet Avraham <ayelet.avraham@mail.huji.ac.il> username ayeletavr
 * @date 03 june 2020
 * @brief this file includes functions on two structs: Vector and strings. This functions can be
 * implemnted on RBtree.
*/

#include <stdio.h>
#include "Structs.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#define LESS (-1)
#define EQUAL (0)
#define GREATER (1)


/**
 * CompFunc for Vectors, compares element by element, the vector that has the first larger
 * element is considered larger. If vectors are of different lengths and identify for the length
 * of the shorter vector, the shorter vector is considered smaller.
 * @param a - first vector
 * @param b - second vector
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a.
 */
int vectorCompare1By1(const void *a, const void *b) // implement it in Structs.c
{
    const Vector v1 = *(const Vector *) a;
    const Vector v2 = *(const Vector *) b;
    int shorterLen;
    if (v1.len < v2.len)
    {
        shorterLen = v1.len;
    }
    else
    {
        shorterLen = v2.len;
    }
    for (int i = 0; i < shorterLen; i++)
    {
        if (v1.vector[i] > v2.vector[i])
        {
            return GREATER;
        }
        else if (v1.vector[i] < v2.vector[i])
        {
            return LESS;
        }
        else // v1.vector[i] == v2.vector[i]
        {
            continue;
        }
    }
    if (v1.len > v2.len)
    {
        return GREATER;
    }
    else if (v1.len < v2.len)
    {
        return LESS;
    }
    else
    {
        return EQUAL;
    }
}

/**
 * @brief calculate the norm of a given vector.
 * @param data: vector's doubles.
 * @param len: vector's len.
 * @return the vector's norm.
 */
double getNorm(const Vector *vector)
{
    if (vector == NULL || vector->vector == NULL || vector->len < 0)
    {
        return -1;
    }
    double norm = 0;
    for (int i = 0; i < vector->len; i++)
    {
        norm += (vector->vector[i]) * (vector->vector[i]);
    }
    return norm; // maybe no sqrt?
}

/**
 * copy pVector to pMaxVector if : 1. The norm of pVector is greater then the norm of pMaxVector.
 * 								   2. pMaxVector->vector == NULL.
 * @param pVector pointer to Vector
 * @param pMaxVector pointer to Vector
 * @return 1 on success, 0 on failure (if pVector == NULL: failure).
 */
int copyIfNormIsLarger(const void *pVector, void *pMaxVector) // implement it in Structs.c
{
    const Vector *toCopyVector = (const Vector *) pVector;
    Vector *copyToVector = (Vector *) pMaxVector;
    if (pMaxVector != NULL && pVector != NULL && toCopyVector->vector != NULL &&
        toCopyVector->len > 0)
    {
        if ((getNorm(toCopyVector) > getNorm(copyToVector)) || (toCopyVector->vector == NULL))
        {
            if (copyToVector->vector != NULL)
            {
                free(copyToVector->vector);
            }
            copyToVector->vector = (double*) malloc (sizeof(double) * (toCopyVector->len));
            if (copyToVector->vector == NULL)
            {
                return false;
            }
            copyToVector->len = toCopyVector->len;
            memcpy(copyToVector->vector, toCopyVector->vector, sizeof(double) * toCopyVector->len);
        }
        return true;
    }
    return false;
}

/**
 * @param tree a pointer to a tree of Vectors
 * @return pointer to a *copy* of the vector that has the largest norm (L2 Norm).
 */
Vector *findMaxNormVectorInTree(RBTree *tree) // implement it in Structs.c You must use copyIfNormIsLarger in the implementation!
{
    if (tree == NULL || tree->root == NULL)
    {
        return NULL;
    }
    Vector *res = (Vector*) malloc(sizeof(Vector));
    if (res == NULL)
    {
        return NULL;
    }
    res->len = 0;
    res->vector = NULL;
    if (!forEachRBTree(tree, copyIfNormIsLarger, res))
    {
        freeVector(res);
        return NULL;
    }
    return res;
}

/**
 * FreeFunc for vectors
 */
void freeVector(void *pVector) // implement it in Structs.c
{
    Vector *v = (Vector *) pVector;
    if (v == NULL || v->len == 0)
    {
        return;
    }
    else
    {
        free(v->vector);
        v->vector = NULL;
    }
    free(v);
}

/**
 * CompFunc for strings (assumes strings end with "\0")
 * @param a - char* pointer
 * @param b - char* pointer
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a. (lexicographic
 * order)
 */
int stringCompare(const void *a, const void *b) // implement it in Structs.c
{
    char *c1 = (char*) a;
    char *c2 = (char*) b;
    return strcmp(c1, c2);
}

/**
 * ForEach function that concatenates the given word and \n to pConcatenated. pConcatenated is
 * already allocated with enough space.
 * @param word - char* to add to pConcatenated
 * @param pConcatenated - char*
 * @return 0 on failure, other on success
 */
int concatenate(const void *word, void *pConcatenated) // implement it in Structs.c
{
    if (pConcatenated == NULL || word == NULL)
    {
        return false;
    }
    if (strcat((char*) pConcatenated, (
        char*) word) == NULL || strcat((char*) pConcatenated, "\n") == NULL)
    {
        return false;
    }
    return true;
}

/**
 * FreeFunc for strings
 */
void freeString(void *s) // implement it in Structs.c
{
    if ((char*) s != NULL)
    {
        free((char*) s);
    }
}