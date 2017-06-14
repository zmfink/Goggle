#ifndef INDEXER_H
#define INDEXER_H

#include <iosfwd>

class Indexer {
public:
    void index(std::ifstream& content, std::ostream& outfile);
};

#endif
