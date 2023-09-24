# sequential-index

## Description

This project implements a sequential index file structure. It is composed of three files: the index file, the auxiliar file and the duplicates file. Each records keeps a pointer to its next duplicate and its next record. The index file is sorted by the key of each record, while the auxiliar is not. The duplicates file is used to store the duplicates of each key. The index file is rebuilt after each delete operation or if the threshold $*$ does not hold.

## Complexity 

$n := Number \ of \ records \ in \ the \ index file$

$m := Number \ of \ records \ in \ the \ auxiliar file*$

$K := Average \ number \ of \ duplicates \ of \ a \ key$

$*(lg(n) > m)$

If indexing over unique keys, $K = 1$.

| Operation     | Complexity             | Description       |
| ---------     | ----------             | -----------       |
| ```Search```        | $O(lg(n) + m + K)$     | Binary search in index file and linear search in auxiliar file. Retrieve duplicates from linked list.|
| ```Range_search```  | $O((n + m) * K)$             | Search operation of beginning key and iterates over the next records linearly.                   |
| ```Insert```        | $O(lg(n) + m)$         | Search operation. New records are written at the end of the auxiliar file. Duplicates are linked with LIFO method.|
| ```Delete```        | $O(lg(n) + m)$     | Search operation of key to be deleted (REBUILDS FILES AFTER THIS OPERATION).                  |
| ```Rebuild```       | $O( (n + m) * K)$             | Rebuilds index file with sorted keys and empties auxiliar file.                                                               |

