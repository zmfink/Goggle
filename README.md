# Goggle

Goggle is a search engine that searches 301.64 MB of Wikipedia articles* and uses Hadoop MapReduce to analyze the large data set, employing tf-idf and PageRank statistics in order to return accurately ranked search results. 

Running ./eecs485pa6pr.sh creates the pagerank output file proutput/pgrkoutput and running ./eecs485pa6pr.sh creates the Inverted Index file invoutput/invOutfile

Running
      source dropadd.sql
will fill create the database group92, create the sql tables and populate them.

Inside IndexServer:
        make clean.
        make.
        ./IndexServer [port Number] [Inverted Index file]
  will run the Index Server, allowing queries to be processed

Running
    python app.py
will run the website.


*input files hadoop/dataset/mining.imageURLS.xml and hadoop/dataset/mining.articles.xml and output file invoutput/invOutfile exceed Github's maximum file size so they aren't included.
